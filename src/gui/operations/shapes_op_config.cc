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

#define SHAPE_KEY_SET_SOURCE_POINT    PF::MOD_KEY_ALT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_DELETE_SHAPE        PF::MOD_KEY_NONE
#define SHAPE_KEY_DELETE_NODE         PF::MOD_KEY_NONE
#define SHAPE_KEY_ADD_NODE            PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_SELECT_NODE         PF::MOD_KEY_SHIFT+PF::MOD_KEY_ALT
#define SHAPE_KEY_ADD_CONTROL_POINT   PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_NOT_ADD_SHAPE       PF::MOD_KEY_SHIFT+PF::MOD_KEY_ALT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_UNLOCK_SOURCE       PF::MOD_KEY_SHIFT
#define SHAPE_KEY_SELECT_ADD_SHAPE    PF::MOD_KEY_ALT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_ADD_LINE            PF::MOD_KEY_SHIFT+PF::MOD_KEY_CTRL
#define SHAPE_KEY_ADD_BEZIER3         PF::MOD_KEY_SHIFT+PF::MOD_KEY_ALT
#define SHAPE_KEY_ADD_BEZIER4         PF::MOD_KEY_NONE

#define SHAPE_BEZIER_T 0.5f

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
              lock_cursor_mode_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-inactive.png", true ), 
              show_outline_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-inactive.png", true ), 
              lock_source_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-inactive.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-active.png", true, false ), 
              lock_shapes_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-inactive.png", true, false ), 
              fill_shape_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/fill-shape-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/fill-shape-inactive.png", true ),
              rect_rounded_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rounded-corner-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rounded-corner-inactive.png", true ),
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
              lock_cursor_mode_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-cursor-mode-inactive.png", true ), 
              show_outline_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/show-outline-inactive.png", true ), 
              lock_source_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-inactive.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-source-active.png", true, false ), 
              lock_shapes_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/lock-shapes-inactive.png", true, false ), 
              fill_shape_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/fill-shape-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/fill-shape-inactive.png", true ),
              rect_rounded_btn( PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rounded-corner-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rounded-corner-inactive.png", true ),
              last_pt_origin(-1, -1)
{
  btn_shapes.add_button( PF::Shape::shape, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/cursor-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/cursor-inactive.png" );
  btn_shapes.add_button( PF::Shape::circle, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/circle-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/circle-inactive.png" );
  btn_shapes.add_button( PF::Shape::ellipse, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/ellipse-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/ellipse-inactive.png" );
  btn_shapes.add_button( PF::Shape::rectangle, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rectangle-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/rectangle-inactive.png" );
  btn_shapes.add_button( PF::Shape::polygon, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/polygon-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/polygon-inactive.png" );
  btn_shapes.add_button( PF::Shape::line, PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/line-active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/tools/line-inactive.png" );

  hbox_shapes_type.pack_end( btn_shapes, Gtk::PACK_SHRINK );
  btn_shapes.signal_clicked.connect(sigc::mem_fun(*this, &ShapesConfigGUI::btn_shapes_type_clicked) );
  controlsBox.pack_start( hbox_shapes_type );
  
  hbox_shapes_options.pack_start( lock_cursor_mode_btn, Gtk::PACK_SHRINK, 5 );
  lock_cursor_mode_btn.set_tooltip_text( _("returns to cursor mode after adding a shape") );
  hbox_shapes_options.pack_start( show_outline_btn, Gtk::PACK_SHRINK, 5 );
  show_outline_btn.set_tooltip_text( _("shows/hide the outline arround the shape") );
  hbox_shapes_options.pack_start( lock_source_btn, Gtk::PACK_SHRINK, 5 );
  lock_source_btn.set_tooltip_text( _("sets the source shape at fixed position or relative to the destination") );
  hbox_shapes_options.pack_start( lock_shapes_btn, Gtk::PACK_SHRINK, 5 );
  lock_shapes_btn.set_tooltip_text( _("prevents the shapes or nodes to be deleted") );
  hbox_shapes_options.pack_start( fill_shape_btn, Gtk::PACK_SHRINK, 5 );
  fill_shape_btn.set_tooltip_text( _("fills the shape") );
  hbox_shapes_options.pack_start( rect_rounded_btn, Gtk::PACK_SHRINK, 5 );
  rect_rounded_btn.set_tooltip_text( _("draws the rectangle with rounded corners") );
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
  fill_shape_btn.signal_activated.connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::fill_shape_btn_changed));
  fill_shape_btn.signal_deactivated.connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::fill_shape_btn_changed));
  rect_rounded_btn.signal_activated.connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::rect_rounded_btn_changed));
  rect_rounded_btn.signal_deactivated.connect(sigc::mem_fun(*this,&PF::ShapesConfigGUI::rect_rounded_btn_changed));
  
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
  
//  std::cout<<"PF::ShapesConfigGUI::falloff_sl_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    shape->set_falloff( get_falloff() );
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

//  std::cout<<"PF::ShapesConfigGUI::opacity_sl_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    shape->set_opacity( get_opacity() );
    modified = true;
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
  
  set_editting(false);
}

void PF::ShapesConfigGUI::pen_size_sl_changed()
{
  if( get_editting() ) return;

//  std::cout<<"PF::ShapesConfigGUI::pen_size_sl_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    shape->set_pen_size( get_pen_size() );
    modified = true;
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
  
  set_editting(false);
}

void PF::ShapesConfigGUI::radius_sl_changed()
{
  if( get_editting() ) return;

//  std::cout<<"PF::ShapesConfigGUI::radius_sl_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    
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
  
  set_editting(false);
}

void PF::ShapesConfigGUI::radius_x_sl_changed()
{
  if( get_editting() ) return;

//  std::cout<<"PF::ShapesConfigGUI::radius_x_sl_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    
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
  
  set_editting(false);
}

void PF::ShapesConfigGUI::radius_y_sl_changed()
{
  if( get_editting() ) return;

//  std::cout<<"PF::ShapesConfigGUI::radius_y_sl_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    
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
  
  set_editting(false);
}

void PF::ShapesConfigGUI::angle_sl_changed()
{
  if( get_editting() ) return;

//  std::cout<<"PF::ShapesConfigGUI::angle_sl_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    
    if (shape->get_type() == PF::Shape::ellipse) {
      shape->set_angle(get_angle());
      modified = true;
    }
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
  
  set_editting(false);
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

void PF::ShapesConfigGUI::fill_shape_btn_changed()
{
  if( get_editting() ) return;

//  std::cout<<"PF::ShapesConfigGUI::fill_shape_btn_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    shape->set_fill_shape(get_fill_shape());
    modified = true;
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
  
  set_editting(false);
}

void PF::ShapesConfigGUI::rect_rounded_btn_changed()
{
  if( get_editting() ) return;

//  std::cout<<"PF::ShapesConfigGUI::rect_rounded_btn_changed()"<<std::endl;
  
  CALLBACK_BEGIN();
  
  set_editting(true);
  
  bool modified = false;

  for ( int i = 0; i < get_selection_count(); i++) {
    Shape* shape = par->get_shape( get_selected(i) );
    if ( shape->get_type() == PF::Shape::rectangle ) {
      Rectangle* rect = dynamic_cast<Rectangle*>(shape);
      rect->set_rounded_corners(get_rect_rounded());
      modified = true;
    }
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
  
  set_editting(false);
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
/*
void PF::ShapesConfigGUI::draw_circle(PF::Point& center, float radius, PF::Point& pt_from, PF::Point& pt_to, 
    PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out)
{
  int  pixelX, pixelY;
  const float R2 = radius*radius;
  
  guint8* inp;
  guint8* p;
  
  const int IMAGE_TOP = buf_out.get_rect().top;
  const int IMAGE_LEFT = buf_out.get_rect().left;
  const int IMAGE_BOT = buf_out.get_rect().top+buf_out.get_rect().height;
  const int IMAGE_RIGHT = buf_out.get_rect().left+buf_out.get_rect().width;
  
  guint8* inpx = buf_in.get_pxbuf()->get_pixels();
  guint8* px = buf_out.get_pxbuf()->get_pixels();
  const int rs = buf_out.get_pxbuf()->get_rowstride();
  const int bl = 3;

  const int topleft_x = std::max(IMAGE_LEFT, (int)(center.x-radius));
  const int bottomright_x = std::min(IMAGE_RIGHT, (int)(center.x));
  
  const int topleft_y = std::max(IMAGE_TOP, (int)(center.y-radius));
  const int bottomright_y = std::min(IMAGE_BOT, (int)(center.y));
  
  const int row_from = std::max(IMAGE_TOP, (int)(center.y-radius));
  const int row_to = std::min(IMAGE_BOT, (int)center.y);
  
  int last_from = IMAGE_RIGHT;
  
  //  Loop through the rows on image coordinates
  for ( int row = row_from; row <= row_to; row++ ) {
    const float dy = (center.y-row);
    const float dy2 = dy*dy;
    const float rad = std::sqrt(R2 - dy2);
    
    if ( center.x >= rad) {
    const int col = center.x - rad;

    const int xfrom = std::max(IMAGE_LEFT, col);
    const int xto = std::min(last_from, std::min(IMAGE_RIGHT, (int)(center.x+.5)));
    
    const int y_off = rs*(row-IMAGE_TOP);
    const int x_off = ((int)xfrom-IMAGE_LEFT)*bl;
    
    // quar 0
    inp = inpx + y_off + x_off;
    p = px + y_off + x_off;
    
    for ( int ii = xfrom; ii <= xto; ii++, inp += bl, p += bl ) {
      PX_MOD( inp, p );
    }
    
    // quar 1
    const int x_off_r = bl*(center.x-xfrom)*2 + x_off;
    inp = inpx + y_off + x_off_r;
    p = px + y_off + x_off_r;
    
    for ( int ii = xto; ii >= xfrom; ii--, inp -= bl, p -= bl ) {
      PX_MOD( inp, p );
    }
    
    // quar 2
    const int y_off_b = y_off+2*((int)center.y-row)*rs;
    inp = inpx + y_off_b + x_off_r;
    p = px + y_off_b +x_off_r;
    
    for ( int ii = xto; ii >= xfrom; ii--, inp -= bl, p -= bl ) {
      PX_MOD( inp, p );
    }
    
    // quar 3
    inp = inpx + y_off_b + x_off;
    p = px + y_off_b + x_off;
    
    for ( int ii = xfrom; ii <= xto; ii++, inp += bl, p += bl ) {
      PX_MOD( inp, p );
    }
    
    last_from = xfrom;
    }

  }

}
*/
void PF::ShapesConfigGUI::draw_shape(Shape* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
                                      int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  if (shape->get_type() == PF::Shape::line) {
    Line* line = dynamic_cast<Line*>(shape);
    draw_shape_line(line, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
  }
  else if (shape->get_type() == PF::Shape::circle) {
    Circle* circle = dynamic_cast<Circle*>(shape);
    draw_shape_circle(circle, buf_in, buf_out, hit_t, hit_additional, selected);
  }
  else if (shape->get_type() == PF::Shape::ellipse) {
    Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
    draw_shape_ellipse(ellipse, buf_in, buf_out, hit_t, hit_additional, selected);
  }
  else if (shape->get_type() == PF::Shape::rectangle) {
    Rectangle* rect = dynamic_cast<Rectangle*>(shape);
    draw_shape_rectangle(rect, buf_in, buf_out, hit_t, hit_additional, selected);
  }
  else if (shape->get_type() == PF::Shape::polygon) {
    Polygon* polygon = dynamic_cast<Polygon*>(shape);
    draw_shape_polygon(polygon, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
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
/*
void PF::ShapesConfigGUI::draw_dot(Point& pt, PF::PixelBuffer& buf_out, bool active_point)
{
  const int point_size = 1;
  const int point_size2 = point_size*2;
  const int x = pt.x;
  const int y = pt.y;
  
  VipsRect point1 = { x-point_size-1, y-point_size-1,
      point_size2+3, point_size2+3};
  VipsRect point2 = { x-point_size,   y-point_size,
      point_size2+1, point_size2+1};
  buf_out.fill( point1, 0, 255, 0 );
  if( active_point )
    buf_out.fill( point2, 0, 255, 0 );
//  else
//    buf_out.fill( point2, 255, 255, 255 );
  
}
*/

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

void PF::ShapesConfigGUI::draw_line(Point& pt1, Point& pt2, float pen_size, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  Point a, b, c, d;
  
  PF::Line::get_expanded_rec_from_line(pt1, pt2, pen_size, a, b, c, d);
  
  if (hit_tested) {
    buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), 255, 0, 0 );
    buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), 255, 0, 0 );
  }
  else if (selected) {
    buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), 0, 0, 255 );
    buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), 0, 0, 255 );
  }
  else {
    buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), buf_in );
    buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), buf_in );
  }

