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

#ifndef PF_SHAPES_HH
#define PF_SHAPES_HH

#include <math.h>

#include <assert.h>
#include <iostream>

#include <gtkmm.h>

#include "../base/format_info.hh"
#include "../base/color.hh"
#include "../base/processor.hh"
#include "../base/splinecurve.hh"
#include "../gui/doublebuffer.hh"

#include "curves.hh"

namespace PF 
{

class ShapeMask
{
public:
  ShapeMask() { }
  
  
};

// -----------------------------------
// Point
// -----------------------------------

class Point
{
  int x, y;

public:
  Point(): x(0), y(0)
  {
  }
  Point(int x1, int y1)
  {
    x = x1; y = y1;
  }
  virtual ~Point() {}
  
  int get_x() const { return x; }
  void set_x(int s) { x = s; }
  int get_y() const { return y; }
  void set_y(int s) { y = s; }

  void set(int x1, int y1) { x = x1; y = y1; }
  void set(Point& pt) { x = pt.get_x(); y = pt.get_y(); }
  
  void offset(int x1, int y1) { x += x1; y += y1; }
  void offset(Point& pt) { offset(pt.get_x(), pt.get_y()); }
  void offset(Point prev, Point curr);
  void offset_pos(int x1, int y1) { x = std::max(0, x+x1); y = std::max(0, y+y1); }
  void offset_pos(Point prev, Point curr);
  
  void scale(int scale);
  
  // Distance from pt to pt1, squared
  int distance2(int x, int y)
  {
    const float dx = x - this->get_x();
    const float dy = y - this->get_y();
     return dx * dx + dy * dy;
  };
  int distance2(Point& pt) { return distance2(pt.get_x(), pt.get_y()); };
  float distance2segment2(Point& l1, Point& l2); // distance from point to segment (l1, l2) squared
  
};

inline
bool operator ==(const Point& l, const Point& r)
{
  if( l.get_x() != r.get_x() ) return false;
  if( l.get_y() != r.get_y() ) return false;
  return true;
}

inline
bool operator !=(const Point& l, const Point& r)
{
  return( !(l==r) );
}


inline std::istream& operator >>( std::istream& str, Point& shape )
{
  int x, y;

  str>>x>>y;
  shape.set_x( x );
  shape.set_y( y );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Point& shape )
{
  str<<shape.get_x()<<" "<<shape.get_y()<<" ";
  return str;
}

template<> inline
void set_gobject_property< Point >(gpointer object, const std::string name,
    const Point& value)
{
}

template<> inline
void set_gobject_property< std::vector<Point> >(gpointer object, const std::string name,
    const std::vector<Point>& value)
{
}


// -----------------------------------
// Shape
// -----------------------------------

class Shape
{
  int shape_type;
  float opacity;
  float falloff;
  std::vector<Point> points;
  std::vector<Point> falloff_points;
  bool has_source;
  Point source_point;

protected:
  float *mask;

public:
  Shape(): shape_type(shape), opacity(1.f), falloff(0), has_source(false), mask(NULL)
  {
  }
  virtual ~Shape() { if (mask != NULL) free(mask); }
  
  enum shapeType
  {
    shape = 0,
    line = 1,
    circle = 2,
    rectangle = 3,
  };
  
  enum hitTest
  {
    hit_none = 0,
    hit_shape = 1,
    hit_falloff = 2,
    hit_segment = 3,
    hit_falloff_segment = 4,
    hit_node = 5,
    hit_falloff_node = 6,
    hit_source = 7
  };
  
  static const int hit_test_delta;
  
  int get_type() const { return shape_type; }
   void set_type(int s) { shape_type = s; }
   
   virtual int get_size() const { return 0; }
   virtual void set_size(int s) { }

   float get_opacity() const { return opacity; }
  void set_opacity(float s) { opacity = std::min(1.f, std::max(0.f, s)); }

