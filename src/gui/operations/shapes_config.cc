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


#define CURVE_SIZE 192


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
  if (shape->get_type() == PF::Shape::ellipse) {
    Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
    draw_ellipse(ellipse, buf_in, buf_out, hit_t, hit_additional);
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
  if (shape->get_has_source()) {
    std::cout<<"PF::ShapesConfigGUI::draw_line(): shape->get_source_point() "<<shape->get_source_point().get_x()<<std::endl;
  }
  Point pt1;
  Point pt2;
  Point a, b, c, d;
  int shape_dist = shape->get_size();
  pt1.set(shape_dist, shape_dist);
  pt_image2screen( pt1 );
  shape_dist = pt1.get_x();
  int fall_dist = shape->get_size() + shape->get_falloff();
  pt1.set(fall_dist, fall_dist);
  pt_image2screen( pt1 );
  fall_dist = pt1.get_x();
  
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
//    if (fall_dist > 0) {
      PF::Line::get_expanded_rec_from_line(pt1, pt2, shape_dist, a, b, c, d);
      
      if (segm_selected == -1 || segm_selected == i) {
        buf_out.draw_line( b.get_x(), b.get_y(), c.get_x(), c.get_y(), 255, 0, 0 );
        buf_out.draw_line( d.get_x(), d.get_y(), a.get_x(), a.get_y(), 255, 0, 0 );
      }
      else {
        buf_out.draw_line( b.get_x(), b.get_y(), c.get_x(), c.get_y(), buf_in );
        buf_out.draw_line( d.get_x(), d.get_y(), a.get_x(), a.get_y(), buf_in );
      }
      
      if (segm_selected == -1 || segm_selected == i) {
        buf_out.draw_circle( pt1.get_x(), pt1.get_y(), shape_dist, 255, 0, 0 );
      }
      else {
        buf_out.draw_circle( pt1.get_x(), pt1.get_y(), shape_dist, buf_in );
      }
//    }

    // draw falloff
    if (shape->get_falloff() > 0) {
      PF::Line::get_expanded_rec_from_line(pt1, pt2, fall_dist, a, b, c, d);
      
      if (fall_selected == -1 || fall_selected == i) {
        buf_out.draw_line( b.get_x(), b.get_y(), c.get_x(), c.get_y(), 255, 0, 0 );
        buf_out.draw_line( d.get_x(), d.get_y(), a.get_x(), a.get_y(), 255, 0, 0 );
      }
      else {
        buf_out.draw_line( b.get_x(), b.get_y(), c.get_x(), c.get_y(), buf_in );
        buf_out.draw_line( d.get_x(), d.get_y(), a.get_x(), a.get_y(), buf_in );
      }
      
      if (fall_selected == -1 || fall_selected == i) {
        buf_out.draw_circle( pt1.get_x(), pt1.get_y(), fall_dist, 255, 0, 0 );
      }
      else {
        buf_out.draw_circle( pt1.get_x(), pt1.get_y(), fall_dist, buf_in );
      }
    }
    
    // draw node
    draw_node(pt1.get_x(), pt1.get_y(), buf_out, (node_selected==i));
  }
  
  // draw last node
  if (shape->get_points().size() >= i) {
    // end point node
    draw_node(pt2.get_x(), pt2.get_y(), buf_out, (node_selected==i));
    
    // end point shape
//    if (fall_dist > 0) {
      if (segm_selected == -1 || segm_selected == i) {
        buf_out.draw_circle( pt2.get_x(), pt2.get_y(), shape_dist, 255, 0, 0 );
      }
      else {
        buf_out.draw_circle( pt2.get_x(), pt2.get_y(), shape_dist, buf_in );
      }
//    }
    
    // end point falloff
    if (shape->get_falloff() > 0) {
      if (fall_selected == -1 || fall_selected == i) {
        buf_out.draw_circle( pt2.get_x(), pt2.get_y(), fall_dist, 255, 0, 0 );
      }
      else {
        buf_out.draw_circle( pt2.get_x(), pt2.get_y(), fall_dist, buf_in );
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
  pt2.set(shape->get_size(), 0);

  pt_image2screen( pt1 );
  pt_image2screen( pt2 );
  
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source)
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), 255, 0, 0 );
  else
    buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), buf_in );

  if (shape->get_falloff() > 0) {
    pt2.set(shape->get_size() + shape->get_falloff(), 0);
    pt_image2screen( pt2 );
  
    if (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source)
      buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), 255, 0, 0 );
    else
      buf_out.draw_circle( pt1.get_x(), pt1.get_y(), pt2.get_x(), buf_in );
  }
  
  if (shape->get_has_source()) {
    Circle1 source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_circle(&source, buf_in, buf_out,  hit_t, hit_additional);
  }
}

