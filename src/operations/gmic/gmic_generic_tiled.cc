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
#include "gmic_generic_tiled.hh"
#include "../vips/gmic/gmic/src/gmic.h"
#include "../vips/gmic/gmic/src/gmic_stdlib.h"
#include "../vips/gmic/gmic/src/CImg.h"
#include "gmic_load.hh"
#include "../operations/draw.hh"

using namespace cimg_library;


PF::GmicGenericTiledPar::GmicGenericTiledPar(): 
OpParBase(), 
prop_name("prop_name", this, ""), 
prop_command("prop_command", this, ""), 
prop_arguments("prop_arguments", this, "")
{ 
  gmic = PF::new_gmic();
  gmic_generic_algo = new PF::Processor<PF::GmicGenericTiledAlgoPar,PF::GmicGenericTiledAlgoProc>();
  
  padding = 0;
  
  set_type( "gmic_generic_tiled" );
  set_default_name( _("GMIC Generic") );
  
}

void PF::GmicGenericTiledPar::post_init(std::vector<std::string>& columns)
{
	OpParBase::post_init(columns);
	
	set_prop_name(columns[0]);
	set_prop_command(columns[1]);
	set_prop_arguments(columns[2]);

	create_properties(columns);
	
}

VipsImage* PF::GmicGenericTiledPar::build(std::vector<VipsImage*>& in, int first, 
                                        VipsImage* imap, VipsImage* omap, 
                                        unsigned int& level)
{
  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];
  VipsImage* mask;
  VipsImage* out = srcimg;

  if( !out ) return NULL;
  
  if( !(gmic->get_par()) ) return NULL;
  PF::GMicPar* gpar = dynamic_cast<PF::GMicPar*>( gmic->get_par() );
  if( !gpar ) return NULL;

  GmicGenericTiledAlgoPar* ggen_par = dynamic_cast<GmicGenericTiledAlgoPar*>( gmic_generic_algo->get_par() );

  std::string command;
  
  GMicArgumentList arg_list;
  arg_list.parse_arguments(get_columns()[2]);
  command = arg_list.build_command(get_columns()[1], this);
  
  gpar->set_command( command );
  gpar->set_iterations( 1 );
  gpar->set_padding( get_padding( level ) );
  gpar->set_x_scale( 1.0f );
  gpar->set_y_scale( 1.0f );

  gpar->set_image_hints( srcimg );
  gpar->set_format( get_format() );

  out = gpar->build( in, first, imap, omap, level );
  if( !out ) {
    std::cout<<"gmic.build() failed!!!!!!!"<<std::endl;
  }

  return out;
}


PF::ProcessorBase* PF::new_gmic_generic_tiled()
{
  return( new PF::Processor<PF::GmicGenericTiledPar,PF::GmicGenericTiledProc>() );
}


void PF::GmicGenericTiledPar::create_properties(std::vector<std::string>& filter_arguments) {
	GMicArgumentList arg_list;
  GMicArgumentList phf_arg_list;
	PropertyBase* prop;
	
	arg_list.parse_arguments(filter_arguments[2]);
	std::list<GMicArgument>arg_l = arg_list.get_arg_list();
  phf_arg_list.parse_arguments(filter_arguments[3]);
  std::list<GMicArgument>phf_arg_l = phf_arg_list.get_arg_list();

  for (std::list<GMicArgument>::iterator it=phf_arg_l.begin(); it != phf_arg_l.end(); ++it) {
    if (it->arg_description == "phf_padding") padding = std::stoi(it->arg_value);
  }

	for (std::list<GMicArgument>::iterator it=arg_l.begin(); it != arg_l.end(); ++it) {
		prop = NULL;
		switch (it->arg_type)
		{
		case arg_type_float:
			prop = new Property<float>(it->arg_name, this, it->arg_default);
			break;
		case arg_type_int:
			prop = new Property<int>(it->arg_name, this, it->arg_default);
			break;
		case arg_type_bool:
			prop = new Property<bool>(it->arg_name, this, it->arg_default);
			break;
		case arg_type_button:
			break;
		case arg_type_choise:
		{
			int enum_val = 0;
			std::list<std::pair<std::string, Glib::ustring>>l_arg_l = it->arg_list;
			prop = new PropertyBase(it->arg_name, this );
			for (std::list<std::pair<std::string, Glib::ustring>>::iterator itl=l_arg_l.begin(); itl != l_arg_l.end(); ++itl) {
					prop->add_enum_value( enum_val++, (*itl).first, (*itl).second );
			}
			prop->set_enum_value((int) it->arg_default);
		}
			break;
		case arg_type_text_multi:
			prop = new Property<std::string>(it->arg_name, this, it->arg_description);
			break;
		case arg_type_text_single:
			prop = new Property<std::string>(it->arg_name, this, it->arg_description);
			break;
		case arg_type_file:
			break;
		case arg_type_folder:
			break;
		case arg_type_color:
			prop = new Property<RGBColor>(it->arg_name, this, RGBColor(1,1,1));
			break;
		case arg_type_const:
			prop = new Property<std::string>(it->arg_name, this, it->arg_value);
			break;
		case arg_type_note:
			break;
		case arg_type_link:
			break;
		case arg_type_separator:
			break;
		}
	}

}