  float get_falloff() const { return falloff; }
  void set_falloff(float f) { falloff = std::max(0.f, f); for (int i=0; i<falloff_points.size(); i++) falloff_points[i].set(falloff,falloff); }
  
  std::vector<Point>& get_points() { return points; }
  const std::vector<Point>& get_points() const { return points; }

  std::vector<Point>& get_falloff_points() { return falloff_points; }
  const std::vector<Point>& get_falloff_points() const { return falloff_points; }

  virtual Point& get_point(int n) { return points[n]; }
  virtual const Point& get_point(int n) const { return points[n]; }
  virtual void set_point(Point& pt, int n) { points[n] = pt; }
  
  virtual Point& get_falloff_point(int n) { return falloff_points[n]; }
  virtual const Point& get_falloff_point(int n) const { return falloff_points[n]; }

  virtual Point& point_back() { return points.back(); }
  
  bool get_has_source() const { return has_source; }
  void set_has_source(bool s) { has_source = s; }

  Point& get_source_point() { return source_point; }
  const Point& get_source_point() const { return source_point; }
  void set_source_point(Point s) { source_point = s; }

  virtual void build_mask() { }
  float *get_mask() { return mask; }
  void free_mask() { if (mask != NULL) free(mask); mask = NULL; }

  virtual int hit_test(Point& pt, int& additional) { return hit_none; }
  virtual bool hit_test_node(Point& pt_test, Point& pt_node);
  virtual int hit_test_node(int n, Point& pt, int& additional);
//  virtual int hit_test_falloff_node(Point& pt_node, Point& pt_test);
//  virtual int hit_test_falloff_node(int n, Point& pt, int& additional);
  
  virtual void offset(int x, int y);
  virtual void offset(Point& pt) { offset(pt.get_x(), pt.get_y()); }
  virtual void offset(Point prev, Point curr);
  virtual void offset_point(int n, int x, int y);
  virtual void offset_point(int n, Point prev, Point curr);
  void offset_falloff_point(int n, int x, int y);
  void offset_falloff_point(int n, Point prev, Point curr);
  virtual void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);

  void add_point(Point pt);
  void insert_point(Point& pt, int position);
  
  virtual void get_center(Point& pt);
  virtual void get_rect(VipsRect* rc);
  virtual void get_falloff_rect(VipsRect* rc);
  virtual void get_mask_rect(VipsRect* rc) { get_falloff_rect(rc); }

  virtual void expand(int n);
  virtual void expand_falloff(int n);
  virtual void expand_opacity(float f);
  
  virtual void scale(int scale);
  
  float get_triangle_area(Point& a, Point& b, Point& c);
  bool point_in_triangle(Point& pt, Point& a, Point& b, Point& c);
  bool point_in_rectangle(Point& pt, Point& a, Point& b, Point& c, Point& d);
//  static void circle_falloff(float* mask, VipsRect& rc, Point& center, int radius, int falloff);

};

inline
bool operator ==(const Shape& l, const Shape& r)
{
  if( l.get_type() != r.get_type() ) return false;
  if( l.get_opacity() != r.get_opacity() ) return false;
  if( l.get_falloff() != r.get_falloff() ) return false;
  if( l.get_points() != r.get_points() ) return false;
  if( l.get_falloff_points() != r.get_falloff_points() ) return false;
  if( l.get_has_source() != r.get_has_source() ) return false;
  if( l.get_source_point() != r.get_source_point() ) return false;
  return true;
      }

inline
bool operator !=(const Shape& l, const Shape& r)
      {
  return( !(l==r) );
      }


inline std::istream& operator >>( std::istream& str, Shape& shape )
{
  int s_type;
  float opacity;
  float falloff;
  bool has_source;

  str>>s_type>>opacity>>falloff>>shape.get_points()>>shape.get_falloff_points()>>has_source>>shape.get_source_point();
  shape.set_type( s_type );
  shape.set_opacity( opacity );
  shape.set_falloff( falloff );
  shape.set_has_source( has_source );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Shape& shape )
{
  str<<shape.get_type()<<" "<<shape.get_opacity()<<" "<<shape.get_falloff()<<" "<<shape.get_points()<<shape.get_falloff_points()<<shape.get_has_source()<<" "<<shape.get_source_point();
  return str;
}