void PF::ShapesConfigGUI::draw_ellipse(Ellipse* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional)
{
  Point center, radius;
  int selected;
  const int falloff_x = shape->get_falloff_x();
  const int falloff_y = shape->get_falloff_y();
  
  center = shape->get_point();
  radius.set(shape->get_radius_x(), shape->get_radius_y());

  pt_image2screen( center );
  pt_image2screen( radius );
  
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source)
    buf_out.draw_ellipse( center.get_x(), center.get_y(), radius.get_x(), radius.get_y(), 0, 255, 0, 0 );
  else
    buf_out.draw_ellipse( center.get_x(), center.get_y(), radius.get_x(), radius.get_y(), 0, buf_in );

  if (falloff_x > 0) {
    Point radiusf;
    radiusf.set(shape->get_radius_x() + falloff_x, shape->get_radius_y() + falloff_y);
    pt_image2screen( radiusf );
  
    if (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source)
      buf_out.draw_ellipse( center.get_x(), center.get_y(), radiusf.get_x(), radiusf.get_y(), 0, 255, 0, 0 );
    else
      buf_out.draw_ellipse( center.get_x(), center.get_y(), radiusf.get_x(), radiusf.get_y(), 0, buf_in );
    
/*    // draw falloff nodes
    if (hit_t != PF::Shape::hit_none) {
      if (hit_t == PF::Shape::hit_falloff_node) {
        selected = hit_additional+1;
      }
      else
        selected = 0;
      
      draw_node(center.get_x(), center.get_y()-radiusf.get_y(), buf_out, (selected==1));
      draw_node(center.get_x()+radiusf.get_x(), center.get_y(), buf_out, (selected==2));
      draw_node(center.get_x(), center.get_y()+radiusf.get_y(), buf_out, (selected==3));
      draw_node(center.get_x()-radiusf.get_x(), center.get_y(), buf_out, (selected==4));
    }
*/
  }
  
  // draw nodes
  if (hit_t != PF::Shape::hit_none) {
    if (hit_t == PF::Shape::hit_node) {
      selected = hit_additional+1;
    }
    else
      selected = 0;
    
    draw_node(center.get_x(), center.get_y()-radius.get_y(), buf_out, (selected==1));
    draw_node(center.get_x()+radius.get_x(), center.get_y(), buf_out, (selected==2));
    draw_node(center.get_x(), center.get_y()+radius.get_y(), buf_out, (selected==3));
    draw_node(center.get_x()-radius.get_x(), center.get_y(), buf_out, (selected==4));
  }

  if (shape->get_has_source()) {
    Ellipse source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_ellipse(&source, buf_in, buf_out,  hit_t, hit_additional);
  }
}

void PF::ShapesConfigGUI::draw_rectangle(Rect1* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional)
{
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
  
  if (shape->get_has_source()) {
    Rect1 source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_rectangle(&source, buf_in, buf_out, hit_t, hit_additional);
  }
}

PF::ShapesConfigGUI::ShapesConfigGUI( PF::Layer* layer ):
              OperationConfigGUI( layer, "Shapes tool" ),
              falloffCurveEditor( this, "falloff_curve", new PF::CurveArea(), 0, 100, 0, 100, CURVE_SIZE, CURVE_SIZE ),
              btn_shapes(),
              falloff_sl( this, "falloff", "falloff: ", 10, 0, 1000, 0.1, 1, 1 ),
              opacity_sl( this, "opacity", "opacity: ", 100, 0, 100, 0.1, 1, 1 ),
              size_sl( this, "size", "size: ", 50, 0, 1000000, 1, 10, 1 )
{
  vbox_shape.pack_start( falloff_sl );
  vbox_shape.pack_start( opacity_sl );
  vbox_shape.pack_start( size_sl );
  add_widget( vbox_shape );
  add_widget( curvesBox );

  curvesBox.pack_start( falloffCurveEditor, Gtk::PACK_SHRINK );
  falloffCurveEditor.show();
  
  mo_hit_test = PF::Shape::hit_none;
  mo_shape_index = -1;
  mo_shape_additional = -1;

  dragging = false;
  adding = PF::Shape::shape;

}

