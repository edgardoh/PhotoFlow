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
#include "../../../operations/gmic/gmic_generic_untiled.hh"
#include "../../widgets/colorbtn.hh"


PF::GmicGenericConfigGUI::GmicGenericConfigGUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "G'MIC Generic" )/*,
  updateButton( "Update" )*/
{
//  controlsBox.pack_start( updateButton );

//  updateButton.signal_clicked().connect( sigc::mem_fun(this, &GmicGenericConfigGUI::on_update) );
  
  add_widget( controlsBox );
  
  get_main_box().show_all_children();
}

void PF::GmicGenericConfigGUI::post_init()
{
	OperationConfigGUI::post_init();
	
	create_controls();
	
}

void PF::GmicGenericConfigGUI::create_controls()
{
//  std::cout<<"PF::GmicGenericConfigGUI::post_init() "<<get_pf_filter().get_name()<<", "<<get_pf_filter().get_type()<<std::endl;
//  GMicArgumentList arg_list;
	
//  arg_list.parse_arguments(columns[GMicLoad::col_arguments_gmic]);
//  std::list<GMicArgument>arg_l = arg_list.get_arg_list();
  std::list<FilterField>field_list = get_pf_filter()->get_fields();

	Slider *sl;
	CheckBox *chb;
	Gtk::Button *btn;
	Selector *sel;
	TextBox *txt;
	Gtk::Label *lbl;
	Gtk::HSeparator *sep;
	Gtk::LinkButton *linkb;
	ColorBtn *clrbtn;
	
	for (std::list<FilterField>::iterator it=field_list.begin(); it != field_list.end(); ++it) {
//	  std::cout<<"PF::GmicGenericConfigGUI::post_init()"<<", "<<it->field_type<<", "<<it->field_description<<std::endl;
		switch (it->field_type)
		{
		case FilterField::field_type_float:
            sl = new Slider( this, it->field_name, it->field_description, 
            		(double)it->val_default, (double)it->val_min, (double)it->val_max, 
            		(double)it->val_step1, (double)it->val_step2, 1);
            controlsBox.pack_start( *sl );
			break;
		case FilterField::field_type_int:
            sl = new Slider( this, it->field_name, it->field_description, 
            		(double)it->val_default, (double)it->val_min, (double)it->val_max, 
            		(double)it->val_step1, (double)it->val_step2, 1);
            controlsBox.pack_start( *sl );
            break;
		case FilterField::field_type_bool:
			chb = new CheckBox( this, it->field_name, it->field_description, it->val_default );
			controlsBox.pack_start( *chb );
			break;
		case FilterField::field_type_button:
			btn = new Gtk::Button(it->field_description);
			controlsBox.pack_start( *btn );
			break;
		case FilterField::field_type_choise:
			sel = new Selector( this, it->field_name, it->field_description, (int)it->val_default );
			controlsBox.pack_start( *sel );
			break;
		case FilterField::field_type_text_multi:
			txt = new TextBox( this, it->field_name, it->field_description, it->field_description );
			controlsBox.pack_start( *txt );
			break;
		case FilterField::field_type_text_single:
			txt = new TextBox( this, it->field_name, it->field_description, it->field_description );
			controlsBox.pack_start( *txt );
			break;
		case FilterField::field_type_file:
			break;
		case FilterField::field_type_folder:
			break;
		case FilterField::field_type_color:
		{
#ifdef GTKMM_2
			Gdk::Color clr;
			clr.set_rgb(it->val_default, it->val_min, it->val_max);
//			clrbtn = new Gtk::ColorButton(clr);
#endif
#ifdef GTKMM_3
			Gdk::RGBA clr;
			clr.set_rgba(it->val_default, it->val_min, it->val_max);
//			clrbtn = new Gtk::ColorButton(clr);
#endif
			clrbtn = new ColorBtn(this, it->field_name, it->field_description, clr);
			controlsBox.pack_start( *clrbtn );
			
//			clrbtn->signal_color_set().
//			    connect( sigc::mem_fun(this, &PF::GmicGenericConfigGUI::on_color_btn_changed) );
		}
			break;
		case FilterField::field_type_const:
			break;
		case FilterField::field_type_note:
			lbl = new Gtk::Label();
			lbl->set_use_markup(true);
			lbl->set_label( it->str_value );
			lbl->set_line_wrap(true);
			lbl->set_halign(Gtk::Align::ALIGN_START);
			controlsBox.pack_start( *lbl, Gtk::PACK_SHRINK );
			break;
		case FilterField::field_type_link:
			linkb = new Gtk::LinkButton();
			linkb->set_label( it->field_description );
			linkb->set_uri( it->str_value );
			linkb->set_halign(Gtk::Align::ALIGN_START);
			controlsBox.pack_start( *linkb );
			break;
		case FilterField::field_type_separator:
			sep = new Gtk::HSeparator();
			controlsBox.pack_start( *sep );
			break;
		}
	}

}
/*
void PF::GmicGenericConfigGUI::on_update()
{
  if( get_layer() && get_layer()->get_image() && 
      get_layer()->get_processor() &&
      get_layer()->get_processor()->get_par() ) {
    GmicGenericUntiledPar* par = dynamic_cast<GmicGenericUntiledPar*>( get_layer()->get_processor()->get_par() );
    if( !par ) return;
    par->refresh();
    get_layer()->get_image()->lock();
    std::cout<<"  updating image"<<std::endl;
    get_layer()->get_image()->update();
    get_layer()->get_image()->unlock();
  }
}
*/

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