template<> inline
void set_gobject_property< Shape >(gpointer object, const std::string name,
    const Shape& value)
{
}

template<> inline
void set_gobject_property< std::vector<Shape> >(gpointer object, const std::string name,
    const std::vector<Shape>& value)
{
}

// -----------------------------------
// Line
// -----------------------------------

class Line: public Shape
{

public:
  Line(): Shape()
  {
    set_type(line);
  }
  
  void expand_falloff(int n);
  
  void get_falloff_rect(VipsRect* rc);
  static void get_segment_rect(Point& pt1, Point& pt2, VipsRect* rc);
  static void get_segment_falloff_rect(Point& pt1, Point& pt2, int falloff, VipsRect* rc);
  static void get_expanded_rec_from_line(Point& pt1, Point& pt2, int amount, Point& a, Point& b, Point& c, Point& d);
  
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }
  void offset(Point prev, Point curr) { PF::Shape::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);

  int hit_test(Point& pt, int& additional);
  void build_mask();
  
};


// -----------------------------------
// Circle1
// -----------------------------------
class Circle1: public Shape
{
  int radius;

public:
  Circle1();
  
  int get_radius() const { return radius; }
  void set_radius(int s) { radius = s; }

  int get_size() const { return radius; }
  void set_size(int s) { radius = s; }

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(pt, 0); }
  
  void expand_falloff(int n);
  
  int hit_test(Point& pt, int& additional);
  
  void get_center(Point& pt) { pt = PF::Shape::get_point(0); }
  void expand(int n) { radius = std::max(0, radius+n); }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }

  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void build_mask();
  virtual void scale(int scale);
};

inline
bool operator ==(const Circle1& l, const Circle1& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
  if( l.get_radius() != r.get_radius() ) return false;
  return true;      
}

inline
bool operator !=(const Circle1& l, const Circle1& r)
{
  return( !(l==r) );
}

inline std::istream& operator >>( std::istream& str, Circle1& shape )
{
  assert(typeid(l) == typeid(r));

  Shape& pshape = static_cast<Shape&>(shape);
  int radius;
  
  str>>pshape>>radius;
  shape.set_radius(radius);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Circle1& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<shape.get_radius()<<" ";
  return str;
}

template<> inline
void set_gobject_property< Circle1 >(gpointer object, const std::string name,
    const Circle1& value)
{
}

template<> inline
void set_gobject_property< std::vector<Circle1> >(gpointer object, const std::string name,
    const std::vector<Circle1>& value)
{
}

// -----------------------------------
// Rect1
// -----------------------------------
class Rect1: public Shape
{

public:
  Rect1();
  
  Point& get_top_left() { return get_point(0); }
  void set_top_left(Point& s) { get_point(0) = s; }
  Point& get_bottom_right() { return get_point(1); }
  void set_bottom_right(Point& s) { get_point(1) = s; }

  Point& get_top_left_falloff() { return get_falloff_point(0); }
  void set_top_left_fallof(Point& s) { get_falloff_point(0) = s; }
  Point& get_bottom_right_falloff() { return get_falloff_point(1); }
  void set_bottom_right_fallof(Point& s) { get_falloff_point(1) = s; }

  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }


//  void offset_point(int n, int x, int y);
//  void offset_point(int n, Point prev, Point curr);
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

