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

#include "shapes_op_config.hh"


#define CURVE_SIZE 192


#define SHAPE_KEY_DELETE_SHAPE        PF::MOD_KEY_NONE
#define SHAPE_KEY_DELETE_NODE         PF::MOD_KEY_NONE
#define SHAPE_KEY_ADD_NODE            PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_ADD_CONTROL_POINT   PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_NOT_ADD_SHAPE       PF::MOD_KEY_SHIFT
#define SHAPE_KEY_UNLOCK_SOURCE       PF::MOD_KEY_SHIFT
#define SHAPE_KEY_SELECT_ADD_SHAPE    PF::MOD_KEY_ALT+PF::MOD_KEY_CTRL


PF::ShapesConfigGUI::ShapesConfigGUI( PF::Layer* layer ):
              OperationConfigGUI( layer, "Shapes tool" ),
              falloffCurveEditor( this, "falloff_curve", new PF::CurveArea(), 0, 100, 0, 100, CURVE_SIZE, CURVE_SIZE ),
              btn_shapes(),
              falloff_sl( this, "falloff", "falloff: ", 10, 0, 1000, 0.1, 1, 1 ),
              opacity_sl( this, "opacity", "opacity: ", 100, 0, 100, 0.1, 1, 1 ),
              pen_size_sl( this, "pen_size", "pen size: ", 50, 0, 1000000, 1, 10, 1 ),
              radius_sl( this, "radius", "radius: ", 50, 0, 1000000, 1, 10, 1 ),
              radius_x_sl( this, "radius_x", "radius x: ", 50, 0, 1000000, 1, 10, 1 ),
              radius_y_sl( this, "radius_y", "radius y: ", 50, 0, 1000000, 1, 10, 1 ),
              angle_sl( this, "angle", "angle: ", 0, 0, 360, 1, 10, 1 ),
//              show_outline_chk( this, "show_outline", _("show shapes"), false ),
//              lock_source_chk( this, "lock_source", _("lock source point"), false ),
//              lock_shapes_chk( this, "lock_shapes", _("lock shapes"), false ),
              lock_cursor_mode_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-inactive.png", true ), 
              show_outline_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-inactive.png", true ), 
              lock_source_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-inactive.png", true, false ), 
              lock_shapes_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-inactive.png", true, false ), 
              last_pt_origin(-1, -1)
{
  
  mo_hit_test = PF::Shape::hit_none;
  mo_shape_index = -1;
  mo_shape_additional = -1;
  mo_shape_node_selected = -1;
  mo_node_selected = -1;

  mo_dragging = false;
  mo_adding = PF::Shape::shape;

}

PF::ShapesConfigGUI::ShapesConfigGUI( Layer* layer, const Glib::ustring& title, bool has_ch_sel ):
              OperationConfigGUI( layer, title, has_ch_sel ),
              falloffCurveEditor( this, "falloff_curve", new PF::CurveArea(), 0, 100, 0, 100, CURVE_SIZE, CURVE_SIZE ),
              btn_shapes(),
              falloff_sl( this, "falloff", "falloff: ", 10, 0, 1000, 0.1, 1, 1 ),
              opacity_sl( this, "opacity", "opacity: ", 100, 0, 100, 0.1, 1, 1 ),
              pen_size_sl( this, "pen_size", "pen size: ", 50, 0, 1000000, 1, 10, 1 ),
              radius_sl( this, "radius", "radius: ", 50, 0, 1000000, 1, 10, 1 ),
              radius_x_sl( this, "radius_x", "radius x: ", 50, 0, 1000000, 1, 10, 1 ),
              radius_y_sl( this, "radius_y", "radius y: ", 50, 0, 1000000, 1, 10, 1 ),
              angle_sl( this, "angle", "angle: ", 0, 0, 360, 1, 10, 1 ),
//              show_outline_chk( this, "show_outline", _("show shapes"), false ),
//              lock_source_chk( this, "lock_source", _("lock source point"), false ),
//              lock_shapes_chk( this, "lock_shapes", _("lock shapes"), false ),
              lock_cursor_mode_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-inactive.png", true ), 
              show_outline_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-inactive.png", true ), 
              lock_source_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-inactive.png", true, false ), 
              lock_shapes_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-inactive.png", true, false ), 
              last_pt_origin(-1, -1)
{
  btn_shapes.add_button( PF::Shape::shape, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/cursor-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/cursor-inactive.png" );
  btn_shapes.add_button( PF::Shape::line, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/line-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/line-inactive.png" );
  btn_shapes.add_button( PF::Shape::circle, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/circle-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/circle-inactive.png" );
  btn_shapes.add_button( PF::Shape::ellipse, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/ellipse-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/ellipse-inactive.png" );
  btn_shapes.add_button( PF::Shape::rectangle, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rectangle-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rectangle-inactive.png" );
  btn_shapes.add_button( PF::Shape::polygon, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/polygon-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/polygon-inactive.png" );

  hbox_shapes_type.pack_end( btn_shapes, Gtk::PACK_SHRINK );
  btn_shapes.signal_clicked.connect(sigc::mem_fun(*this, &ShapesConfigGUI::btn_shapes_type_clicked) );
  controlsBox.pack_start( hbox_shapes_type );
  
//  hbox_shapes_options.pack_start( show_outline_chk );
//  hbox_shapes_options.pack_start( lock_source_chk );
//  hbox_shapes_options.pack_start( lock_shapes_chk );
  
  hbox_shapes_options.pack_start( lock_cursor_mode_btn, Gtk::PACK_SHRINK, 5 );
  lock_cursor_mode_btn.set_tooltip_text( _("returns to cursor mode after adding a shape") );
  hbox_shapes_options.pack_start( show_outline_btn, Gtk::PACK_SHRINK, 5 );
  show_outline_btn.set_tooltip_text( _("shows/hide the outline arround the shape") );
  hbox_shapes_options.pack_start( lock_source_btn, Gtk::PACK_SHRINK, 5 );
  lock_source_btn.set_tooltip_text( _("sets the source shape at fixed position or relative to the destination") );
  hbox_shapes_options.pack_start( lock_shapes_btn, Gtk::PACK_SHRINK, 5 );
  lock_shapes_btn.set_tooltip_text( _("prevents the shapes or nodes to be deleted") );
  controlsBox.pack_start( hbox_shapes_options );

  controlsBox.pack_start( separator, Gtk::PACK_SHRINK, 10 );
  
  vbox_shape_values.pack_start( pen_size_sl );
  vbox_shape_values.pack_start( opacity_sl );
  vbox_shape_values.pack_start( falloff_sl );
  vbox_shape_values.pack_start( radius_sl );
  vbox_shape_values.pack_start( radius_x_sl );
  vbox_shape_values.pack_start( radius_y_sl );
  vbox_shape_values.pack_start( angle_sl );
  controlsBox.pack_start( vbox_shape_values );

  controlsBox.pack_start( separator2, Gtk::PACK_SHRINK, 10 );
  
  controlsBox.pack_start( vbox_curves_prev );
  
  hbox_curves.pack_start( falloffCurveEditor, Gtk::PACK_SHRINK );
  controlsBox.pack_start( hbox_curves );
  falloffCurveEditor.show();

  add_widget( controlsBox );
  
  falloff_sl.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::falloff_sl_changed));
  opacity_sl.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::opacity_sl_changed));
  pen_size_sl.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::pen_size_sl_changed));
  radius_sl.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::radius_sl_changed));
  radius_x_sl.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::radius_x_sl_changed));
  radius_y_sl.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::radius_y_sl_changed));
  angle_sl.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::angle_sl_changed));

  show_outline_btn.signal_activated.connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::show_outline_btn_changed));
  show_outline_btn.signal_deactivated.connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::show_outline_btn_changed));

  mo_current_shape_type = PF::Shape::shape;
  
  mo_hit_test = PF::Shape::hit_none;
  mo_shape_index = -1;
  mo_shape_additional = -1;
  mo_shape_node_selected = -1;
  mo_node_selected = -1;

  editting = false;
  mo_dragging = false;
  mo_adding = PF::Shape::shape;
  
}

#define CALLBACK_BEGIN() \
/* Retrieve the layer associated to the filter */ \
PF::Layer* layer = get_layer(); \
if( !layer ) return; \
\
/* Retrieve the image the layer belongs to */ \
PF::Image* image = layer->get_image(); \
if( !image ) return; \
\
/* Retrieve the pipeline #0 (full resolution preview) */ \
PF::Pipeline* pipeline = image->get_pipeline( 0 ); \
if( !pipeline ) return; \
\
/* Find the pipeline node associated to the current layer */ \
PF::PipelineNode* node = pipeline->get_node( layer->get_id() ); \
if( !node ) return; \
if( !node->image ) return; \
\
PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par()); \
if( !par ) return; \

void PF::ShapesConfigGUI::falloff_sl_changed()
{
  if( get_editting() ) return;
  
  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < shapes_selected.size(); i++) {
    Shape* shape = par->get_shape(shapes_selected[i]);
    shape->set_falloff(get_falloff());
    modified = true;
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}

void PF::ShapesConfigGUI::opacity_sl_changed()
{
  if( get_editting() ) return;

  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < shapes_selected.size(); i++) {
    Shape* shape = par->get_shape(shapes_selected[i]);
    shape->set_opacity(get_opacity());
    modified = true;
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}

void PF::ShapesConfigGUI::pen_size_sl_changed()
{
  if( get_editting() ) return;

  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < shapes_selected.size(); i++) {
    Shape* shape = par->get_shape(shapes_selected[i]);
    shape->set_pen_size(get_pen_size());
    modified = true;
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}

void PF::ShapesConfigGUI::radius_sl_changed()
{
  if( get_editting() ) return;

  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < shapes_selected.size(); i++) {
    Shape* shape = par->get_shape(shapes_selected[i]);
    if (shape->get_type() == PF::Shape::circle) {
      Circle* circle = dynamic_cast<Circle*>(shape);
      circle->set_radius(get_radius());
      modified = true;
    }
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}

void PF::ShapesConfigGUI::radius_x_sl_changed()
{
  if( get_editting() ) return;

  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < shapes_selected.size(); i++) {
    Shape* shape = par->get_shape(shapes_selected[i]);
    if (shape->get_type() == PF::Shape::ellipse) {
      Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
      ellipse->set_radius_x(get_radius_x());
      modified = true;
    }
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}

void PF::ShapesConfigGUI::radius_y_sl_changed()
{
  if( get_editting() ) return;

  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < shapes_selected.size(); i++) {
    Shape* shape = par->get_shape(shapes_selected[i]);
    if (shape->get_type() == PF::Shape::ellipse) {
      Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
      ellipse->set_radius_y(get_radius_y());
      modified = true;
    }
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}

