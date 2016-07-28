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
gmic_filter_prop("gmic_filter_prop", this),
prop_name("prop_name", this, ""), 
prop_command("prop_command", this, ""), 
prop_arguments("prop_arguments", this, ""),
prop_phf_arguments("prop_phf_arguments", this, ""),
prop_arr_string("prop_arr_string", this)
{ 
  gmic = PF::new_gmic();
//  gmic_generic_algo = new PF::Processor<PF::GmicGenericTiledAlgoPar,PF::GmicGenericTiledAlgoProc>();
  
  m_padding = 0;
  
  set_type( "gmic_generic_tiled" );
  set_default_name( _("GMIC Generic") );
  
}
/*
void PF::GmicGenericTiledPar::post_init()
{
	OpParBase::post_init();
	
//	set_prop_name(columns[GMicLoad::col_name]);
//	set_prop_command(columns[GMicLoad::col_command]);
//	set_prop_arguments(columns[GMicLoad::col_arguments_gmic]);
	
//  set_default_name( get_pf_filter()->get_name() );

//	create_properties();
	
}
*/
std::string PF::GmicGenericTiledPar::build_command()
{
  std::cout<<"PF::GmicGenericTiledPar::build_command()"<<std::endl;
  
//  std::list<FilterField> field_list = get_pf_filter()->get_fields();
//  std::cout<<"PF::GmicGenericTiledPar::build_command() 2"<<std::endl;
//  std::string gmic_command = static_cast<GmicFilter&>(get_pf_filter()).get_command();
//  GmicFilter* gmic_filter = dynamic_cast<GmicFilter*>( get_pf_filter() );
//  std::cout<<"PF::GmicGenericTiledPar::build_command() 3"<<std::endl;
//  std::string gmic_command = gmic_filter->get_command();
//  std::cout<<"PF::GmicGenericTiledPar::build_command(): "<<gmic_command<<std::endl;
  
  GmicFilter1& gmic_prop = gmic_filter_prop.get();
  std::list<GmicProperty>& field_list = gmic_prop.get_prop_list();
  
/*  std::cout<<"PF::GmicGenericTiledPar::build_command() get_prop_name(): "<<get_prop_name()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() get_prop_command(): "<<get_prop_command()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() get_prop_arguments(): "<<get_prop_arguments()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() get_prop_phf_arguments(): "<<get_prop_phf_arguments()<<std::endl;
  for ( int i = 0; i < get_prop_arr_string().size(); i++ ) {
    std::cout<<"PF::GmicGenericTiledPar::build_command() get_prop_arr_string(): "<<get_prop_arr_string()[i].first<<std::endl;
    std::cout<<"PF::GmicGenericTiledPar::build_command() get_prop_arr_string(): "<<get_prop_arr_string()[i].second<<std::endl;
  }*/
  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_prop.get_is_folder(): "<<gmic_prop.get_is_folder()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_prop.get_entry_level(): "<<gmic_prop.get_entry_level()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_prop.get_filter_name(): "<<gmic_prop.get_filter_name()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_prop.get_filter_type(): "<<gmic_prop.get_filter_type()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_prop.get_filter_command(): "<<gmic_prop.get_filter_command()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_prop.get_filter_arguments(): "<<gmic_prop.get_filter_arguments()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_prop.get_filter_phf_arguments(): "<<gmic_prop.get_filter_phf_arguments()<<std::endl;
  std::cout<<"PF::GmicGenericTiledPar::build_command() field_list.size(): "<<field_list.size()<<std::endl;

  std::string gmic_command = gmic_prop.get_filter_command();
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
    for (std::list<GmicProperty>::iterator it=field_list.begin(); it != field_list.end(); ++it) {
      if ( it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_float || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_int || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_bool || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_choise || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_text_multi || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_text_single || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_file || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_folder || 
          it->get_prop_type() == GmicArgument::GmicArgumentType::arg_type_color ) {
/*        PropertyBase* prop = get_property(it->field_name);
        if (prop) {
          std::string _ss;
          if (it->get_arg_type() == GmicArgument::GmicArgumentType::arg_type_choise)
            _ss = prop->get_enum_value_str();
          else
            _ss = prop->get_str();
  
          const unsigned int l = (unsigned int)_ss.length();
          for (unsigned int i = 1; i<l - 1; ++i) { const char c = _ss[i]; _ss[i] = c=='\"'?gmic_dquote:c; }
  
          lres += _ss + ',';
        }
*/
        std::string _ss;
        it->get_str_value(_ss);
        
        const unsigned int l = (unsigned int)_ss.length();
        for (unsigned int i = 1; i<l - 1; ++i) {
          const char c = _ss[i];
          _ss[i] = c=='\"'?gmic_dquote:c;
          
          std::cout<<"PF::GmicGenericTiledPar::build_command() _ss: "<<_ss<<std::endl;
        }

        lres += _ss + ',';
      }
      else {
        std::cout<<"PF::GmicGenericTiledPar::build_command() it->get_prop_type(): "<<it->get_prop_type()<<std::endl;
      }
    }
    if (lres.back() == ',') lres.back() = 0;
  }

  gmic_command = lres;

  std::cout<<"PF::GmicGenericTiledPar::build_command() gmic_command: "<<gmic_command<<std::endl;
  
  return gmic_command;
}

VipsImage* PF::GmicGenericTiledPar::build(std::vector<VipsImage*>& in, int first, 
                                        VipsImage* imap, VipsImage* omap, 
                                        unsigned int& level)
{
  std::cout<<"PF::GmicGenericTiledPar::build()"<<std::endl;
  
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

void PF::GmicGenericTiledPar::set_gmic_filter(GmicFilter1* gmf)
{
  std::cout<<"PF::GmicGenericTiledPar::set_gmic_filter()"<<std::endl;
  
  if ( gmf != NULL ) {
    GmicFilter1& gmf_p = gmic_filter_prop.get();
    
    std::cout<<"PF::GmicGenericTiledPar::set_gmic_filter() gmf_p.get_poperties_count(): "<<gmf_p.get_poperties_count()<<std::endl;
    if ( gmf_p.get_poperties_count() == 0 ) {
      gmf_p = *gmf;
      gmf_p.create_properties();
      
      set_default_name( gmf_p.get_filter_name() );
      
      set_prop_name( gmf_p.get_filter_name() );
      set_prop_command( gmf_p.get_filter_command() );
      set_prop_arguments( gmf_p.get_filter_arguments() );
      set_prop_phf_arguments( gmf_p.get_filter_phf_arguments() );
      
      std::vector< std::pair<int,std::string> > temp;
      
//      std::pair<int,std::string>(1, 
      temp.push_back( std::pair<int,std::string>(1, gmf_p.get_filter_name()) );
      temp.push_back( std::pair<int,std::string>(2, gmf_p.get_filter_command()) );
      temp.push_back( std::pair<int,std::string>(3, gmf_p.get_filter_arguments()) );
      temp.push_back( std::pair<int,std::string>(4, gmf_p.get_filter_phf_arguments()) );
      
      set_prop_arr_string(temp);
    }
  }
  
}

PF::ProcessorBase* PF::new_gmic_generic_tiled()
{
  return( new PF::Processor<PF::GmicGenericTiledPar,PF::GmicGenericTiledProc>() );
}
#if 0
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
#endif
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