//  void get_rect(VipsRect* rc);
//  void get_falloff_rect(VipsRect* rc);
  bool hit_test_rect(Point& pt, Point& pt1, Point& pt2)
  {
    return ( pt.get_x() >= pt1.get_x()-PF::Shape::hit_test_delta && pt.get_x() <= pt2.get_x()+PF::Shape::hit_test_delta && 
        pt.get_y() >= pt1.get_y()-PF::Shape::hit_test_delta && pt.get_y() <= pt2.get_y()+PF::Shape::hit_test_delta );
  }
  bool hit_test_rect(Point& pt, VipsRect* rc)
  {
    return ( pt.get_x() >= rc->left-PF::Shape::hit_test_delta && pt.get_x() <= rc->left+rc->width+PF::Shape::hit_test_delta && 
        pt.get_y() >= rc->top-PF::Shape::hit_test_delta && pt.get_y() <= rc->top+rc->height+PF::Shape::hit_test_delta );
  }

  int hit_test(Point& pt, int& additional);
  
//  void get_center(Point& pt) { pt.set( get_top_left().get_x()+(get_bottom_right().get_x()-get_top_left().get_x())/2, 
//      get_top_left().get_y()+(get_bottom_right().get_y()-get_top_left().get_y())/2 ); }
//  void expand(int n);
  void build_mask();
  
  void lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt);
  
};

inline
bool operator ==(const Rect1& l, const Rect1& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
  return true;      
}

inline
bool operator !=(const Rect1& l, const Rect1& r)
{
  return( !(l==r) );
}


inline std::istream& operator >>( std::istream& str, Rect1& shape )
{
  assert(typeid(l) == typeid(r));

  Shape& pshape = static_cast<Shape&>(shape);
  
  str>>pshape;
  
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Rect1& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape;
  return str;
}

template<> inline
void set_gobject_property< Rect1 >(gpointer object, const std::string name,
    const Rect1& value)
{
}

template<> inline
void set_gobject_property< std::vector<Rect1> >(gpointer object, const std::string name,
    const std::vector<Rect1>& value)
{
}



/*
class Path: public Shape
{
  ClosedSplineCurve smod;

public:
  Path(): smod()
{
}
  
  ClosedSplineCurve& get_smod() { return smod; }
  const ClosedSplineCurve& get_smod() const { return smod; }
  void set_smod(ClosedSplineCurve& s) { smod = s; }

};

inline
bool operator ==(const Path& l, const Path& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
  if( l.get_smod() != r.get_smod() ) return false;
  return true;      
}

inline
bool operator !=(const Path& l, const Path& r)
{
  return( !(l==r) );
}


inline std::istream& operator >>( std::istream& str, Path& shape )
{
  str>>shape.get_smod();
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Path& shape )
{
  str<<shape.get_smod();
  return str;
}



template<> inline
void set_gobject_property< std::vector<Path> >(gpointer object, const std::string name,
    const std::vector<Path>& value)
    {
    }
*/

class ShapesPar: public OpParBase
{
/*  Property<bool> invert;
  Property<bool> enable_falloff;

  Property<SplineCurve> falloff_curve;

  Property<ClosedSplineCurve> smod;

  Property<float> border_size;*/
  Property< std::vector<Line> > lines;
  Property< std::vector<Circle1> > circles;
  Property< std::vector<Rect1> > rectangles1;
  Property< std::vector< std::pair<int,int> > > shapes_order; // shape_type - shape_index
//  Property< std::vector<Rectangle> > rectangles;
  
  PropertyBase shape_type;

//  std::vector< std::pair<float,float> > spoints;
//  std::vector< std::pair<float,float> > spoints2;

  ProcessorBase* curve;

public:
/*  float* modvec;
  float falloff_vec[65536];

  std::vector<shape_point>* edgevec;
  //std::vector<float>* edgevec;
  std::vector< shape_falloff_segment >* segvec;
  std::vector< std::pair<int,int> > ptvec;
*/
  ShapesPar();

