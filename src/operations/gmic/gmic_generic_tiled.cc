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
#include "../../base/gmic_filter.hh"
#include "../vips/gmic/gmic/src/gmic.h"
//#include "../vips/gmic/gmic/src/gmic_stdlib.h"
//#include "../vips/gmic/gmic/src/CImg.h"
//#include "gmic_load.hh"
#include "../operations/draw.hh"

//using namespace cimg_library;


PF::GmicGenericTiledPar::GmicGenericTiledPar(): 
OpParBase(), 
prop_name("prop_name", this, ""), 
prop_command("prop_command", this, ""), 
prop_arguments("prop_arguments", this, "")
{ 
  gmic = PF::new_gmic();
//  gmic_generic_algo = new PF::Processor<PF::GmicGenericTiledAlgoPar,PF::GmicGenericTiledAlgoProc>();
  
  padding = 0;
  
  set_type( "gmic_generic_tiled" );
  set_default_name( _("GMIC Generic") );
  
}

void PF::GmicGenericTiledPar::post_init()
{
	OpParBase::post_init();
	
//	set_prop_name(columns[GMicLoad::col_name]);
//	set_prop_command(columns[GMicLoad::col_command]);
//	set_prop_arguments(columns[GMicLoad::col_arguments_gmic]);
	
  set_default_name( get_pf_filter()->get_name() );

	create_properties();
	
}

std::string PF::GmicGenericTiledPar::build_command()
{
//  std::cout<<"PF::GmicGenericTiledPar::build_command()"<<std::endl;
  
  std::list<FilterField> field_list = get_pf_filter()->get_fields();
//  std::cout<<"PF::GmicGenericTiledPar::build_command() 2"<<std::endl;
//  std::string gmic_command = static_cast<GmicFilter&>(get_pf_filter()).get_command();
  GmicFilter* gmic_filter = dynamic_cast<GmicFilter*>( get_pf_filter() );
//  std::cout<<"PF::GmicGenericTiledPar::build_command() 3"<<std::endl;
  std::string gmic_command = gmic_filter->get_command();
//  std::cout<<"PF::GmicGenericTiledPar::build_command(): "<<gmic_command<<std::endl;
  
  int nbparams = field_list.size();
  std::string lres;
  switch (get_verbosity_mode()) {
  case 0: case 1: case 2: case 3: lres = "-v -99 -"; break;  // Quiet or Verbose.
  case 4: case 5 : lres = "-v 0 -"; break;                   // Very verbose.
  default: lres = "-debug -";                                // Debug.
  }

  lres += gmic_command;
  if (nbparams) {
    lres += ' ';
    for (std::list<FilterField>::iterator it=field_list.begin(); it != field_list.end(); ++it) {
      if ( it->field_type == FilterField::field_type_float || 
          it->field_type == FilterField::field_type_int || 
          it->field_type == FilterField::field_type_bool || 
          it->field_type == FilterField::field_type_choise || 
          it->field_type == FilterField::field_type_text_multi || 
          it->field_type == FilterField::field_type_text_single || 
          it->field_type == FilterField::field_type_file || 
          it->field_type == FilterField::field_type_folder || 
          it->field_type == FilterField::field_type_color ) {
        PropertyBase* prop = get_property(it->field_name);
        if (prop) {
          std::string _ss;
          if (it->field_type == FilterField::field_type_choise)
            _ss = prop->get_enum_value_str();
          else
            _ss = prop->get_str();
  
          const unsigned int l = (unsigned int)_ss.length();
          for (unsigned int i = 1; i<l - 1; ++i) { const char c = _ss[i]; _ss[i] = c=='\"'?gmic_dquote:c; }
  
          lres += _ss + ',';
        }
      }
    }
    if (lres.back() == ',') lres.back() = 0;
  }

  gmic_command = lres;

  return gmic_command;
}

VipsImage* PF::GmicGenericTiledPar::build(std::vector<VipsImage*>& in, int first, 
                                        VipsImage* imap, VipsImage* omap, 
                                        unsigned int& level)
{
  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];
//  VipsImage* mask;
  VipsImage* out = srcimg;

  if( !out ) return NULL;
  
  if( !(gmic->get_par()) ) return NULL;
  PF::GMicPar* gpar = dynamic_cast<PF::GMicPar*>( gmic->get_par() );
  if( !gpar ) return NULL;

