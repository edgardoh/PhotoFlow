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
#include "wavelet_decompose.hh"
#include "wavdec.hh"
#include "subtr_image.hh"


PF::WaveletDecomposePar::WaveletDecomposePar():
OpParBase(),
numScales("numScales",this,0),
blendFactor("blendFactor",this,0.5f)
{
  wav_dec_algo = new PF::Processor<PF::WavDecAlgoPar,PF::WavDecAlgoProc>();
  subtract_image_algo = new PF::Processor<PF::SubtractImageAlgoPar,PF::SubtractImageAlgoProc>();

  set_type("wavelet_decompose" );

  set_default_name( _("Wavelet Decompose") );
}

std::vector<VipsImage*> PF::WaveletDecomposePar::build_many(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  std::cout<<"WaveletDecomposePar::build "<<std::endl;
  
  std::vector<VipsImage*> outvec;

  if( (in.size()<1) || (in[0]==NULL) )
    return outvec;
  
  // zero scales return the original image
  if (numScales.get() <= 0) {
    outvec = PF::OpParBase::build_many(in, first, imap, omap, level);
    return outvec;
  }
  
  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  bool do_caching = true;
  int tw = 128, th = 128, nt = 1000;
  VipsAccess acc = VIPS_ACCESS_RANDOM;
  int threaded = 1, persistent = 0;


  WavDecAlgoPar* wav_dec_par = dynamic_cast<WavDecAlgoPar*>( wav_dec_algo->get_par() );
  int max_scales = wav_dec_par->get_maxScales(srcimg->Xsize, srcimg->Ysize);
  if (numScales.get() > max_scales) {
    std::cout<<"WaveletDecomposePar::build: max scale is "<<max_scales<<" for this image preview size"<<std::endl;
  }
  else {
    max_scales = numScales.get();
  }

  int padding = get_padding(max_scales);

  SubtractImageAlgoPar* subtract_img_par = dynamic_cast<SubtractImageAlgoPar*>( subtract_image_algo->get_par() );

  // Extend the original image by padding() pixels in order to decompose into scales
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

  // loop on the number of scales, decomposing one at a time
  while (max_scales > 0) {
    // decompose on the next scale
    wav_dec_par->set_numScales( 1 );
    wav_dec_par->set_currScale( 1 );
    wav_dec_par->set_blendFactor( blendFactor.get() );
  
    wav_dec_par->set_image_hints( cached );
    wav_dec_par->set_format( get_format() );
    in2.clear();
    in2.push_back( cached );
    VipsImage* wavdec = wav_dec_par->build( in2, 0, NULL, NULL, level );
  
    // crop the decomposed image to remove the padding pixels in order to return it
    VipsImage* cropped;
    if( vips_crop(wavdec, &cropped, padding, padding,
        srcimg->Xsize, srcimg->Ysize, NULL) ) {
      std::cout<<"WaveletDecomposePar::build(): vips_crop() failed."<<std::endl;
      PF_UNREF( wavdec, "WaveletDecomposePar::build(): wavdec unref" );
      PF_REF( in[0], "WaveletDecomposePar::build(): vips_crop() failed" );
      return outvec;
    }

    // return cropped scale
    set_image_hints( cropped );
    outvec.push_back( cropped );
  
    // subtract decomposed (un-cropped) image from the input one 
    subtract_img_par->set_blendFactor( blendFactor.get() );
    subtract_img_par->set_image_hints( wavdec );
    subtract_img_par->set_format( get_format() );
    in2.clear();
    in2.push_back( wavdec );
    in2.push_back( cached );
    VipsImage* subtrimg = subtract_img_par->build( in2, 0, NULL, NULL, level );
    PF_UNREF( wavdec, "WaveletDecomposePar::build(): wavdec unref" );
    PF_UNREF( cached, "WaveletDecomposePar::build(): cached unref" );
  
    // will decompose the subtracted image on the next loop
    cached = subtrimg;
  
    max_scales--;
  }
  
  // Final cropping to remove the padding pixels from the residual image
  VipsImage* cropped;
  if( vips_crop(cached, &cropped, padding, padding,
      srcimg->Xsize, srcimg->Ysize, NULL) ) {
    std::cout<<"WaveletDecomposePar::build(): vips_crop() failed."<<std::endl;
    PF_UNREF( cached, "WaveletDecomposePar::build(): wavdec unref" );
    PF_REF( in[0], "WaveletDecomposePar::build(): vips_crop() failed" );
    return outvec;
  }
  PF_UNREF( cached, "WaveletDecomposePar::build(): cached unref" );
  
  set_image_hints( cropped );
  
  outvec.push_back( cropped );

  return outvec;
}


PF::ProcessorBase* PF::new_wavelet_decompose()
{
  return new PF::Processor<PF::WaveletDecomposePar,PF::WaveletDecomposeProc>();
}