void PF::ShapesConfigGUI::angle_sl_changed()
{
  if( get_editting() ) return;

  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < shapes_selected.size(); i++) {
    Shape* shape = par->get_shape(shapes_selected[i]);
    if (shape->get_type() == PF::Shape::ellipse) {
      shape->set_angle(get_angle());
      modified = true;
    }
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}

void PF::ShapesConfigGUI::show_outline_btn_changed()
{
  CALLBACK_BEGIN();
  
  if ( !get_show_outline() ) {
    selection_clear();
    
    mo_hit_test = PF::Shape::hit_none;
    mo_shape_index = -1;
    mo_shape_additional = -1;
    mo_shape_node_selected = -1;
    mo_node_selected = -1;
  }
  
  par->shapes_modified();
  image->update();

}

void PF::ShapesConfigGUI::shape_expanded(Shape* shape)
{
  if (shape->get_type() == PF::Shape::circle) {
    Circle* circle = dynamic_cast<Circle*>(shape);
    radius_sl.get_adjustment()->set_value(circle->get_radius());
  }
  else if (shape->get_type() == PF::Shape::ellipse) {
    Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
    radius_x_sl.get_adjustment()->set_value(ellipse->get_radius_x());
    radius_y_sl.get_adjustment()->set_value(ellipse->get_radius_y());
  }
}

void PF::ShapesConfigGUI::pt_screen2image(Point& pt)
{
  double x = pt.get_x(), y = pt.get_y(), w = 1, h = 1;
  screen2image( x, y, w, h );
//  screen2layer( x, y, w, h );
  pt.set( x, y );
}

void PF::ShapesConfigGUI::pt_image2screen(Point& pt)
{
  double x = pt.get_x(), y = pt.get_y(), w = 1, h = 1;
  image2screen( x, y, w, h );
//  layer2screen( x, y, w, h );
  pt.set( x, y );
}

void PF::ShapesConfigGUI::draw_shape(Shape* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
                                      int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  if (shape->get_type() == PF::Shape::line) {
    Line* line = dynamic_cast<Line*>(shape);
    draw_line(line, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
  }
  else if (shape->get_type() == PF::Shape::circle) {
    Circle* circle = dynamic_cast<Circle*>(shape);
    draw_circle(circle, buf_in, buf_out, hit_t, hit_additional, selected);
  }
  else if (shape->get_type() == PF::Shape::ellipse) {
    Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
    draw_ellipse(ellipse, buf_in, buf_out, hit_t, hit_additional, selected);
  }
  else if (shape->get_type() == PF::Shape::rectangle) {
    Rectangle* rect = dynamic_cast<Rectangle*>(shape);
    draw_rectangle(rect, buf_in, buf_out, hit_t, hit_additional, selected);
  }
  else if (shape->get_type() == PF::Shape::polygon) {
    Polygon* polygon = dynamic_cast<Polygon*>(shape);
    draw_polygon(polygon, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
  }
  else {
    std::cout<<"PF::ShapesConfigGUI::draw_shape(): invalid shape type: "<<shape->get_type()<<std::endl;
  }
}

void PF::ShapesConfigGUI::draw_node(int x, int y, PF::PixelBuffer& buf_out, bool active_point)
{
  const int point_size = 2;
  const int point_size2 = point_size*2;
  
  VipsRect point1 = { x-point_size-1, y-point_size-1,
      point_size2+3, point_size2+3};
  VipsRect point2 = { x-point_size,   y-point_size,
      point_size2+1, point_size2+1};
  buf_out.fill( point1, 0, 0, 0 );
  if( active_point )
    buf_out.fill( point2, 255, 0, 0 );
  else
    buf_out.fill( point2, 255, 255, 255 );
  
}


void PF::ShapesConfigGUI::draw_line(Point& pt1, Point& pt2, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  if (hit_tested) {
    buf_out.draw_line( pt1.x, pt1.y, pt2.x, pt2.y, 255, 0, 0 );
  }
  else if (selected) {
    buf_out.draw_line( pt1.x, pt1.y, pt2.x, pt2.y, 0, 0, 255 );
  }
  else {
    buf_out.draw_line( pt1.x, pt1.y, pt2.x, pt2.y, buf_in );
  }
}

void PF::ShapesConfigGUI::draw_circle(Point& pt1, int radius, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
                                        bool hit_tested, bool selected)
{
  if (hit_tested) {
    buf_out.draw_circle( pt1.x, pt1.y, radius, 255, 0, 0 );
  }
  else if (selected) {
    buf_out.draw_circle( pt1.x, pt1.y, radius, 0, 0, 255 );
  }
  else {
    buf_out.draw_circle( pt1.x, pt1.y, radius, buf_in );
  }
}

void PF::ShapesConfigGUI::draw_ellipse(Point& pt1, int radius_x, int radius_y, float angle, int quadrant, 
                                        PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  if (hit_tested) {
    buf_out.draw_ellipse( pt1.x, pt1.y, radius_x, radius_y, angle, 255, 0, 0, quadrant );
  }
  else if (selected) {
    buf_out.draw_ellipse( pt1.x, pt1.y, radius_x, radius_y, angle, 0, 0, 255, quadrant );
  }
  else {
    buf_out.draw_ellipse( pt1.x, pt1.y, radius_x, radius_y, angle, buf_in, quadrant );
  }
}

void PF::ShapesConfigGUI::draw_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, 
    PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  std::vector<Point> points;
  PF::Line::inter_bezier4(anchor1, anchor2, control1, control2, points);
  for (int i = 1; i < points.size(); i++) {
    draw_line(points[i-1], points[i], buf_in, buf_out, hit_tested, selected);
 }
}

void PF::ShapesConfigGUI::draw_bezier3(Point& anchor1, Point& anchor2, Point& control, 
    PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  std::vector<Point> points;
  PF::Line::inter_bezier3(anchor1, anchor2, control, points);
    for (int i = 1; i < points.size(); i++) {
      draw_line(points[i-1], points[i], buf_in, buf_out, hit_tested, selected);
   }
}

void PF::ShapesConfigGUI::draw_circle(Circle* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected)
{
  Point pt1, pt2;
  
  pt1 = shape->get_point();
  pt2.set(shape->get_radius(), 0);

  pt_image2screen( pt1 );
  pt_image2screen( pt2 );
  
/*  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source)
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), 255, 0, 0 );
  else
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), buf_in );*/
  draw_circle( pt1, pt2.get_x(), buf_in, buf_out, (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source), selected );

  if (shape->get_falloff() > 0) {
    pt2.set(shape->get_radius() + shape->get_falloff(), 0);
    pt_image2screen( pt2 );
  
/*    if (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source)
      buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), 255, 0, 0 );
    else
      buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), buf_in );*/
    draw_circle( pt1, pt2.get_x(), buf_in, buf_out, (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source), selected );
  }
  
  if (shape->get_has_source()) {
    Circle source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_circle(&source, buf_in, buf_out,  hit_t, hit_additional, selected);
  }
}

void PF::ShapesConfigGUI::draw_ellipse(Ellipse* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected)
{
  Point center, radius;
  int node_selected;
  const int falloff_x = shape->get_falloff_x();
  const int falloff_y = shape->get_falloff_y();
  
  center = shape->get_point();
  radius.set(shape->get_radius_x(), shape->get_radius_y());

  pt_image2screen( center );
  pt_image2screen( radius );
  
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source) {
//    buf_out.draw_ellipse( center.get_x(), center.get_y(), radius.get_x(), radius.get_y(), shape->get_angle(), 255, 0, 0, -1 );
    draw_ellipse(center, radius.get_x(), radius.get_y(), shape->get_angle(), -1, buf_in, buf_out, true, selected);
  }
  else if (hit_t == PF::Shape::hit_segment ) {
    for (int i = 0; i <= 3; i++) {
/*      if ( hit_additional == i )
        buf_out.draw_ellipse( center.get_x(), center.get_y(), radius.get_x(), radius.get_y(), shape->get_angle(), 255, 0, 0, i );
      else
        buf_out.draw_ellipse( center.get_x(), center.get_y(), radius.get_x(), radius.get_y(), shape->get_angle(), buf_in, i );*/
      draw_ellipse(center, radius.get_x(), radius.get_y(), shape->get_angle(), i, buf_in, buf_out, ( hit_additional == i ), selected);
    }
  }
  else {
//    buf_out.draw_ellipse( center.get_x(), center.get_y(), radius.get_x(), radius.get_y(), shape->get_angle(), buf_in, -1 );
    draw_ellipse(center, radius.get_x(), radius.get_y(), shape->get_angle(), -1, buf_in, buf_out, false, selected);
  }
  
  if (falloff_x > 0) {
    Point radiusf;
    radiusf.set(shape->get_radius_x() + falloff_x, shape->get_radius_y() + falloff_y);
    pt_image2screen( radiusf );
  
/*    if (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source)
      buf_out.draw_ellipse( center.get_x(), center.get_y(), radiusf.get_x(), radiusf.get_y(), shape->get_angle(), 255, 0, 0, -1 );
    else
      buf_out.draw_ellipse( center.get_x(), center.get_y(), radiusf.get_x(), radiusf.get_y(), shape->get_angle(), buf_in, -1 );*/
    draw_ellipse(center, radius.get_x(), radius.get_y(), shape->get_angle(), -1, buf_in, buf_out, (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source), selected);
  }
  
  // draw nodes
  if (hit_t != PF::Shape::hit_none) {
    if (hit_t == PF::Shape::hit_node) {
      node_selected = hit_additional+1;
    }
    else
      node_selected = 0;
    
    if ( shape->get_angle() == 0.f ) {
      draw_node(center.get_x(), center.get_y()-radius.get_y(), buf_out, (node_selected==1));
      draw_node(center.get_x()+radius.get_x(), center.get_y(), buf_out, (node_selected==2));
      draw_node(center.get_x(), center.get_y()+radius.get_y(), buf_out, (node_selected==3));
      draw_node(center.get_x()-radius.get_x(), center.get_y(), buf_out, (node_selected==4));
    }
    else {
      const float _angle = (360.f-shape->get_angle())*M_PI/180.f;
      const float sin_angle = sinf(_angle);
      const float cos_angle = cosf(_angle);

      Point pt;
      
      pt.set( center.get_x(), center.get_y()-radius.get_y() );
      pt.rotate(center, sin_angle, cos_angle);
      draw_node(pt.get_x(), pt.get_y(), buf_out, (node_selected==1));
      
      pt.set( center.get_x()+radius.get_x(), center.get_y() );
      pt.rotate(center, sin_angle, cos_angle);
      draw_node(pt.get_x(), pt.get_y(), buf_out, (node_selected==2));
      
      pt.set( center.get_x(), center.get_y()+radius.get_y() );
      pt.rotate(center, sin_angle, cos_angle);
      draw_node(pt.get_x(), pt.get_y(), buf_out, (node_selected==3));
      
      pt.set( center.get_x()-radius.get_x(), center.get_y() );
      pt.rotate(center, sin_angle, cos_angle);
      draw_node(pt.get_x(), pt.get_y(), buf_out, (node_selected==4));
    }
  }

  if (shape->get_has_source()) {
    Ellipse source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_ellipse(&source, buf_in, buf_out,  hit_t, hit_additional, selected);
  }
}

