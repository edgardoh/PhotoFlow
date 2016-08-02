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


#include "gmic.hh"
#include "gmic_generic_untiled2.hh"



PF::GmicGenericUntiled2Par::GmicGenericUntiled2Par(): 
PF::GmicUntiledOperationPar(),
gmic_filter_prop("gmic_filter_prop", this)/*,
  iterations("iterations",this,1),
  prop_interations("smooth_interations",this,1),
  prop_equalize("equalize",this,0),
  prop_merging_option("merging_option", this, 1, "alpha", "alpha"),
  prop_opacity("opacity",this,0.8),
  prop_reverse("reverse",this,0),
  prop_smoothness("smoothness",this,0.8),
  padding("padding",this,0)*/
{	
/*  prop_merging_option.add_enum_value( 0, "add", "add" );
  prop_merging_option.add_enum_value( 2, "and", "and" );
  prop_merging_option.add_enum_value( 3, "average", "average" );
  prop_merging_option.add_enum_value( 4, "blue", "blue" );
  prop_merging_option.add_enum_value( 5, "burn", "burn" );
  prop_merging_option.add_enum_value( 6, "darken", "darken" );
  prop_merging_option.add_enum_value( 7, "difference", "difference" );
  prop_merging_option.add_enum_value( 8, "divide", "divide" );
  prop_merging_option.add_enum_value( 9, "dodge", "dodge" );
  prop_merging_option.add_enum_value( 10, "exclusion", "exclusion" );
  prop_merging_option.add_enum_value( 11, "freeze", "freeze" );
  prop_merging_option.add_enum_value( 12, "grainextract", "grainextract" );
  prop_merging_option.add_enum_value( 13, "grainmerge", "grainmerge" );
  prop_merging_option.add_enum_value( 14, "green", "green" );
  prop_merging_option.add_enum_value( 15, "hardlight", "hardlight" );
  prop_merging_option.add_enum_value( 16, "hardmix", "hardmix" );
  prop_merging_option.add_enum_value( 17, "hue", "hue" );
  prop_merging_option.add_enum_value( 18, "interpolation", "interpolation" );
  prop_merging_option.add_enum_value( 19, "lighten", "lighten" );
  prop_merging_option.add_enum_value( 20, "lightness", "lightness" );
  prop_merging_option.add_enum_value( 21, "linearburn", "linearburn" );
  prop_merging_option.add_enum_value( 22, "linearlight", "linearlight" );
  prop_merging_option.add_enum_value( 23, "luminance", "luminance" );
  prop_merging_option.add_enum_value( 24, "multiply", "multiply" );
  prop_merging_option.add_enum_value( 25, "negation", "negation" );
  prop_merging_option.add_enum_value( 26, "or", "or" );
  prop_merging_option.add_enum_value( 27, "overlay", "overlay" );
  prop_merging_option.add_enum_value( 28, "pinlight", "pinlight" );
  prop_merging_option.add_enum_value( 29, "red", "red" );
  prop_merging_option.add_enum_value( 30, "reflect", "reflect" );
  prop_merging_option.add_enum_value( 31, "saturation", "saturation" );
  prop_merging_option.add_enum_value( 32, "screen", "screen" );
  prop_merging_option.add_enum_value( 33, "shapeaverage", "shapeaverage" );
  prop_merging_option.add_enum_value( 34, "softburn", "softburn" );
  prop_merging_option.add_enum_value( 35, "softdodge", "softdodge" );
  prop_merging_option.add_enum_value( 36, "softlight", "softlight" );
  prop_merging_option.add_enum_value( 37, "stamp", "stamp" );
  prop_merging_option.add_enum_value( 38, "subtract", "subtract" );
  prop_merging_option.add_enum_value( 39, "value", "value" );
  prop_merging_option.add_enum_value( 40, "vividlight", "vividlight" );
  prop_merging_option.add_enum_value( 41, "xor", "xor" );
  prop_merging_option.add_enum_value( 42, "edges", "edges" );
*/
  set_cache_files_num(1);
  set_type( "gmic_generic_untiled2" );
  set_default_name( _("GMIC Generic Untiled") );
}

std::string PF::GmicGenericUntiled2Par::build_command()
{
  return gmic_filter_prop.get().get_gmic_command(get_verbosity_mode());
}


void PF::GmicGenericUntiled2Par::set_gmic_filter(GmicFilter* gmf)
{
  if ( gmf != NULL ) {
    GmicFilter& gmf_p = gmic_filter_prop.get();
    
    if ( gmf_p.get_properties_count() == 0 ) {
      gmf_p = *gmf;
      gmf_p.create_properties();
      
      set_default_name( gmf_p.get_filter_name() );
      
    }
  }
}