//  buf_out.draw_line( b.get_x(), b.get_y(), c.get_x(), c.get_y(), buf_in );
//  buf_out.draw_line( d.get_x(), d.get_y(), a.get_x(), a.get_y(), buf_in );

  draw_circle(pt1, pen_size, buf_in, buf_out, hit_tested, selected);
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
    
//    draw_dot( points[i-1], buf_out, selected );
//    draw_dot( points[i], buf_out, selected );
 }
}

void PF::ShapesConfigGUI::draw_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, float pen_size, 
    PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  Point a, b, c, d;
  std::vector<Point> points;
  PF::Line::inter_bezier4(anchor1, anchor2, control1, control2, points);
  for (int i = 1; i < points.size(); i++) {
//    draw_line(points[i-1], points[i], buf_in, buf_out, hit_tested, selected);
    
    PF::Line::get_expanded_rec_from_line(points[i-1], points[i], pen_size, a, b, c, d);
    
    if (hit_tested) {
      buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), 255, 0, 0 );
      buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), 255, 0, 0 );
    }
    else if (selected) {
      buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), 0, 0, 255 );
      buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), 0, 0, 255 );
    }
    else {
      buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), buf_in );
      buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), buf_in );
    }

//    draw_dot( points[i-1], buf_out, selected );
//    draw_dot( points[i], buf_out, selected );
 }
  
  Point& pt1 = points[0];
  
  draw_circle(pt1, pen_size, buf_in, buf_out, hit_tested, selected);
}

void PF::ShapesConfigGUI::draw_bezier3(Point& anchor1, Point& anchor2, Point& control, 
    PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  std::vector<Point> points;
  PF::Line::inter_bezier3(anchor1, anchor2, control, points);
    for (int i = 1; i < points.size(); i++) {
      draw_line(points[i-1], points[i], buf_in, buf_out, hit_tested, selected);
      
//      draw_dot( points[i-1], buf_out, selected );
//      draw_dot( points[i], buf_out, selected );
   }
}

void PF::ShapesConfigGUI::draw_bezier3(Point& anchor1, Point& anchor2, Point& control, float pen_size, 
    PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool hit_tested, bool selected)
{
  Point a, b, c, d;
  std::vector<Point> points;
  PF::Line::inter_bezier3(anchor1, anchor2, control, points);
    for (int i = 1; i < points.size(); i++) {
//      draw_line(points[i-1], points[i], buf_in, buf_out, hit_tested, selected);
  
      PF::Line::get_expanded_rec_from_line(points[i-1], points[i], pen_size, a, b, c, d);
      
      if (hit_tested) {
        buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), 255, 0, 0 );
        buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), 255, 0, 0 );
      }
      else if (selected) {
        buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), 0, 0, 255 );
        buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), 0, 0, 255 );
      }
      else {
        buf_out.draw_line( a.get_x(), a.get_y(), b.get_x(), b.get_y(), buf_in );
        buf_out.draw_line( d.get_x(), d.get_y(), c.get_x(), c.get_y(), buf_in );
      }

