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

#include "gmic_generic_config.hh"
#include "../../../operations/gmic/gmic_generic_tiled.hh"
#include "../../widgets/colorbtn.hh"


PF::GmicGenericConfigGUI::GmicGenericConfigGUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "G'MIC Generic" ),
  m_controls_created(false)
{
  add_widget( controlsBox );
  
  get_main_box().show_all_children();
}

PF::GmicFilter& PF::GmicGenericConfigGUI::get_gmic_filter()
{
  PF::GmicGenericTiledPar* par = dynamic_cast<PF::GmicGenericTiledPar*>(get_par());
  return par->get_gmic_filer_prop();
}

bool PF::GmicGenericConfigGUI::value_changed_widg_internal(PF::PFWidget* widg, GmicFilter& gmic_filter)
{
  bool modified = false;
  
  GmicProperty prop_value;

  if ( gmic_filter.get_property( widg->get_prop_name(), prop_value) ) {
    if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_int) {
      Slider *sl = dynamic_cast<Slider*>(widg);
      if ( prop_value.get_val_int() != (int)sl->get_adj_value() ) {
        prop_value.set_val_int( (int)sl->get_adj_value() );
        modified = true;
      }
    }
    else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_float) {
      Slider *sl = dynamic_cast<Slider*>(widg);
      if ( prop_value.get_val_float() != (float)sl->get_adj_value() ) {
        prop_value.set_val_float( (float)sl->get_adj_value() );
        modified = true;
      }
    }
    else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_bool) {
      CheckBox *chk = dynamic_cast<CheckBox*>(widg);
      if ( prop_value.get_val_int() != (int)chk->get_active() ) {
        prop_value.set_val_int( (int)chk->get_active() );
        modified = true;
      }
    }
    else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_choise) {
      Selector *sel = dynamic_cast<Selector*>(widg);
      if ( prop_value.get_val_int() != (int)sel->get_active_row() ) {
        prop_value.set_val_int( (int)sel->get_active_row() );
        modified = true;
      }
    }
    else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_text_multi || 
        prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_text_single) {
      TextBox *txt = dynamic_cast<TextBox*>(widg);
      std::string str;
      txt->get_value( str );
      if ( str != prop_value.get_val_str() ) {
        prop_value.set_val_str( str );
        modified = true;
      }
    }
    else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_color) {
      ColorBtn *btn = dynamic_cast<ColorBtn*>(widg);
      PF::GmicColor color;
      btn->get_color( color );
      if ( color != prop_value.get_val_color() ) {
        prop_value.set_val_color( color );
        modified = true;
      }
    }
    else {
      std::cout<<"PF::GmicGenericConfigGUI::value_changed_widg() invalid property type: "<<prop_value.get_prop_type()<<std::endl;
    }

  }

  if ( modified ) {
    gmic_filter.set_property( prop_value );
  }
  
  return modified;
}

bool PF::GmicGenericConfigGUI::value_reset_widg_internal(PF::PFWidget* widg, GmicFilter& gmic_filter)
{
  bool modified = false;
  GmicProperty prop_value;

  std::cout<<"PF::GmicGenericConfigGUI::value_reset_widg_internal() called "<<std::endl;
  
  gmic_filter.parse_arguments();

  GmicArgument arg_value;
  if ( gmic_filter.get_argument( widg->get_prop_name(), arg_value ) ) {

    if ( gmic_filter.get_property( widg->get_prop_name(), prop_value) ) {
      if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_int) {
        Slider *sl = dynamic_cast<Slider*>(widg);
        prop_value.set_val_int( (int)arg_value.get_val_default() );
        std::cout<<"PF::GmicGenericConfigGUI::value_reset_widg_internal() prop_value.get_val_int(): "<<prop_value.get_val_int()<<std::endl;
        sl->set_adj_value( (double)prop_value.get_val_int() );
        modified = true;
      }
      else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_float) {
        Slider *sl = dynamic_cast<Slider*>(widg);
        prop_value.set_val_float( (float)arg_value.get_val_default() );
        std::cout<<"PF::GmicGenericConfigGUI::value_reset_widg_internal() prop_value.get_val_float(): "<<prop_value.get_val_float()<<std::endl;
        sl->set_adj_value( (double)prop_value.get_val_float() );
        modified = true;
      }
      else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_bool) {
        CheckBox *chk = dynamic_cast<CheckBox*>(widg);
        prop_value.set_val_int( (int)arg_value.get_val_default() );
        chk->set_active( (bool)prop_value.get_val_int() );
        modified = true;
      }
      else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_choise) {
        Selector *sel = dynamic_cast<Selector*>(widg);
        prop_value.set_val_int( (int)arg_value.get_val_default() );
        sel->set_active_row( (int)prop_value.get_val_int() );
        modified = true;
      }
      else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_text_multi || 
          prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_text_single) {
        TextBox *txt = dynamic_cast<TextBox*>(widg);
        prop_value.set_val_str( arg_value.get_val_str() );
        txt->set_value( prop_value.get_val_str() );
        modified = true;
      }
      else if ( prop_value.get_prop_type() == PF::GmicArgument::GmicArgumentType::arg_type_color) {
        ColorBtn *btn = dynamic_cast<ColorBtn*>(widg);
        prop_value.set_val_color( arg_value.get_val_color() );
        btn->set_color( prop_value.get_val_color() );
        modified = true;
      }
      else {
        std::cout<<"PF::GmicGenericConfigGUI::value_reset_widg_internal() invalid property type: "<<prop_value.get_prop_type()<<std::endl;
      }
    }
    else {
      std::cout<<"PF::GmicGenericConfigGUI::value_reset_widg_internal() property not found: "<<widg->get_prop_name()<<std::endl;
    }
  }
  else {
    std::cout<<"PF::GmicGenericConfigGUI::value_reset_widg_internal() argument not found: "<<widg->get_prop_name()<<std::endl;
  }

  if ( modified ) {
    gmic_filter.set_property( prop_value );
  }
  
  return modified;
}