PF::ShapesConfigGUI::ShapesConfigGUI( Layer* layer, const Glib::ustring& title, bool has_ch_sel ):
              OperationConfigGUI( layer, title, has_ch_sel ),
              falloffCurveEditor( this, "falloff_curve", new PF::CurveArea(), 0, 100, 0, 100, CURVE_SIZE, CURVE_SIZE ),
              btn_shapes(),
              falloff_sl( this, "falloff", "falloff: ", 10, 0, 1000, 0.1, 1, 1 ),
              opacity_sl( this, "opacity", "opacity: ", 100, 0, 100, 0.1, 1, 1 ),
              size_sl( this, "size", "size: ", 50, 0, 1000000, 1, 10, 1 )
{
  btn_shapes.add_button( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/cursor-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/cursor-inactive.png" );
  btn_shapes.add_button( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/line-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/line-inactive.png" );
  btn_shapes.add_button( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/circle-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/circle-inactive.png" );
  btn_shapes.add_button( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/ellipse-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/ellipse-inactive.png" );
  btn_shapes.add_button( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rectangle-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rectangle-inactive.png" );

  hbox_shapes_type.pack_end( btn_shapes, Gtk::PACK_SHRINK );
  btn_shapes.signal_clicked.connect(sigc::mem_fun(*this, &ShapesConfigGUI::btn_shapes_type_clicked) );
  add_widget( hbox_shapes_type );
  
  vbox_shape.pack_start( falloff_sl );
  vbox_shape.pack_start( opacity_sl );
  vbox_shape.pack_start( size_sl );
  add_widget( vbox_shape );
  
  add_widget( curvesBox );

  curvesBox.pack_start( falloffCurveEditor, Gtk::PACK_SHRINK );
  falloffCurveEditor.show();

  shape_type_selected = PF::ShapesPar::type_none;
  
  mo_hit_test = PF::Shape::hit_none;
  mo_shape_index = -1;
  mo_shape_additional = -1;

  editting = false;
  dragging = false;
  adding = PF::Shape::shape;
  
}

void PF::ShapesConfigGUI::btn_shapes_type_clicked()
{
  shape_type_selected = btn_shapes.get_btn_active();
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
\
/* Find the pipeline node associated to the current layer */ \
PF::PipelineNode* node = pipeline->get_node( layer->get_id() ); \
if( !node ) return false; \
if( !node->image ) return false; \
\
PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par()); \
if( !par ) return false; \


void PF::ShapesConfigGUI::add_new_shape(PF::ShapesPar* par, int shape_type, Point& initial_pos, Point& source_pos)
{
  switch (shape_type)
  {
  case PF::ShapesPar::type_line:
  {
    Line shape( initial_pos, get_size(get_shape_type()), get_opacity(), get_falloff(), get_has_source(), source_pos );
    
    line_add = shape; // set the shape to be added
    adding = PF::Shape::line; // we are adding a new shape
  }
   break;
  case PF::ShapesPar::type_circle:
    {
      Circle1 shape( initial_pos, get_size(get_shape_type()), get_opacity(), get_falloff(), get_has_source(), source_pos );
      
      par->add_shape(shape);
    }
    break;
  case PF::ShapesPar::type_ellipse:
    {
      Ellipse shape( initial_pos, get_size(get_shape_type()), get_opacity(), get_falloff(), get_has_source(), source_pos );
      
      par->add_shape(shape);
    }
    break;
  case PF::ShapesPar::type_rectangle:
  {
    Rect1 shape( initial_pos, get_size(get_shape_type()), get_opacity(), get_falloff(), get_has_source(), source_pos );
    
    par->add_shape(shape);
    
    mo_hit_test = PF::Shape::hit_node;
    mo_shape_index = par->get_shapes_order().size()-1;
    mo_shape_additional = 2;
    dragging = true;
  }
   break;
  }
}

bool PF::ShapesConfigGUI::pointer_press_event( int button, double sx, double sy, int mod_key )
{
//  std::cout<<"ShapesConfigGUI::pointer_press_event(): button="<<button<<std::endl;

  if( get_editting() ) return false;
  EVENT_BEGIN();
  
  // finish adding a shape
  if (button == 3 && adding != PF::Shape::shape && mod_key == PF::MOD_KEY_NONE) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): finish adding a shape"<<std::endl;
    set_editting(true);
    
    switch (adding)
    {
    case PF::Shape::line:
      std::cout<<"ShapesConfigGUI::pointer_press_event(): adding line commit"<<std::endl;
      if (line_add.get_points().size() > 2) {
        line_add.remove_point(line_add.get_points().size()-1);
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

    switch (get_shape_type())
    {
    case PF::ShapesPar::type_line:
    case PF::ShapesPar::type_circle:
    case PF::ShapesPar::type_ellipse:
    case PF::ShapesPar::type_rectangle:
      {
        Point pt(sx, sy);
        pt_screen2image( pt );
        Point pts(pt_source);
        pts.offset(-sx, -sy); // source is a relative value
        pt_screen2image( pts );
        
        add_new_shape(par, get_shape_type(), pt, pts);
        
        pt_current.set(sx, sy); // save the current mouse position
        
        par->shapes_modified();
        image->update();
  
        set_editting(false);
        
        return true;
      }
    break;
    }

  if (false && get_shape_type() == PF::ShapesPar::type_rectangle) {
    std::cout<<"ShapesConfigGUI::pointer_press_event(): adding rectangle"<<std::endl;
    set_editting(true);
    
/*    Rect1 shape;
    Point pt(sx, sy);
    pt_screen2image( pt );

    shape.set_falloff(get_falloff());
    shape.set_opacity(get_opacity());
    shape.set_size( get_size(get_shape_type()) );
    
    // set source point
    if (get_has_source()) {
      Point pts(pt_source);
      pts.offset(-sx, -sy); // source is a relative value
      pt_screen2image( pts );
      
      shape.set_has_source(true);
      shape.set_source_point(pts);
    }
    
    shape.set_point(0, pt);
    shape.set_point(1, pt);
    
    par->add_shape(shape);
    
    mo_hit_test = PF::Shape::hit_node;
    mo_shape_index = par->get_shapes_order().size()-1;
    mo_shape_additional = 2;
    dragging = true;
*/
    Point pt(sx, sy);
    pt_screen2image( pt );
    Point pts(pt_source);
    pts.offset(-sx, -sy); // source is a relative value
    pt_screen2image( pts );
    
    Rect1 shape( pt, get_size(get_shape_type()), get_opacity(), get_falloff(), get_has_source(), pts );
    
    par->add_shape(shape);
    
    mo_hit_test = PF::Shape::hit_node;
    mo_shape_index = par->get_shapes_order().size()-1;
    mo_shape_additional = 2;
    dragging = true;

    
    
    
    
    par->shapes_modified();
    image->update();

    set_editting(false);
    
    return true;
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
  
//  std::cout<<"ShapesConfigGUI::pointer_motion_event()"<<std::endl;
  
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
//    std::cout<<"ShapesConfigGUI::pointer_motion_event(): dragging"<<std::endl;
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
//  std::cout<<"ShapesConfigGUI::pointer_motion_event(): save the shape the mouse is over"<<std::endl;
  
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
  
  
  // expand shapes
  if (mo_hit_test != PF::Shape::hit_none) {
    Shape* shape = par->get_shape(mo_shape_index);
    if (shape != NULL) {
      std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand shapes"<<std::endl;
      set_editting(true);
      
      if (mod_key == PF::MOD_KEY_NONE) {
        std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand"<<std::endl;
        shape->expand((direction==PF::DIRECTION_KEY_UP)?10:-10);
        set_size(par->get_shape_type(mo_shape_index), shape->get_size());
      }
      if (mod_key == PF::MOD_KEY_SHIFT) {
        std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand falloff"<<std::endl;
        shape->expand_falloff((direction==PF::DIRECTION_KEY_UP)?10:-10);
        set_falloff(shape->get_falloff());
      }
      if (mod_key == PF::MOD_KEY_CTRL) {
        std::cout<<"ShapesConfigGUI::pointer_scroll_event() expand opacity"<<std::endl;
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
