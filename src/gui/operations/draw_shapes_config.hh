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

#ifndef DRAW_SHAPES_CONFIG_DIALOG_HH
#define DRAW_SHAPES_CONFIG_DIALOG_HH

#include <gtkmm.h>

#include "../operation_config_gui.hh"
#include "../../operations/draw_shapes.hh"
#include "shapes_op_config.hh"

namespace PF {

  class DrawShapesConfigGUI: public ShapesConfigGUI
  {
    Gtk::HBox colorButtonsBox1;
    Gtk::HBox colorButtonsBox2;
    
    Gtk::HSeparator draw_shapes_separator;

    Gtk::Label fgd_color_label, bgd_color_label;

    Gtk::ColorButton fgd_color_button, bgd_color_button;
    CheckBox bgd_transparent_checkbox;

    virtual bool get_has_source() { return false; }

  public:
    DrawShapesConfigGUI( Layer* l );
    
    void selection_changed();
    
    void on_fgd_color_changed();
    void on_bgd_color_changed();

    void do_update();
  };

}

#endif