void PF::ShapesConfigGUI::draw_rectangle(Rectangle* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected)
{
  Polygon* polygon = dynamic_cast<Polygon*>(shape);
  draw_polygon(polygon, buf_in, buf_out, hit_t, hit_additional, -1, selected);
  return;
/*  
  Point pt1, pt2;
  VipsRect rc;
  int selected;

  // draw the shape
  shape->get_rect(&rc);
  
  pt1 = shape->get_point(0);
  pt2 = shape->get_point(1);
  pt_image2screen( pt1 );
  pt_image2screen( pt2 );
  
  selected = -1;
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source) {
    selected = 0;
  }
  else if (hit_t == PF::Shape::hit_segment) {
    selected = hit_additional+1;
  }
  
  if (selected == 1 || selected == 0) 
    buf_out.draw_line( pt1.get_x(), pt1.get_y(), pt2.get_x(), pt1.get_y(), 255, 0, 0 );
  else
    buf_out.draw_line( pt1.get_x(), pt1.get_y(), pt2.get_x(), pt1.get_y(), buf_in );
  if (selected == 2 || selected == 0)   
    buf_out.draw_line( pt2.get_x(), pt1.get_y(), pt2.get_x(), pt2.get_y(), 255, 0, 0 );
  else
    buf_out.draw_line( pt2.get_x(), pt1.get_y(), pt2.get_x(), pt2.get_y(), buf_in );
  if (selected == 3 || selected == 0)   
    buf_out.draw_line( pt2.get_x(), pt2.get_y(), pt1.get_x(), pt2.get_y(), 255, 0, 0 );
  else
    buf_out.draw_line( pt2.get_x(), pt2.get_y(), pt1.get_x(), pt2.get_y(), buf_in );
  if (selected == 4 || selected == 0)
    buf_out.draw_line( pt1.get_x(), pt2.get_y(), pt1.get_x(), pt1.get_y(), 255, 0, 0 );
  else 
    buf_out.draw_line( pt1.get_x(), pt2.get_y(), pt1.get_x(), pt1.get_y(), buf_in );
  
  // shape nodes
  if (hit_t != PF::Shape::hit_none) {
      if (hit_t == PF::Shape::hit_node) {
      selected = hit_additional+1;
    }
    else
      selected = 0;
    
    draw_node(pt1.get_x(), pt1.get_y(), buf_out, (selected==1));
    draw_node(pt2.get_x(), pt1.get_y(), buf_out, (selected==2));
    draw_node(pt2.get_x(), pt2.get_y(), buf_out, (selected==3));
    draw_node(pt1.get_x(), pt2.get_y(), buf_out, (selected==4));
  }

  //draw the falloff
  if (shape->get_falloff_point(0).get_x() > 0 || shape->get_falloff_point(0).get_y() > 0 || 
      shape->get_falloff_point(1).get_x() > 0 || shape->get_falloff_point(1).get_y() > 0) {
    shape->get_falloff_point_absolute(0, pt1);
    shape->get_falloff_point_absolute(1, pt2);
    
    pt_image2screen( pt1 );
    pt_image2screen( pt2 );
  
    selected = -1;
    if (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source) {
      selected = 0;
    }
    else if (hit_t == PF::Shape::hit_falloff_segment) {
      selected = hit_additional+1;
    }
    
    if (selected == 1 || selected == 0) 
      buf_out.draw_line( pt1.get_x(), pt1.get_y(), pt2.get_x(), pt1.get_y(), 255, 0, 0 );
    else
      buf_out.draw_line( pt1.get_x(), pt1.get_y(), pt2.get_x(), pt1.get_y(), buf_in );
    if (selected == 2 || selected == 0)   
      buf_out.draw_line( pt2.get_x(), pt1.get_y(), pt2.get_x(), pt2.get_y(), 255, 0, 0 );
    else
      buf_out.draw_line( pt2.get_x(), pt1.get_y(), pt2.get_x(), pt2.get_y(), buf_in );
    if (selected == 3 || selected == 0)   
      buf_out.draw_line( pt2.get_x(), pt2.get_y(), pt1.get_x(), pt2.get_y(), 255, 0, 0 );
    else
      buf_out.draw_line( pt2.get_x(), pt2.get_y(), pt1.get_x(), pt2.get_y(), buf_in );
    if (selected == 4 || selected == 0)
      buf_out.draw_line( pt1.get_x(), pt2.get_y(), pt1.get_x(), pt1.get_y(), 255, 0, 0 );
    else 
      buf_out.draw_line( pt1.get_x(), pt2.get_y(), pt1.get_x(), pt1.get_y(), buf_in );
  
    // falloff nodes
    if (hit_t != PF::Shape::hit_none) {
      if (hit_t == PF::Shape::hit_falloff_node) {
        selected = hit_additional+1;
      }
      else
        selected = 0;
      
      draw_node(pt1.get_x(), pt1.get_y(), buf_out, (selected==1));
      draw_node(pt2.get_x(), pt1.get_y(), buf_out, (selected==2));
      draw_node(pt2.get_x(), pt2.get_y(), buf_out, (selected==3));
      draw_node(pt1.get_x(), pt2.get_y(), buf_out, (selected==4));
  
    }
  }
  
  if (shape->get_has_source()) {
    Rectangle source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_rectangle(&source, buf_in, buf_out, hit_t, hit_additional);
  }
  */
}

void PF::ShapesConfigGUI::draw_node_control_points(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
    int node, int cntrl1_selected, int cntrl2_selected, int selected)
{
  Point pt1, pt2;
  Point ptc1, ptc2;
  PF::SegmentInfo si = shape->get_segment_info(node);
  if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier4) {
    pt1 = shape->get_point(node);
    pt_image2screen( pt1 );
    
    ptc1 = si.get_control_pt1();
    pt_image2screen( ptc1 );
    
    draw_node(ptc1.get_x(), ptc1.get_y(), buf_out, (cntrl1_selected==node));
    draw_line(pt1, ptc1, buf_in, buf_out, false, selected);
  }
  
  int node_prev = (node > 0) ? node-1: shape->get_points_count()-1;
  si = shape->get_segment_info(node_prev);
  if (si.get_segment_type() == PF::SegmentInfo::bezier3_r || si.get_segment_type() == PF::SegmentInfo::bezier4) {
    pt1 = shape->get_point(node);
    pt_image2screen( pt1 );
/*    if (si.get_segment_type() == PF::SegmentInfo::bezier3_r)
      ptc1 = si.get_control_pt1();
    else*/
      ptc1 = si.get_control_pt2();
    pt_image2screen( ptc1 );
    
    draw_node(ptc1.get_x(), ptc1.get_y(), buf_out, (cntrl2_selected==node_prev));
    draw_line(pt1, ptc1, buf_in, buf_out, false, selected);
  }

}

void PF::ShapesConfigGUI::draw_polygon(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
                                        int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  Point pt1, pt2, ptc1, ptc2;
  SegmentInfo si;
  Point a, b, c, d;
/*  int shape_dist = shape->get_size();
  pt1.set(shape_dist, shape_dist);
  pt_image2screen( pt1 );
  shape_dist = pt1.get_x();
  int fall_dist = shape->get_size() + shape->get_falloff();
  pt1.set(fall_dist, fall_dist);
  pt_image2screen( pt1 );
  fall_dist = pt1.get_x();
*/
  
  // hit_test for highlighted lines
  int segm_selected = -2;
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source) {
    segm_selected = -1;
  }
  else if (hit_t == PF::Shape::hit_segment) {
    segm_selected = hit_additional;
  }
  int fall_selected = -2;
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source || hit_t == PF::Shape::hit_falloff_segment) {
    fall_selected = -1;
  }
  
  int node_selected = -1;
  if (hit_t == PF::Shape::hit_node) 
    node_selected = hit_additional;

  int cntrl1_selected = -1;
  if (hit_t == PF::Shape::hit_control_point) 
    cntrl1_selected = hit_additional;

  int cntrl2_selected = -1;
  if (hit_t == PF::Shape::hit_control_point_end) 
    cntrl2_selected = hit_additional;

  for (int i = 0; i < shape->get_segments_count(); i++) {
    shape->get_segment(i, pt1, pt2, si);
    
    pt_image2screen( pt1 );
    pt_image2screen( pt2 );
    
    // draw segment
    if (si.get_segment_type() == PF::SegmentInfo::line) {
      draw_line(pt1, pt2, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
    }
    else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
      ptc1 = si.get_control_pt1();
      pt_image2screen( ptc1 );
      draw_bezier3(pt1, pt2, ptc1, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
    }
    else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
      ptc1 = si.get_control_pt1();
      pt_image2screen( ptc1 );
      ptc2 = si.get_control_pt2();
      pt_image2screen( ptc2 );
      draw_bezier4(pt1, pt2, ptc1, ptc2, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
    }
  }
  
  // draw nodes
  if (hit_t != PF::Shape::hit_none) {
    for (int i = shape->get_points_count(); i >= 0 ; i--) {
      pt1 = shape->get_point(i);
      pt_image2screen( pt1 );
  
      draw_node(pt1.get_x(), pt1.get_y(), buf_out, (node_selected==i));
    }
  }
  
  // draw control points
  if (hit_node_selected >= 0) {
    draw_node_control_points(shape, buf_in, buf_out, hit_node_selected, cntrl1_selected, cntrl2_selected, selected);
  }
  
  if (shape->get_has_source()) {
    Polygon source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_polygon(&source, buf_in, buf_out, hit_t, hit_additional, -1, selected);
  }

}

