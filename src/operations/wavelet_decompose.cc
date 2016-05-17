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
//method("method",this,PF::SHAHI_GAUSSIAN,"SHAHI_GAUSSIAN","gaussian"),
numScales("numScales",this,0),
currScale("currScale",this,0),
/*wp_adjustment("wp_adjustment",this,0),
radius("radius",this,100),
compress("compress",this,50),
sh_color_adjustment("sh_color_adjustment",this,100),
hi_color_adjustment("hi_color_adjustment",this,50),*/
in_profile( NULL )
{
  //method.add_enum_value(PF::SHAHI_BILATERAL,"SHAHI_BILATERAL","bilateral");

//  gauss = new_gaussblur();
  convert2lab = PF::new_operation( "convert2lab", NULL );
  convert2input = new_icc_transform();
  wavelet_decompose_algo = new PF::Processor<PF::WaveletDecomposeAlgoPar,PF::WaveletDecomposeAlgoProc>();

  set_type("wavelet_decompose" );

  set_default_name( _("Wavelet Decompose") );
}

/*
bool PF::WaveletDecomposePar::needs_caching()
{
  return false;
}
*/


VipsImage* PF::WaveletDecomposePar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
#if 1
  std::cout<<"WaveletDecomposePar::build "<<std::endl;
  
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;
  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  WaveletDecomposeAlgoPar* wavelet_decompose_par = dynamic_cast<WaveletDecomposeAlgoPar*>( wavelet_decompose_algo->get_par() );
  wavelet_decompose_par->set_numScales( numScales.get() );
  wavelet_decompose_par->set_currScale( currScale.get() );

  int padding = wavelet_decompose_par->get_padding();

  // Extend the image by two pixels to account for the pixel averaging window
  // of the impulse noise reduction algorithm
  VipsImage* extended;
  VipsExtend extend = VIPS_EXTEND_COPY;
  if( vips_embed(srcimg, &extended, padding, padding,
      srcimg->Xsize+padding*2, srcimg->Ysize+padding*2,
      "extend", extend, NULL) ) {
    std::cout<<"ImpulseNRPar::build(): vips_embed() failed."<<std::endl;
    PF_REF( in[0], "ImpulseNRPar::build(): vips_embed() failed." );
    return in[0];
  }

/*  in2.clear();
  in2.push_back(extended);
  VipsImage* wavdec = PF::OpParBase::build( in2, first, NULL, omap, level );
  PF_UNREF( extended, "GaussBlurPar::build(): impnrimg unref" );
*/
  wavelet_decompose_par->set_image_hints( extended );
  wavelet_decompose_par->set_format( get_format() );
  in2.clear();
  in2.push_back( extended );
  VipsImage* wavdec = wavelet_decompose_par->build( in2, 0, NULL, NULL, level );
//  PF_UNREF( blurred, "DefringePar::build(): extended unref after convert2lab" );

  // Final cropping to remove the padding pixels
  VipsImage* cropped;
  //std::cout<<"srcimg->Xsize="<<srcimg->Xsize<<"  impnrimg->Xsize="<<impnrimg->Xsize<<std::endl;
  if( vips_crop(wavdec, &cropped, padding, padding,
      srcimg->Xsize, srcimg->Ysize, NULL) ) {
    std::cout<<"GaussBlurPar::build(): vips_crop() failed."<<std::endl;
    PF_UNREF( wavdec, "GaussBlurPar::build(): impnrimg unref" );
    PF_REF( in[0], "ImpulseNRPar::build(): vips_crop() failed" );
    return in[0];
  }
  PF_UNREF( wavdec, "GaussBlurPar::build(): impnrimg unref" );

  std::cout<<"wavdec->Xsize="<<wavdec->Xsize<<", wavdec->Ysize="<<wavdec->Ysize<<std::endl;
  
  set_image_hints( cropped );

  return cropped;

#endif
  
#if 0
  std::vector<VipsImage*> in2;
  if( in.size() < 1 )
    return NULL;

  in2.push_back( in[0] );
