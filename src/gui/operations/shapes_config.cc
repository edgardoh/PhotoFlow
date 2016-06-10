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

#include "shapes_config.hh"


//#define CURVE_SIZE 192

static std::ostream& operator <<( std::ostream& str, const VipsRect& r )
{
  str<<r.width<<","<<r.height<<"+"<<r.left<<"+"<<r.top;
  return str;
}

void PF::ShapesConfigGUI::pt_screen2image(Circle1& shape)
{
  std::vector<Point>&points = shape.get_points();
  std::vector<Point>&f_points = shape.get_falloff_points();
  
  for (int i = 0; i < points.size(); i++) {
    pt_screen2image(points[i]);
    pt_screen2image(f_points[i]);
  }
  pt_screen2image(shape.get_source_point());
  
  Point pt(shape.get_radius(), 0);
  pt_screen2image(pt);
  shape.set_radius(pt.get_x());
  
}

void PF::ShapesConfigGUI::pt_image2screen(Circle1& shape)
{
  std::vector<Point>&points = shape.get_points();
  std::vector<Point>&f_points = shape.get_falloff_points();
  
  for (int i = 0; i < points.size(); i++) {
    pt_image2screen(points[i]);
    pt_image2screen(f_points[i]);
  }
  pt_image2screen(shape.get_source_point());
  
  Point pt(shape.get_radius(), 0);
  pt_image2screen(pt);
  shape.set_radius(pt.get_x());
  
}

void PF::ShapesConfigGUI::pt_screen2image(Rect1& shape)
{
  std::vector<Point>&points = shape.get_points();
  std::vector<Point>&f_points = shape.get_falloff_points();
  
  for (int i = 0; i < points.size(); i++) {
    pt_screen2image(points[i]);
    pt_screen2image(f_points[i]);
  }
  pt_screen2image(shape.get_source_point());
  
}

