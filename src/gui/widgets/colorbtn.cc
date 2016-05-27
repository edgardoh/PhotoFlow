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

#include "colorbtn.hh"


#ifdef GTKMM_2
PF::ColorBtn::ColorBtn( OperationConfigGUI* dialog, std::string pname, Glib::ustring l, const Color& val ):
	/*PF::PFWidget( dialog, pname ),*/
	Gtk::HBox()
{
	label.set_label( l );
	colorbtn.set_color( val );
	colorbtn.set_title( l );
	colorbtn.set_border_width(5);
	
	hbox_btn.pack_start( label, false, true );
	hbox_btn.pack_end( colorbtn, false, true );
	colorbtn.set_halign(Gtk::Align::ALIGN_END);
	pack_start( hbox_btn );

	/*	colorbtn.signal_activate().
	    connect(sigc::mem_fun(*this,
				  &PFWidget::changed));
	*/

  show_all_children();
}
#endif

#ifdef GTKMM_3
PF::ColorBtn::ColorBtn( OperationConfigGUI* dialog, std::string pname, Glib::ustring l, Gdk::RGBA val ):
		/*PF::PFWidget( dialog, pname ),*/
		Gtk::HBox()
{
	label.set_label( l );
	colorbtn.set_rgba( val );
	colorbtn.set_title( l );
	colorbtn.set_border_width(5);

	hbox_btn.pack_start( label, false, true );
	hbox_btn.pack_end( colorbtn, false, true );
	colorbtn.set_halign(Gtk::Align::ALIGN_END);
	pack_start( hbox_btn );
	
/*	colorbtn.signal_activate().
    connect(sigc::mem_fun(*this,
			  &PFWidget::changed));
*/
  show_all_children();
}
#endif

void PF::ColorBtn::get_value()
{
  //if( !get_prop() ) return;
#ifdef GTKMM_2
  Gdk::Color value;
//  get_prop()->get(value);
  value.get().r *= 65535;
  value.get().g *= 65535;
  value.get().b *= 65535;
  colorbtn.set_color(value);
#endif

#ifdef GTKMM_3
  Gdk::RGBA value;
//  get_prop()->get(value);
  colorbtn.set_rgba(value);
#endif
}


void PF::ColorBtn::set_value()
{
//  if( !get_prop() ) return;
#ifdef GTKMM_2
  Gdk::Color value = colorbtn.get_color();
  value.get().r /= 65535;
  value.get().g /= 65535;
  value.get().b /= 65535;
//  get_prop()->update(value);
#endif

#ifdef GTKMM_3
  //get_prop()->update(colorbtn.get_rgba());
#endif
}

#ifdef GTKMM_3
void PF::ColorBtn::get_color(Gdk::RGBA value)
{ 
	value = colorbtn.get_rgba();
}
#endif

#ifdef GTKMM_2
void PF::ColorBtn::get_color(Gdk::Color value)
{ 
    value = colorbtn.get_color();
    value.get().r /= 65535;
    value.get().g /= 65535;
    value.get().b /= 65535;
    
	return;
}
#endif

#ifdef GTKMM_3
void PF::ColorBtn::set_color(Gdk::RGBA value)
{ 
	colorbtn.set_rgba(value);
}
#endif

#ifdef GTKMM_2
void PF::ColorBtn::set_color(Gdk::Color value)
{ 
    value.get().r /= 65535;
    value.get().g /= 65535;
    value.get().b /= 65535;
    colorbtn.set_color(value);
  
	return;
}
#endif

Glib::ustring PF::ColorBtn::get_text()
{
	return label.get_label();
}

void PF::ColorBtn::set_text(Glib::ustring l)
{
	label.set_label(l);
	colorbtn.set_title( l );
}

