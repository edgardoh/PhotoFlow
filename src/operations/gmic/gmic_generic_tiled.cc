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


#include "gmic_generic_tiled.hh"
#include "../vips/gmic/gmic/src/gmic.h"

#include "../convertformat.hh"


PF::GmicGenericTiledPar::GmicGenericTiledPar(): 
OpParBase(), 
gmic_filter_prop("gmic_filter_prop", this)
{ 
  convert_format = PF::new_convert_format();  
  convert_format2 = PF::new_convert_format(); 

  m_padding = 0;
  
  set_type( "gmic_generic_tiled" );
  set_default_name( _("GMIC Generic") );
  
}

std::string PF::GmicGenericTiledPar::build_command()
{
  return gmic_filter_prop.get().get_gmic_command(get_verbosity_mode());
}

VipsImage* PF::GmicGenericTiledPar::build(std::vector<VipsImage*>& in, int first, 
           VipsImage* imap, VipsImage* omap, 
           unsigned int& level)
{
  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];

  if( !srcimg ) {
    std::cout<<"GMicPar::build(): null input image"<<std::endl;
    return NULL;
  }

  std::string command = build_command();
  
  if( command.empty() ) {
    PF_REF( srcimg, "GMicPar::build(): empty command string" );
    return srcimg;
  }

//  std::cout<<"PF::GmicGenericTiledPar::build() called "<<std::endl;
//  std::cout<<"PF::GmicGenericTiledPar::build() srcimg->Xsize: "<<srcimg->Xsize<<", srcimg->Ysize"<<srcimg->Ysize<<", srcimg->Bands: "<<srcimg->Bands<<std::endl;
//  std::cout<<"PF::GmicGenericTiledPar::build() command: "<<command<<std::endl;

  int target_ch = 0;
  PF::colorspace_t cs = PF::convert_colorspace( get_interpretation() );
  switch( cs ) {
  case PF_COLORSPACE_GRAYSCALE: break;
  case PF_COLORSPACE_RGB:
    target_ch = get_rgb_target_channel();
    break;
  case PF_COLORSPACE_LAB:
    target_ch = get_lab_target_channel();
    break;
  case PF_COLORSPACE_CMYK:
    target_ch = get_cmyk_target_channel();
    break;
  default:
    break;
  }

  VipsImage* srcimg2 = srcimg;
  if( (target_ch>=0) && (target_ch<srcimg->Bands) ) {
    if( vips_extract_band( srcimg, &srcimg2, target_ch, NULL ) ) {
      std::cout<<"GMicPar::build(): vips_extract_band() failed"<<std::endl;
      return NULL;
    }
    vips_image_init_fields( srcimg2,
                            get_xsize(), get_ysize(), 
                            1, get_format(),
                            get_coding(),
                            get_interpretation(),
                            1.0, 1.0);
  } else {
    PF_REF( srcimg2, "GMicPar::build(): srcimg2 ref" );
  }

  
  std::vector<VipsImage*> in2;
  in2.push_back( srcimg2 );
  convert_format->get_par()->set_image_hints( srcimg2 );
  convert_format->get_par()->set_format( VIPS_FORMAT_FLOAT );
  VipsImage* convimg = convert_format->get_par()->build( in2, 0, NULL, NULL, level );
  if( !convimg ) {
    std::cout<<"GMicPar::build(): null convimg"<<std::endl;
    return NULL;
  }
  PF_UNREF( srcimg2, "GMicPar::build(): srcimg2 unref" );

  
  int tw = 128, th = 128, nt = 1000;
  VipsAccess acc = VIPS_ACCESS_RANDOM;
  bool threaded = true, persistent = false;
  
  VipsImage* iter_in = convimg;
  VipsImage* iter_out = NULL;
  int iterations = 1;
  bool cache_tiles = false;
  int padding = get_padding(level);
  double x_scale = 1;
  double y_scale = 1;
  
  std::string cmd = /*std::string("-verbose - ")+*/command;
  for( int i = 0; i < iterations; i++ ) {
    // Memory caching of the padded image
    VipsImage* cached = iter_in;
    if( cache_tiles ) {
      if( vips_tilecache(iter_in, &cached,
          "tile_width", tw, "tile_height", th, "max_tiles", nt,
          "access", acc, "threaded", threaded, "persistent", persistent, NULL) ) {
        std::cout<<"GMicPar::build(): vips_tilecache() failed."<<std::endl;
        return NULL;
      }
      PF_UNREF( iter_in, "GaussBlurPar::build(): iter_in unref" );
    }

    VipsImage* inv[2] = { cached, NULL };
    if( vips_gmic( inv, &iter_out, 1,
                   padding, x_scale,
                   y_scale, 
                   cmd.c_str(), NULL ) ) {
      std::cout<<"vips_gmic() failed!!!!!!!"<<std::endl;
      PF_UNREF( cached, "GMicPar::build(): vips_gmic() failed, iter_in unref" );
      return NULL;
    }
    PF_UNREF( cached, "GMicPar::build(): iter_in unref" );
    iter_in = iter_out;
  }
  
  
  in2.clear();
  in2.push_back( iter_out );
  convert_format2->get_par()->set_image_hints( iter_out );
  convert_format2->get_par()->set_format( get_format() );
  VipsImage* out = convert_format2->get_par()->build( in2, 0, NULL, NULL, level );
  PF_UNREF( iter_out, "GMicPar::build(): iter_out unref" );

  VipsImage* out2 = out;
  
  if( (target_ch>=0) && (target_ch<(srcimg->Bands)) ) {
    
    VipsImage* bandv[4] = {NULL, NULL, NULL, NULL};
    bandv[target_ch] = out;
    for( int i = 0; i < srcimg->Bands; i++ ) {
      
      if( i==target_ch ) continue;
      VipsImage* band;
      if( vips_extract_band( srcimg, &band, i, NULL ) ) {
        std::cout<<"vips_extract_band( srcimg, &band, "<<i<<", NULL ) failed"<<std::endl;
        return NULL;
      }
      vips_image_init_fields( band,
                              get_xsize(), get_ysize(), 
                              1, get_format(),
                              get_coding(),
                              get_interpretation(),
                              1.0, 1.0);
      
      bandv[i] = band;
    }
    if( vips_bandjoin( bandv, &out2, srcimg->Bands, NULL ) ) {
      return NULL;
    }
    for( int i = 0; i < srcimg->Bands; i++ ) {
      PF_UNREF( bandv[i], "GMicPar::build(): bandv[i] unref after bandjoin" );
    }
  }

  return out2;
}


void PF::GmicGenericTiledPar::set_gmic_filter(GmicFilter* gmf)
{
  if ( gmf != NULL ) {
    GmicFilter& gmf_p = gmic_filter_prop.get();
    
    if ( gmf_p.get_properties_count() == 0 ) {
      gmf_p = *gmf;
      gmf_p.create_properties();
      
      GmicProperty prop_value;
      
      if ( gmf_p.get_phf_property("phf_padding", prop_value) ) {
        set_padding( std::stoi(prop_value.get_val_str()) );
      }
      
      set_default_name( gmf_p.get_filter_name() );
    }
  }
 
}

PF::ProcessorBase* PF::new_gmic_generic_tiled()
{
  return( new PF::Processor<PF::GmicGenericTiledPar,PF::GmicGenericTiledProc>() );
}