void PF::ShapesConfigGUI::pt_image2screen(Rect1& shape)
{
  std::vector<Point>&points = shape.get_points();
  std::vector<Point>&f_points = shape.get_falloff_points();
  
  for (int i = 0; i < points.size(); i++) {
    pt_image2screen(points[i]);
    pt_image2screen(f_points[i]);
  }
  pt_image2screen(shape.get_source_point());
  
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

void PF::ShapesConfigGUI::draw_shape(Shape* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional)
{
  if (shape->get_type() == PF::Shape::line) {
    Line* line = dynamic_cast<Line*>(shape);
    draw_line(line, buf_in, buf_out, hit_t, hit_additional);
  }
  if (shape->get_type() == PF::Shape::circle) {
    Circle1* circle = dynamic_cast<Circle1*>(shape);
    draw_circle(circle, buf_in, buf_out, hit_t, hit_additional);
  }
  if (shape->get_type() == PF::Shape::rectangle) {
    Rect1* rect = dynamic_cast<Rect1*>(shape);
    draw_rectangle(rect, buf_in, buf_out, hit_t, hit_additional);
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


void PF::ShapesConfigGUI::draw_line(Line* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional)
{
/* Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(buf_out.get_pxbuf()->get_pixels(), 
                                                       Cairo::FORMAT_RGB24, buf_out.get_pxbuf()->get_width(), 
                                                       buf_out.get_pxbuf()->get_height(), buf_out.get_pxbuf()->get_rowstride());
   Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

   cr->set_source_rgb(1.0, .0, .0);
*/
  Point pt1;
  Point pt2;
  Point a, b, c, d;
  int amount = shape->get_falloff();
  Point px(amount, amount);
  pt_image2screen( px );
  amount = px.get_x();
  
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

  // draw segments, nodes and falloff
  int i=0;
  for (; i < shape->get_points().size()-1; i++) {
    pt1 = shape->get_points()[i];
    pt2 = shape->get_points()[i+1];
    pt_image2screen( pt1 );
    pt_image2screen( pt2 );

/*  cr->move_to(pt1.get_x(), pt1.get_y());
    cr->line_to(pt2.get_x(), pt2.get_y());
    cr->stroke();
*/
    // draw segment
    if (segm_selected == -1 || segm_selected == i) 
      buf_out.draw_line( pt1.get_x(), pt1.get_y(), pt2.get_x(), pt2.get_y(), 255, 0, 0 );
    else
      buf_out.draw_line( pt1.get_x(), pt1.get_y(), pt2.get_x(), pt2.get_y(), buf_in );

    // draw falloff
    if (amount > 0) {
      PF::Line::get_expanded_rec_from_line(pt1, pt2, amount, a, b, c, d);
      
      if (fall_selected == -1 || fall_selected == i) {
        buf_out.draw_line( b.get_x(), b.get_y(), c.get_x(), c.get_y(), 255, 0, 0 );
        buf_out.draw_line( d.get_x(), d.get_y(), a.get_x(), a.get_y(), 255, 0, 0 );
      }
      else {
        buf_out.draw_line( b.get_x(), b.get_y(), c.get_x(), c.get_y(), buf_in );
        buf_out.draw_line( d.get_x(), d.get_y(), a.get_x(), a.get_y(), buf_in );
      }
      
      if (fall_selected == -1 || fall_selected == i) {
        buf_out.draw_circle( pt1.get_x(), pt1.get_y(), amount, 255, 0, 0 );
      }
      else {
        buf_out.draw_circle( pt1.get_x(), pt1.get_y(), amount, buf_in );
      }
    }
    
    // draw node
    draw_node(pt1.get_x(), pt1.get_y(), buf_out, (node_selected==i));
  }
  
  // draw last node
  if (shape->get_points().size() >= i) {
    // end point node
    draw_node(pt2.get_x(), pt2.get_y(), buf_out, (node_selected==i));
    
    // end point falloff
    if (amount > 0) {
      if (segm_selected == -1 || segm_selected == i) {
        buf_out.draw_circle( pt2.get_x(), pt2.get_y(), amount, 255, 0, 0 );
      }
      else {
        buf_out.draw_circle( pt2.get_x(), pt2.get_y(), amount, buf_in );
      }
    }
  }

  if (shape->get_has_source()) {
    Line source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_line(&source, buf_in, buf_out, hit_t, hit_additional);
  }

/*
surface->flush();
surface->finish();
*/

}

void PF::ShapesConfigGUI::draw_circle(Circle1* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional)
{
  Point pt1, pt2;
  
  pt1 = shape->get_point();
  pt2.set(shape->get_radius(), 0);

  pt_image2screen( pt1 );
  pt_image2screen( pt2 );
  
  if (hit_t != PF::Shape::hit_none)
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), 255, 0, 0 );
  else
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), buf_in );

//  pt1 = shape->get_point();
  pt2.set(shape->get_radius() + shape->get_falloff(), 0);
//  pt_image2screen( pt1 );
  pt_image2screen( pt2 );

  if (hit_t != PF::Shape::hit_none)
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), 255, 0, 0 );
  else
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), buf_in );

  if (shape->get_has_source()) {
    Circle1 source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_circle(&source, buf_in, buf_out, hit_t, hit_additional);
  }
}

void PF::ShapesConfigGUI::draw_rectangle(Rect1* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional)
{
  
//  double tx, ty,tw, th;
  Point pt1, pt2;
  int selected;

  // draw the shape
/*  tx = shape.get_top_left().get_x();
  ty = shape.get_top_left().get_y();
  tw = shape.get_bottom_right().get_x()-tx; 
  th = shape.get_bottom_right().get_y()-ty;
  image2screen( tx, ty, tw, th );
*/
  pt1 = shape->get_top_left();
  pt2 = shape->get_bottom_right();
  pt_image2screen( pt1 );
  pt_image2screen( pt2 );
  
  selected = -1;
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source) {
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
/*  tx = shape.get_top_left_falloff().get_x();
  ty = shape.get_top_left_falloff().get_y();
  tw = shape.get_bottom_right_falloff().get_x()-tx; 
  th = shape.get_bottom_right_falloff().get_y()-ty;
  image2screen( tx, ty, tw, th );
*/
  pt1.set( shape->get_top_left().get_x()-shape->get_top_left_falloff().get_x(), shape->get_top_left().get_y()-shape->get_top_left_falloff().get_y()) ;
  pt2.set( shape->get_bottom_right().get_x()+shape->get_bottom_right_falloff().get_x(), shape->get_bottom_right().get_y()+shape->get_bottom_right_falloff().get_y()) ;
  pt_image2screen( pt1 );
  pt_image2screen( pt2 );

  selected = -1;
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source) {
    selected = 0;
  }
  else if (hit_t == PF::Shape::hit_falloff_segment) {
    selected = hit_additional+1;
  }
  