void PF::ShapesConfigGUI::draw_line(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
                                      int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  Point pt1, pt2, ptc1, ptc2;
  SegmentInfo si;
  Point a, b, c, d;
/*  int shape_dist = shape->get_size();
  pt1.set(shape_dist, shape_dist);
  pt_image2screen( pt1 );
  shape_dist = pt1.get_x();
  int fall_dist = shape->get_size() + shape->get_falloff();
  pt1.set(fall_dist, fall_dist);
  pt_image2screen( pt1 );
  fall_dist = pt1.get_x();
*/
  
  // hit_test for highlighted lines
  int segm_selected = -2;
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source) {
    segm_selected = -1;
  }
  else if (hit_t == PF::Shape::hit_segment) {
    segm_selected = hit_additional;
  }
  int fall_selected = -2;
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source || hit_t == PF::Shape::hit_falloff_segment) {
    fall_selected = -1;
  }
  
  int node_selected = -1;
  if (hit_t == PF::Shape::hit_node) 
    node_selected = hit_additional;

  int cntrl1_selected = -1;
  if (hit_t == PF::Shape::hit_control_point) 
    cntrl1_selected = hit_additional;

  int cntrl2_selected = -1;
  if (hit_t == PF::Shape::hit_control_point_end) 
    cntrl2_selected = hit_additional;

  for (int i = 0; i < shape->get_segments_count(); i++) {
    shape->get_segment(i, pt1, pt2, si);
    
    pt_image2screen( pt1 );
    pt_image2screen( pt2 );
    
    // draw segment
    if (si.get_segment_type() == PF::SegmentInfo::line) {
      draw_line(pt1, pt2, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
    }
    else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
      ptc1 = si.get_control_pt1();
      pt_image2screen( ptc1 );
      draw_bezier3(pt1, pt2, ptc1, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
    }
    else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
      ptc1 = si.get_control_pt1();
      pt_image2screen( ptc1 );
      ptc2 = si.get_control_pt2();
      pt_image2screen( ptc2 );
      draw_bezier4(pt1, pt2, ptc1, ptc2, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
    }
  }
  
  // draw nodes
  if (hit_t != PF::Shape::hit_none) {
    for (int i = shape->get_points_count(); i >= 0 ; i--) {
      pt1 = shape->get_point(i);
      pt_image2screen( pt1 );
  
      draw_node(pt1.get_x(), pt1.get_y(), buf_out, (node_selected==i));
    }
  }
  
  // draw control points
  if (hit_node_selected >= 0) {
    if (hit_node_selected == 0) {
      PF::SegmentInfo si = shape->get_segment_info(hit_node_selected);
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier4) {
        pt1 = shape->get_point(hit_node_selected);
        pt_image2screen( pt1 );
        
        ptc1 = si.get_control_pt1();
        pt_image2screen( ptc1 );
        
        draw_node(ptc1.get_x(), ptc1.get_y(), buf_out, (cntrl1_selected==hit_node_selected));
        draw_line(pt1, ptc1, buf_in, buf_out, false, selected);
      }
    }
    else if (hit_node_selected == shape->get_points_count()-1) {
      PF::SegmentInfo si = shape->get_segment_info(hit_node_selected-1);
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_r || si.get_segment_type() == PF::SegmentInfo::bezier4) {
        pt1 = shape->get_point(hit_node_selected);
        pt_image2screen( pt1 );
        
        if (si.get_segment_type() == PF::SegmentInfo::bezier3_r)
          ptc1 = si.get_control_pt1();
        else
          ptc1 = si.get_control_pt2();
        pt_image2screen( ptc1 );
        
        draw_node(ptc1.get_x(), ptc1.get_y(), buf_out, (cntrl2_selected==hit_node_selected-1));
        draw_line(pt1, ptc1, buf_in, buf_out, false, selected);
      }
    }
    else {
      draw_node_control_points(shape, buf_in, buf_out, hit_node_selected, cntrl1_selected, cntrl2_selected, selected);
    }
  }
  
  if (shape->get_has_source()) {
    Polygon source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_polygon(&source, buf_in, buf_out, hit_t, hit_additional, -1, selected);
  }

}

void PF::ShapesConfigGUI::btn_shapes_type_clicked()
{
  mo_current_shape_type = btn_shapes.get_btn_active();
  
  selection_clear();
}

void PF::ShapesConfigGUI::parameters_reset()
{
  PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par());
//  if( par ) par->shape_reset();
  OperationConfigGUI::parameters_reset();
}

void PF::ShapesConfigGUI::do_update()
{
  PF::OperationConfigGUI::do_update();
  
  selection_changed();
}


void PF::ShapesConfigGUI::add_new_shape(PF::ShapesPar* par, int shape_type, Point& initial_pos, Point& source_pos)
{
  switch (shape_type)
  {
  case PF::Shape::polygon:
  {
    std::cout<<"PF::ShapesConfigGUI::add_new_shape(): PF::Shape::polygon "<<std::endl;
    
    Polygon shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_has_source(), source_pos );
    shape.set_color(par->get_shapes_group().get_current_color());
    
    polygon_add = shape; // set the shape to be added
    mo_adding = PF::Shape::polygon; // we are adding a new shape
  }
   break;
  case PF::Shape::line:
  {
    Line shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_has_source(), source_pos );
    shape.set_color(par->get_shapes_group().get_current_color());
    
    line_add = shape; // set the shape to be added
    mo_adding = PF::Shape::line; // we are adding a new shape
  }
   break;
  case PF::Shape::circle:
    {
      Circle shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_has_source(), source_pos );
      shape.set_color(par->get_shapes_group().get_current_color());
      shape.set_radius(get_radius());
      
      mo_shape_index = par->add_shape(&shape);
      
      mo_hit_test = PF::Shape::hit_shape;
    }
    break;
  case PF::Shape::ellipse:
    {
      Ellipse shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_has_source(), source_pos );
      shape.set_color(par->get_shapes_group().get_current_color());
      shape.set_angle(get_angle());
      shape.set_radius_x(get_radius_x());
      shape.set_radius_y(get_radius_y());

      mo_shape_index = par->add_shape(&shape);
      
      mo_hit_test = PF::Shape::hit_shape;
    }
    break;
  case PF::Shape::rectangle:
  {
    Rectangle shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_has_source(), source_pos );
    shape.set_color(par->get_shapes_group().get_current_color());
    
    mo_shape_index = par->add_shape(&shape);
    
    mo_hit_test = PF::Shape::hit_node;
    mo_shape_additional = 2;

    mo_dragging = true;
  }
   break;
  default:
    std::cout<<"PF::ShapesConfigGUI::add_new_shape(): invalid shape type: "<<shape_type<<std::endl;
    break;
  }
}

void PF::ShapesConfigGUI::selection_changed()
{
  bool circle_selected = false;
  bool ellipse_selected = false;
  
  PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par());
  if( par ) {
  
    for (int i = 0; i < shapes_selected.size(); i++ ) {
        Shape* shape = par->get_shape(shapes_selected[i]);
        
//        set_pen_size(shape->get_pen_size());
//        set_falloff(shape->get_falloff());
//        set_opacity(shape->get_opacity());
//        set_angle(shape->get_angle());
      
        if (shape->get_type() == PF::Shape::line) {
          Line* line = dynamic_cast<Line*>(shape);
        }
        else if (shape->get_type() == PF::Shape::circle) {
          Circle* circle = dynamic_cast<Circle*>(shape);
          set_radius( circle->get_radius() );
          circle_selected = true;
        }
        else if (shape->get_type() == PF::Shape::ellipse) {
          Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
          set_radius_x( ellipse->get_radius_x() );
          set_radius_y( ellipse->get_radius_y() );
          set_angle( ellipse->get_angle() );
          ellipse_selected = true;
        }
        else if (shape->get_type() == PF::Shape::rectangle) {
          Rectangle* rect = dynamic_cast<Rectangle*>(shape);
        }
        else if (shape->get_type() == PF::Shape::polygon) {
          Polygon* polygon = dynamic_cast<Polygon*>(shape);
        }
        else {
          std::cout<<"PF::ShapesConfigGUI::draw_shape(): invalid shape type: "<<shape->get_type()<<std::endl;
        }
      }
  }

  if ( circle_selected || get_current_shape_type() == PF::Shape::circle )
    radius_sl.show();
  else
    radius_sl.hide();
  
  if ( ellipse_selected || get_current_shape_type() == PF::Shape::ellipse ) {
    radius_x_sl.show();
    radius_y_sl.show();
    angle_sl.show();
  }
  else {
    radius_x_sl.hide();
    radius_y_sl.hide();
    angle_sl.hide();
  }

}

void PF::ShapesConfigGUI::selection_replace(int n)
{
  selection_clear();
  selection_add(n);
  
  selection_changed();
}

void PF::ShapesConfigGUI::selection_add(int n)
{
  shapes_selected.push_back(n);
  selection_changed();
}

void PF::ShapesConfigGUI::selection_remove(int n)
{
  for (int i=0; i<shapes_selected.size(); i++) {
    if (shapes_selected[i] == n) {
      shapes_selected.erase(shapes_selected.begin()+i);
    }
  }
  
  selection_changed();
}

bool PF::ShapesConfigGUI::is_shape_selected(int n)
{
  bool is_selected = false;
  int i=0;
  while ( i<shapes_selected.size() && !is_selected ) {
    is_selected = (shapes_selected[i] == n);

    i++;
  }

  return is_selected;
}

void PF::ShapesConfigGUI::adjust_polygon_falloff_point(Polygon* shape, int n)
{
  Point center, ptf;
  Point& pt = shape->get_point(n);
  shape->get_center(center);
    
  if ( pt.get_x() < center.get_x() )
    ptf.set_x(-get_falloff());
  else
    ptf.set_x(get_falloff());
  
  if ( pt.get_y() < center.get_y() )
    ptf.set_y(-get_falloff());
  else
    ptf.set_y(get_falloff());
  
  shape->set_falloff_point(n, ptf);
  
}

void PF::ShapesConfigGUI::defalut_new_polygon_control_point(int point)
{
  Point ptc;

    if ( polygon_add.get_segment_type(point-1) == PF::SegmentInfo::bezier4 ) {
      ptc.line_mid_point(polygon_add.get_point(point-1), polygon_add.get_point(point));
      int amount = std::sqrt(ptc.distance2(polygon_add.get_point(point)))/2;
      if (ptc != polygon_add.get_point(point))
        PF::Shape::get_pt_proyected_from_segment(ptc, polygon_add.get_point(point-1), amount, ptc);
      else
        ptc.offset(amount, amount);

      polygon_add.set_control_pt2( point-1, ptc );
      
      if ( polygon_add.get_segment_type(point) == PF::SegmentInfo::bezier4 || polygon_add.get_segment_type(point) == PF::SegmentInfo::bezier3_l )
        polygon_add.synch_control_pt1(point, ptc, polygon_add.get_point(point));
    }
    else if ( polygon_add.get_segment_type(point) == PF::SegmentInfo::bezier4 || polygon_add.get_segment_type(point) == PF::SegmentInfo::bezier3_l ) {
      ptc.line_mid_point(polygon_add.get_point(point), polygon_add.get_point(point+1));
      int amount = -std::sqrt(ptc.distance2(polygon_add.get_point(point)))/2;
      if (ptc != polygon_add.get_point(point))
        PF::Shape::get_pt_proyected_from_segment(ptc, polygon_add.get_point(point+1), amount, ptc);
      else
        ptc.offset(amount, amount);

      polygon_add.set_control_pt1( point, ptc );
    }

}

