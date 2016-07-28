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
//#include "../operations/gmic/gmic_load.hh"
#include "../../../operations/gmic/gmic_generic_tiled.hh"
#include "../../widgets/colorbtn.hh"


PF::GmicGenericConfigGUI::GmicGenericConfigGUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "G'MIC Generic" ),
  m_controls_created(false) /*,
  updateButton( "Update" )*/
{
//  controlsBox.pack_start( updateButton );

//  updateButton.signal_clicked().connect( sigc::mem_fun(this, &GmicGenericConfigGUI::on_update) );
  
  add_widget( controlsBox );
  
  get_main_box().show_all_children();
}
/*
void PF::GmicGenericConfigGUI::post_init()
{
	OperationConfigGUI::post_init();
	
	create_controls();
	
}
*/
void PF::GmicGenericConfigGUI::sl_changed()
{
  std::cout<<"PF::GmicGenericConfigGUI::sl_changed()"<<std::endl;
}

void PF::GmicGenericConfigGUI::create_controls()
{
  std::cout<<"PF::GmicGenericConfigGUI::create_controls() m_controls_created: "<<m_controls_created<<std::endl;
  
  if (m_controls_created) return;
  
  PF::GmicGenericTiledPar* par = dynamic_cast<PF::GmicGenericTiledPar*>(get_par());
  if( !par ) return;
  
  GmicFilter1& gmf = par->get_gmic_filer_prop();
  std::list<GmicArgument> field_list;
  std::string arguments = gmf.get_filter_arguments();
  
  std::cout<<"PF::GmicGenericConfigGUI::create_controls() arguments: "<<arguments<<std::endl;
  
  PF::GmicFilter1::parse_arguments(arguments, field_list);

  std::cout<<"PF::GmicGenericConfigGUI::create_controls() field_list.size(): "<<field_list.size()<<std::endl;

  if ( field_list.size() == 0 ) return;
  
  m_controls_created = true;
  

//  std::cout<<"PF::GmicGenericConfigGUI::post_init() "<<get_pf_filter().get_name()<<", "<<get_pf_filter().get_type()<<std::endl;
//  GMicArgumentList arg_list;
	
//  arg_list.parse_arguments(columns[GMicLoad::col_arguments_gmic]);
//  std::list<GMicArgument>arg_l = arg_list.get_arg_list();
//  std::list<GmicArgument>field_list = gmf.get_prop_list();

	Slider *sl;
	CheckBox *chb;
	Gtk::Button *btn;
	Selector *sel;
	TextBox *txt;
	Gtk::Label *lbl;
	Gtk::HSeparator *sep;
	Gtk::LinkButton *linkb;
	ColorBtn *clrbtn;
	
	for (std::list<GmicArgument>::iterator it=field_list.begin(); it != field_list.end(); ++it) {
	  std::cout<<"PF::GmicGenericConfigGUI::create_controls()"<<"it->get_arg_type(): "<<it->get_arg_type()<<", "<<it->get_arg_name()<<", "<<it->get_arg_description()<<std::endl;
	  
		switch (it->get_arg_type())
		{
		case PF::GmicArgument::GmicArgumentType::arg_type_float:
            sl = new Slider( this, it->get_arg_name(), it->get_arg_description(), 
            		(double)it->get_val_default(), (double)it->get_val_min(), (double)it->get_val_max(), 
            		(double)it->val_step1, (double)it->val_step2, 1);
            sl->get_adjustment()->signal_value_changed().connect( sigc::mem_fun(*this, &PF::GmicGenericConfigGUI::sl_changed) );
            controlsBox.pack_start( *sl );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_int:
            sl = new Slider( this, it->get_arg_name(), it->get_arg_description(), 
            		(double)it->get_val_default(), (double)it->get_val_min(), (double)it->get_val_max(), 
            		(double)it->val_step1, (double)it->val_step2, 1);
            controlsBox.pack_start( *sl );
            break;
		case PF::GmicArgument::GmicArgumentType::arg_type_bool:
			chb = new CheckBox( this, it->get_arg_name(), it->get_arg_description(), it->get_val_default() );
			controlsBox.pack_start( *chb );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_button:
			btn = new Gtk::Button(it->get_arg_description());
			controlsBox.pack_start( *btn );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_choise:
			sel = new Selector( this, it->get_arg_name(), it->get_arg_description(), (int)it->get_val_default() );
			controlsBox.pack_start( *sel );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_text_multi:
			txt = new TextBox( this, it->get_arg_name(), it->get_arg_description(), it->get_arg_description() );
			controlsBox.pack_start( *txt );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_text_single:
			txt = new TextBox( this, it->get_arg_name(), it->get_arg_description(), it->get_arg_description() );
			controlsBox.pack_start( *txt );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_file:
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_folder:
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_color:
		{
#ifdef GTKMM_2
			Gdk::Color clr;
			clr.set_rgb(it->get_val_default(), it->get_val_min(), it->get_val_max());
//			clrbtn = new Gtk::ColorButton(clr);
#endif
#ifdef GTKMM_3
			Gdk::RGBA clr;
			clr.set_rgba(it->get_val_default(), it->get_val_min(), it->get_val_max());
//			clrbtn = new Gtk::ColorButton(clr);
#endif
			clrbtn = new ColorBtn(this, it->get_arg_name(), it->get_arg_description(), clr);
			controlsBox.pack_start( *clrbtn );
			
//			clrbtn->signal_color_set().
//			    connect( sigc::mem_fun(this, &PF::GmicGenericConfigGUI::on_color_btn_changed) );
		}
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_const:
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_note:
			lbl = new Gtk::Label();
			lbl->set_use_markup(true);
			lbl->set_label( it->get_val_str() );
			lbl->set_line_wrap(true);
			lbl->set_halign(Gtk::Align::ALIGN_START);
			controlsBox.pack_start( *lbl, Gtk::PACK_SHRINK );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_link:
			linkb = new Gtk::LinkButton();
			linkb->set_label( it->get_arg_description() );
			linkb->set_uri( it->get_val_str() );
			linkb->set_halign(Gtk::Align::ALIGN_START);
			controlsBox.pack_start( *linkb );
			break;
		case PF::GmicArgument::GmicArgumentType::arg_type_separator:
			sep = new Gtk::HSeparator();
			controlsBox.pack_start( *sep );
			break;
		default:
		  std::cout<<"PF::GmicGenericConfigGUI::create_controls() it->get_arg_type(): "<<it->get_arg_type()<<std::endl;
		  break;
		}
	}
	get_main_box().show_all_children();
}

void PF::GmicGenericConfigGUI::do_update()
{
  std::cout<<"PF::GmicGenericConfigGUI::do_update()"<<std::endl;

  
/*  if( get_layer() && get_layer()->get_image() && 
      get_layer()->get_processor() &&
      get_layer()->get_processor()->get_par() ) {
    GmicGenericUntiledPar* par = dynamic_cast<GmicGenericUntiledPar*>( get_layer()->get_processor()->get_par() );
    if( !par ) return;
    par->refresh();
    get_layer()->get_image()->lock();
    std::cout<<"  updating image"<<std::endl;
    get_layer()->get_image()->update();
    get_layer()->get_image()->unlock();
  }*/
  
  create_controls();
  
  PF::OperationConfigGUI::do_update();
}


/*
void PF::DrawConfigGUI::on_color_btn_changed()
{
	// Pointer to the associated Layer object
	PF::Layer* layer = get_layer();
	if( !layer ) return;
	
	PF::ProcessorBase* processor = layer->get_processor();
	if( !processor || !(processor->get_par()) ) return;
	
	PF::GmicGenericPar* par = dynamic_cast<PF::GmicGenericPar*>( processor->get_par() );
	if( !par ) return;
	  

}
*/

