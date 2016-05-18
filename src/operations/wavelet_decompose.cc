/* 
 */

/*

    Copyright (C) 2014 Ferrero Andrea

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.


 */

/*

    These files are distributed with PhotoFlow - http://aferrero2707.github.io/PhotoFlow/

 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../base/new_operation.hh"
#include "icc_transform.hh"
#include "wavelet_decompose.hh"


PF::WaveletDecomposePar::WaveletDecomposePar():
OpParBase(),
numScales("numScales",this,0),
currScale("currScale",this,0),
blendFactor("blendFactor",this,0.5f),
in_profile( NULL )
{
  convert2lab = PF::new_operation( "convert2lab", NULL );
  convert2input = new_icc_transform();
  wavelet_decompose_algo = new PF::Processor<PF::WaveletDecomposeAlgoPar,PF::WaveletDecomposeAlgoProc>();

  set_type("wavelet_decompose" );

  set_default_name( _("Wavelet Decompose") );
}

VipsImage* PF::WaveletDecomposePar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  std::cout<<"WaveletDecomposePar::build "<<std::endl;
  
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;
  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  WaveletDecomposeAlgoPar* wavelet_decompose_par = dynamic_cast<WaveletDecomposeAlgoPar*>( wavelet_decompose_algo->get_par() );
  int max_scales = wavelet_decompose_par->get_maxScales(srcimg->Xsize, srcimg->Ysize);
  if (numScales.get() > max_scales) {
    std::cout<<"WaveletDecomposePar::build: max scale is "<<max_scales<<" for this image preview size"<<std::endl;
  }
  else
    max_scales = numScales.get();
  wavelet_decompose_par->set_numScales( max_scales );
  wavelet_decompose_par->set_currScale( currScale.get() );
  wavelet_decompose_par->set_blendFactor( blendFactor.get() );

  int padding = wavelet_decompose_par->get_padding();

  // Extend the image by padding() pixels
  VipsImage* extended;
  VipsExtend extend = VIPS_EXTEND_COPY;
  if( vips_embed(srcimg, &extended, padding, padding,
      srcimg->Xsize+padding*2, srcimg->Ysize+padding*2,
      "extend", extend, NULL) ) {
    std::cout<<"WaveletDecomposePar::build(): vips_embed() failed."<<std::endl;
    PF_REF( in[0], "WaveletDecomposePar::build(): vips_embed() failed." );
    return in[0];
  }

  wavelet_decompose_par->set_image_hints( extended );
  wavelet_decompose_par->set_format( get_format() );
  in2.clear();
  in2.push_back( extended );
  VipsImage* wavdec = wavelet_decompose_par->build( in2, 0, NULL, NULL, level );

  // Final cropping to remove the padding pixels
  VipsImage* cropped;
  if( vips_crop(wavdec, &cropped, padding, padding,
      srcimg->Xsize, srcimg->Ysize, NULL) ) {
    std::cout<<"WaveletDecomposePar::build(): vips_crop() failed."<<std::endl;
    PF_UNREF( wavdec, "WaveletDecomposePar::build(): wavdec unref" );
    PF_REF( in[0], "WaveletDecomposePar::build(): vips_crop() failed" );
    return in[0];
  }
  PF_UNREF( wavdec, "WaveletDecomposePar::build(): wavdec unref" );

  std::cout<<"wavdec->Xsize="<<wavdec->Xsize<<", wavdec->Ysize="<<wavdec->Ysize<<std::endl;
  
  set_image_hints( cropped );

  return cropped;

}


PF::ProcessorBase* PF::new_wavelet_decompose()
{
  return new PF::Processor<PF::WaveletDecomposePar,PF::WaveletDecomposeProc>();
}