std::vector<VipsImage*> PF::GmicGenericUntiled2Par::build_many(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];
  std::vector<VipsImage*> outvec;
  if( !srcimg ) return outvec;

  if( get_render_mode() == PF_RENDER_PREVIEW ) {
    PF_REF( srcimg, "GmicGenericUntiled2Par::build_many(): srcimg ref in preview render mode" );
    outvec.push_back( srcimg );
    return outvec;
  }

/*  if( prop_text.get().empty() ) {
    PF_REF( srcimg, "GmicGenericUntiled2Par::build_many(): srcimg ref with empty watermark" );
    outvec.push_back( srcimg );
    return outvec;
  }
*/
  update_raster_images();
  PF::RasterImage* raster_image = get_raster_image(0);
  //if( !raster_image || (raster_image->get_file_name () != get_cache_file_name()) ) {
  if( !raster_image ) {
    std::string tempfile1 = save_image( srcimg, IM_BANDFMT_FLOAT );

/*    std::string prop_text = "Water";
    std::string prop_text_size = "5";
    
    std::string command = "-verbose + ";
    command = command + "-input " + tempfile1 + " -mul 255 ";
    command = command + "-watermark_fourier \"" + prop_text + "\"," + prop_text_size;
    command = command + " -c 0,255 -div 255 -output " + get_cache_file_name(0) + ",float";
    */
    std::string command /*= "-verbose + "*/;
    command = command + "-input " + tempfile1 + " "; //" -mul 255 ";
    command = command + build_command();
    command = command + /*" -c 0,255 -div 255*/ " - -output " + get_cache_file_name(0) + ",float";

    std::cout<<"command: "<<command<<std::endl;
    
    try {
    run_gmic( srcimg, command );
    }
    catch( gmic_exception e ) { 
//      images.assign( (guint) 0 );

      std::cout<<"G'MIC error: "<<e.what()<<std::endl;
      
      //std::cout<<"G'MIC command: \""<<vipsgmic->command<<"\"  seq="<<seq<<"  gmic_instance="<<seq->gmic_instance<<" failed!"<<std::endl;
      //g_mutex_lock( gmic_mutex );
      //delete seq->gmic_instance;
      //g_mutex_unlock( gmic_mutex );
//      vips_error( "VipsGMic", "%s", e.what() );

//      return( -1 );
    }

    //unlink( tempfile1.c_str() );
  }
  std::cout<<"GmicGenericUntiled2Par::build(): calling get_output()"<<std::endl;
  outvec = get_output( level );

  for(std::size_t i = 0; i < outvec.size(); i++ )
    PF::vips_copy_metadata( srcimg, outvec[i] );

  return outvec;
}

#if 0
VipsImage* PF::GmicGenericUntiled2Par::build(std::vector<VipsImage*>& in, int first, 
                                        VipsImage* imap, VipsImage* omap, 
                                        unsigned int& level)
{
  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];

  if( !srcimg ) return NULL;

  update_raster_images();
  PF::RasterImage* raster_image = get_raster_image(0);
  //if( !raster_image || (raster_image->get_file_name () != get_cache_file_name()) ) {
  if( !raster_image ) {
    std::string tempfile = save_image( srcimg, IM_BANDFMT_FLOAT );

/*    std::string command = "-verbose - -input ";
    command = command + tempfile + std::string(" -n 0,255 -gimp_dreamsmooth ");
    command = command + prop_interations.get_str();
    command = command + std::string(",") + prop_equalize.get_str();
    command = command + std::string(",") + prop_merging_option.get_enum_value_str();
    command = command + std::string(",") + prop_opacity.get_str();
    command = command + std::string(",") + prop_reverse.get_str();
    command = command + std::string(",") + prop_smoothness.get_str() + ",1,0 -n 0,1 -output " + get_cache_file_name(0) + ",float,lzw";
    std::cout<<"dream smooth command: "<<command<<std::endl;
*/
    std::string command = " -input ";
    command = command + tempfile + std::string(" ");
    command = command + build_command();
    command = command + " -n 0,1 -output " + get_cache_file_name(0) + ",float,lzw";

    run_gmic( srcimg, command );

    unlink( tempfile.c_str() );
  }
  
  std::vector<VipsImage*> outvec = get_output( level );
  VipsImage* out = (outvec.size()>0) ? outvec[0] : NULL;

  return out;
}
#endif

PF::ProcessorBase* PF::new_gmic_generic_untiled2()
{
  return( new PF::Processor<PF::GmicGenericUntiled2Par,PF::GmicGenericUntiled2Proc>() );
}