//  GmicGenericTiledAlgoPar* ggen_par = dynamic_cast<GmicGenericTiledAlgoPar*>( gmic_generic_algo->get_par() );

  std::string command;
  
//  GMicArgumentList arg_list;
//  arg_list.parse_arguments(get_columns()[2]);
//  PFFilter ppf_filter = get_pf_filter();
//  GmicFilter *gm_filter = dynamic_cast<GmicFilter*>( &ppf_filter );
//  command = static_cast<GmicFilter&>(get_pf_filter()).build_command(this);
  command = build_command();
  
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

void PF::GmicGenericTiledPar::create_properties() 
{
  
//  GMicArgumentList arg_list;
//  GMicArgumentList phf_arg_list;
  PropertyBase* prop;
  std::list<FilterField>field_list = get_pf_filter()->get_fields();
  
//  std::cout<<"PF::OpParBase::create_properties() "<<get_pf_filter().get_name()<<", "<<get_pf_filter().get_type()<<std::endl;
  
/*  arg_list.parse_arguments(filter_arguments[2]);
  std::list<GMicArgument>arg_l = arg_list.get_arg_list();
  phf_arg_list.parse_arguments(filter_arguments[3]);
  std::list<GMicArgument>phf_arg_l = phf_arg_list.get_arg_list();
*/
/*  for (std::list<GMicArgument>::iterator it=phf_arg_l.begin(); it != phf_arg_l.end(); ++it) {
    if (it->field_description == "phf_padding") padding = std::stoi(it->str_value);
  }
*/
  for (std::list<FilterField>::iterator it=field_list.begin(); it != field_list.end(); ++it) {
    
    std::cout<<"PF::GmicGenericTiledPar::create_properties()"<<", "<<it->field_name<<", "<<it->field_description<<std::endl;
    // Only GMIC arguments are properties
    if (it->field_name.compare(0, 3, "arg") == 0) {
      prop = NULL;
      switch (it->field_type)
      {
      case FilterField::field_type_float:
        prop = new Property<float>(it->field_name, this, it->val_default);
        break;
      case FilterField::field_type_int:
        prop = new Property<int>(it->field_name, this, it->val_default);
        break;
      case FilterField::field_type_bool:
        prop = new Property<bool>(it->field_name, this, it->val_default);
        break;
      case FilterField::field_type_button:
        break;
      case FilterField::field_type_choise:
      {
        int enum_val = 0;
        std::list<std::pair<std::string, Glib::ustring>>l_arg_l = it->arg_list;
        prop = new PropertyBase(it->field_name, this );
        for (std::list<std::pair<std::string, Glib::ustring>>::iterator itl=l_arg_l.begin(); itl != l_arg_l.end(); ++itl) {
            prop->add_enum_value( enum_val++, (*itl).first, (*itl).second );
        }
        prop->set_enum_value((int) it->val_default);
      }
        break;
      case FilterField::field_type_text_multi:
        prop = new Property<std::string>(it->field_name, this, it->field_description);
        break;
      case FilterField::field_type_text_single:
        prop = new Property<std::string>(it->field_name, this, it->field_description);
        break;
      case FilterField::field_type_file:
        break;
      case FilterField::field_type_folder:
        break;
      case FilterField::field_type_color:
        prop = new Property<RGBColor>(it->field_name, this, RGBColor(it->val_default, it->val_min, it->val_max));
        break;
      case FilterField::field_type_const:
        prop = new Property<std::string>(it->field_name, this, it->str_value);
        break;
      case FilterField::field_type_note:
        break;
      case FilterField::field_type_link:
        break;
      case FilterField::field_type_separator:
        break;
      }
    } else if (it->field_name == "phf_padding") {
      set_padding(std::atoi(it->str_value.c_str()));
    }
  }

}

#if 0
void PF::GmicGenericTiledPar::create_properties(std::vector<std::string>& filter_arguments) {
	GMicArgumentList arg_list;
  GMicArgumentList phf_arg_list;
	PropertyBase* prop;
	
	arg_list.parse_arguments(filter_arguments[GMicLoad::col_arguments_gmic]);
	std::list<GMicArgument>arg_l = arg_list.get_arg_list();
  phf_arg_list.parse_arguments(filter_arguments[GMicLoad::col_arguments_phf]);
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
#endif