//      draw_dot( points[i-1], buf_out, selected );
//      draw_dot( points[i], buf_out, selected );
   }
    
    Point& pt1 = points[0];
    
    draw_circle(pt1, pen_size, buf_in, buf_out, hit_tested, selected);
}
/*
void PF::ShapesConfigGUI::draw_rect(VipsRect* rc, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, bool selected)
{
  Point pt1, pt2;
  
  pt1.set(rc->left, rc->top);
  pt2.set(rc->left+rc->width, rc->top);
  pt_image2screen( pt1 );
  pt_image2screen( pt2 );
  draw_line(pt1, pt2, buf_in, buf_out, false, selected);
  
  pt1 = pt2;
  pt2.set(rc->left+rc->width, rc->top+rc->height);
  pt_image2screen( pt2 );
  draw_line(pt1, pt2, buf_in, buf_out, false, selected);

  pt1 = pt2;
  pt2.set(rc->left, rc->top+rc->height);
  pt_image2screen( pt2 );
  draw_line(pt1, pt2, buf_in, buf_out, false, selected);

  pt1 = pt2;
  pt2.set(rc->left, rc->top);
  pt_image2screen( pt2 );
  draw_line(pt1, pt2, buf_in, buf_out, false, selected);

}
*/
void PF::ShapesConfigGUI::draw_shape_circle(Circle* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected)
{
  Point pt1, pt2;
  
/*  {
    VipsRect rc;
    shape->get_mask_rect(&rc);
    draw_rect(&rc, buf_in, buf_out, selected);
  }
*/
  pt1 = shape->get_point();
  pt2.set(shape->get_radius(), 0);

  pt_image2screen( pt1 );
  pt_image2screen( pt2 );
  
  draw_circle( pt1, pt2.get_x(), buf_in, buf_out, (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source), selected );

  if (shape->get_falloff() > 0) {
    float pen_size = (shape->get_fill_shape()) ? 0.f: (shape->get_pen_size()/2.f);
    pt2.set(shape->get_radius() + shape->get_falloff() + pen_size, 0);
    pt_image2screen( pt2 );
  
    draw_circle( pt1, pt2.get_x(), buf_in, buf_out, (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source), selected );
  }
  
  if (shape->get_has_source()) {
    Circle source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_shape_circle(&source, buf_in, buf_out,  hit_t, hit_additional, selected);
  }
}

void PF::ShapesConfigGUI::draw_shape_ellipse(Ellipse* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected)
{
  Point center, radius;
  int node_selected;
  const int falloff_x = shape->get_falloff_x();
  const int falloff_y = shape->get_falloff_y();
  
/*  {
    VipsRect rc;
    shape->get_mask_rect(&rc);
    draw_rect(&rc, buf_in, buf_out, selected);
  }
  */
  center = shape->get_point();
  radius.set(shape->get_radius_x(), shape->get_radius_y());

  pt_image2screen( center );
  pt_image2screen( radius );
  
  if (hit_t == PF::Shape::hit_shape || hit_t == PF::Shape::hit_source) {
    draw_ellipse(center, radius.get_x(), radius.get_y(), shape->get_angle(), -1, buf_in, buf_out, true, selected);
  }
  else if (hit_t == PF::Shape::hit_segment ) {
    for (int i = 0; i <= 3; i++) {
      draw_ellipse(center, radius.get_x(), radius.get_y(), shape->get_angle(), i, buf_in, buf_out, ( hit_additional == i ), selected);
    }
  }
  else {
    draw_ellipse(center, radius.get_x(), radius.get_y(), shape->get_angle(), -1, buf_in, buf_out, false, selected);
  }
  
  if (falloff_x > 0) {
    Point radiusf;
    float pen_size = (shape->get_fill_shape()) ? 0.f: (shape->get_pen_size()/2.f);
    radiusf.set(shape->get_radius_x() + falloff_x + pen_size, shape->get_radius_y() + falloff_y + pen_size);
    pt_image2screen( radiusf );
  
    draw_ellipse(center, radiusf.get_x(), radiusf.get_y(), shape->get_angle(), -1, buf_in, buf_out, (hit_t == PF::Shape::hit_falloff || hit_t == PF::Shape::hit_source), selected);
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
    draw_shape_ellipse(&source, buf_in, buf_out,  hit_t, hit_additional, selected);
  }
}

void PF::ShapesConfigGUI::draw_shape_rectangle(Rectangle* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, int hit_t, int hit_additional, bool selected)
{
  Polygon* polygon = dynamic_cast<Polygon*>(shape);
  draw_shape_polygon(polygon, buf_in, buf_out, hit_t, hit_additional, -1, selected);
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
  
  int node_prev = (node >0) ? node-1: shape->get_points_count()-1;
  si = shape->get_segment_info(node_prev);
  if (si.get_segment_type() == PF::SegmentInfo::bezier3_r || si.get_segment_type() == PF::SegmentInfo::bezier4) {
    pt1 = shape->get_point(node);
    pt_image2screen( pt1 );
    bool node_sel;
    if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
      ptc1 = si.get_control_pt1();
      node_sel = (cntrl1_selected==node_prev);
    }
    else {
      ptc1 = si.get_control_pt2();
      node_sel = (cntrl2_selected==node_prev);
    }
    pt_image2screen( ptc1 );
    
    draw_node(ptc1.get_x(), ptc1.get_y(), buf_out, node_sel);
    draw_line(pt1, ptc1, buf_in, buf_out, false, selected);
  }

}
/*
void PF::ShapesConfigGUI::draw_node_control_points_line(Line* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
    int node, int cntrl1_selected, int cntrl2_selected, int selected)
{
  if ( node > 0 && node < shape->get_points_count()-1 ) {
    Polygon* polygon = dynamic_cast<Polygon*>(shape);
    draw_node_control_points(polygon, buf_in, buf_out, node, cntrl1_selected, cntrl2_selected, selected);
  }
  else if ( node == 0) {
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
  } else if ( node == shape->get_points_count()-1 ) {
    Point pt1, pt2;
    Point ptc1, ptc2;
    int segment = node-1;
    PF::SegmentInfo si = shape->get_segment_info(segment);
    if (si.get_segment_type() == PF::SegmentInfo::bezier3_r || si.get_segment_type() == PF::SegmentInfo::bezier4) {
      pt1 = shape->get_point(node);
      pt_image2screen( pt1 );
      bool node_sel;
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        ptc1 = si.get_control_pt1();
        node_sel = (cntrl1_selected==node);
      }
      else {
        ptc1 = si.get_control_pt2();
        node_sel = (cntrl2_selected==node);
      }
      pt_image2screen( ptc1 );
      
      draw_node(ptc1.get_x(), ptc1.get_y(), buf_out, node_sel);
      draw_line(pt1, ptc1, buf_in, buf_out, false, selected);
    }
  }
    else {
      std::cout<<"PF::ShapesConfigGUI::draw_node_control_points_line(): invalid node: "<<node<<std::endl;
    }
}
*/

void PF::ShapesConfigGUI::draw_polygon_vec(std::vector<Point>& vec_pt, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
    int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  std::cout<<"PF::ShapesConfigGUI::draw_polygon_vec(): vec_pt.size(): "<<vec_pt.size()<<std::endl;
  //buf_out.draw_line( pt1.x, pt1.y, pt2.x, pt2.y, 0, 0, 255 );
  
  Point pt1, pt2;
  if ( vec_pt.size() > 0 ) {
    pt1 = vec_pt[0];
    
    if (isnan(pt1.x) || isnan(pt1.y)) std::cout<<"PF::ShapesConfigGUI::draw_polygon_vec(): pt is NaN: "<<std::endl;
    
    pt_image2screen( pt1 );
    for ( int i = 1; i < vec_pt.size(); i++) {
      pt2 = vec_pt[i];
      if (isnan(pt2.x) || isnan(pt2.y)) std::cout<<"PF::ShapesConfigGUI::draw_polygon_vec(): pt is NaN: "<<std::endl;
      
      pt_image2screen( pt2 );
      draw_line(pt1, pt2, buf_in, buf_out, false, selected);
      pt1 = pt2;
    }
/*    pt2 = vec_pt[0];
    if (isnan(pt2.x) || isnan(pt2.y)) std::cout<<"PF::ShapesConfigGUI::draw_polygon_vec(): pt is NaN: "<<std::endl;
    
    pt_image2screen( pt2 );
    draw_line(pt1, pt2, buf_in, buf_out, false, selected);*/
  }
}