  enum shape_type
  {
    type_line = 1,
    type_circle = 2,
    type_rectangle = 3
  };
/*  bool get_invert() { return shape_prop.invert.get(); }

  bool get_falloff_enabled() { return enable_falloff.get(); }

  void shape_modified()
  {
    //std::cout<<"Shapes::shape_modified() called"<<std::endl;
    smod.modified();
    //std::cout<<"Shapes::shape_modified() finished"<<std::endl;
  }
  void shape_reset() { smod.reset(); }

  SplineCurve& get_shapes_falloff_curve() { return falloff_curve.get(); }
  ClosedSplineCurve& get_smod() { return smod.get(); }

  float get_border_size() { return border_size.get(); }
  void set_border_size( float sz ) { border_size.update(sz); }
*/
  void shapes_modified()
  {
    shapes_order.modified();
  }
  void add_shape(Line& shape);
  void add_shape(Circle1& shape);
  void add_shape(Rect1& shape);
//  void add_shape(Rectangle& shape);
  PF::Shape* get_shape(int index);
  int get_shapes_count() { return get_shapes_order().size(); }
  int get_shape_type(int n) { return get_shapes_order().at(n).first; }
  
  std::vector<Line>& get_lines() { return lines.get(); }
  std::vector<Circle1>& get_circles() { return circles.get(); }
  std::vector<Rect1>& get_rectangles1() { return rectangles1.get(); }
  std::vector< std::pair<int,int> >& get_shapes_order() { return shapes_order.get(); }
//  std::vector<Rectangle>& get_rectangles() { return rectangles.get(); }

  int get_shape_type() { return shape_type.get_enum_value().first; }
  
  bool needs_input() { return false; }
  bool has_intensity() { return false; }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};

/*
void get_shape_line_points(int x1, int y1, int x2, int y2, std::vector< std::pair<int,int> >& points);


template<class T>
void get_shapes_falloff_curve( float* vec, float val, T& out ) {
  std::cout<<"WARNING!!!!!!!!! default get_shapes_falloff_curve() called"<<std::endl;
  out = 0;
}
*/


template < OP_TEMPLATE_DEF >
class Shapes: public IntensityProc<T, has_imap>
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, ShapesPar* par);

/*  void render_spline(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, ShapesPar* par);

  void draw_segment(VipsRegion* oreg, const shape_falloff_segment& seg, float* vec);*/
};


template< OP_TEMPLATE_DEF >
void Shapes< OP_TEMPLATE_IMP >::
render(VipsRegion** ir, int n, int in_first,
    VipsRegion* imap, VipsRegion* omap,
    VipsRegion* oreg, ShapesPar* par)
    {
  //BLENDER blender( par->get_blend_mode(), par->get_opacity() );

 //   render_spline( ir, n, in_first, imap, omap, oreg, par );
};


/*
#define SWAP_PX( X, Y ) {                         \
  pout = (T*)VIPS_REGION_ADDR( oreg, X, Y );      \
  if( pout[0] == PF::FormatInfo<T>::MIN ) {       \
    for( b = 0; b < bands; b++ )                  \
      pout[b] = PF::FormatInfo<T>::MAX;           \
  } else {                                        \
    for( b = 0; b < bands; b++ )                  \
      pout[b] = PF::FormatInfo<T>::MIN;           \
  }                                               \
  xlast = X; ylast = Y;                           \
}


#define DRAW_OUTLINE_PX( X, Y ) {                           \
  if( vips_rect_includespoint(r,X,Y) ) {                           \
    pout = (T*)VIPS_REGION_ADDR( oreg, X, Y );                           \
    for( b = 0; b < bands; b++ ) pout[b] = FormatInfo<T>::MAX;                           \
    xlast = X; ylast = Y;                           \
  } else {                           \
    if( (X == xlast) && (Y == ylast) ) continue;                           \
    if( (Y < top) && (X >= left) && (X <= right) ) {                           \
      SWAP_PX( X, top );                           \
    }                           \
    if( (Y > bottom) && (X >= left) && (X <= right) ) {                           \
      SWAP_PX( X, bottom );                           \
    }                           \
    if( (X < left) && (Y >= top) && (Y <= bottom) ) {                           \
      SWAP_PX( left, Y );                           \
    }                           \
    if( (X > right) && (Y >= top) && (Y <= bottom) ) {                           \
      SWAP_PX( right, Y );                           \
    }                           \
  }                          \
}


template< OP_TEMPLATE_DEF >
void Shapes< OP_TEMPLATE_IMP >::render_spline(VipsRegion** ir, int n, int in_first,
                                                VipsRegion* imap, VipsRegion* omap,
                                                VipsRegion* oreg, ShapesPar* par)
{
  Rect *r = &oreg->valid;
  Rect *i_r = &ir[0]->valid;
  int bands = oreg->im->Bands;
  int line_size = r->width * bands;

  T* pout;
  T* pin;

  for( int y = 0; y < r->height; y++ ) {
    pout = (T*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );
    pin = (T*)VIPS_REGION_ADDR( ir[0], i_r->left, i_r->top+y );

    for( int x = 0; x < r->width; x++, pout += bands, pin += bands ) {
      for( int b = 0; b < bands; b++ )
//        pout[b] = FormatInfo<T>::MAX;
      pout[b] = pin[b];
    }

  }
  

  return;

}
*/


