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

#ifndef SHAPES_OP_CONFIG_HH
#define SHAPES_OP_CONFIG_HH

#include <gtkmm.h>

#include "../operation_config_gui.hh"
#include "../../operations/shapes.hh"
#include "../../operations/shapes_op.hh"

#include "../widgets/selector.hh"
#include "../widgets/curveeditor.hh"


namespace PF {

  class ShapesConfigGUI: public OperationConfigGUI
  {
  protected:
    Gtk::VBox controlsBox;
    
    Gtk::HBox hbox_shapes_type;
    RadioImageButton btn_shapes;
   
    Gtk::HBox hbox_shapes_options;
    ToggleImageButton lock_cursor_mode_btn;
    ToggleImageButton show_outline_btn;
    ToggleImageButton lock_source_btn;
    ToggleImageButton lock_shapes_btn;
    ToggleImageButton fill_shape_btn;
    ToggleImageButton rect_rounded_btn;

    Gtk::HSeparator separator, separator2;
    
    Gtk::VBox vbox_shape_values;
    Slider falloff_sl;
    Slider opacity_sl;
    Slider pen_size_sl;
    Slider radius_sl;
    Slider radius_x_sl;
    Slider radius_y_sl;
    Slider angle_sl;

    Gtk::VBox vbox_curves_prev;
    
    Gtk::HBox hbox_curves;
    CurveEditor falloffCurveEditor;


  private:
    Point pt_current;
    Point pt_source;
    Point last_pt_origin;
    
    int mo_current_shape_type;
    
    int mo_hit_test;
    int mo_shape_index;
    int mo_shape_additional;
    
    int mo_shape_node_selected;
    int mo_node_selected;

    bool mo_dragging;
    int mo_adding;
    bool editting;

    Line line_add;
    Polygon polygon_add;
    Rectangle rectangle_add;
    
    std::vector<int> shapes_selected;

    void falloff_sl_changed();
    void opacity_sl_changed();
    void pen_size_sl_changed();
    void radius_sl_changed();
    void radius_x_sl_changed();
    void radius_y_sl_changed();
    void angle_sl_changed();
    void fill_shape_btn_changed();
    void rect_rounded_btn_changed();
    void show_outline_btn_changed();
    
public:
    void pt_screen2image(Point& pt);
    void pt_image2screen(Point& pt);

    virtual bool get_has_source() { return true; }
    
    int get_falloff() { return falloff_sl.get_adjustment()->get_value(); }
    void set_falloff(int v) { falloff_sl.get_adjustment()->set_value(v); }
    
    float get_opacity() { return opacity_sl.get_adjustment()->get_value()/100.f; }
    void set_opacity(float v) { opacity_sl.get_adjustment()->set_value(v*100.f); }
    
    int get_pen_size() { return pen_size_sl.get_adjustment()->get_value(); }
    void set_pen_size(int v) { pen_size_sl.get_adjustment()->set_value(v); }
    
    int get_radius() { return radius_sl.get_adjustment()->get_value(); }
    void set_radius(int v) { radius_sl.get_adjustment()->set_value(v); }
    
    int get_radius_x() { return radius_x_sl.get_adjustment()->get_value(); }
    void set_radius_x(int v) { radius_x_sl.get_adjustment()->set_value(v); }
    
    int get_radius_y() { return radius_y_sl.get_adjustment()->get_value(); }
    void set_radius_y(int v) { radius_y_sl.get_adjustment()->set_value(v); }
    
    float get_angle() { return angle_sl.get_adjustment()->get_value(); }
    void set_angle(float v) { angle_sl.get_adjustment()->set_value(v); }