void PF::ShapesConfigGUI::defalut_new_polygon(Point& pt, int mod_key )
{
  const int new_point = polygon_add.get_points_count()-1;
  const int new_segment = polygon_add.get_segments_count()-1;
  
  // set the new position of the new point
  polygon_add.get_point(new_point).set(pt);
  
  // set the new segment type based on the pressed key
  if (mod_key == PF::MOD_KEY_SHIFT) {
    polygon_add.set_segment_type(new_segment, PF::SegmentInfo::bezier3_l);
  }
  else if (mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
    polygon_add.set_segment_type(new_segment, PF::SegmentInfo::bezier4);
  }
  else {
    polygon_add.set_segment_type(new_segment, PF::SegmentInfo::line);
  }

  defalut_new_polygon_control_point(new_point-1);
  defalut_new_polygon_control_point(new_point);
  defalut_new_polygon_control_point(new_point+1);

}

void PF::ShapesConfigGUI::defalut_new_line_control_point(int point)
{
  Point ptc;

  if (point == 0) {
    ptc.line_mid_point(line_add.get_point(point), line_add.get_point(point+1));
    int amount = -std::sqrt(ptc.distance2(line_add.get_point(point)))/2;
    if (ptc != line_add.get_point(point))
      PF::Shape::get_pt_proyected_from_segment(ptc, line_add.get_point(point+1), amount, ptc);
    else
      ptc.offset(amount, amount);

    line_add.set_control_pt1( point, ptc );
  }
  else if (point == line_add.get_points_count()-1) {
    if ( line_add.get_segment_type(point-1) == PF::SegmentInfo::bezier4 ) {
      ptc.line_mid_point(line_add.get_point(point-1), line_add.get_point(point));
      int amount = std::sqrt(ptc.distance2(line_add.get_point(point)))/2;
      if (ptc != line_add.get_point(point))
        PF::Shape::get_pt_proyected_from_segment(ptc, line_add.get_point(point-1), amount, ptc);
      else
        ptc.offset(amount, amount);
  
      line_add.set_control_pt2( point-1, ptc );
    }
  }
  else {
    if ( line_add.get_segment_type(point-1) == PF::SegmentInfo::bezier4 ) {
      ptc.line_mid_point(line_add.get_point(point-1), line_add.get_point(point));
      int amount = std::sqrt(ptc.distance2(line_add.get_point(point)))/2;
      if (ptc != line_add.get_point(point))
        PF::Shape::get_pt_proyected_from_segment(ptc, line_add.get_point(point-1), amount, ptc);
      else
        ptc.offset(amount, amount);
  
      line_add.set_control_pt2( point-1, ptc );
      
      if ( line_add.get_segment_type(point) == PF::SegmentInfo::bezier4 || line_add.get_segment_type(point) == PF::SegmentInfo::bezier3_l )
        line_add.synch_control_pt1(point, ptc, line_add.get_point(point));
    }
    else if ( line_add.get_segment_type(point) == PF::SegmentInfo::bezier4 || line_add.get_segment_type(point) == PF::SegmentInfo::bezier3_l ) {
      ptc.line_mid_point(line_add.get_point(point), line_add.get_point(point+1));
      int amount = -std::sqrt(ptc.distance2(line_add.get_point(point)))/2;
      if (ptc != line_add.get_point(point))
        PF::Shape::get_pt_proyected_from_segment(ptc, line_add.get_point(point+1), amount, ptc);
      else
        ptc.offset(amount, amount);
  
      line_add.set_control_pt1( point, ptc );
    }
  }

}

void PF::ShapesConfigGUI::defalut_new_line(Point& pt, int mod_key )
{
  const int new_point = line_add.get_points_count()-1;
  const int new_segment = line_add.get_segments_count()-1;
  
  // set the new position of the new point
  line_add.get_point(new_point).set(pt);
  
  // set the new segment type based on the pressed key
  if (mod_key == PF::MOD_KEY_SHIFT) {
    line_add.set_segment_type(new_segment, PF::SegmentInfo::bezier3_l);
  }
  else if (mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
    line_add.set_segment_type(new_segment, PF::SegmentInfo::bezier4);
  }
  else {
    line_add.set_segment_type(new_segment, PF::SegmentInfo::line);
  }

  defalut_new_line_control_point(new_point-1);
  defalut_new_line_control_point(new_point);

}


// deletes the shape currently hit_tested based on the hit_test position
bool PF::ShapesConfigGUI::delete_shape(PF::ShapesPar* par)
{
  bool deleted = false;
  
  if ( mo_shape_index >= 0 ) {
    Shape* shape = par->get_shape(mo_shape_index);
    
    if ( (shape->get_type() != PF::Shape::line && (mo_hit_test == PF::Shape::hit_shape || mo_hit_test == PF::Shape::hit_falloff) ) ||
        (shape->get_type() == PF::Shape::line && (mo_hit_test == PF::Shape::hit_segment || mo_hit_test == PF::Shape::hit_falloff_segment)) ) {
      
      par->remove_shape(mo_shape_index);
      
/*      mo_hit_test = PF::Shape::hit_none;
      mo_shape_index = -1;
      mo_shape_additional = -1;
      
      mo_shape_node_selected = -1;
      mo_node_selected = -1;
*/      
      selection_clear();
      
      par->shapes_modified();
      
      deleted = true;
    }
  }
  
  return deleted;
}

// deletes all the shapes selected
bool PF::ShapesConfigGUI::selection_delete_shape(PF::ShapesPar* par)
{
  bool deleted = false;
  
  if ( get_selection_count() > 1 ) {
    // if more than one shape selected, click anywhere delete it
    if ( mo_hit_test != PF::Shape::hit_none ) {
      int i = 0;
      while ( i < shapes_selected.size()-1 ) {
        if ( shapes_selected[i] > shapes_selected[i+1] ) {
          std::swap( shapes_selected[i], shapes_selected[i+1] );
          if (i) i--; 
        }
        else {
          i++; 
        }
      }

      for ( i = shapes_selected.size()-1; i >= 0; i--) {
        par->remove_shape( shapes_selected[i] );
      }
      
/*      mo_hit_test = PF::Shape::hit_none;
      mo_shape_index = -1;
      mo_shape_additional = -1;
      
      mo_shape_node_selected = -1;
      mo_node_selected = -1;
*/      
      selection_clear();
      
      par->shapes_modified();
    }
  }
  else {
    deleted = delete_shape(par);
  }
  
  return deleted;
}

void PF::ShapesConfigGUI::done_adding_shape()
{
  if ( get_lock_cursor_mode() ) {
    btn_shapes.set_btn_active(PF::Shape::shape);
  }
}


bool PF::ShapesConfigGUI::handle_add_new_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  if ( mo_adding == PF::Shape::shape && button == 1 && mod_key != SHAPE_KEY_NOT_ADD_SHAPE && get_current_shape_type() != PF::Shape::shape ) {
    Point pt(pt_current);
    pt_screen2image( pt );
    Point pts(pt_source);

    if ( !get_lock_source() ) {
      if ( last_pt_origin.get_x() == -1 && last_pt_origin.get_y() == -1 )
        last_pt_origin.set(pt_current);
      else
        pts.offset(pt_current.x-last_pt_origin.get_x(), pt_current.y-last_pt_origin.get_y());
    }
    pts.offset(-pt_current.x, -pt_current.y); // source is a relative value
    pt_screen2image( pts );

    add_new_shape(par, get_current_shape_type(), pt, pts);

    switch (get_current_shape_type())
    {
    case PF::Shape::polygon:
      if (mod_key == PF::MOD_KEY_SHIFT) {
        polygon_add.set_segment_type(0, PF::SegmentInfo::bezier3_l);
      }
      else if (mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
        polygon_add.set_segment_type(0, PF::SegmentInfo::bezier4);
      }
      else {
        polygon_add.set_segment_type(0, PF::SegmentInfo::line);
      }

      defalut_new_polygon(pt, mod_key );
      break;
    case PF::Shape::line:
      if (mod_key == PF::MOD_KEY_SHIFT) {
        line_add.set_segment_type(0, PF::SegmentInfo::bezier3_l);
      }
      else if (mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
        line_add.set_segment_type(0, PF::SegmentInfo::bezier4);
      }
      else {
        line_add.set_segment_type(0, PF::SegmentInfo::line);
      }

      defalut_new_line(pt, mod_key );
      break;
    default:
      done_adding_shape();
      break;
    }

    par->shapes_modified();

    refresh = true;
    handled = true;
  }

  return handled;
}

bool PF::ShapesConfigGUI::handle_delete_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;
  
  // right click on a shape delete it
  if ( button == 3 ) {
    if ( mod_key == SHAPE_KEY_DELETE_SHAPE ) {
      handled = selection_delete_shape(par);
    }
  }

  if ( handled )
    update_hit_test(par, button, mod_key, sx, sy, refresh);
  
  return handled;
}

bool PF::ShapesConfigGUI::handle_delete_node(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  // right click on a node delete it
  if ( button == 3 ) {
    if ( mod_key == SHAPE_KEY_DELETE_NODE ) {
      if ( mo_shape_index >= 0 ) {
        Shape* shape = par->get_shape(mo_shape_index);

        if ( shape->get_type() == PF::Shape::polygon || shape->get_type() == PF::Shape::line ) {
          if (mo_hit_test == PF::Shape::hit_node || mo_hit_test == PF::Shape::hit_control_point || mo_hit_test == PF::Shape::hit_control_point_end) {
            shape->remove_point(mo_hit_test, mo_shape_additional);

            mo_shape_node_selected = -1;
            mo_node_selected = -1;

            par->shapes_modified();

            handled = true;
          }
        }
      }
    }
  }

  if ( handled )
    update_hit_test(par, button, mod_key, sx, sy, refresh);
  
  return handled;
}

bool PF::ShapesConfigGUI::handle_add_node(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  // left click on a segment adds a new node
  if ( button == 1 ) {
    if ( mod_key == SHAPE_KEY_ADD_NODE ) {
      if ( mo_hit_test == PF::Shape::hit_segment ) {
        if ( mo_shape_index < 0 || mo_shape_additional < 0 ) {
          std::cout<<"ShapesConfigGUI::handle_add_node(): add a node to an existing shape: invalid shape index: "<<mo_shape_index<<", "<<mo_shape_additional<<std::endl;
          return false;
        }

        Shape* shape = par->get_shape(mo_shape_index);

        if ( shape->get_type() == PF::Shape::polygon || shape->get_type() == PF::Shape::line ) {
          Point pt(pt_current);
          pt_screen2image( pt );

          //        std::cout<<"ShapesConfigGUI::pointer_press_event(): add a node to a line: mo_shape_additional: "<<mo_shape_additional<<std::endl;       
          shape->insert_point(pt, mo_hit_test, mo_shape_additional);

          par->shapes_modified();

          handled = true;
        }
      }
    }
  }

  if ( handled )
    update_hit_test(par, button, mod_key, sx, sy, refresh);
  
  return handled;
}

