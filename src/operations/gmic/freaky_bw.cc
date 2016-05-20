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
#include "freaky_bw.hh"



PF::GmicFreaky_BWPar::GmicFreaky_BWPar(): 
PF::GmicUntiledOperationPar(),
  prop_strength("strength",this,90),
  prop_oddness("oddness",this,20),
  prop_brightness("brightness",this,0),
  prop_contrast("contrast",this,0.1)
{	
//  set_cache_files_num(1);
  set_type( "gmic_freaky_bw" );
}


VipsImage* PF::GmicFreaky_BWPar::build(std::vector<VipsImage*>& in, int first, 
                                        VipsImage* imap, VipsImage* omap, 
                                        unsigned int& level)
/*std::vector<VipsImage*> PF::GmicFreaky_BWPar::build_many(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)*/
{
/*  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];
  
  std::vector<VipsImage*> outvec;
  
//  if( !srcimg ) return NULL;
  if( !srcimg ) return outvec;

//  std::vector<VipsImage*> in2;
//  in2.clear(); in2.push_back( in[0] );
//  VipsImage* out = GmicUntiledOperationPar::build( in2, 0, imap, omap, level );
  
  update_raster_images();
  PF::RasterImage* raster_image = get_raster_image(0);
  //if( !raster_image || (raster_image->get_file_name () != get_cache_file_name()) ) {
  if( !raster_image ) {
    std::string tempfile = save_image( srcimg, IM_BANDFMT_FLOAT );

//    std::string command = "-verbose - -input ";
    std::string command = "-verbose + ";
    command = command + "-input " + tempfile + "  ";
//    command = command + tempfile + std::string(" -gimp_freaky_bw ");
    command = command + std::string(" -gimp_freaky_bw ");
    command = command + prop_strength.get_str();
    command = command + std::string(",") + prop_oddness.get_str();
    command = command + std::string(",") + prop_brightness.get_str();
//    command = command + std::string(",") + prop_contrast.get_str() + " -n 0,1 -output " + get_cache_file_name(0) + ",float"; 
    command = command + std::string(",") + prop_contrast.get_str() + " "; 
    command = command + "  -output " + get_cache_file_name(0) + ",float";
    std::cout<<"freaky bw command: "<<command<<std::endl;

    run_gmic( srcimg, command );

    unlink( tempfile.c_str() );
  }
  
  outvec = get_output( level );
  
  for(std::size_t i = 0; i < outvec.size(); i++ )
    PF::vips_copy_metadata( srcimg, outvec[i] );

//  VipsImage* out = (outvec.size()>0) ? outvec[0] : NULL;

//  return out;
    return outvec;
*/    
    
    VipsImage* srcimg = NULL;
    if( in.size() > 0 ) srcimg = in[0];
    
//    std::vector<VipsImage*> outvec;
    
  //  if( !srcimg ) return NULL;
    if( !srcimg ) return NULL;

    std::string command = std::string(" -gimp_freaky_bw ");
        command = command + prop_strength.get_str();
        command = command + std::string(",") + prop_oddness.get_str();
        command = command + std::string(",") + prop_brightness.get_str();
        command = command + std::string(",") + prop_contrast.get_str() + " "; 
        std::cout<<"freaky bw command: "<<command<<std::endl;

    VipsImage* out = NULL;

      run_gmic2( srcimg, &out, command );
      
//      PF::vips_copy_metadata( srcimg, out );
     
//      outvec.push_back(out);


    return out;
    
}


PF::ProcessorBase* PF::new_gmic_freaky_bw()
{
  return( new PF::Processor<PF::GmicFreaky_BWPar,PF::GmicFreaky_BWProc>() );
}
