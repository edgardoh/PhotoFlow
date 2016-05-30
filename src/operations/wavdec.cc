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
#include "wavdec.hh"


PF::WavDecPar::WavDecPar():
OpParBase(),
numScales("numScales",this,0),
currScale("currScale",this,0),
blendFactor("blendFactor",this,0.5f),
in_profile( NULL )
{
  convert2lab = PF::new_operation( "convert2lab", NULL );
  convert2input = new_icc_transform();
  wavdec_algo = new PF::Processor<PF::WavDecAlgoPar,PF::WavDecAlgoProc>();

  set_type("wavdec" );

  set_default_name( _("Wav Dec") );
}

VipsImage* PF::WavDecPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  std::cout<<"WavDecPar::build "<<std::endl;
  
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;
  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  bool do_caching = true;
  int tw = 128, th = 128, nt = 1000;
  VipsAccess acc = VIPS_ACCESS_RANDOM;
  int threaded = 1, persistent = 0;


  WavDecAlgoPar* wavdec_par = dynamic_cast<WavDecAlgoPar*>( wavdec_algo->get_par() );
  int max_scales = wavdec_par->get_maxScales(srcimg->Xsize, srcimg->Ysize);
  if (numScales.get() > max_scales) {
    std::cout<<"WavDecPar::build: max scale is "<<max_scales<<" for this image preview size"<<std::endl;
  }
  else
    max_scales = numScales.get();
  wavdec_par->set_numScales( max_scales );
  wavdec_par->set_currScale( currScale.get() );
  wavdec_par->set_blendFactor( blendFactor.get() );

  int padding = wavdec_par->get_padding();

  // Extend the image by padding() pixels
  VipsImage* extended;
  VipsExtend extend = VIPS_EXTEND_COPY;
  if( vips_embed(srcimg, &extended, padding, padding,
      srcimg->Xsize+padding*2, srcimg->Ysize+padding*2,
      "extend", extend, NULL) ) {
    std::cout<<"WavDecPar::build(): vips_embed() failed."<<std::endl;
    PF_REF( in[0], "WavDecPar::build(): vips_embed() failed." );
    return in[0];
  }

  // Memory caching of the padded image
  VipsImage* cached = extended;
  if( do_caching ) {
    if( vips_tilecache(extended, &cached,
        "tile_width", tw, "tile_height", th, "max_tiles", nt,
        "access", acc, "threaded", threaded, "persistent", persistent, NULL) ) {
      std::cout<<"GaussBlurPar::build(): vips_tilecache() failed."<<std::endl;
      return NULL;
    }
    PF_UNREF( extended, "WavDecPar::build(): extended unref" );
  }

  wavdec_par->set_image_hints( cached );
  wavdec_par->set_format( get_format() );
  in2.clear();
  in2.push_back( cached );
  VipsImage* wavdec = wavdec_par->build( in2, 0, NULL, NULL, level );
  PF_UNREF( cached, "WavDecPar::build(): cached unref" );
  
  // Final cropping to remove the padding pixels
  VipsImage* cropped;
  if( vips_crop(wavdec, &cropped, padding, padding,
      srcimg->Xsize, srcimg->Ysize, NULL) ) {
    std::cout<<"WavDecPar::build(): vips_crop() failed."<<std::endl;
    PF_UNREF( wavdec, "WavDecPar::build(): wavdec unref" );
    PF_REF( in[0], "WavDecPar::build(): vips_crop() failed" );
    return in[0];
  }
  PF_UNREF( wavdec, "WavDecPar::build(): wavdec unref" );

//  std::cout<<"wavdec->Xsize="<<wavdec->Xsize<<", wavdec->Ysize="<<wavdec->Ysize<<std::endl;
  
  set_image_hints( cropped );

  return cropped;

}


PF::ProcessorBase* PF::new_wavdec()
{
  return new PF::Processor<PF::WavDecPar,PF::WavDecProc>();
}