bool PF::ShapesConfigGUI::handle_add_control_point(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  // left click on a node adds a new control point
  if ( button == 1 ) {
    if ( mod_key == SHAPE_KEY_ADD_CONTROL_POINT ) {
      if ( mo_hit_test == PF::Shape::hit_node ) {
        if ( mo_shape_index < 0 || mo_shape_additional < 0 ) {
          std::cout<<"ShapesConfigGUI::handle_add_node(): add a control point to an existing shape: invalid shape index: "<<mo_shape_index<<", "<<mo_shape_additional<<std::endl;
          return false;
        }

        Shape* shape = par->get_shape(mo_shape_index);

        if ( shape->get_type() == PF::Shape::polygon || shape->get_type() == PF::Shape::line ) {
          Point pt(pt_current);
          pt_screen2image( pt );

          //        std::cout<<"ShapesConfigGUI::pointer_press_event(): add a node to a line: mo_shape_additional: "<<mo_shape_additional<<std::endl;       
          shape->insert_point(pt, mo_hit_test, mo_shape_additional);

          if ( shape->get_type() == PF::Shape::polygon )
            defalut_new_polygon_control_point(mo_shape_additional);
          else if ( shape->get_type() == PF::Shape::line )
            defalut_new_line_control_point(mo_shape_additional);

          par->shapes_modified();

          handled = true;
        }
      }
    }
  }

  return handled;
}

bool PF::ShapesConfigGUI::handle_selection(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  // handle selection only in cursor mode - no dragging - no adding - show outline
  if ( get_current_shape_type() == PF::Shape::shape && !mo_dragging && mo_adding == PF::Shape::shape && get_show_outline() ) {
    if ( (mo_hit_test != PF::Shape::hit_control_point && mo_hit_test != PF::Shape::hit_control_point_end) || 
        mo_shape_node_selected != mo_shape_index ) {
      mo_shape_node_selected = -1;
      mo_node_selected = -1;
    }
    
    if ( mo_hit_test == PF::Shape::hit_none ) {
      // mouse down and not a shape: clear selection
      if ( button == 1 && mod_key == PF::MOD_KEY_NONE ) {
        selection_clear();
      }
    }
    else if ( mo_hit_test == PF::Shape::hit_shape || mo_hit_test == PF::Shape::hit_falloff ) {
      // mouse down on a non selected shape: replace selection
      if ( button == 1 && mod_key == PF::MOD_KEY_NONE && !is_shape_selected(mo_shape_index) ) {
        selection_replace(mo_shape_index);
      }
      // cntrl+mouse down on a non selected shape: add to selection
      else if ( button == 1 && mod_key == SHAPE_KEY_SELECT_ADD_SHAPE && !is_shape_selected(mo_shape_index) ) {
        selection_add(mo_shape_index);
      }
    }
    else if ( mo_hit_test == PF::Shape::hit_segment || mo_hit_test == PF::Shape::hit_falloff_segment ) {
      Shape* shape = par->get_shape(mo_shape_index);
      if ( shape->get_type() == PF::Shape::line ) {
        // mouse down on a non selected shape: replace selection
        if ( button == 1 && mod_key == PF::MOD_KEY_NONE && !is_shape_selected(mo_shape_index) ) {
          selection_replace(mo_shape_index);
        }
        // cntrl+mouse down on a non selected shape: add to selection
        else if ( button == 1 && mod_key == SHAPE_KEY_SELECT_ADD_SHAPE && !is_shape_selected(mo_shape_index) ) {
          selection_add(mo_shape_index);
        }
      }
    }
    else if (mo_hit_test == PF::Shape::hit_node ) {
      if ( mod_key == PF::MOD_KEY_NONE ) {
        mo_shape_node_selected = mo_shape_index;
        mo_node_selected = mo_shape_additional;
      }
    }
    
    refresh = true;
    handled = true;
  }

  return handled;
}

bool PF::ShapesConfigGUI::handle_end_adding_new_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  // if adding a new shape right click it ends it
  if ( button == 3 ) {
    if ( mo_adding != PF::Shape::shape ) {
      
      switch (mo_adding)
      {
      case PF::Shape::polygon:
        //      std::cout<<"ShapesConfigGUI::end_adding_new_shape(): adding line commit"<<std::endl;
        if (polygon_add.get_segments_count() > 2) {
          par->add_shape(&polygon_add);
        }
        break;
      case PF::Shape::line:
        //      std::cout<<"ShapesConfigGUI::end_adding_new_shape(): adding line commit"<<std::endl;
        if (line_add.get_points().size() > 1) {
          par->add_shape(&line_add);
        }
        break;
      default:
        std::cout<<"ShapesConfigGUI::end_adding_new_shape(): invalid shape type: "<<mo_adding<<std::endl;
        break;
      }

      done_adding_shape();
      
      mo_adding = PF::Shape::shape;

      refresh = true;
      handled = true;
    }
  }

  return handled;
}

bool PF::ShapesConfigGUI::handle_update_adding_new_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  if ( mo_adding != PF::Shape::shape ) {

    switch (mo_adding)
    {
    case PF::Shape::polygon:
      {
        //      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding new point to line"<<std::endl;
        Point pt(sx, sy);
        pt_screen2image( pt );
        polygon_add.add_point(pt);
  
        defalut_new_polygon( pt, mod_key );
      }
      break;
    case PF::Shape::line:
      {
        //      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding new point to line"<<std::endl;
        Point pt(sx, sy);
        pt_screen2image( pt );
        line_add.add_point(pt);
  
        defalut_new_line( pt, mod_key );
      }
      break;
    default:
      std::cout<<"ShapesConfigGUI::handle_update_adding_new_shape(): invalid shape type: "<<mo_adding<<std::endl;
      break;
    }

//    par->shapes_modified();

    refresh = true;
    handled = true;
  }

  return handled;
}

bool PF::ShapesConfigGUI::handle_update_pos_adding_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  if ( mo_adding != PF::Shape::shape ) {
    
//    std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point"<<std::endl;
    switch (mo_adding)
    {
    case PF::Shape::line:
    {
//      std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point to line"<<std::endl;
      Point pt(sx, sy);
      pt_screen2image( pt );
      defalut_new_line( pt, mod_key );
    }
      break;
    case PF::Shape::polygon:
    {
//      std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point to polygon"<<std::endl;
      Point pt(sx, sy);
      pt_screen2image( pt );
      defalut_new_polygon( pt, mod_key );
    }
      break;
    default:
      std::cout<<"ShapesConfigGUI::handle_update_adding_new_shape(): invalid shape type: "<<mo_adding<<std::endl;
      break;
    }
    
    par->shapes_modified();
    
    refresh = true;
    handled = true;
  }

  return handled;
}

bool PF::ShapesConfigGUI::handle_drag_shape(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  if ( !mo_dragging && button == 1 && mod_key == PF::MOD_KEY_NONE ) {
    // to start dragging check if mouse is over a shape
    if ( mo_shape_index >= 0 ) {
      mo_dragging = true;
/*      // if more than one shape selected, anywhere is ok to drag
      if ( get_selection_count() > 1 ) {
        mo_dragging = true;
      }
      else {
        if ( mo_hit_test == PF::Shape::hit_shape || mo_hit_test == PF::Shape::hit_falloff || mo_hit_test == PF::Shape::hit_source ) {
          mo_dragging = true;
        }
        else {
          Shape* shape = par->get_shape(mo_shape_index);
          if ( mo_hit_test == PF::Shape::hit_segment || mo_hit_test == PF::Shape::hit_falloff_segment ) {
            mo_dragging = (shape->get_type() == PF::Shape::line);
          }
        }
      }*/
    }
  }
  
  if ( mo_dragging ) {
//    std::cout<<"PF::ShapesConfigGUI::handle_drag_shape(): dragging"<<std::endl;
    
    Point prev_pt(pt_current);
    Point curr_pt(sx, sy);
    pt_screen2image(prev_pt);
    pt_screen2image(curr_pt);
    
//    std::cout<<"PF::ShapesConfigGUI::handle_drag_shape(): get_selection_count(): "<<get_selection_count()<<std::endl;
    
   // if just one shape is selected, just drag it
    if ( get_selection_count() <= 1 ) {
      // get the active shape
      Shape* shape = par->get_shape(mo_shape_index);

      shape->offset(mo_hit_test, prev_pt, curr_pt, mo_shape_additional, (mod_key == SHAPE_KEY_UNLOCK_SOURCE), PF::SegmentInfo::in_sinch);
      shape_expanded(shape);
    }
    else {
      for (int i=0; i<shapes_selected.size(); i++) {
        Shape* shape = par->get_shape(shapes_selected[i]);
//        int hit_t = (shape->get_type() == PF::Shape::line) ? PF::Shape::hit_segment :PF::Shape::hit_shape;
        int hit_t = PF::Shape::hit_shape;
        
        shape->offset(hit_t, prev_pt, curr_pt, mo_shape_additional, (mod_key == SHAPE_KEY_UNLOCK_SOURCE), PF::SegmentInfo::in_sinch);
//        shape_expanded(shape);
      }
   }
    
    pt_current.set(sx, sy);

    par->shapes_modified();

    refresh = true;
    handled = true;
  }

  return handled;
}


bool PF::ShapesConfigGUI::update_hit_test(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  // save the current position
  pt_current.set(sx, sy);
  
  if ( get_show_outline() ) {
    int prev_hit = mo_hit_test;
    mo_hit_test = PF::Shape::hit_none;
    Point hit_pt;
    int index;
        
    for (index = par->get_shapes_count()-1; index >= 0 && mo_hit_test == PF::Shape::hit_none; index--) {
      Shape* shape = par->get_shape(index);
  
      hit_pt = pt_current;
      pt_screen2image(hit_pt);
      
      mo_hit_test = shape->hit_test(hit_pt, mo_shape_additional);
    }
    
    if (mo_hit_test != PF::Shape::hit_none) {
      mo_shape_index = index+1;
    } 
    else {
      mo_shape_index = -1;
    }
    
    if (prev_hit != mo_hit_test) refresh = true;
    handled = true;
  }

//  if ( handled ) std::cout<<"ShapesConfigGUI::update_hit_test(): handled"<<std::endl;
  
  return handled;
}



#define EVENT_BEGIN() \
/* Retrieve the layer associated to the filter */ \
PF::Layer* layer = get_layer(); \
if( !layer ) return false; \
\
/* Retrieve the image the layer belongs to */ \
PF::Image* image = layer->get_image(); \
if( !image ) return false; \
\
/* Retrieve the pipeline #0 (full resolution preview) */ \
PF::Pipeline* pipeline = image->get_pipeline( 0 ); \
if( !pipeline ) return false; \
\
/* Find the pipeline node associated to the current layer */ \
PF::PipelineNode* node = pipeline->get_node( layer->get_id() ); \
if( !node ) return false; \
if( !node->image ) return false; \
\
PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par()); \
if( !par ) return false; \



