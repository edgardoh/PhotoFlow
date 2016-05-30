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
#include "wavdec.hh"


PF::WaveletDecomposePar::WaveletDecomposePar():
OpParBase(),
numScales("numScales",this,0),
//currScale("currScale",this,0),
blendFactor("blendFactor",this,0.5f),
in_profile( NULL )
{
//  convert2lab = PF::new_operation( "convert2lab", NULL );
//  convert2input = new_icc_transform();
//  wavelet_decompose_algo = new PF::Processor<PF::WaveletDecomposeAlgoPar,PF::WaveletDecomposeAlgoProc>();
  wav_dec_algo = new PF::Processor<PF::WavDecAlgoPar,PF::WavDecAlgoProc>();

  set_type("wavelet_decompose" );

  set_default_name( _("Wavelet Decompose") );
}

/*VipsImage* PF::WaveletDecomposePar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)*/
std::vector<VipsImage*> PF::WaveletDecomposePar::build_many(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  std::cout<<"WaveletDecomposePar::build "<<std::endl;
  
  std::vector<VipsImage*> outvec;

  if( (in.size()<1) || (in[0]==NULL) )
    return outvec;
  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

/*  bool do_caching = true;
  int tw = 128, th = 128, nt = 1000;
  VipsAccess acc = VIPS_ACCESS_RANDOM;
  int threaded = 1, persistent = 0;
*/

/*  WaveletDecomposeAlgoPar* wavelet_decompose_par = dynamic_cast<WaveletDecomposeAlgoPar*>( wavelet_decompose_algo->get_par() );
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
*/
  WavDecAlgoPar* wav_dec_par = dynamic_cast<WavDecAlgoPar*>( wav_dec_algo->get_par() );
  int max_scales = wav_dec_par->get_maxScales(srcimg->Xsize, srcimg->Ysize);
  if (numScales.get() > max_scales) {
    std::cout<<"WaveletDecomposePar::build: max scale is "<<max_scales<<" for this image preview size"<<std::endl;
  }
  else {
    max_scales = numScales.get();
  }
  
  while (max_scales > 0) {
  wav_dec_par->set_numScales( 1 );
  wav_dec_par->set_currScale( 1 );
  wav_dec_par->set_blendFactor( blendFactor.get() );

/*  int padding = wav_dec_par->get_padding();

  // Extend the image by padding() pixels
  VipsImage* extended;
  VipsExtend extend = VIPS_EXTEND_COPY;
  if( vips_embed(srcimg, &extended, padding, padding,
      srcimg->Xsize+padding*2, srcimg->Ysize+padding*2,
      "extend", extend, NULL) ) {
    std::cout<<"WaveletDecomposePar::build(): vips_embed() failed."<<std::endl;
    PF_REF( in[0], "WaveletDecomposePar::build(): vips_embed() failed." );
    return outvec;
  }

  // Memory caching of the padded image
  VipsImage* cached = extended;
  if( do_caching ) {
    if( vips_tilecache(extended, &cached,
        "tile_width", tw, "tile_height", th, "max_tiles", nt,
        "access", acc, "threaded", threaded, "persistent", persistent, NULL) ) {
      std::cout<<"GaussBlurPar::build(): vips_tilecache() failed."<<std::endl;
      return outvec;
    }
    PF_UNREF( extended, "WaveletDecomposePar::build(): extended unref" );
  }
*/
/*  wavelet_decompose_par->set_image_hints( cached );
  wavelet_decompose_par->set_format( get_format() );
  in2.clear();
  in2.push_back( cached );
  VipsImage* wavdec = wavelet_decompose_par->build( in2, 0, NULL, NULL, level );
  PF_UNREF( cached, "WaveletDecomposePar::build(): cached unref" );
*/
  wav_dec_par->set_image_hints( srcimg );
  wav_dec_par->set_format( get_format() );
  in2.clear();
  in2.push_back( srcimg );
  VipsImage* wavdec = wav_dec_par->build( in2, 0, NULL, NULL, level );
//  PF_UNREF( cached, "WaveletDecomposePar::build(): cached unref" );
  
  // Final cropping to remove the padding pixels
/*  VipsImage* cropped;
  if( vips_crop(wavdec, &cropped, padding, padding,
      srcimg->Xsize, srcimg->Ysize, NULL) ) {
    std::cout<<"WaveletDecomposePar::build(): vips_crop() failed."<<std::endl;
    PF_UNREF( wavdec, "WaveletDecomposePar::build(): wavdec unref" );
    PF_REF( in[0], "WaveletDecomposePar::build(): vips_crop() failed" );
    return outvec;
  }
  PF_UNREF( wavdec, "WaveletDecomposePar::build(): wavdec unref" );
*/
  std::cout<<"wavdec->Xsize="<<wavdec->Xsize<<", wavdec->Ysize="<<wavdec->Ysize<<std::endl;
  std::cout<<"wavelet decompose scale: "<<max_scales<<std::endl;
  
  set_image_hints( wavdec );
  
  outvec.push_back( wavdec );
  
  srcimg = wavdec;
  
  max_scales--;
  }
  
//  return cropped;
  return outvec;
}


PF::ProcessorBase* PF::new_wavelet_decompose()
{
  return new PF::Processor<PF::WaveletDecomposePar,PF::WaveletDecomposeProc>();
}