/*  if (selected == 1 || selected == 0)
    buf_out.draw_line( tx, ty, tx+tw, ty, 255, 0, 0 );
  else
    buf_out.draw_line( tx, ty, tx+tw, ty, buf_in );
  if (selected == 2 || selected == 0)
    buf_out.draw_line( tx+tw, ty, tx+tw, ty+th, 255, 0, 0 );
  else
    buf_out.draw_line( tx+tw, ty, tx+tw, ty+th, buf_in );
  if (selected == 3 || selected == 0)
    buf_out.draw_line( tx+tw, ty+th, tx, ty+th, 255, 0, 0 );
  else
    buf_out.draw_line( tx+tw, ty+th, tx, ty+th, buf_in );
  if (selected == 4 || selected == 0)
    buf_out.draw_line( tx, ty+th, tx, ty, 255, 0, 0 );
  else
    buf_out.draw_line( tx, ty+th, tx, ty, buf_in );
    */
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
    
/*    draw_node(tx, ty, buf_out, (selected==1));
    draw_node(tx+tw, ty, buf_out, (selected==2));
    draw_node(tx+tw, ty+th, buf_out, (selected==3));
    draw_node(tx, ty+th, buf_out, (selected==4));*/
    draw_node(pt1.get_x(), pt1.get_y(), buf_out, (selected==1));
    draw_node(pt2.get_x(), pt1.get_y(), buf_out, (selected==2));
    draw_node(pt2.get_x(), pt2.get_y(), buf_out, (selected==3));
    draw_node(pt1.get_x(), pt2.get_y(), buf_out, (selected==4));

  }
  
  if (shape->get_has_source()) {
    Rect1 source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
/*    if (hit_t == PF::Shape::hit_source) 
      hit_t = PF::Shape::hit_shape;
    else
      hit_t == PF::Shape::hit_none;*/
    draw_rectangle(&source, buf_in, buf_out, hit_t, hit_additional);
  }
}

PF::ShapesConfigGUI::ShapesConfigGUI( PF::Layer* layer ):
              OperationConfigGUI( layer, "Shapes tool" ),
              /*invert_box( this, "invert", _("invert"), true ),
              enable_falloff_box( this, "enable_falloff", _("enable falloff"), true ),
              falloffCurveEditor( this, "falloff_curve", new PF::CurveArea(), 0, 100, 0, 100, CURVE_SIZE, CURVE_SIZE ),
              active_point_id( -1 ), center_selected( false ),
              initializing( false )*/
              shape_type_sel( this, "shape_type", _("shape: "), 0 ),
              invert_box( this, "invert", _("invert"), false ),
              falloff_sl( this, "falloff", "falloff: ", 10, 0, 1000, 0.1, 1, 1 ),
              opacity_sl( this, "opacity", "opacity: ", 100, 0, 100, 0.1, 1, 1 ),
              radius_sl( this, "radius", "radius: ", 50, 0, 1000000, 1, 10, 1 )
{
/*  hbox.pack_start( invert_box, Gtk::PACK_SHRINK );
  hbox.pack_start( enable_falloff_box, Gtk::PACK_SHRINK );
  hbox.set_spacing(10);
  add_widget( hbox );

  add_widget( curvesBox );

  curvesBox.pack_start( falloffCurveEditor, Gtk::PACK_SHRINK );
  falloffCurveEditor.show();*/
  
  vbox_shape.pack_start( shape_type_sel );
  vbox_shape.pack_start( invert_box );
  vbox_shape.pack_start( falloff_sl );
  vbox_shape.pack_start( opacity_sl );
  add_widget( vbox_shape );
  
  vbox_circle.pack_start( radius_sl );
  add_widget( vbox_circle );

  mo_hit_test = PF::Shape::hit_none;
//  mo_shape_type = PF::Shape::shape;
  mo_shape_index = -1;
  mo_shape_additional = -1;

  dragging = false;
  adding = PF::Shape::shape;

}