bool PF::ShapesConfigGUI::pointer_press_event( int button, double sx, double sy, int mod_key )
{
  if( get_editting() ) return false;
  EVENT_BEGIN();
  
  bool refresh = false;
  bool handled = false;
  
  set_editting(true);

  // if dragging a shape right-click ends it
  if ( button == 3 && mo_dragging ) {
    mo_dragging = false;
    refresh = true;
    handled = true;
  }
  if ( !handled ) {
    handled = handle_add_new_shape(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_add_new_shape"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_end_adding_new_shape(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_end_adding_new_shape"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_update_adding_new_shape(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_update_adding_new_shape"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_delete_shape(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_delete_shape"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_delete_node(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_delete_node"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_add_node(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_add_node"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_add_control_point(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_add_control_point"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_selection(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_selection"<<std::endl;
  }
  
  if ( !handled ) 
    update_hit_test(par, button, mod_key, sx, sy, refresh);

  // refresh preview if something changed
  if ( refresh ) {
    image->update();
  }
  
  set_editting(false);

  return refresh;

/*  // right button pressed
  if (button == 3 ) {
    // if adding a new shape it ends it
    if ( mo_adding != PF::Shape::shape ) {
       refresh = true;
    }
    // if dragging just top it
    else if ( mo_dragging ) {
      mo_dragging = false;
      refresh = true;
    }
    else {
      // now right-click can..
      
      // if not, just clear selection
      if ( !handled ) {
        if ( get_selection_count() > 0 ) {
          selection_clear();
          refresh = true;
        }
      }
      // somewhere in a shape...
      else {
        // there's just one shape selected
        if ( get_selection_count() <= 0 ) {
          // now it depends on where it clicks and the key that is pressed
          
          // right click on a node delete it (sometimes...)
          else if ( ( mo_hit_test == PF::Shape::hit_node || mo_hit_test == PF::Shape::hit_control_point || mo_hit_test == PF::Shape::hit_control_point_end ) && 
              ( mod_key == SHAPE_KEY_DELETE_SHAPE ) ) {
            delete_node();
            refresh = true;
          }
        }
        else {
        }
      }
    }
  }
  
  // left button pressed
  else if ( button == 1 ) {
    // if dragging the button should be already pressed!!!
    if ( mo_dragging ) {
    }
    // if adding a new shape right click adds a new node
    else if ( mo_adding != PF::Shape::shape ) {
      update_adding_new_shape();
      refresh = true;
    }
    else {
      // if not adding a new shape, it depends on where the user click and what key is pressed
      
      // set the source point for next shape
      if ( get_has_source() && mod_key == SHAPE_KEY_SET_SOURCE_POINT ) {
        pt_source.set(sx, sy);
        last_pt_origin.set(-1, -1);
//        refresh = true;
      }
      // click somewhere, not a shape
      else if ( mo_hit_test == PF::Shape::hit_none ) {
        // if no shape and no keys just clear the selection
        if ( mod_key != SHAPE_KEY_ADD_2_SELECTION ) {
          if ( get_selection_count() > 0 ) {
            clear_selection();
            refresh = true;
          }
        }
      }
      else {
        // click somewhere in a shape...
        
        // if more than one shape is selected, anywhere on a shape inverts the selection status
        if ( get_selection_count() > 1 ) {
          // (de)select a shape
          if ( mod_key == SHAPE_KEY_ADD_2_SELECTION ) {
            selection_invert_shape();
            refresh = true;
          }
          else {
            // with any other key just select the new shape
            replace_selection();
            refresh = true;
          }
        }
        // only one shape is selected
        else {
          // click on shape, falloff or source just change the selection
          if ( mo_hit_test == PF::Shape::hit_shape || mo_hit_test == PF::Shape::hit_falloff || mo_hit_test == PF::Shape::hit_source ) {
            // add shape to selection
            if ( mod_key == SHAPE_KEY_ADD_2_SELECTION ) {
              selection_add_shape();
              refresh = true;
            }
            else {
              // with any other key just select the new shape
              replace_selection();
              refresh = true;
            }
          }
          // click on a segment may add a new node
          else if ( mo_hit_test == PF::Shape::hit_segment ) {
            if ( mod_key == SHAPE_KEY_ADD_NODE ) {
              add_node();
              refresh = true;
            }
          }
          // click on a node selects it and may add a new control point
          else if ( mo_hit_test == PF::Shape::hit_node ) {
            select_node();
            
            if ( mod_key == SHAPE_KEY_ADD_CONTROL_POINT ) {
              add_control_point();
              refresh = true;
            }
          }
        }
      }
    }
  }
  
  // refresh preview if something changed
  if (refresh ) {
    par->shapes_modified();
    image->update();
  }
  
  set_editting(false);

  return refresh;
*/
#if 0
  refresh_hit_test(sx, sy);

  // selection
  if ( button == 1 && adding == PF::Shape::shape && 
      mod_key == PF::MOD_KEY_NONE && get_show_outline() ) {
    if (mo_hit_test == PF::Shape::hit_none) {
      selection_clear();
    }
    else if (mo_hit_test == PF::Shape::hit_shape || mo_hit_test == PF::Shape::hit_falloff) {
      selection_replace(mo_shape_index);
    }
    else if (mo_hit_test == PF::Shape::hit_segment || mo_hit_test == PF::Shape::hit_falloff_segment) {
      Shape* shape = par->get_shape(mo_shape_index);
      if (shape->get_type() == PF::Shape::line)
        selection_replace(mo_shape_index);
    }
  }
  
  // node selection
  if ( button == 1 ) {
    mo_shape_node_selected = -1;
    mo_node_selected = -1;
    
    if ( adding == PF::Shape::shape && mod_key == PF::MOD_KEY_NONE && get_show_outline() ) {
      if (mo_shape_index >= 0) {
        Shape* shape = par->get_shape(mo_shape_index);
        if (shape->get_type() == PF::Shape::polygon) {
          if (mo_hit_test == PF::Shape::hit_node || mo_hit_test == PF::Shape::hit_control_point) {
            mo_shape_node_selected = mo_shape_index;
              mo_node_selected = mo_shape_additional;
          }
          if (mo_hit_test == PF::Shape::hit_control_point_end) {
            mo_shape_node_selected = mo_shape_index;
            if (mo_shape_additional < shape->get_points_count()-1)
              mo_node_selected = mo_shape_additional+1;
            else
              mo_node_selected = 0;
          }
        }
        if (shape->get_type() == PF::Shape::line) {
          if (mo_hit_test == PF::Shape::hit_node || mo_hit_test == PF::Shape::hit_control_point) {
            mo_shape_node_selected = mo_shape_index;
              mo_node_selected = mo_shape_additional;
          }
          if (mo_hit_test == PF::Shape::hit_control_point_end) {
            mo_shape_node_selected = mo_shape_index;
            if (mo_shape_additional < shape->get_points_count()-1)
              mo_node_selected = mo_shape_additional+1;
            else
              mo_node_selected = 0;
          }
        }
      }
    }
  }
  
  // delete a shape
  if (button == 3 && adding == PF::Shape::shape && mod_key == PF::MOD_KEY_NONE && get_show_outline() && !get_lock_shapes()) {
    Shape* shape = par->get_shape(mo_shape_index);
    
    if ( (shape->get_type() != PF::Shape::line && (mo_hit_test == PF::Shape::hit_shape || mo_hit_test == PF::Shape::hit_falloff) ) ||
        (shape->get_type() == PF::Shape::line && (mo_hit_test == PF::Shape::hit_segment || mo_hit_test == PF::Shape::hit_falloff_segment)) ) {
      set_editting(true);
      
      par->remove_shape(mo_shape_index);
      
      par->shapes_modified();
      image->update();
  
      set_editting(false);
      
      return true;
    }
  }
  
  // delete a node
  if (button == 3 && adding == PF::Shape::shape && mod_key == PF::MOD_KEY_NONE && get_show_outline() && !get_lock_shapes()) {
    Shape* shape = par->get_shape(mo_shape_index);

    switch (shape->get_type())
    {
    case PF::Shape::line:
    case PF::Shape::polygon:
      {
        if (mo_hit_test == PF::Shape::hit_node || mo_hit_test == PF::Shape::hit_control_point || mo_hit_test == PF::Shape::hit_control_point_end) {
          set_editting(true);
          
          shape->remove_point(mo_hit_test, mo_shape_additional);
          
          mo_shape_node_selected = -1;
          mo_node_selected = -1;
          
          par->shapes_modified();
          image->update();
      
          set_editting(false);
          
          return true;
        }
      }
      break;
    }
  }
  
  // finish adding a shape
  if (button == 3 && adding != PF::Shape::shape /*&& mod_key == PF::MOD_KEY_NONE*/) {
//    std::cout<<"ShapesConfigGUI::pointer_press_event(): finish adding a shape"<<std::endl;
    set_editting(true);
    
    switch (adding)
    {
    case PF::Shape::polygon:
//      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding line commit"<<std::endl;
      if (polygon_add.get_segments_count() > 2) {
        par->add_shape(&polygon_add);
      }
      break;
    case PF::Shape::line:
//      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding line commit"<<std::endl;
      if (line_add.get_points().size() > 1) {
        par->add_shape(&line_add);
      }
      break;
    }
    
    adding = PF::Shape::shape;
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
  }
    
  // adding a new node to a (in process of adding) new shape
  if (button == 1 && adding != PF::Shape::shape) {
//    std::cout<<"ShapesConfigGUI::pointer_press_event(): adding new point"<<std::endl;
    set_editting(true);
    
    switch (adding)
    {
    case PF::Shape::polygon:
    {
//      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding new point to line"<<std::endl;
      Point pt(sx, sy);
      pt_screen2image( pt );
      polygon_add.add_point(pt);

      defalut_new_polygon( pt, mod_key );
    }
    break;
    case PF::Shape::line:
      {
//      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding new point to line"<<std::endl;
        Point pt(sx, sy);
        pt_screen2image( pt );
        line_add.add_point(pt);

        defalut_new_line( pt, mod_key );
      }
      break;
    }
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
  }
  
  // start draggin a shape, segment or node
  if (mo_hit_test != PF::Shape::hit_none && (mod_key == PF::MOD_KEY_NONE || mod_key == PF::MOD_KEY_SHIFT) && 
      get_show_outline()) {
//    std::cout<<"ShapesConfigGUI::pointer_press_event(): start draggin a shape, segment or node"<<std::endl;
    dragging = true;
    return true;
  }
 
  // add a node to an existing shape
  if (mo_hit_test != PF::Shape::hit_none && mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL &&
      get_show_outline()) {
    if (mo_shape_index < 0 || mo_shape_additional< 0) {
      std::cout<<"ShapesConfigGUI::pointer_press_event(): add a node to an existing shape: invalid shape index: "<<mo_shape_index<<", "<<mo_shape_additional<<std::endl;
      return false;
    }
    
    Shape* shape = par->get_shape(mo_shape_index);
    
    switch (shape->get_type())
    {
    case PF::Shape::line:
    case PF::Shape::polygon:
      if (mo_hit_test == PF::Shape::hit_segment || mo_hit_test == PF::Shape::hit_node 
          /*|| mo_hit_test == PF::Shape::hit_control_point || mo_hit_test == PF::Shape::hit_control_point_end*/) {
      set_editting(true);

      Point pt(sx, sy);
      pt_screen2image( pt );
      
//        std::cout<<"ShapesConfigGUI::pointer_press_event(): add a node to a line: mo_shape_additional: "<<mo_shape_additional<<std::endl;       
      shape->insert_point(pt, mo_hit_test, mo_shape_additional);
      
      if (mo_hit_test == PF::Shape::hit_node) {
        if ( shape->get_type() == PF::Shape::polygon )
          defalut_new_polygon_control_point(mo_shape_additional);
        else if ( shape->get_type() == PF::Shape::line )
          defalut_new_line_control_point(mo_shape_additional);
      }
      
      par->shapes_modified();
      image->update();
      
      set_editting(false);
      
      return true;
    }
    break;
    }
  }
  
  
  // add a new shape
  if (button ==  1) {
//    std::cout<<"ShapesConfigGUI::pointer_press_event(): add a new shape"<<std::endl;

    switch (get_current_shape_type())
    {
    case PF::Shape::line:
    case PF::Shape::circle:
    case PF::Shape::ellipse:
    case PF::Shape::rectangle:
    case PF::Shape::polygon:
      {
        Point pt(sx, sy);
        pt_screen2image( pt );
        Point pts(pt_source);
        if ( !get_lock_source() ) {
          if ( last_pt_origin.get_x() == -1 && last_pt_origin.get_y() == -1 )
            last_pt_origin.set(sx, sy);
          else
            pts.offset(sx-last_pt_origin.get_x(), sy-last_pt_origin.get_y());
        }
        pts.offset(-sx, -sy); // source is a relative value
        pt_screen2image( pts );
        
        add_new_shape(par, get_current_shape_type(), pt, pts);
        
        switch (get_current_shape_type())
        {
        case PF::Shape::polygon:
          if (mod_key == PF::MOD_KEY_SHIFT) {
            polygon_add.set_segment_type(0, PF::SegmentInfo::bezier3_l);
          }
          else if (mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
            polygon_add.set_segment_type(0, PF::SegmentInfo::bezier4);
          }
          else {
            polygon_add.set_segment_type(0, PF::SegmentInfo::line);
          }
          
          defalut_new_polygon(pt, mod_key );
          break;
       case PF::Shape::line:
         if (mod_key == PF::MOD_KEY_SHIFT) {
           line_add.set_segment_type(0, PF::SegmentInfo::bezier3_l);
         }
         else if (mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
           line_add.set_segment_type(0, PF::SegmentInfo::bezier4);
         }
         else {
           line_add.set_segment_type(0, PF::SegmentInfo::line);
         }
         
         defalut_new_line(pt, mod_key );
          break;
        }
        
        pt_current.set(sx, sy); // save the current mouse position
        
        par->shapes_modified();
        image->update();
  
        set_editting(false);
        
        return true;
      }
    break;
    }
  }
  
  // set the source point for next shape
  if (button ==  1 && mod_key == PF::MOD_KEY_CTRL+PF::MOD_KEY_ALT) {
//    std::cout<<"ShapesConfigGUI::pointer_press_event(): setting source"<<std::endl;
    pt_source.set(sx, sy);
    last_pt_origin.set(-1, -1);
    return true;
  }
      
  return false;
  
#endif
  
}


bool PF::ShapesConfigGUI::pointer_release_event( int button, double sx, double sy, int mod_key )
{
  if( get_editting() ) return false;
  EVENT_BEGIN()
  
  bool refresh = false;
  bool handled = false;
  
  set_editting(true);

  if (mo_dragging) {
    mo_dragging = false;
    refresh = true;
  }
  if ( !handled ) {
    handled = handle_selection(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_selection"<<std::endl;
  }

//  if ( !handled ) 
//    update_hit_test(par, button, mod_key, sx, sy, refresh);

  // refresh preview if something changed
  if ( refresh ) {
    image->update();
  }

  set_editting(false);

  return refresh;
}

bool PF::ShapesConfigGUI::pointer_motion_event( int button, double sx, double sy, int mod_key )
{
  if( get_editting() ) return false;
  EVENT_BEGIN();
  
//  std::cout<<"ShapesConfigGUI::pointer_motion_event()"<<std::endl;
  
  bool refresh = false;
  bool handled = false;
  
  set_editting(true);

  if ( !handled ) {
    handled = handle_update_pos_adding_shape(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_motion_event(): handle_update_pos_adding_shape"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_drag_shape(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_motion_event(): handle_drag_shape"<<std::endl;
  }
  
  if ( !handled ) 
    update_hit_test(par, button, mod_key, sx, sy, refresh);
  
  // refresh preview if something changed
  if ( refresh ) {
    image->update();
  }
  
  set_editting(false);

  return refresh;
    
#if 0
  // adding a new shape
  if (mo_adding != PF::Shape::shape) {
    set_editting(true);
    
//    std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point"<<std::endl;
    switch (mo_adding)
    {
    case PF::Shape::line:
    {
//      std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point to line"<<std::endl;
      Point pt(sx, sy);
      pt_screen2image( pt );
      defalut_new_line( pt, mod_key );
    }
      break;
    case PF::Shape::polygon:
    {
//      std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point to polygon"<<std::endl;
      Point pt(sx, sy);
      pt_screen2image( pt );
      defalut_new_polygon( pt, mod_key );
    }
      break;
    }
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
  }
  
  
  // drag a shape
  if (mo_dragging) {
//    std::cout<<"ShapesConfigGUI::pointer_motion_event(): dragging"<<std::endl;
    set_editting(true);
    
    // get the active shape
    Shape* shape = par->get_shape(mo_shape_index);
    Point prev_pt(pt_current);
    Point curr_pt(sx, sy);
    pt_screen2image(prev_pt);
    pt_screen2image(curr_pt);
    
    shape->offset(mo_hit_test, prev_pt, curr_pt, mo_shape_additional, (mod_key == PF::MOD_KEY_SHIFT), PF::SegmentInfo::in_sinch);
    shape_expanded(shape);
    
    pt_current.set_x(sx); pt_current.set_y(sy);

    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
  }
  
  // save the current position
  pt_current.set(sx, sy);
  
  // save the shape the mouse is over
//  std::cout<<"ShapesConfigGUI::pointer_motion_event(): save the shape the mouse is over"<<std::endl;
  
  if ( get_show_outline() ) {
    set_editting(true);
    
    mo_hit_test = PF::Shape::hit_none;
    Point hit_pt;
    int index;
        
    for (index = par->get_shapes_count()-1; index >= 0 && mo_hit_test == PF::Shape::hit_none; index--) {
      Shape* shape = par->get_shape(index);
  
      hit_pt = pt_current;
      pt_screen2image(hit_pt);
      
      mo_hit_test = shape->hit_test(hit_pt, mo_shape_additional);
    }
    
    if (mo_hit_test != PF::Shape::hit_none) {
      mo_shape_index = index+1;
    } 
    else {
      mo_shape_index = -1;
    }
    
    set_editting(false);
    
    return true;
  }
  
  return false;
  
#endif
  
}

bool PF::ShapesConfigGUI::pointer_scroll_event( int direction, int mod_key )
{
  //  std::cout<<"ShapesConfigGUI::pointer_scroll_event() called"<<std::endl;

  if( get_editting() ) return false;
  EVENT_BEGIN();


  // expand shapes
  if (mo_hit_test != PF::Shape::hit_none && get_show_outline()) {
    Shape* shape = par->get_shape(mo_shape_index);
    //      std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand shapes"<<std::endl;
    set_editting(true);

    if (mod_key == PF::MOD_KEY_NONE) {
      //        std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand"<<std::endl;
      shape->expand((direction==PF::DIRECTION_KEY_UP)?10:-10);
      shape_expanded(shape);
    }
    if (mod_key == PF::MOD_KEY_SHIFT) {
      //        std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand falloff"<<std::endl;
      shape->expand_falloff((direction==PF::DIRECTION_KEY_UP)?10:-10);
      set_falloff(shape->get_falloff());
    }
    if (mod_key == PF::MOD_KEY_CTRL) {
      //        std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand opacity"<<std::endl;
      shape->expand_opacity((direction==PF::DIRECTION_KEY_UP)?.10f:-.10f);
      set_opacity(shape->get_opacity());
    }

    par->shapes_modified();
    image->update();

    set_editting(false);

    return true;
  }

  return false;
}

bool PF::ShapesConfigGUI::modify_preview( PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out,
    float scale, int xoffset, int yoffset )
{
//  std::cout<<"PF::ShapesConfigGUI::modify_preview() called"<<std::endl;
  EVENT_BEGIN();
  
  bool refresh = false;

  // Resize the output buffer to match the input one
  buf_out.resize( buf_in.get_rect() );

  // Copy pixel data from input to outout
  buf_out.copy( buf_in );

  // draw the shapes
  int hit_t, hit_add, hit_node;
  
  if (mo_adding != PF::Shape::shape) {
    switch (mo_adding)
    {
    case PF::Shape::line:
      draw_line(&line_add, buf_in, buf_out, PF::Shape::hit_none, -1, -1, false);
      
      refresh = true;
      break;
    case PF::Shape::polygon:
      draw_polygon(&polygon_add, buf_in, buf_out, PF::Shape::hit_none, -1, -1, false);
      
      refresh = true;
      break;
    }
  }
  
  // draw shapes
  if ( get_show_outline() ) {
    for (int i = 0; i < par->get_shapes_count(); i++) {
      Shape* shape = par->get_shape(i);
      if (mo_shape_index == i) {
        hit_t = mo_hit_test;
        hit_add = mo_shape_additional;
      }
      else {
        hit_t = PF::Shape::hit_none;
        hit_add = -1;
      }
      hit_node = -1;
      if (mo_node_selected >= 0 && mo_shape_node_selected == i) {
        hit_node = mo_node_selected;
      }
      draw_shape(shape, buf_in, buf_out, hit_t, hit_add, hit_node, is_shape_selected(i));
    }
    
    refresh = true;
  }
  else if (mo_dragging) {
    Shape* shape = par->get_shape(mo_shape_index);
    hit_t = mo_hit_test;
    hit_add = mo_shape_additional;
    draw_shape(shape, buf_in, buf_out, hit_t, hit_add, -1, false);
    
    refresh = true;
  }

  return refresh;
}