#define GMIC_EVENT_BEGIN() \
/* Retrieve the layer associated to the filter */ \
PF::Layer* layer = get_layer(); \
if( !layer ) return; \
\
/* Retrieve the image the layer belongs to */ \
PF::Image* image = layer->get_image(); \
if( !image ) return; \

void PF::GmicGenericConfigGUI::value_changed_widg(PF::PFWidget* widg)
{
  if (widg) {

    GMIC_EVENT_BEGIN()

    bool modified = false;
    GmicFilter& gmic_filter = get_gmic_filter();

    modified = value_changed_widg_internal(widg, gmic_filter);
    
    if ( modified && get_par() ) {
      get_par()->modified();
      image->update();
    }
  }
  
}

void PF::GmicGenericConfigGUI::value_reset_widg(PF::PFWidget* widg)
{
  if (widg) {

    GMIC_EVENT_BEGIN()

    bool modified = false;
    GmicFilter& gmic_filter = get_gmic_filter();

    modified = value_reset_widg_internal(widg, gmic_filter);
    
    if ( modified && get_par() ) {
      get_par()->modified();
      image->update();
    }
  }
  
}

void PF::GmicGenericConfigGUI::create_controls_internal(GmicFilter& gmf)
{
  GmicProperty prop_value;
  
  gmf.parse_arguments();
  
  if ( gmf.get_arg_list_count() == 0 ) return;
  
  m_controls_created = true;
  
  
  Slider *sl;
  CheckBox *chb;
  Gtk::Button *btn;
  Selector *sel;
  TextBox *txt;
  Gtk::Label *lbl;
  Gtk::HSeparator *sep;
  Gtk::LinkButton *linkb;
  ColorBtn *clrbtn;
  
  for ( int i = 0; i < gmf.get_arg_list_count(); i++ ) {
    GmicArgument& arg = gmf.get_argument(i);
    
    bool prop_found = gmf.get_property(arg.get_arg_name(), prop_value);
    
    switch (arg.get_arg_type())
    {
    case PF::GmicArgument::GmicArgumentType::arg_type_float:
      if (!prop_found) prop_value.set_val_float( arg.get_val_default() );
            sl = new Slider( this, arg.get_arg_name(), arg.get_arg_description(), 
                (double)prop_value.get_val_float(), (double)arg.get_val_min(), (double)arg.get_val_max(), 
                (double)arg.val_step1, (double)arg.val_step2, 1);
            sl->value_changed_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_changed_widg) );
            sl->value_reset_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_reset_widg) );
            controlsBox.pack_start( *sl );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_int:
      if (!prop_found) prop_value.set_val_int( arg.get_val_default() );
            sl = new Slider( this, arg.get_arg_name(), arg.get_arg_description(), 
                (double)prop_value.get_val_int(), (double)arg.get_val_min(), (double)arg.get_val_max(), 
                (double)arg.val_step1, (double)arg.val_step2, 1);
            sl->value_changed_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_changed_widg) );
            sl->value_reset_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_reset_widg) );
            controlsBox.pack_start( *sl );
            break;
    case PF::GmicArgument::GmicArgumentType::arg_type_bool:
      if (!prop_found) prop_value.set_val_int( arg.get_val_default() );
      chb = new CheckBox( this, arg.get_arg_name(), arg.get_arg_description(), prop_value.get_val_int() );
      chb->value_changed_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_changed_widg) );
      chb->value_reset_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_reset_widg) );
      controlsBox.pack_start( *chb );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_button:
      btn = new Gtk::Button(arg.get_arg_description());
      controlsBox.pack_start( *btn );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_choise:
      if ( arg.get_arg_description() != "Preview type" ) {
        sel = new Selector( this, arg.get_arg_name(), arg.get_arg_description(), (int)arg.get_val_default() );
        {
          int enum_val = 0;
          int def_val = -1;
          std::map< int, std::pair<std::string,std::string> > values;
          std::pair< int, std::pair<std::string,std::string> > active;

          std::string id_str;
          std::string desc_str;

          if (!prop_found) prop_value.set_val_int( arg.get_val_default() );
          active.first = -1;

          for ( int j = 0; j < arg.get_arg_list_count(); j++ ) {
            arg.arg_list_get(j, id_str, desc_str);
            if ( prop_value.get_val_int() == enum_val ) {
              active = make_pair( prop_value.get_val_int(), make_pair( id_str, desc_str ) );
            }
            values.insert( make_pair( enum_val++, make_pair( id_str, desc_str ) ) );
          }
          sel->add_rows(values, active);
        }
        sel->value_changed_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_changed_widg) );
        sel->value_reset_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_reset_widg) );
        controlsBox.pack_start( *sel );
      }
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_text_multi:
      if (!prop_found) prop_value.set_val_str( arg.get_val_str() );
      txt = new TextBox( this, arg.get_arg_name(), arg.get_arg_description(), prop_value.get_val_str() );
      txt->value_changed_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_changed_widg) );
      txt->value_reset_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_reset_widg) );
      controlsBox.pack_start( *txt );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_text_single:
      if (!prop_found) prop_value.set_val_str( arg.get_val_str() );
      
      txt = new TextBox( this, arg.get_arg_name(), arg.get_arg_description(), prop_value.get_val_str() );
      txt->value_changed_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_changed_widg) );
      txt->value_reset_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_reset_widg) );
      controlsBox.pack_start( *txt );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_file:
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_folder:
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_color:
      if (!prop_found) prop_value.set_val_color( arg.get_val_color() );
        
      clrbtn = new ColorBtn(this, arg.get_arg_name(), arg.get_arg_description(), prop_value.get_val_color());
      clrbtn->value_changed_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_changed_widg) );
      clrbtn->value_reset_widg.connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::value_reset_widg) );
      controlsBox.pack_start( *clrbtn );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_const:
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_note:
      lbl = new Gtk::Label();
      lbl->set_use_markup(true);
      lbl->set_label( arg.get_val_str() );
      lbl->set_line_wrap(true);
      lbl->set_halign(Gtk::Align::ALIGN_START);
      controlsBox.pack_start( *lbl, Gtk::PACK_SHRINK );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_link:
      linkb = new Gtk::LinkButton();
      linkb->set_label( arg.get_arg_description() );
      linkb->set_uri( arg.get_val_str() );
      linkb->set_halign(Gtk::Align::ALIGN_START);
      controlsBox.pack_start( *linkb );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_separator:
      sep = new Gtk::HSeparator();
      controlsBox.pack_start( *sep );
      break;
    default:
      std::cout<<"PF::GmicGenericConfigGUI::create_controls() arg.get_arg_type(): "<<arg.get_arg_type()<<std::endl;
      break;
    }
  }
  
  get_main_box().show_all_children();

}

void PF::GmicGenericConfigGUI::create_controls()
{
  if (m_controls_created) return;
  
  GmicFilter& gmf = get_gmic_filter();
  
  create_controls_internal(gmf);
}

void PF::GmicGenericConfigGUI::do_update()
{
  create_controls();
  
  PF::OperationConfigGUI::do_update();
}

void PF::GmicGenericConfigGUI::parameters_reset()
{
  GmicFilter& gmf = get_gmic_filter();
  
  gmf.reset_values();
  
  OperationConfigGUI::parameters_reset();
}


