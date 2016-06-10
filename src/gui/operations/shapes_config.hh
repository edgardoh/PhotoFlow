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
/*    Gtk::HBox hbox;
    CheckBox invert_box, enable_falloff_box;

    Gtk::HBox curvesBox;

    CurveEditor falloffCurveEditor;
*/
    Gtk::VBox vbox_shape;
    Selector shape_type_sel;
    CheckBox invert_box;
    Slider falloff_sl;
    Slider opacity_sl;

    Gtk::VBox vbox_circle;
    Slider radius_sl;

    Point pt_current;
    Point pt_source;
    
    int mo_hit_test;
//    int mo_shape_type;
    int mo_shape_index;
    int mo_shape_additional;

    bool dragging;
    int adding;
    
    Line line_add;
    
    bool editting;
    
    int active_point_id;
    bool center_selected;
    double cxlast, cylast;

    bool shape_resizing;
    double shape_resizing_last_point_x, shape_resizing_last_point_y;


    bool border_resizing;
    double border_resizing_shape_point_x, border_resizing_shape_point_y;
    int border_resizing_lstart;
    float border_resizing_size_start;

    bool initializing;

    void pt_screen2image(Point& pt);
//    void pt_screen2image(Point& pt1, Point& pt2);
    void pt_image2screen(Point& pt);
//    void pt_image2screen(Point& pt1, Point& pt2);

    void pt_screen2image(Circle1& shape);
    void pt_image2screen(Circle1& shape);
    void pt_screen2image(Rect1& shape);
    void pt_image2screen(Rect1& shape);

    bool get_has_source() { return true; }
    
    int get_falloff() { return falloff_sl.get_adjustment()->get_value(); }
    void set_falloff(int v) { falloff_sl.get_adjustment()->set_value(v); }
    float get_opacity() { return opacity_sl.get_adjustment()->get_value()/100.f; }
    void set_opacity(float v) { opacity_sl.get_adjustment()->set_value(v*100.f); }
    int get_size(int shape_type) { return radius_sl.get_adjustment()->get_value(); }
    void set_size(int shape_type, int v) { radius_sl.get_adjustment()->set_value(v); }
    
    void draw_shape(Shape* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    void draw_node(int x, int y, PF::PixelBuffer& buf_out, bool active_point);
    inline void draw_node(Point& pt, PF::PixelBuffer& buf_out, bool active_point)
    {
      draw_node(pt.get_x(), pt.get_y(), buf_out, active_point);
    }

    void draw_line(Line* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    
    void draw_circle(Circle1* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    void draw_rectangle(Rect1* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional);
    
//    void drag_circle(Circle1& shape, Point prev_pt, Point curr_pt, bool lock_source);
//    void drag_rectangle(Rect1& shape, Point prev_pt, Point curr_pt, bool lock_source);
    


    void draw_outline( PixelBuffer& buf_in, PixelBuffer& buf_out, ClosedSplineCurve& shape );

  public:
    ShapesConfigGUI( Layer* l );
    ShapesConfigGUI( Layer* layer, const Glib::ustring& title, bool has_ch_sel=true );
    void do_update();

    void parameters_reset();
    
    void set_editting(bool e) { editting = e; }
    bool get_editting() { return editting && !get_editing_flag(); }

    bool pointer_press_event( int button, double x, double y, int mod_key );
    bool pointer_release_event( int button, double x, double y, int mod_key );
    bool pointer_motion_event( int button, double x, double y, int mod_key );
    bool pointer_scroll_event( int direction, int mod_key );
    bool modify_preview( PixelBuffer& buf_in, PixelBuffer& buf_out,
        float scale, int xoffset, int yoffset );
  };

}

#endif