/*  PF::HSLMaskPar* mask_par = dynamic_cast<PF::HSLMaskPar*>( mask->get_par() );
  PF::GaussBlurPar* blur_par = dynamic_cast<PF::GaussBlurPar*>( blur->get_par() );
  if( mask_par ) {
    std::vector<VipsImage*> in3; in3.push_back( in[0] );
    mask_par->get_H_curve() = hue_H_equalizer;
    mask_par->get_S_curve() = hue_S_equalizer;
    mask_par->get_L_curve() = hue_L_equalizer;
    mask_par->set_H_curve_enabled( hue_H_equalizer_enabled.get() );
    mask_par->set_S_curve_enabled( hue_S_equalizer_enabled.get() );
    mask_par->set_L_curve_enabled( hue_L_equalizer_enabled.get() );
    mask_par->set_invert( get_invert_mask() );
    mask_par->set_image_hints( in[0] );
    mask_par->set_format( get_format() );
    VipsImage* imask = mask_par->build( in3, 0, NULL, NULL, level );

    if( feather_mask.get() == true ) {
      blur_par->set_radius( feather_radius.get() );
      blur_par->set_image_hints( in[0] );
      blur_par->set_format( get_format() );
      in3.clear();
      in3.push_back( imask );
      VipsImage* blurred = blur_par->build( in3, 0, NULL, NULL, level );
      std::cout<<"blurred = "<<blurred<<std::endl;
      PF_UNREF( imask, "HueSaturationPar::build(): imask unref");
      imask = blurred;
    }

    in2.push_back( imask );
  }
*/
  VipsImage* out = PF::OpParBase::build( in2, 0, imap, omap, level );

  if( in2.size()>1 ) PF_UNREF( in2[1], "HueSaturationPar::build(): in[2] unref");

  return out;

#endif
  
#if 0
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;

  std::vector<VipsImage*> in2;

  void *data;
  size_t data_length;
  if( in_profile ) cmsCloseProfile( in_profile );
  in_profile = NULL;
  if( !vips_image_get_blob( in[0], VIPS_META_ICC_NAME,
      &data, &data_length ) ) {
    in_profile = cmsOpenProfileFromMem( data, data_length );
  }

/*  convert2lab->get_par()->set_image_hints( in[0] );
  convert2lab->get_par()->set_format( get_format() );
  in2.clear(); in2.push_back( in[0] );
  VipsImage* labimg = convert2lab->get_par()->build( in2, 0, NULL, NULL, level );
  if( !labimg ) {
    std::cout<<"WaveletDecomposePar::build(): null Lab image"<<std::endl;
    PF_REF( in[0], "WaveletDecomposePar::build(): null Lab image" );
    return in[0];
  }
  //std::cout<<"srcimg->Xsize="<<srcimg->Xsize<<"  extended->Xsize="<<extended->Xsize<<std::endl;
*/
  VipsImage* labimg = in[0];
  
  VipsImage* out = NULL;
/*  VipsImage* blurred = NULL;
  switch( method.get_enum_value().first ) {
  case PF::SHAHI_GAUSSIAN: {
    GaussBlurPar* gausspar = dynamic_cast<GaussBlurPar*>( gauss->get_par() );
    if( gausspar ) {
      gausspar->set_radius( radius.get() );
      gausspar->set_image_hints( labimg );
      gausspar->set_format( get_format() );
      in2.clear(); in2.push_back( labimg );
      blurred = gausspar->build( in2, 0, NULL, NULL, level );
      PF_UNREF( labimg, "ImageReaderPar::build(): extended unref after convert2lab" );
    }
    break;
  }
  default:
    break;
  }

  if( !blurred ) {
    std::cout<<"WaveletDecomposePar::build(): null Lab image"<<std::endl;
    PF_REF( in[0], "WaveletDecomposePar::build(): null Lab image" );
    return in[0];
  }
*/
  in2.clear();
  in2.push_back(labimg);
//  in2.push_back(blurred);
  VipsImage* wavdec = OpParBase::build( in2, 0, imap, omap, level );
  //PF_UNREF( labimg, "ImageArea::update() labimg unref" );
//  PF_UNREF( blurred, "ImageArea::update() blurred unref" );

  PF::ICCTransformPar* icc_par = dynamic_cast<PF::ICCTransformPar*>( convert2input->get_par() );
  //std::cout<<"ImageArea::update(): icc_par="<<icc_par<<std::endl;
  if( icc_par ) {
    //std::cout<<"ImageArea::update(): setting display profile: "<<current_display_profile<<std::endl;
    icc_par->set_out_profile( in_profile );
  }
  convert2input->get_par()->set_image_hints( wavdec );
  convert2input->get_par()->set_format( get_format() );
  in2.clear(); in2.push_back( wavdec );
  std::cout<<"WaveletDecomposePar::build(): calling convert2input->get_par()->build()"<<std::endl;
  out = convert2input->get_par()->build(in2, 0, NULL, NULL, level );
  PF_UNREF( wavdec, "ImageArea::update() cropped unref" );


  set_image_hints( in[0] );

  return out;
#endif
}


PF::ProcessorBase* PF::new_wavelet_decompose()
{
  return new PF::Processor<PF::WaveletDecomposePar,PF::WaveletDecomposeProc>();
}