PF::ShapesConfigGUI::ShapesConfigGUI( Layer* layer, const Glib::ustring& title, bool has_ch_sel ):
              OperationConfigGUI( layer, title, has_ch_sel ),
              /*invert_box( this, "invert", _("invert"), true ),
              enable_falloff_box( this, "enable_falloff", _("enable falloff"), true ),
              falloffCurveEditor( this, "falloff_curve", new PF::CurveArea(), 0, 100, 0, 100, CURVE_SIZE, CURVE_SIZE ),
              active_point_id( -1 ), center_selected( false ),
              initializing( false )*/
              shape_type_sel( this, "shape_type", _("shape: "), 0 ),
              invert_box( this, "invert", _("invert"), false ),
              falloff_sl( this, "falloff", "falloff: ", 10, 0, 1000, 0.1, 1, 1 ),
              opacity_sl( this, "opacity", "opacity: ", 100, 0, 100, 0.1, 1, 1 ),
              radius_sl( this, "radius", "radius: ", 50, 0, 1000000, 1, 10, 1 )
{
/*  hbox.pack_start( invert_box, Gtk::PACK_SHRINK );
  hbox.pack_start( enable_falloff_box, Gtk::PACK_SHRINK );
  hbox.set_spacing(10);
  add_widget( hbox );

  add_widget( curvesBox );

  curvesBox.pack_start( falloffCurveEditor, Gtk::PACK_SHRINK );
  falloffCurveEditor.show();*/
  
  vbox_shape.pack_start( shape_type_sel );
  vbox_shape.pack_start( invert_box );
  vbox_shape.pack_start( falloff_sl );
  vbox_shape.pack_start( opacity_sl );
  add_widget( vbox_shape );
  
  vbox_circle.pack_start( radius_sl );
  add_widget( vbox_circle );

  mo_hit_test = PF::Shape::hit_none;
//  mo_shape_type = PF::Shape::shape;
  mo_shape_index = -1;
  mo_shape_additional = -1;

  editting = false;
  dragging = false;
  adding = PF::Shape::shape;
  
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
/*int level = pipeline->get_level();*/ \
/*float scale = 1;*/ \
/*for( int i = 1; i <= level; i++ ) scale *= 2;*/ \
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
  std::cout<<"ShapesConfigGUI::pointer_press_event(): button="<<button<<std::endl;

  if( get_editting() ) return false;
  EVENT_BEGIN();
  
/*  // Retrieve the layer associated to the filter
  PF::Layer* layer = get_layer();
  if( !layer ) return false;

  // Retrieve the image the layer belongs to
  PF::Image* image = layer->get_image();
  if( !image ) return false;

  // Retrieve the pipeline #0 (full resolution preview)
  PF::Pipeline* pipeline = image->get_pipeline( 0 );
  if( !pipeline ) return false;
  int level = pipeline->get_level();
  float scale = 1;
  for( int i = 1; i <= level; i++ ) scale *= 2;

  // Find the pipeline node associated to the current layer
  PF::PipelineNode* node = pipeline->get_node( layer->get_id() );
  if( !node ) return false;
  if( !node->image ) return false;

  PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par());
  if( !par ) return false;
*/
  // finish adding a shape
  if (button == 3 && adding != PF::Shape::shape && mod_key == PF::MOD_KEY_NONE) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): finish adding a shape"<<std::endl;
    set_editting(true);
    
    switch (adding)
    {
    case PF::Shape::line:
      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding line commit"<<std::endl;
      if (line_add.get_points().size() > 2) {
        line_add.get_points().pop_back();
        par->add_shape(line_add);
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
  if (button == 1 && adding != PF::Shape::shape && mod_key == PF::MOD_KEY_NONE) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): adding new point"<<std::endl;
    set_editting(true);
    
    switch (adding)
    {
    case PF::Shape::line:
    {
      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding new point to line"<<std::endl;
      Point pt(sx, sy);
      pt_screen2image( pt );
      line_add.add_point(pt);
    }
      break;
    }
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
  }
  
  // start draggin a shape, segment or node
  if (mo_hit_test != PF::Shape::hit_none && (mod_key == PF::MOD_KEY_NONE || mod_key == PF::MOD_KEY_SHIFT)) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): start draggin a shape, segment or node"<<std::endl;
    dragging = true;
    return true;
  }
    
  // add a node to an existing shape
  if (mo_hit_test == PF::Shape::hit_segment && mod_key == PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
    std::cout<<"ShapesConfigGUI::pointer_press_event():  add a node to an existing shape"<<std::endl;
    set_editting(true);
    
    Shape* shape = NULL;
    
    switch (par->get_shape_type(mo_shape_index))
    {
    case PF::ShapesPar::type_line:
      shape = par->get_shape(mo_shape_index);
      break;
    }
    
    if (shape != NULL && mo_shape_additional >= 0) {
      Point pt(sx, sy);
      pt_screen2image( pt );
      
      std::cout<<"ShapesConfigGUI::pointer_press_event(): add a node to a line: mo_shape_additional: "<<mo_shape_additional<<std::endl;
      
      shape->insert_point(pt, mo_shape_additional+1);
      mo_hit_test = PF::Shape::hit_node;
      mo_shape_additional++;
    }
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
  }
    
  // add a new shape
  if (button ==  1 && mod_key == PF::MOD_KEY_NONE) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): add a new shape"<<std::endl;
    // add a new shape
    if (par->get_shape_type() == PF::ShapesPar::type_line) {
      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding line"<<std::endl;
      set_editting(true);
      
      Line shape;
      Point pt(sx, sy);
      pt_screen2image( pt );
  
      shape.set_falloff(get_falloff());
      shape.set_opacity(get_opacity());
      shape.set_size( get_size(par->get_shape_type()) );
      
      // set source point
      if (get_has_source()) {
        Point pts(pt_source);
        pts.offset(-sx, -sy); // source is a relative value
        pt_screen2image( pts );
        
        shape.set_has_source(true);
        shape.set_source_point(pts);
      }
      
      shape.add_point(pt); // first point (where the user clicked)
      shape.add_point(pt); // second point, the next to be added
      
      line_add = shape; // set the shape to be added
      
      pt_current.set(sx, sy); // save the current mouse position
      
      adding = PF::Shape::line; // we are adding a new shape
      
      par->shapes_modified();
      image->update();

      set_editting(false);
      
      return true;
    }
  if (par->get_shape_type() == PF::ShapesPar::type_circle) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): adding circle"<<std::endl;
    set_editting(true);
    
    Circle1 shape;
    Point pt(sx, sy);
    pt_screen2image( pt );

    shape.set_falloff(get_falloff());
    shape.set_opacity(get_opacity());
    shape.set_size( get_size(par->get_shape_type()) );
    
    // set source point
    if (get_has_source()) {
      Point pts(pt_source);
      pts.offset(-sx, -sy); // source is a relative value
      pt_screen2image( pts );
      
      shape.set_has_source(true);
      shape.set_source_point(pts);
    }
    
    shape.set_point(pt);
    
    par->add_shape(shape);
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
/*    Circle1 circle;
    circle.set_has_source(true);
    circle.set_source_point(pt_source);
    circle.get_source_point().offset(-sx, -sy);
    circle.set_radius( radius_sl.get_adjustment()->get_value() );
    circle.set_falloff( get_falloff() );
    circle.get_point().set( sx, sy );
    pt_screen2image( circle.get_point() );
    pt_screen2image( circle.get_source_point() );
    par->add_shape(circle);*/
  }
  if (par->get_shape_type() == PF::ShapesPar::type_rectangle) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): adding rectangle"<<std::endl;
    set_editting(true);
    
    Rect1 shape;
    Point pt(sx, sy);
    pt_screen2image( pt );

    shape.set_falloff(get_falloff());
    shape.set_opacity(get_opacity());
    shape.set_size( get_size(par->get_shape_type()) );
    
    // set source point
    if (get_has_source()) {
      Point pts(pt_source);
      pts.offset(-sx, -sy); // source is a relative value
      pt_screen2image( pts );
      
      shape.set_has_source(true);
      shape.set_source_point(pts);
    }
    
    shape.set_point(pt, 0);
    shape.set_point(pt, 1);
    
    par->add_shape(shape);
    
    mo_hit_test = PF::Shape::hit_node;
    mo_shape_index = par->get_shapes_order().size()-1;
    mo_shape_additional = 2;
    dragging = true;

    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