void PF::ShapesConfigGUI::draw_polygon_segment_falloff(std::vector<Point>& points, Point& pt3_first, Point& pt4_prev, bool clockwise, float falloff, 
    Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
    int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
//  std::cout<<"PF::Polygon::fill_polygon_segment_falloff(): clockwise: "<<clockwise<<std::endl;
  
//  if ( get_closed_shape() ) {
  bool closed_shape = true;
//    bool first, intersects;
//    int i;
    std::vector<Point> vec_pt;
    Point pt1, pt2, pt3, pt4, pt_expand, mid_pt;
//    Point pts1, pts2;
//    Point a, b, c, d;
//    Point b1, c1;
//    Point pt_end, pt_start, pt_intersection;
//    SegmentInfo si;
//    bool clock = is_clockwise();
//    float pen_size = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
    float expand = falloff;
    float falloff2 = falloff*falloff;
    float falloff2_1 = 1.f / falloff2;
    bool first_time = true;

/*    if ( closed_shape ) {
      points.push_back( points[0] );
      
      pt1 = points.back();
      int i = points.size()-2;
      while ( i > 0 && pt1 == points[i] ) i--;
      if ( i >= 0 ) {
        get_pt2_proyected_from_segment(points[i], pt1, expand, pt3, pt4);
        if ( clockwise )
          pt4_prev = pt3;
        else
          pt4_prev = pt4;
      }
    }
    */
    pt1 = points[0];
    for (int i = 1; i < points.size(); i++) {
      pt2 = points[i];
      if ( pt1 != pt2 ) {
       
        if ( clockwise )
          PF::Polygon::get_pt1_2_proyected_from_segment(pt1, pt2, expand, pt3, pt4);
        else
          PF::Polygon::get_pt3_4_proyected_from_segment(pt1, pt2, expand, pt3, pt4);
        
        vec_pt.clear();
        vec_pt.push_back( pt3 );
        vec_pt.push_back( pt4 );
//        vec_pt.push_back( pt2 );
//        vec_pt.push_back( pt1 );

//        fill_segment_falloff(vec_pt, pt1, pt2, falloff2, falloff2_1, opacity, scurve, buffer, rc);
        draw_polygon_vec(vec_pt, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);

        if ( !first_time ) {
          vec_pt.clear();
//          if ( generate_arc_points(pt1, pt4_prev, pt3, vec_pt, clockwise) ) {
//            vec_pt.push_back( pt1 );
//            fill_point_falloff(vec_pt, pt1, falloff2, falloff2_1, opacity, scurve, buffer, rc);
//          }
          
          mid_pt.line_mid_point(pt4_prev, pt3);
//          PF::Polygon::expand_segment_to(pt1, mid_pt, pt_expand, expand);
          
          vec_pt.push_back( pt4_prev );
//          vec_pt.push_back( pt_expand );
          vec_pt.push_back( pt3 );
//          vec_pt.push_back( pt1 );
//            fill_point_falloff(vec_pt, pt1, falloff2, falloff2_1, opacity, scurve, buffer, rc);
          draw_polygon_vec(vec_pt, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
        }
        else {
          pt3_first = pt3;
          first_time = false;
        }

        pt1 = pt2;
        pt4_prev = pt4;
      }
    }



    if ( closed_shape ) {
      points.pop_back();
    }

}

void PF::ShapesConfigGUI::draw_polygon_falloff2(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
    int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  if ( shape->get_closed_shape() ) {
    bool first, intersects;
    int i;
    std::vector<Point> vec_pt;
    Point pt1, pt2;
    Point pt1_prev, pt2_prev, pt1_first;
    Point pts1, pts2;
    Point a, b, c, d;
    Point pt_end, pt_start, pt_intersection;
//    Point pt4_prev;
    SegmentInfo si;
    bool clock = shape->is_clockwise();
    float pen_size = (shape->get_fill_shape()) ? 0.f: (shape->get_pen_size()/2.f);
    float expand = pen_size+shape->get_falloff();
    bool first_time = true;
    float falloff2 = shape->get_falloff()*shape->get_falloff();
    float falloff2_1 = 1.f / falloff2;

    //    std::cout<<"PF::Polygon::get_falloff_points(): clock: "<<clock<<std::endl;

    for (int s = 0; s < shape->get_segments_count(); s++) {
      shape->get_segment(s, pts1, pts2, si);
      vec_pt.clear();

      if (si.get_segment_type() == PF::SegmentInfo::line) {
        vec_pt.push_back(pts1);
        vec_pt.push_back(pts2);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        shape->inter_bezier3(pts1, pts2, si.get_control_pt1(), vec_pt);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        shape->inter_bezier4(pts1, pts2, si.get_control_pt1(), si.get_control_pt2(), vec_pt);
      }
      
//      fill_polygon_segment_falloff(vec_pt, pt1, pt2, clock, expand, get_opacity(), scurve, buffer, rc);
      draw_polygon_segment_falloff(vec_pt, pt1, pt2, clock, expand, shape, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);

      if ( !first_time ) {
        vec_pt.clear();
          if ( shape->generate_arc_points(pts1, pt2_prev, pt1, vec_pt, clock) ) {
//            vec_pt.push_back( pts1 );
//            fill_point_falloff(vec_pt, pts1, falloff2, falloff2_1, get_opacity(), scurve, buffer, rc);
            draw_polygon_vec(vec_pt, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
          }
      }
      else {
        pt1_first = pt1;
        first_time = false;
      }
      
      pt1_prev = pt1;
      pt2_prev = pt2;
    }

    vec_pt.clear();
    shape->get_segment(0, pts1, pts2, si);
      if ( shape->generate_arc_points(pts1, pt2_prev, pt1_first, vec_pt, clock) ) {
//        vec_pt.push_back( pts1 );
//        fill_point_falloff(vec_pt, pts1, falloff2, falloff2_1, get_opacity(), scurve, buffer, rc);
        draw_polygon_vec(vec_pt, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
      }

  }

}


void PF::ShapesConfigGUI::draw_shape_polygon(Polygon* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
                                        int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  Point pt1, pt2, pt3, ptc1, ptc2;
  SegmentInfo si;
  Point a, b, c, d;
  Point a1, b1, c1, d1;

  int fall_dist = shape->get_falloff();
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

  int cntrl1_selected = -1;
  if (hit_t == PF::Shape::hit_control_point) 
    cntrl1_selected = hit_additional;

  int cntrl2_selected = -1;
  if (hit_t == PF::Shape::hit_control_point_end) 
    cntrl2_selected = hit_additional;

  // draw segments
  if ( shape->get_fill_shape() ) {
    for (int i = 0; i < shape->get_segments_count(); i++) {
      shape->get_segment(i, pt1, pt2, si);
  
      pt_image2screen( pt1 );
      pt_image2screen( pt2 );
  
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
  }
  else {
    pt1.x = pt1.y = shape->get_pen_size()/2.f;
    pt_image2screen( pt1 );
    float pen_size = pt1.x;
    
    for (int i = 0; i < shape->get_segments_count(); i++) {
      shape->get_segment(i, pt1, pt2, si);
  
      pt_image2screen( pt1 );
      pt_image2screen( pt2 );
  
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        draw_line(pt1, pt2, pen_size, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        ptc1 = si.get_control_pt1();
        pt_image2screen( ptc1 );
        draw_bezier3(pt1, pt2, ptc1, pen_size, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        ptc1 = si.get_control_pt1();
        pt_image2screen( ptc1 );
        ptc2 = si.get_control_pt2();
        pt_image2screen( ptc2 );
        draw_bezier4(pt1, pt2, ptc1, ptc2, pen_size, buf_in, buf_out, (segm_selected == -1 || segm_selected == i), selected);
      }
    }
    
    if ( !shape->get_closed_shape() ) {
      draw_circle(pt2, pen_size, buf_in, buf_out, (segm_selected == -1), selected);
    }
  }
  
  // draw nodes
  if (hit_t != PF::Shape::hit_none) {
    for (int i = shape->get_points_count()-1; i >= 0 ; i--) {
      pt1 = shape->get_point(i);
      pt_image2screen( pt1 );

      draw_node(pt1.get_x(), pt1.get_y(), buf_out, (node_selected==i));
    }
  }

  // draw control points
    if (hit_node_selected >= 0) {
    draw_node_control_points(shape, buf_in, buf_out, hit_node_selected, cntrl1_selected, cntrl2_selected, selected);
  }
   
/*  if (hit_t != PF::Shape::hit_none) {
    for (int i = shape->get_points_count()-1; i >= 0 ; i--) {
      draw_node_control_points(shape, buf_in, buf_out, i, cntrl1_selected, cntrl2_selected, selected);
    }
  }
*/
//    draw_polygon_falloff2(shape, buf_in, buf_out, hit_t, hit_additional, hit_node_selected, selected);
    
  if ( false && shape->get_falloff() > 0.f && shape->get_closed_shape() ) {
    std::vector<Point> points;
    
    shape->get_falloff_points(points);
    if ( points.size() > 0 ) {
      pt1 = points[0];
      pt_image2screen( pt1 );
      for (int i = 1; i < points.size(); i++) {
        pt2 = points[i];  
        pt_image2screen( pt2 );
    
        draw_line(pt1, pt2, buf_in, buf_out, (segm_selected == -1), selected);
//        draw_dot( pt1, buf_out, selected );
        
        pt1 = pt2;
      }
      
      pt2 = points[0];  
      pt_image2screen( pt2 );
  
      draw_line(pt1, pt2, buf_in, buf_out, (segm_selected == -1), selected);

    }
  }

  if (shape->get_has_source()) {
    Polygon source = *shape;
    source.set_has_source(false);
    source.offset(shape->get_source_point());
    draw_shape_polygon(&source, buf_in, buf_out, hit_t, hit_additional, -1, selected);
  }

}

void PF::ShapesConfigGUI::draw_shape_line(Line* shape, PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out, 
                                      int hit_t, int hit_additional, int hit_node_selected, bool selected)
{
  Polygon* polygon = dynamic_cast<Polygon*>(shape);
  draw_shape_polygon(polygon, buf_in, buf_out, hit_t, hit_additional, -1, selected);
  
}

void PF::ShapesConfigGUI::draw_mouse_pointer_circle(PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out)
{
  float radius = 0, radiusf = 0;
  float pen_size = (get_fill_shape()) ? 0: (get_pen_size()/2.f);
  Point pt1;
  
  if ( get_radius() > 0.f ) {
    pt1.set( get_radius(), get_radius() );
    pt_image2screen( pt1 );
    radius = pt1.x;
  }
  
  if ( get_falloff() > 0.f ) {
    pt1.set( get_falloff()+get_radius()+pen_size, get_falloff()+get_radius()+pen_size );
    pt_image2screen( pt1 );
    radiusf = pt1.x;
  }
  
  pt1 = pt_current;
  
  if ( radius > 0.f ) buf_out.draw_circle( pt1.x, pt1.y, radius, buf_in );
  if ( radiusf > 0.f ) buf_out.draw_circle( pt1.x, pt1.y, radiusf, buf_in );
}

bool PF::ShapesConfigGUI::draw_mouse_pointer(PF::PixelBuffer& buf_in, PF::PixelBuffer& buf_out)
{
  bool refresh = false;
  
  if (!mo_dragging && !mo_adding) {
    if ( get_current_shape_type() == PF::Shape::circle ) {
      draw_mouse_pointer_circle(buf_in, buf_out);
      refresh = true;
    }
  }
  
  return refresh;
}

void PF::ShapesConfigGUI::btn_shapes_type_clicked()
{
  mo_current_shape_type = btn_shapes.get_btn_active();
  
  if ( mo_current_shape_type == PF::Shape::shape ) {
    mo_hit_test = PF::Shape::hit_none;
    mo_shape_index = -1;
    mo_shape_additional = -1;
  }
  
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


void PF::ShapesConfigGUI::add_new_shape(PF::ShapesPar* par, int shape_type, Point& initial_pos, Point& source_pos, int mod_key)
{
  switch (shape_type)
  {
  case PF::Shape::polygon:
  {
//    std::cout<<"PF::ShapesConfigGUI::add_new_shape(): PF::Shape::polygon "<<std::endl;
    
    Polygon shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_fill_shape(), get_has_source(), source_pos );
    shape.set_color(par->get_shapes_group().get_current_color());
    
    polygon_add = shape; // set the shape to be added
    mo_adding = PF::Shape::polygon; // we are adding a new shape
    
    if (mod_key == SHAPE_KEY_ADD_BEZIER3) {
      polygon_add.set_segment_type(0, PF::SegmentInfo::bezier3_l);
    }
    else if (mod_key == SHAPE_KEY_ADD_BEZIER4) {
      polygon_add.set_segment_type(0, PF::SegmentInfo::bezier4);
    }
    else if (mod_key == SHAPE_KEY_ADD_LINE) {
      polygon_add.set_segment_type(0, PF::SegmentInfo::line);
    }

    defalut_new_polygon(initial_pos, mod_key );
  }
   break;
  case PF::Shape::line:
  {
    Line shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_fill_shape(), get_has_source(), source_pos );
    shape.set_color(par->get_shapes_group().get_current_color());
    
    line_add = shape; // set the shape to be added
    mo_adding = PF::Shape::line; // we are adding a new shape
    
    defalut_new_line(initial_pos, mod_key );
  }
   break;
  case PF::Shape::circle:
    {
      Circle shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_fill_shape(), get_has_source(), source_pos );
      shape.set_color(par->get_shapes_group().get_current_color());
      shape.set_radius(get_radius());
      
      mo_hit_test = PF::Shape::hit_shape;
      mo_shape_index = par->add_shape(&shape);
      
      new_shape_added( mo_shape_index );
    }
    break;
  case PF::Shape::ellipse:
    {
      Ellipse shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_fill_shape(), get_has_source(), source_pos );
      shape.set_color(par->get_shapes_group().get_current_color());
      shape.set_angle(get_angle());
      shape.set_radius_x(get_radius_x());
      shape.set_radius_y(get_radius_y());

      mo_hit_test = PF::Shape::hit_shape;
      mo_shape_index = par->add_shape(&shape);
      
      new_shape_added( mo_shape_index );
    }
    break;
  case PF::Shape::rectangle:
  {
    Rectangle shape( initial_pos, get_pen_size(), get_opacity(), get_falloff(), get_fill_shape(), get_has_source(), source_pos );
        shape.set_color(par->get_shapes_group().get_current_color());
        shape.set_rounded_corners( get_rect_rounded() );
        
    rectangle_add = shape;
    
    mo_adding = PF::Shape::rectangle; // we are adding a new shape
  }
   break;
  default:
    std::cout<<"PF::ShapesConfigGUI::add_new_shape(): invalid shape type: "<<shape_type<<std::endl;
    break;
  }
  
  if ( get_current_shape_type() != PF::Shape::polygon && get_current_shape_type() != PF::Shape::line && get_current_shape_type() != PF::Shape::rectangle )
  {
    done_adding_shape();
  }

}

void PF::ShapesConfigGUI::selection_changed()
{
  bool circle_selected = false;
  bool ellipse_selected = false;
  bool rectangle_selected = false;
  
  PF::ShapesPar* par = dynamic_cast<PF::ShapesPar*>(get_par());
  if( par ) {
  
    if ( get_selection_count() > 0 ) {
      Shape* shape = par->get_shape( get_selected(0) );
      
      set_pen_size( shape->get_pen_size() );
      set_falloff( shape->get_falloff() );
      set_opacity( shape->get_opacity() );
      par->get_shapes_group().set_current_color( shape->get_color() );
      set_fill_shape( shape->get_fill_shape() );
    }
    
    for (int i = 0; i < get_selection_count(); i++ ) {
        Shape* shape = par->get_shape( get_selected(i) );
        
        if (shape->get_type() == PF::Shape::line) {
          Line* line = dynamic_cast<Line*>(shape);
        }
        else if (shape->get_type() == PF::Shape::circle) {
          if ( !circle_selected ) {
            Circle* circle = dynamic_cast<Circle*>(shape);
            set_radius( circle->get_radius() );
            circle_selected = true;
          }
        }
        else if (shape->get_type() == PF::Shape::ellipse) {
          if ( !ellipse_selected ) {
            Ellipse* ellipse = dynamic_cast<Ellipse*>(shape);
            set_radius_x( ellipse->get_radius_x() );
            set_radius_y( ellipse->get_radius_y() );
            set_angle( ellipse->get_angle() );
            ellipse_selected = true;
          }
        }
        else if (shape->get_type() == PF::Shape::rectangle) {
          if ( !rectangle_selected ) {
            Rectangle* rect = dynamic_cast<Rectangle*>(shape);
            set_rect_rounded( rect->get_rounded_corners() );
            rectangle_selected = true;
          }
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
  
  if ( rectangle_selected || get_current_shape_type() == PF::Shape::rectangle )
    rect_rounded_btn.show();
  else
    rect_rounded_btn.hide();
  
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

void PF::ShapesConfigGUI::selection_clear()
{
  shapes_selected.clear();
  
  mo_shape_node_selected = -1;
  mo_node_selected = -1;

  selection_changed();
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
/*
void PF::ShapesConfigGUI::adjust_polygon_falloff_point(Polygon* shape, int n)
{
  Point center, ptf;
  Point& pt = shape->get_point(n);
  center = shape->get_center();
    
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

void PF::ShapesConfigGUI::defalut_polygon_new_control_point(Polygon* shape, int hit_t, int node)
{
  const int node_prev = (node>0) ? node-1: shape->get_points_count()-1;
  const int node_next = (node<shape->get_points_count()-1) ? node+1: 0;
  const int segment = node;
  const int segment_prev = (node>0) ? node-1: shape->get_points_count()-1;
  
  if ( hit_t == PF::SegmentInfo::bezier4 ) {
    // segment was changed from bezier3_l/r to bezier4, the new control point is 2 for the previous segment
    if ( shape->get_segment_type(segment) == PF::SegmentInfo::bezier4 || shape->get_segment_type(segment) == PF::SegmentInfo::bezier3_l ) {
      // if there is a right node, mirror it
      shape->mirror_control_pt2(segment_prev, shape->get_control_pt1(segment), shape->get_point(node));
    }
    else {
      std::cout<<"PF::ShapesConfigGUI::defalut_polygon_new_control_point(): invalid segment type: "<<shape->get_segment_type(segment)<<std::endl;
    }
  }
  else if ( hit_t == PF::SegmentInfo::bezier3_r ) {
    // segment was changed from line to bezier3_r, the new control point is 2 for the previous segment
    // should not be a control point on the current segment
    if ( shape->get_segment_type(segment) == PF::SegmentInfo::line ) {
      PF::Polygon::default_segment_control_point(shape->get_point(node_prev), shape->get_point(node), shape->get_control_pt1(node_prev), SHAPE_BEZIER_T, shape->is_clockwise());
    }
    else {
      std::cout<<"PF::ShapesConfigGUI::defalut_polygon_new_control_point(): invalid segment type(2): "<<shape->get_segment_type(segment)<<std::endl;
    }
  }
  else if ( hit_t == PF::SegmentInfo::bezier3_l ) {
    // segment was changed from line to bezier3_l, the new control point is 1 for the current segment
    // if there is a left control point, mirror it
    if ( shape->get_segment_type(segment_prev) == PF::SegmentInfo::bezier4 ) {
      // if there is a right node, mirror it
      shape->mirror_control_pt1(segment, shape->get_control_pt2(segment_prev), shape->get_point(node));
    }
    else if ( shape->get_segment_type(segment_prev) == PF::SegmentInfo::bezier3_r ) {
      // if there is a right node, mirror it
      shape->mirror_control_pt1(segment, shape->get_control_pt1(segment_prev), shape->get_point(node));
    }
    else if ( shape->get_segment_type(segment) == PF::SegmentInfo::line ) {
      // just default the new control point
      PF::Polygon::default_segment_control_point(shape->get_point(node), shape->get_point(node_next), shape->get_control_pt1(node), SHAPE_BEZIER_T, shape->is_clockwise());
    }
    else {
      std::cout<<"PF::ShapesConfigGUI::defalut_polygon_new_control_point(): invalid segment type(3): "<<shape->get_segment_type(segment)<<std::endl;
    }
  }
  else {
    std::cout<<"PF::ShapesConfigGUI::defalut_polygon_new_control_point(): invalid segment type(4): "<<shape->get_segment_type(segment)<<std::endl;
  }

}
*/
void PF::ShapesConfigGUI::defalut_polygon_last_point( PF::Polygon* shape )
{
  if (shape->get_segments_count() < 3) { // a polygon has two segments when creating it
    int segment = 0;
    int point = 0;
    if ( shape->get_segment_type(segment) == PF::SegmentInfo::bezier3_l || shape->get_segment_type(segment) == PF::SegmentInfo::bezier3_r ) {
      PF::Polygon::default_segment_control_point(shape->get_point(point), shape->get_point(point+1), shape->get_control_pt1(segment), SHAPE_BEZIER_T, shape->is_clockwise());
    }
    else if ( shape->get_segment_type(segment) == PF::SegmentInfo::bezier4 ) {
      PF::Polygon::default_segment_control_point(shape->get_point(point), shape->get_point(point+1), shape->get_control_pt1(segment), SHAPE_BEZIER_T, shape->is_clockwise());
      PF::Polygon::mirror_control_point(shape->get_point(point), shape->get_control_pt1(segment), shape->get_control_pt2(segment+1));
    }
    segment = 1;
    point = 1;
    if ( shape->get_segment_type(segment) == PF::SegmentInfo::bezier3_l || shape->get_segment_type(segment) == PF::SegmentInfo::bezier3_r ) {
      PF::Polygon::default_segment_control_point(shape->get_point(point), shape->get_point(point-1), shape->get_control_pt1(segment), SHAPE_BEZIER_T, shape->is_clockwise());
    }
    else if ( shape->get_segment_type(segment) == PF::SegmentInfo::bezier4 ) {
      PF::Polygon::default_segment_control_point(shape->get_point(point), shape->get_point(point-1), shape->get_control_pt1(segment), SHAPE_BEZIER_T, shape->is_clockwise());
      PF::Polygon::mirror_control_point(shape->get_point(point), shape->get_control_pt1(segment), shape->get_control_pt2(segment-1));
    }
  }
  else {
    shape->default_node_controls_point(shape->get_points_count()-2, SHAPE_BEZIER_T);        
    shape->default_node_controls_point(shape->get_points_count()-1, SHAPE_BEZIER_T);        
    shape->default_node_controls_point(0, SHAPE_BEZIER_T);        
  }
}

void PF::ShapesConfigGUI::defalut_new_polygon(Point& pt, int mod_key )
{
  const int new_point = polygon_add.get_points_count()-1;
  const int new_segment = polygon_add.get_segments_count()-1;
  
  // set the new position of the new point
  polygon_add.get_point(new_point).set(pt);
  
  // set the new segment type based on the pressed key
  if (mod_key == SHAPE_KEY_ADD_BEZIER3) {
    polygon_add.set_segment_type(new_segment, PF::SegmentInfo::bezier3_l);
  }
  else if (mod_key == SHAPE_KEY_ADD_BEZIER4) {
    polygon_add.set_segment_type(new_segment, PF::SegmentInfo::bezier4);
  }
  else if (mod_key == SHAPE_KEY_ADD_LINE) {
    polygon_add.set_segment_type(new_segment, PF::SegmentInfo::line);
  }

  defalut_polygon_last_point( &polygon_add );

}
/*
void PF::ShapesConfigGUI::defalut_line_new_control_point(Line* shape, int hit_t, int node)
{
  if ( node > 0 && node < shape->get_points_count()-1 ) {
    Polygon* polygon = dynamic_cast<Polygon*>(shape);
    defalut_polygon_new_control_point(polygon, hit_t, node);
  }
  else {
    const int segment = node;
    
    if (node == 0) {
      PF::Polygon::default_segment_control_point(shape->get_point(node), shape->get_point(node+1), shape->get_control_pt1(node), SHAPE_BEZIER_T, shape->is_clockwise());
    }
    else if ( node == shape->get_points_count()-1 ) {
      PF::Polygon::default_segment_control_point(shape->get_point(node-1), shape->get_point(node), shape->get_control_pt1(node-1), SHAPE_BEZIER_T, shape->is_clockwise());
    }
  }
}
*/
void PF::ShapesConfigGUI::defalut_line_last_point( PF::Line* shape )
{
  if (shape->get_segments_count() == 1) {
    int segment = 0;
    int point = 0;
    if ( shape->get_segment_type(segment) == PF::SegmentInfo::bezier3_l || shape->get_segment_type(segment) == PF::SegmentInfo::bezier3_r ) {
      PF::Line::default_segment_control_point(shape->get_point(point), shape->get_point(point+1), shape->get_control_pt1(segment), SHAPE_BEZIER_T, shape->is_clockwise());
    }
    else if ( shape->get_segment_type(segment) == PF::SegmentInfo::bezier4 ) {
      PF::Polygon::default_segment_control_point(shape->get_point(point), shape->get_point(point+1), shape->get_control_pt1(segment), SHAPE_BEZIER_T, shape->is_clockwise());
      PF::Polygon::default_segment_control_point(shape->get_point(point), shape->get_point(point+1), shape->get_control_pt2(segment), SHAPE_BEZIER_T, shape->is_clockwise());
    }
  }
  else {
    shape->default_node_controls_point(shape->get_points_count()-2, SHAPE_BEZIER_T);        
    shape->default_node_controls_point(shape->get_points_count()-1, SHAPE_BEZIER_T);        
  }
}

void PF::ShapesConfigGUI::defalut_new_line(Point& pt, int mod_key )
{
  const int new_point = line_add.get_points_count()-1;
  const int new_segment = line_add.get_segments_count()-1;
  
  // set the new position of the new point
  line_add.get_point(new_point).set(pt);
  
  // set the new segment type based on the pressed key
  if ( new_segment > 0 ) {
    if (mod_key == SHAPE_KEY_ADD_BEZIER3) {
      line_add.set_segment_type(new_segment, PF::SegmentInfo::bezier3_l);
    }
    else if (mod_key == SHAPE_KEY_ADD_BEZIER4) {
      line_add.set_segment_type(new_segment, PF::SegmentInfo::bezier3_l);
      if ( new_segment > 1 )
        line_add.set_segment_type(new_segment-1, PF::SegmentInfo::bezier4);
      else
        line_add.set_segment_type(new_segment-1, PF::SegmentInfo::bezier3_r);
    }
    else if (mod_key == SHAPE_KEY_ADD_LINE) {
      line_add.set_segment_type(new_segment, PF::SegmentInfo::line);
    }
  }

  defalut_line_last_point( &line_add );

}


// deletes the shape currently hit_tested based on the hit_test position
bool PF::ShapesConfigGUI::delete_shape(PF::ShapesPar* par)
{
  bool deleted = false;
  
  if ( mo_shape_index >= 0 ) {
    Shape* shape = par->get_shape(mo_shape_index);
    
    if ( (shape->get_type() != PF::Shape::line && (mo_hit_test == PF::Shape::hit_shape || mo_hit_test == PF::Shape::hit_falloff) ) ||
        (shape->get_type() == PF::Shape::line && (mo_hit_test == PF::Shape::hit_segment || mo_hit_test == PF::Shape::hit_falloff_segment)) ) {
      
      delete_shape_before( mo_shape_index );
      
      par->remove_shape(mo_shape_index);
      
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
      PF::Shape::bubble_sort(shapes_selected);

      mo_hit_test = PF::Shape::hit_none;
      mo_shape_index = -1;
      mo_shape_additional = -1;
      
      for ( int i = shapes_selected.size()-1; i >= 0; i--) {
        delete_shape_before( shapes_selected[i] );
        par->remove_shape( shapes_selected[i] );
      }
      
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
  else if (!mo_dragging) {
    mo_hit_test = PF::Shape::hit_none;
    mo_shape_index = -1;
    mo_shape_additional = -1;
    
    mo_shape_node_selected = -1;
    mo_node_selected = -1;
  }
}

bool PF::ShapesConfigGUI::handle_set_source_point(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  if ( mo_adding == PF::Shape::shape && button == 1 && mod_key == SHAPE_KEY_SET_SOURCE_POINT && get_current_shape_type() != PF::Shape::shape ) {
    pt_source.set(sx, sy);
    last_pt_origin.set( -1, -1 );
    
    handled = true;
  }

  return handled;
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

    add_new_shape(par, get_current_shape_type(), pt, pts, mod_key);

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
            if ( mo_hit_test == PF::Shape::hit_node ) {
              Polygon* polygon = dynamic_cast<Polygon*>(shape);
              
              SegmentInfo si_prev = polygon->get_segment_info(mo_shape_additional);
              if ( si_prev.get_segment_type() == PF::SegmentInfo::bezier3_r || si_prev.get_segment_type() == PF::SegmentInfo::bezier4 ) {
                SegmentInfo& si_l = polygon->get_segment_info(mo_shape_additional-1);
                if ( si_l.get_segment_type() == PF::SegmentInfo::line || si_l.get_segment_type() == PF::SegmentInfo::bezier3_r ) {
                  si_l.set_segment_type( PF::SegmentInfo::bezier3_r );
                  if ( si_prev.get_segment_type() == PF::SegmentInfo::bezier3_r )
                    si_l.set_control_pt1( si_prev.get_control_pt1() );
                  else
                    si_l.set_control_pt1( si_prev.get_control_pt2() );
                }
                else if ( si_l.get_segment_type() == PF::SegmentInfo::bezier3_l || si_l.get_segment_type() == PF::SegmentInfo::bezier4 ) {
                  si_l.set_segment_type( PF::SegmentInfo::bezier4 );
                  if ( si_prev.get_segment_type() == PF::SegmentInfo::bezier3_r )
                    si_l.set_control_pt2( si_prev.get_control_pt1() );
                  else
                    si_l.set_control_pt2( si_prev.get_control_pt2() );
                }
              }
            }
            
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
          Polygon* polygon = dynamic_cast<Polygon*>(shape);
          
          SegmentInfo si_prev = polygon->get_segment_info(mo_shape_additional);
          
          int new_point = shape->insert_point(pt, mo_hit_test, mo_shape_additional);
          
          if ( new_point >= 0 ) {
//            std::cout<<"ShapesConfigGUI::pointer_press_event(): add a node to a line: mo_shape_additional: "<<mo_shape_additional<<std::endl;
//            std::cout<<"ShapesConfigGUI::pointer_press_event(): add a node to a line: new_point: "<<new_point<<std::endl;
            SegmentInfo& si_l = polygon->get_segment_info(new_point-1);
            SegmentInfo& si_r = polygon->get_segment_info(new_point);
            if ( si_prev.get_segment_type() == PF::SegmentInfo::bezier3_r ) {
              si_r.set_control_pt1( si_prev.get_control_pt1() );
              si_r.set_segment_type( PF::SegmentInfo::bezier3_r );
              si_l.set_segment_type( PF::SegmentInfo::line );
            }
            else if ( si_prev.get_segment_type() == PF::SegmentInfo::bezier4 ) {
              si_r.set_control_pt2( si_prev.get_control_pt2() );
              si_r.set_segment_type( PF::SegmentInfo::bezier4 );
              polygon->default_node_controls_point(new_point, SHAPE_BEZIER_T);
            }
          }

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
          if ( shape->get_type() == PF::Shape::polygon ) {
            Polygon* polygon = dynamic_cast<Polygon*>(shape);
            polygon->insert_control_point(pt, mo_hit_test, mo_shape_additional, SHAPE_BEZIER_T);
          }
          else if ( shape->get_type() == PF::Shape::line ) {
            Line* line = dynamic_cast<Line*>(shape);
            line->insert_control_point(pt, mo_hit_test, mo_shape_additional, SHAPE_BEZIER_T);
          }

          par->shapes_modified();

          refresh = true;
          handled = true;
        }
      }
    }
  }

  return handled;
}

bool PF::ShapesConfigGUI::handle_selection(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh, int action)
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
    else {
      // click anywhere on a shape with no keys: select the shape (action == 2 --> release mouse)
      if ( mo_hit_test != PF::Shape::hit_none && mod_key == PF::MOD_KEY_NONE && action == 2 ) {
        selection_replace(mo_shape_index);
      }
      // mouse down + CNTRL anywhere on a shape: select the shape (action == 1 --> mouse down)
      else if ( button == 1 && mo_hit_test != PF::Shape::hit_none && mod_key == SHAPE_KEY_SELECT_ADD_SHAPE && action == 1 ) {
        selection_add(mo_shape_index);
      }
      // mouse down on a non selected shape: replace selection
      else if ( button == 1 && mod_key == PF::MOD_KEY_NONE && !is_shape_selected(mo_shape_index) && action == 1 ) {
        selection_replace(mo_shape_index);
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
        if ( /*mod_key == PF::MOD_KEY_NONE ||*/ mod_key == SHAPE_KEY_SELECT_NODE ) {
          mo_shape_node_selected = mo_shape_index;
          mo_node_selected = mo_shape_additional;
        }
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
      int new_shape_index = -1;
      
      switch (mo_adding)
      {
      case PF::Shape::polygon:
        //      std::cout<<"ShapesConfigGUI::end_adding_new_shape(): adding line commit"<<std::endl;
        if ( polygon_add.get_segments_count() >= polygon_add.get_min_segments() ) {
          new_shape_index = par->add_shape(&polygon_add);
        }
        break;
      case PF::Shape::line:
        //      std::cout<<"ShapesConfigGUI::end_adding_new_shape(): adding line commit"<<std::endl;
        if ( line_add.get_points().size() >= line_add.get_min_segments() ) {
          new_shape_index = par->add_shape(&line_add);
        }
        break;
      case PF::Shape::rectangle:
        break;
      default:
        std::cout<<"ShapesConfigGUI::end_adding_new_shape(): invalid shape type: "<<mo_adding<<std::endl;
        break;
      }

      if ( new_shape_index >= 0 ) {
        done_adding_shape();
        
        new_shape_added( new_shape_index );
      }
      
      mo_adding = PF::Shape::shape;
      
      refresh = true;
      handled = true;
    }
  }
  else if ( button == 1 ) {
    if ( mo_adding == PF::Shape::rectangle ) {
      int new_shape_index = -1;
      
      new_shape_index = par->add_shape(&rectangle_add);

      done_adding_shape();
      
      mo_adding = PF::Shape::shape;

      if ( new_shape_index >= 0 ) {
        new_shape_added( new_shape_index );
      }
      
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
    case PF::Shape::rectangle:
    {
//      std::cout<<"ShapesConfigGUI::pointer_motion_event(): adjusting new point to polygon"<<std::endl;
      Point pt(sx, sy);
      Point ptc(pt_current);
      pt_screen2image( ptc );
      pt_screen2image( pt );
      rectangle_add.offset(PF::Shape::hit_node, ptc, pt, 2, false, 0);
      pt_current.set(sx,sy);
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

  if ( !mo_dragging && button == 1 && (mod_key == PF::MOD_KEY_NONE || mod_key == SHAPE_KEY_UNLOCK_SOURCE) ) {
    // to start dragging check if mouse is over a shape
    if ( mo_shape_index >= 0 ) {
      mo_dragging = true;
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
//      std::cout<<"PF::ShapesConfigGUI::handle_drag_shape(): mo_shape_index: "<<mo_shape_index<<std::endl;
      Shape* shape = par->get_shape(mo_shape_index);

      shape->offset(mo_hit_test, prev_pt, curr_pt, mo_shape_additional, (mod_key == SHAPE_KEY_UNLOCK_SOURCE), PF::SegmentInfo::in_sinch);
      shape_expanded(shape);
    }
    else {
      for (int i = 0; i < get_selection_count(); i++) {
        Shape* shape = par->get_shape( get_selected(i) );
        int hit_t = PF::Shape::hit_shape;
        
        shape->offset(hit_t, prev_pt, curr_pt, mo_shape_additional, (mod_key == SHAPE_KEY_UNLOCK_SOURCE), PF::SegmentInfo::in_sinch);
      }
   }
    
    pt_current.set(sx, sy);

    par->shapes_modified();

    refresh = true;
    handled = true;
  }

  return handled;
}

void PF::ShapesConfigGUI::handle_expand_shape(Shape* shape, int mod_key, int direction)
{
  if (mod_key == PF::MOD_KEY_NONE) {
    //        std::cout<<"ShapesConfigGUI::handle_expand_shape() expand"<<std::endl;
    shape->expand((direction==PF::DIRECTION_KEY_UP)?10:-10);
//    shape_expanded(shape);
  }
  if (mod_key == PF::MOD_KEY_SHIFT) {
    //        std::cout<<"ShapesConfigGUI::handle_expand_shape() expand falloff"<<std::endl;
    shape->expand_falloff((direction==PF::DIRECTION_KEY_UP)?2:-2);
//    set_falloff(shape->get_falloff());
  }
  if (mod_key == PF::MOD_KEY_CTRL) {
    //        std::cout<<"ShapesConfigGUI::handle_expand_shape() expand opacity"<<std::endl;
    shape->expand_opacity((direction==PF::DIRECTION_KEY_UP)?.10f:-.10f);
//    set_opacity(shape->get_opacity());
  }
  if (mod_key == MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
    //        std::cout<<"ShapesConfigGUI::handle_expand_shape() expand opacity"<<std::endl;
    shape->expand_pen_size((direction==PF::DIRECTION_KEY_UP)?1.f:-1.f);
//    set_pen_size(shape->get_pen_size());
  }

}

bool PF::ShapesConfigGUI::handle_expand_shapes(PF::ShapesPar* par, int mod_key, int direction, bool& refresh)
{
  bool handled = false;

  if (mo_hit_test == PF::Shape::hit_none) {
    if (mod_key == PF::MOD_KEY_NONE) {
      //        std::cout<<"ShapesConfigGUI::handle_expand_shapes() expand"<<std::endl;
      if ( get_current_shape_type() == PF::Shape::circle ) {
        set_radius( get_radius() + ((direction==PF::DIRECTION_KEY_UP)?2.f:-2.f) );
      }
    }
    if (mod_key == PF::MOD_KEY_SHIFT) {
      //        std::cout<<"ShapesConfigGUI::handle_expand_shapes() expand falloff"<<std::endl;
      set_falloff( get_falloff() + ((direction==PF::DIRECTION_KEY_UP)?2.f:-2.f) );
    }
    if (mod_key == PF::MOD_KEY_CTRL) {
      //        std::cout<<"ShapesConfigGUI::handle_expand_shapes() expand opacity"<<std::endl;
      set_opacity( get_opacity() + ((direction==PF::DIRECTION_KEY_UP)?.10f:-.10f) );
    }
    if (mod_key == MOD_KEY_SHIFT+PF::MOD_KEY_CTRL) {
      //        std::cout<<"ShapesConfigGUI::handle_expand_shapes() expand opacity"<<std::endl;
      set_pen_size( get_pen_size() + ((direction==PF::DIRECTION_KEY_UP)?1.f:-1.f) );
    }

    handled = true;
    refresh = true;
  }
  else if (mo_hit_test != PF::Shape::hit_none && get_show_outline()) {
    if ( get_selection_count() > 1 ) {
      for (int i = 0; i < get_selection_count(); i++) {
        Shape* shape = par->get_shape( get_selected(i) );
        
        handle_expand_shape(shape, mod_key, direction);
      }
    }
    else {
      Shape* shape = par->get_shape(mo_shape_index);
      //      std::cout<<"ShapesConfigGUI::handle_expand_shapes() expand shapes"<<std::endl;
      handle_expand_shape(shape, mod_key, direction);
    }
    
    handled = true;
    refresh = true;
  }
  
  return handled;
}


bool PF::ShapesConfigGUI::update_hit_test(PF::ShapesPar* par, int button, int mod_key, double sx, double sy, bool& refresh)
{
  bool handled = false;

  // save the current position
  pt_current.set(sx, sy);
  
  if ( get_show_outline() && get_current_shape_type() == PF::Shape::shape ) {
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
    handled = handle_set_source_point(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_set_source_point"<<std::endl;
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
    handled = handle_selection(par, button, mod_key, sx, sy, refresh, 1);
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
}


bool PF::ShapesConfigGUI::pointer_release_event( int button, double sx, double sy, int mod_key )
{
  if( get_editting() ) return false;
  EVENT_BEGIN()
  
//  std::cout<<"ShapesConfigGUI::pointer_release_event()"<<std::endl;
  
  bool refresh = false;
  bool handled = false;
  
  set_editting(true);

  if (mo_dragging) {
    mo_dragging = false;
    refresh = true;
    handled = true;
  }
  if ( !handled ) {
    handled = handle_end_adding_new_shape(par, button, mod_key, sx, sy, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_press_event(): handle_end_adding_new_shape"<<std::endl;
  }
  if ( !handled ) {
    handled = handle_selection(par, button, mod_key, sx, sy, refresh, 2);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_release_event(): handle_selection"<<std::endl;
  }

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
  
  // draw the mouse pointer
  if ( get_current_shape_type() == PF::Shape::circle ) {
    refresh = true;
  }
  
  // refresh preview if something changed
  if ( refresh ) {
    image->update();
  }
  
  set_editting(false);

  return refresh;  
}

bool PF::ShapesConfigGUI::pointer_scroll_event( int direction, int mod_key )
{
  //  std::cout<<"ShapesConfigGUI::pointer_scroll_event() called"<<std::endl;

  if( get_editting() ) return false;
  EVENT_BEGIN();

  bool refresh = false;
  bool handled = false;
  
  set_editting(true);

  if ( !handled ) {
    handled = handle_expand_shapes(par, mod_key, direction, refresh);
    if ( handled ) std::cout<<"ShapesConfigGUI::pointer_scroll_event(): handle_expand_shapes"<<std::endl;
  }

  // refresh preview if something changed
  if ( refresh ) {
    image->update();
  }
  
  set_editting(false);

  return refresh;  
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
      draw_shape_line(&line_add, buf_in, buf_out, PF::Shape::hit_none, -1, -1, false);
      
      refresh = true;
      break;
    case PF::Shape::polygon:
      draw_shape_polygon(&polygon_add, buf_in, buf_out, PF::Shape::hit_none, -1, -1, false);
      
      refresh = true;
      break;
    case PF::Shape::rectangle:
      draw_shape_rectangle(&rectangle_add, buf_in, buf_out, PF::Shape::hit_none, -1, false);
      
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

  refresh |= draw_mouse_pointer(buf_in, buf_out);
  
  return refresh;
}
