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


PF::ColorBtn::ColorBtn( OperationConfigGUI* dialog, std::string pname, Glib::ustring l, const PF::GmicColor& val ):
    PF::PFWidget( dialog, pname ),
    Gtk::HBox()
{
  label.set_label( l );
  
#ifdef GTKMM_2
  Gdk::Color color;
  color.get().r = val.get_r() * 65535;
  color.get().g = val.get_g() * 65535;
  color.get().b = val.get_b() * 65535;
  colorbtn.set_color(color);
#endif
#ifdef GTKMM_3
  Gdk::RGBA color;
  color.set_rgba( val.get_r(), val.get_g(), val.get_b() );
  colorbtn.set_rgba( color );
#endif

  colorbtn.set_title( l );
  colorbtn.set_border_width(5);

  hbox_btn.pack_start( label, false, true );
  hbox_btn.pack_end( colorbtn, false, true );
  colorbtn.set_halign(Gtk::Align::ALIGN_END);
  pack_start( hbox_btn );
  
  colorbtn.signal_color_set().connect( sigc::mem_fun(this, &PFWidget::changed) );
  
  show_all_children();
}

void PF::ColorBtn::get_value()
{
  if( !get_prop() ) return;
  
  GmicColor color;
  get_prop()->get(color);
  
#ifdef GTKMM_2
  Gdk::Color value;

  value.get().r = color.get_r()*65535;
  value.get().g = color.get_g()*65535;
  value.get().b = color.get_b()*65535;
  colorbtn.set_color(value);
#endif

#ifdef GTKMM_3
  Gdk::RGBA value;
  value.set_rgba(color.get_r(), color.get_g(), color.get_b());

  colorbtn.set_rgba(value);
#endif
}


void PF::ColorBtn::set_value()
{
  if( !get_prop() ) return;
  
  GmicColor color;
  get_color(color);
  get_prop()->update(color);
}

Glib::ustring PF::ColorBtn::get_text()
{
	return label.get_label();
}

void PF::ColorBtn::set_text(Glib::ustring l)
{
	label.set_label(l);
	colorbtn.set_title( l );
}

void PF::ColorBtn::get_color(GmicColor& color)
{
#ifdef GTKMM_2
  Gdk::Color value = colorbtn.get_color();
  color.set( value.get().r * 65535,
  value.get().g * 65535,
  value.get().b * 65535 );
#endif
#ifdef GTKMM_3
  Gdk::RGBA value = colorbtn.get_rgba();
  color.set( value.get_red(), value.get_green(), value.get_blue() );
#endif
}

void PF::ColorBtn::set_color(GmicColor& color)
{
#ifdef GTKMM_2
  Gdk::Color value;

  value.get().r = color.get_r()*65535;
  value.get().g = color.get_g()*65535;
  value.get().b = color.get_b()*65535;
  colorbtn.set_color(value);
#endif

#ifdef GTKMM_3
  Gdk::RGBA value;
  value.set_rgba(color.get_r(), color.get_g(), color.get_b());

  colorbtn.set_rgba(value);
#endif
}