/*//    double x = sx, y = sy, w = 10, h = 10;
//    screen2image( x, y, w, h );
    Rect1 rect_adding;
    rect_adding.set_has_source(true);
    rect_adding.set_source_point(pt_source);
    rect_adding.get_source_point().offset(-sx, -sy);
    rect_adding.get_top_left().set( sx, sy );
//    rect_adding.get_top_left().set_y( 0 );
    rect_adding.get_bottom_right().set( sx+1, sy+1 );
//    rect_adding.get_bottom_right().set_y( y+1 );
    pt_screen2image( rect_adding.get_top_left() );
    pt_screen2image( rect_adding.get_bottom_right() );
    pt_screen2image( rect_adding.get_source_point() );
    
    rect_adding.get_top_left_falloff().set( 30, 30 );
//    rect_adding.get_top_left_falloff().set_y( 30 );
    rect_adding.get_bottom_right_falloff().set( 30, 30 );
//    rect_adding.get_bottom_right_falloff().set_y( 30 );
    pt_screen2image( rect_adding.get_top_left_falloff() );
    pt_screen2image( rect_adding.get_bottom_right_falloff() );
    
    par->add_shape(rect_adding);
//    par->add_shape(Rectangle());
    

    return true;*/
  }
  }
  
  // set the source point for next shape
  if (button ==  1 && mod_key == PF::MOD_KEY_CTRL+PF::MOD_KEY_ALT) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): setting source"<<std::endl;
    pt_source.set(sx, sy);
    return true;
  }
      

  return false;
}


