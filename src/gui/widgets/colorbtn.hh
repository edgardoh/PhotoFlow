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

#ifndef COLORBTN_HH
#define COLORBTN_HH

#include <gtkmm.h>

#include "pfwidget.hh"

namespace PF {

  class ColorBtn: public Gtk::HBox/*, public PFWidget*/
  {
	  Gtk::ColorButton colorbtn;
	  Gtk::Label label;
	  Gtk::HBox hbox_btn;

  public:
#ifdef GTKMM_2
	  ColorBtn(OperationConfigGUI* dialog, std::string pname, Glib::ustring l, const Color& val);
#endif
#ifdef GTKMM_3
	  ColorBtn(OperationConfigGUI* dialog, std::string pname, Glib::ustring l, Gdk::RGBA val);
#endif

    ~ColorBtn() {}

    
    void get_value();
    void set_value();

#ifdef GTKMM_3
    void get_color(Gdk::RGBA value);
#endif
#ifdef GTKMM_2
    void get_color(Gdk::Color value);
#endif
#ifdef GTKMM_3
    void set_color(Gdk::RGBA value);
#endif
#ifdef GTKMM_2
    void set_color(Gdk::Color value);
#endif
    Glib::ustring get_text();
    void set_text(Glib::ustring l);

  };


}

#endif
