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

#ifndef SHAPES_CONFIG_HH
#define SHAPES_CONFIG_HH

#include <gtkmm.h>

#include "../operation_config_gui.hh"
#include "../../operations/shapes.hh"

#include "../widgets/selector.hh"
#include "../widgets/curveeditor.hh"


namespace PF {

  class ShapesConfigGUI: public OperationConfigGUI
  {
    Gtk::HBox curvesBox;
    CurveEditor falloffCurveEditor;

    Gtk::HBox hbox_shapes_type;
    RadioImageButton btn_shapes;
   
    Gtk::VBox vbox_shape;
    Slider falloff_sl;
    Slider opacity_sl;
    Slider size_sl;

    Point pt_current;
    Point pt_source;
    
    int shape_type_selected;
    
    int mo_hit_test;
    int mo_shape_index;
    int mo_shape_additional;

    bool dragging;
    int adding;
    bool editting;

    Line line_add;
    
    

    void pt_screen2image(Point& pt);
    void pt_image2screen(Point& pt);

    virtual bool get_has_source() { return true; }
    
    int get_falloff() { return falloff_sl.get_adjustment()->get_value(); }
    void set_falloff(int v) { falloff_sl.get_adjustment()->set_value(v); }
    float get_opacity() { return opacity_sl.get_adjustment()->get_value()/100.f; }
    void set_opacity(float v) { opacity_sl.get_adjustment()->set_value(v*100.f); }
    int get_size(int shape_type) { return size_sl.get_adjustment()->get_value(); }
    void set_size(int shape_type, int v) { size_sl.get_adjustment()->set_value(v); }
    
    void draw_shape(Shape* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    void draw_node(int x, int y, PF::PixelBuffer& buf_out, bool active_point);
    inline void draw_node(Point& pt, PF::PixelBuffer& buf_out, bool active_point)
    {
      draw_node(pt.get_x(), pt.get_y(), buf_out, active_point);
    }

    void draw_line(Line* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    void draw_circle(Circle1* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    void draw_ellipse(Ellipse* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    void draw_rectangle(Rect1* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    
    void add_new_shape(PF::ShapesPar* par, int shape_type, Point& initial_pos, Point& source_pos);

//    void draw_outline( PixelBuffer& buf_in, PixelBuffer& buf_out, ClosedSplineCurve& shape );

  public:
    ShapesConfigGUI( Layer* l );
    ShapesConfigGUI( Layer* layer, const Glib::ustring& title, bool has_ch_sel=true );
    void do_update();

    void parameters_reset();
    
    void set_editting(bool e) { editting = e; }
    bool get_editting() { return editting && !get_editing_flag(); }
    int get_shape_type() { return shape_type_selected; }

    bool pointer_press_event( int button, double x, double y, int mod_key );
    bool pointer_release_event( int button, double x, double y, int mod_key );
    bool pointer_motion_event( int button, double x, double y, int mod_key );
    bool pointer_scroll_event( int direction, int mod_key );
    bool modify_preview( PixelBuffer& buf_in, PixelBuffer& buf_out,
        float scale, int xoffset, int yoffset );
    
    void btn_shapes_type_clicked();

  };

}

#endif