bool PF::ShapesConfigGUI::pointer_release_event( int button, double sx, double sy, int mod_key )
{
  if( get_editting() ) return false;
//  EVENT_BEGIN()
  
  if (dragging) {
    dragging = false;
    return true;
  }

  return false;
}


bool PF::ShapesConfigGUI::pointer_motion_event( int button, double sx, double sy, int mod_key )
{
  if( get_editting() ) return false;
  EVENT_BEGIN();
  
/*  // Retrieve the layer associated to the filter
  PF::Layer* layer = get_layer();
  if( !layer ) return false;

  // Retrieve the image the layer belongs to
  PF::Image* image = layer->get_image();
  if( !image ) return false;

  // Retrieve the pipeline #0 (full resolution preview)
  PF::Pipeline* pipeline = image->get_pipeline( 0 );
  if( !pipeline ) return false;

  // Find the pipeline node associated to the current layer
  PF::PipelineNode* node = pipeline->get_node( layer->get_id() );
  if( !node ) return false;
  if( !node->image ) return false;

  PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par());
  if( !par ) return false;
*/
  std::cout<<"ShapesConfigGUI::pointer_motion_event()"<<std::endl;
  
  // adding a new shape
  if (adding != PF::Shape::shape) {
    set_editting(true);
    
    std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point"<<std::endl;
    switch (adding)
    {
    case PF::Shape::line:
    {
      std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point to line"<<std::endl;
      Point pt(sx, sy);
      pt_screen2image( pt );
      line_add.point_back().set(pt);
    }
      break;
    }
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
  }
  
  
  // drag a shape
  if (dragging) {
    std::cout<<"ShapesConfigGUI::pointer_motion_event(): dragging"<<std::endl;
    set_editting(true);
    
    // get the active shape
    Shape *shape = par->get_shape(mo_shape_index);
    if (shape != NULL) {
      Point prev_pt(pt_current);
      Point curr_pt(sx, sy);
      pt_screen2image(prev_pt);
      pt_screen2image(curr_pt);
      
      shape->offset(mo_hit_test, prev_pt, curr_pt, mo_shape_additional, (mod_key == PF::MOD_KEY_SHIFT));
      
      pt_current.set_x(sx); pt_current.set_y(sy);
  
      par->shapes_modified();
      image->update();
    }
    else {
      std::cout<<"ShapesConfigGUI::pointer_motion_event() shape == NULL"<<std::endl;
    }
    
    set_editting(false);
    
    return true;
  }
  
  // save the current position
  pt_current.set(sx, sy);
  
  // save the shape the mouse is over
  std::cout<<"ShapesConfigGUI::pointer_motion_event(): save the shape the mouse is over"<<std::endl;
  
  set_editting(true);
  
  mo_hit_test = PF::Shape::hit_none;
  Shape *shape = NULL;
  Point hit_pt;
  int index;
      
  for (index = par->get_shapes_count()-1; index >= 0 && mo_hit_test == PF::Shape::hit_none; index--) {
    shape = par->get_shape(index);
    if (shape == NULL)
      std::cout<<"ShapesConfigGUI::pointer_motion_event() shape == NULL"<<std::endl;

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
  
  return false;
}

bool PF::ShapesConfigGUI::pointer_scroll_event( int direction, int mod_key )
{
  std::cout<<"ShapesConfigGUI::pointer_scroll_event() called"<<std::endl;

  if( get_editting() ) return false;
  EVENT_BEGIN();
  
/*  // Retrieve the layer associated to the filter
  PF::Layer* layer = get_layer();
  if( !layer ) return false;

  // Retrieve the image the layer belongs to
  PF::Image* image = layer->get_image();
  if( !image ) return false;

  // Retrieve the pipeline #0 (full resolution preview)
  PF::Pipeline* pipeline = image->get_pipeline( 0 );
  if( !pipeline ) return false;

  // Find the pipeline node associated to the current layer
  PF::PipelineNode* node = pipeline->get_node( layer->get_id() );
  if( !node ) return false;
  if( !node->image ) return false;

  PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par());
  if( !par ) return false;
*/
  // expand shapes
  if (mo_hit_test != PF::Shape::hit_none) {
    Shape* shape = par->get_shape(mo_shape_index);
    if (shape != NULL) {
      std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand shapes"<<std::endl;
      set_editting(true);
      
      if (mod_key == PF::MOD_KEY_NONE) {
        shape->expand((direction==PF::DIRECTION_KEY_UP)?10:-10);
        set_size(par->get_shape_type(mo_shape_index), shape->get_size());
      }
      if (mod_key == PF::MOD_KEY_SHIFT) {
        shape->expand_falloff((direction==PF::DIRECTION_KEY_UP)?10:-10);
        set_falloff(shape->get_falloff());
      }
      if (mod_key == PF::MOD_KEY_CTRL) {
        shape->expand_opacity((direction==PF::DIRECTION_KEY_UP)?.10f:-.10f);
        set_opacity(shape->get_opacity());
      }
      
      par->shapes_modified();
      image->update();

      set_editting(false);
      
      return true;
    }
    else {
      std::cout<<"ShapesConfigGUI::pointer_scroll_event() shape == NULL"<<std::endl;
    }
  }
  
  return false;
}

bool PF::ShapesConfigGUI::modify_preview( PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out,
    float scale, int xoffset, int yoffset )
{
//  std::cout<<"PF::ShapesConfigGUI::modify_preview() called"<<std::endl;
  EVENT_BEGIN();
  
/*  PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par());
  if( !par ) return false;

  // Retrieve the layer associated to the filter
  PF::Layer* layer = get_layer();
  if( !layer ) return false;

  // Retrieve the image the layer belongs to
  PF::Image* image = layer->get_image();
  if( !image ) return false;

  // Retrieve the pipeline #0 (full resolution preview)
  PF::Pipeline* pipeline = image->get_pipeline( 0 );
  if( !pipeline ) return false;

  // Find the pipeline node associated to the current layer
  PF::PipelineNode* node = pipeline->get_node( layer->get_id() );
  if( !node ) return false;
  if( !node->image ) return false;
*/
  // Resize the output buffer to match the input one
  buf_out.resize( buf_in.get_rect() );

  // Copy pixel data from input to outout
  buf_out.copy( buf_in );

  // draw the shapes
  int hit_t, hit_add;
  
  if (adding != PF::Shape::shape) {
    switch (adding)
    {
    case PF::Shape::line:
      draw_line(&line_add, buf_in, buf_out, PF::Shape::hit_node, line_add.get_points().size()-1);
      break;
    }
  }
  
  // draw shapes
  for (int i = par->get_shapes_count()-1; i>=0; i--) {
    Shape* shape = par->get_shape(i);
    if (mo_shape_index == i) {
      hit_t = mo_hit_test;
      hit_add = mo_shape_additional;
    }
    else {
      hit_t = PF::Shape::hit_none;
      hit_add = -1;
    }
    draw_shape(shape, buf_in, buf_out, hit_t, hit_add);

  }

  return true;
}