/*
template< OP_TEMPLATE_DEF >
void Shapes< OP_TEMPLATE_IMP >::
draw_segment(VipsRegion* oreg, const shape_falloff_segment& seg, float* vec)
{
  Rect *r = &oreg->valid;
  int bands = oreg->im->Bands;
  T* pout;
  int i, x, y, b;

  int left = r->left;
  int top = r->top;
  int right = r->left + r->width - 1;
  int bottom = r->top + r->height - 1;


  //std::cout<<"border: "<<x2<<","<<y2<<"  shape: "<<x1<<","<<y1<<std::endl;

  //--------------------------
  // draw falloff segment

  float lx = seg.x2 - seg.x1;
  float ly = seg.y2 - seg.y1;

  T val0; get_shapes_falloff_curve( vec, 1.0f, val0 );
  for( i = seg.lmin; i <= seg.lmax; i++ ) {
    // position
    x = (int)((float)i * lx / seg.fl) + seg.x1;
    y = (int)((float)i * ly / seg.fl) + seg.y1;
    //std::cout<<"  x="<<x<<"  y="<<y<<std::endl;
    if( !vips_rect_includespoint(r,x,y) )
      continue;
    
    float op = (float)i / seg.fl;
    pout = (T*)VIPS_REGION_ADDR( oreg, x, y );
    T val; get_shapes_falloff_curve( vec, op, val );
    if( false && x<2 && y<2 ) std::cout<<"draw_segment(): x="<<x<<" y="<<y<<"  op="<<op<<"  val="<<val<<std::endl;
    for( b = 0; b < bands; ++b)
      pout[b] = val;


    if( x > left ) {
      pout = (T*)VIPS_REGION_ADDR( oreg, x-1, y );
      if( pout[0] == val0 )
      for( b = 0; b < bands; ++b) {
            pout[b] = val;
      }
    }
    if( x < right ) {
      pout = (T*)VIPS_REGION_ADDR( oreg, x+1, y );
      if( pout[0] == val0 )
      for( b = 0; b < bands; ++b) {
            pout[b] = val;
      }
    }
    
    if( y > top ) {
      pout = (T*)VIPS_REGION_ADDR( oreg, x, y-1 );
      if( pout[0] == val0 )
      for( b = 0; b < bands; ++b) {
            pout[b] = val;
      }
    }
    if( y < bottom ) {
      pout = (T*)VIPS_REGION_ADDR( oreg, x, y+1 );
      if( pout[0] == val0 )
      for( b = 0; b < bands; ++b) {
            pout[b] = val;
      }
    }
   
  }
  
}
*/


  ProcessorBase* new_shapes();
}

#endif 