    bool get_lock_cursor_mode() { return lock_cursor_mode_btn.is_active(); }
    bool get_show_outline() { return show_outline_btn.is_active(); }
    bool get_lock_source() { return lock_source_btn.is_active(); }
    bool get_lock_shapes() { return lock_shapes_btn.is_active(); }
    bool get_fill_shape() { return fill_shape_btn.is_active(); }
    void set_fill_shape(bool b) { fill_shape_btn.set_active(b); }
    bool get_rect_rounded() { return rect_rounded_btn.is_active(); }
    void set_rect_rounded(bool b) { rect_rounded_btn.set_active(b); }
    
/*    void draw_circle(PF::Point& center, float radius, PF::Point& pt_from, PF::Point& pt_to, 
        PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out);
    */
    void draw_shape(Shape* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, int hit_node_selected, bool selected);
    void draw_node(int x, int y, PF::PixelBuffer& buf_out, bool active_point);
    
/*    void draw_dot(Point& pt, PF::PixelBuffer& buf_out, bool active_point);
    
    void draw_node(Point& pt, PF::PixelBuffer& buf_out, bool active_point)
    {
      draw_node(pt.get_x(), pt.get_y(), buf_out, active_point);
    }
    void draw_rect(VipsRect* rc, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool selected);
*/
    void draw_line(Point& pt1, Point& pt2, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);
    void draw_line(Point& pt1, Point& pt2, float pen_size, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);
    void draw_circle(Point& pt1, int radius, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);
    void draw_ellipse(Point& pt1, int radius_x, int radius_y, float angle, int quadrant, 
                      PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);
    void draw_bezier3(Point& anchor1, Point& anchor2, Point& control, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);
    void draw_bezier3(Point& anchor1, Point& anchor2, Point& control, float pen_size, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);
    void draw_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);
    void draw_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, float pen_size, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected);

    void draw_shape_circle(Circle* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected);
    void draw_shape_ellipse(Ellipse* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected);
    void draw_shape_rectangle(Rectangle* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected);
    
    void draw_node_control_points(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int node, int cntrl1_selected, int cntrl2_selected, int selected);
/*    void draw_node_control_points_line(Line* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
        int node, int cntrl1_selected, int cntrl2_selected, int selected);
*/
    void draw_polygon_vec(std::vector<Point>& vec_pt, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
        int hit_t, int hit_additional, int hit_node_selected, bool selected);
    void draw_polygon_segment_falloff(std::vector<Point>& points, Point& pt3_first, Point& pt4_prev, bool clockwise, float falloff, 
        Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
        int hit_t, int hit_additional, int hit_node_selected, bool selected);
    void draw_polygon_falloff2(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
        int hit_t, int hit_additional, int hit_node_selected, bool selected);

    void draw_shape_polygon(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, int hit_node_selected, bool selected);
    void draw_shape_line(Line* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, int hit_node_selected, bool selected);
    
    void draw_mouse_pointer_circle(PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out);
    bool draw_mouse_pointer(PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out);

    void add_new_shape(PF::ShapesPar* par, int shape_type, Point& initial_pos, Point& source_pos, int mod_key);
    
    void defalut_new_polygon(Point& pt, int mod_key );
//    void defalut_polygon_new_control_point(Polygon* shape, int hit_t, int node);
    void defalut_polygon_last_point( PF::Polygon* shape );

//    void adjust_polygon_falloff_point(Polygon* shape, int n);
    
    void defalut_new_line(Point& pt, int mod_key );
//    void defalut_line_new_control_point(Line* shape, int hit_t, int node);
    void defalut_line_last_point( PF::Line* shape );

    void shape_expanded(Shape* shape);
    
    virtual void selection_changed();
    void selection_clear();
    void selection_replace(int n);
    void selection_add(int n);
    void selection_remove(int n);
    bool is_shape_selected(int n);
    int get_selection_count() { return shapes_selected.size(); }
    int get_selected(int n) { return shapes_selected[n]; }
    
    bool delete_shape(PF::ShapesPar* par);
    bool selection_delete_shape(PF::ShapesPar* par);

    void done_adding_shape();
    
    bool handle_set_source_point(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_add_new_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_end_adding_new_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_update_adding_new_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_delete_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_delete_node(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_add_node(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_add_control_point(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_selection(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh, int action);

    bool handle_update_pos_adding_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    bool handle_drag_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);
    
    void handle_expand_shape(Shape* shape, int mod_key, int direction);
    bool handle_expand_shapes(PF::ShapesPar* par, int mod_key, int direction, bool& refresh);

    bool update_hit_test(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh);

  public:
    ShapesConfigGUI( Layer* l );
    ShapesConfigGUI( Layer* layer, const Glib::ustring& title, bool has_ch_sel=true );
    void do_update();

    void parameters_reset();
    
    void set_editting(bool e) { editting = e; }
    bool get_editting() { return editting || !get_editing_flag(); }
    int get_current_shape_type() { return mo_current_shape_type; }

    bool pointer_press_event( int button, double x, double y, int mod_key );
    bool pointer_release_event( int button, double x, double y, int mod_key );
    bool pointer_motion_event( int button, double x, double y, int mod_key );
    bool pointer_scroll_event( int direction, int mod_key );
    bool modify_preview( PixelBuffer& buf_in, PixelBuffer& buf_out,
        float scale, int xoffset, int yoffset );
    
    void btn_shapes_type_clicked();

    virtual void new_shape_added(int new_shape) { }
    virtual void delete_shape_before(int shape_to_delete) { }
    
  };

}

#endif

