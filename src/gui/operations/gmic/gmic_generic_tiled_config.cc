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

#include "../vips/gmic/gmic/src/gmic.h"
#include "../vips/gmic/gmic/src/gmic_stdlib.h"
#include "../operations/gmic/gmic_generic_tiled.hh"
#include "../vips/gmic/gmic/src/CImg.h"

#include "gmic_generic_tiled_config.hh"
#include "../operations/gmic/gmic_load.hh"
#include "../../widgets/colorbtn.hh"


PF::GmicGenericTiledConfigGUI::GmicGenericTiledConfigGUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "G'MIC Generic Tiled" )
{

  add_widget( controlsBox );
  
  get_main_box().show_all_children();
}

void PF::GmicGenericTiledConfigGUI::post_init(std::vector<std::string>& columns)
{
	OperationConfigGUI::post_init(columns);
	
  GMicArgumentList arg_list;
	
  arg_list.parse_arguments(columns[2]);
  std::list<GMicArgument>arg_l = arg_list.get_arg_list();
	Slider *sl;
	CheckBox *chb;
	Gtk::Button *btn;
	Selector *sel;
	TextBox *txt;
	Gtk::Label *lbl;
	Gtk::HSeparator *sep;
	Gtk::LinkButton *linkb;
	ColorBtn *clrbtn;
	
	for (std::list<GMicArgument>::iterator it=arg_l.begin(); it != arg_l.end(); ++it) {
		switch (it->arg_type)
		{
		case arg_type_float:
            sl = new Slider( this, it->arg_name, it->arg_description, 
            		(double)it->arg_default, (double)it->arg_min, (double)it->arg_max, 
            		(double)it->arg_step1, (double)it->arg_step2, 1);
            controlsBox.pack_start( *sl );
			break;
		case arg_type_int:
            sl = new Slider( this, it->arg_name, it->arg_description, 
            		(double)it->arg_default, (double)it->arg_min, (double)it->arg_max, 
            		(double)it->arg_step1, (double)it->arg_step2, 1);
            controlsBox.pack_start( *sl );
            break;
		case arg_type_bool:
			chb = new CheckBox( this, it->arg_name, it->arg_description, it->arg_default );
			controlsBox.pack_start( *chb );
			break;
		case arg_type_button:
			btn = new Gtk::Button(it->arg_description);
			controlsBox.pack_start( *btn );
			break;
		case arg_type_choise:
			sel = new Selector( this, it->arg_name, it->arg_description, (int)it->arg_default );
			controlsBox.pack_start( *sel );
			break;
		case arg_type_text_multi:
			txt = new TextBox( this, it->arg_name, it->arg_description, it->arg_description );
			controlsBox.pack_start( *txt );
			break;
		case arg_type_text_single:
			txt = new TextBox( this, it->arg_name, it->arg_description, it->arg_description );
			controlsBox.pack_start( *txt );
			break;
		case arg_type_file:
			break;
		case arg_type_folder:
			break;
		case arg_type_color:
		{
#ifdef GTKMM_2
			Gdk::Color clr;
			clr.set_rgb(it->arg_default, it->arg_min, it->arg_max);
//			clrbtn = new Gtk::ColorButton(clr);
#endif
#ifdef GTKMM_3
			Gdk::RGBA clr;
			clr.set_rgba(it->arg_default, it->arg_min, it->arg_max);
//			clrbtn = new Gtk::ColorButton(clr);
#endif
			clrbtn = new ColorBtn(this, it->arg_name, it->arg_description, clr);
			controlsBox.pack_start( *clrbtn );
			
//			clrbtn->signal_color_set().
//			    connect( sigc::mem_fun(this, &PF::GmicGenericTiledConfigGUI::on_color_btn_changed) );
		}
			break;
		case arg_type_const:
			break;
		case arg_type_note:
			lbl = new Gtk::Label();
			lbl->set_use_markup(true);
			lbl->set_label( it->arg_description );
			lbl->set_line_wrap(true);
			lbl->set_halign(Gtk::Align::ALIGN_START);
			controlsBox.pack_start( *lbl, Gtk::PACK_SHRINK );
			break;
		case arg_type_link:
			linkb = new Gtk::LinkButton();
			linkb->set_label( it->arg_description );
			linkb->set_uri( it->arg_value );
			linkb->set_halign(Gtk::Align::ALIGN_START);
			controlsBox.pack_start( *linkb );
			break;
		case arg_type_separator:
			sep = new Gtk::HSeparator();
			controlsBox.pack_start( *sep );
			break;
		}
	}

}


/*
void PF::DrawConfigGUI::on_color_btn_changed()
{
	// Pointer to the associated Layer object
	PF::Layer* layer = get_layer();
	if( !layer ) return;
	
	PF::ProcessorBase* processor = layer->get_processor();
	if( !processor || !(processor->get_par()) ) return;
	
	PF::GmicGenericTiledPar* par = dynamic_cast<PF::GmicGenericTiledPar*>( processor->get_par() );
	if( !par ) return;
	  

}
*/

