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

// -----------------------------------
// ShapeColor
// -----------------------------------

class ShapeColor
{
  float r, g, b;

public:
  ShapeColor(): r(0), g(0), b(0)
  {
  }
  ShapeColor(float r1, float g1, float b1): r(r1), g(g1), b(b1)
  {
  }
  ShapeColor(const ShapeColor& other): r(other.r), g(other.g), b(other.b)
  {
  }
  virtual ~ShapeColor() {}
  
  void swap(ShapeColor& first)
  {
    std::swap(r, first.r);
    std::swap(g, first.g);
    std::swap(b, first.b);
  }
  
  ShapeColor& operator=(ShapeColor other)
  {
    this->swap(other);
    return *this;
  }
  
  float get_r() const { return r; }
  void set_r(float s) { r = s; }
  float get_g() const { return g; }
  void set_g(float s) { g = s; }
  float get_b() const { return b; }
  void set_b(float s) { b = s; }

  void set(float r1, float g1, float b1) { r = r1; g = g1; b = b1; }
  void set(ShapeColor& c) { r = c.get_r(); g = c.get_g(); b = c.get_b(); }
    
};

inline
bool operator ==(const ShapeColor& l, const ShapeColor& r)
{
  if( l.get_r() != r.get_r() ) return false;
  if( l.get_g() != r.get_g() ) return false;
  if( l.get_b() != r.get_b() ) return false;
  return true;
}

inline
bool operator !=(const ShapeColor& l, const ShapeColor& r)
{
  return( !(l==r) );
}


inline std::istream& operator >>( std::istream& str, ShapeColor& shape )
{
  float r, g, b;

  str>>r>>g>>b;
  shape.set_r( r );
  shape.set_g( g );
  shape.set_b( b );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const ShapeColor& shape )
{
  str<<shape.get_r()<<" "<<shape.get_g()<<" "<<shape.get_b()<<" ";
  return str;
}

template<> inline
void set_gobject_property< ShapeColor >(gpointer object, const std::string name,
    const ShapeColor& value)
{
}

template<> inline
void set_gobject_property< std::vector<ShapeColor> >(gpointer object, const std::string name,
    const std::vector<ShapeColor>& value)
{
}


// -----------------------------------
// Point
// -----------------------------------

class Point
{
public:
  int x, y;

  Point(): x(0), y(0)
  {
  }
  Point(int x1, int y1): x(x1), y(y1)
  {
  }
  Point(const Point& other): x(other.x), y(other.y)
  {
  }
  virtual ~Point() {}
  
  void swap(Point& first)
  {
    std::swap(x, first.x);
    std::swap(y, first.y);
  }
  
  Point& operator=(Point other)
  {
    this->swap(other);
    return *this;
  }
  
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
  bool is_left_of_line(Point& a, Point& b);
  bool in_rectangle(Point& pt1, Point& pt2);

  bool in_bezier3(Point& anchor1, Point& anchor2, Point& control, float dist);
  bool in_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, float dist);
  bool in_polygon(std::vector<Point>& points);
  
  void line_mid_point(Point& pt1, Point& pt2) { set( std::min(pt1.x, pt2.x) + std::abs(pt1.x - pt2.x)/2, 
                                                     std::min(pt1.y, pt2.y) + std::abs(pt1.y - pt2.y)/2 ); }
  
  int get_quadrant(Point& pt);
  void rotate(Point& pivot, float angle);
  void rotate(Point& pivot, float sin_angle, float cos_angle);
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
  int pen_size;
  float opacity;
  int falloff;
  std::vector<Point> points;
  std::vector<Point> falloff_points;
  bool has_source;
  Point source_point;
  ShapeColor color;
  float angle;

protected:
  float *mask;

public:
  Shape(): shape_type(shape), pen_size(1), opacity(1.f), falloff(0), has_source(false), angle(0), mask(NULL)
  {
  }
  Shape(const Shape& other):
    shape_type(shape), pen_size(other.pen_size), opacity(other.opacity), falloff(other.falloff), 
    points(other.points), falloff_points(other.falloff_points), has_source(other.has_source), 
    source_point(other.source_point), color(other.color), angle(other.angle), mask(NULL)
  {
  }
  Shape(Point& d_init_pos, int d_pen_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    shape_type(shape), mask(NULL)
  {
    set_pen_size(d_pen_size);
    set_opacity(d_opacity);
    set_falloff(d_falloff);
    set_has_source(d_has_source);
    if ( get_has_source() ) set_source_point(d_source_point);
    set_angle(0);
  }
  virtual ~Shape() { if (mask != NULL) { free(mask); } }
  
  enum shapeType
  {
    shape = 0,
    line = 1,
    circle = 2,
    ellipse = 3,
    rectangle = 4,
    polygon = 5,
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
    hit_source = 7,
    hit_control_point = 8,
    hit_control_point_end = 9,
  };
  
  static const int hit_test_delta;
  
  friend void swap(Shape& first, Shape& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      swap(first.pen_size, second.pen_size);
      swap(first.opacity, second.opacity);
      swap(first.falloff, second.falloff);
      first.points.swap(second.points);
      first.falloff_points.swap(second.falloff_points);
      swap(first.has_source, second.has_source);
      first.source_point.swap(second.source_point);
      first.color.swap(second.color);
      swap(first.angle, second.angle);
  }
  
  Shape& operator=(Shape other)
  {
      swap(*this, other);
      return *this;
  }
  
  int get_type() const { return shape_type; }
  void set_type(int s) { shape_type = s; }

  virtual int get_pen_size() const { return pen_size; }
  virtual void set_pen_size(int s) { pen_size = std::max(1, s); }

  float get_opacity() const { return opacity; }
  void set_opacity(float s) { opacity = std::min(1.f, std::max(0.f, s)); }

  int get_falloff() const { return falloff; }
  void set_falloff(int f) { falloff = std::max(0, f); }

  virtual int get_points_count() { return get_points().size(); }
  
  std::vector<Point>& get_points() { return points; }
  const std::vector<Point>& get_points() const { return points; }

  std::vector<Point>& get_falloff_points() { return falloff_points; }
  const std::vector<Point>& get_falloff_points() const { return falloff_points; }

  virtual Point& get_point(int n) { return points[n]; }
  virtual const Point& get_point(int n) const { return points[n]; }
  virtual void set_point(int n, Point& pt) { points[n] = pt; }
  
  virtual Point& get_falloff_point(int n) { return falloff_points[n]; }
  virtual const Point& get_falloff_point(int n) const { return falloff_points[n]; }
  virtual void set_falloff_point(int n, Point& pt) { falloff_points[n] = pt; }
  virtual void get_falloff_point_absolute(int n, Point& absolute);

  bool get_has_source() const { return has_source; }
  void set_has_source(bool s) { has_source = s; }

  Point& get_source_point() { return source_point; }
  const Point& get_source_point() const { return source_point; }
  void set_source_point(Point s) { source_point = s; }

  ShapeColor& get_color() { return color; }
  const ShapeColor& get_color() const { return color; }
  void set_color(ShapeColor s) { color = s; }

  float get_angle() const { return angle; }
  void set_angle(float s) { angle = std::min(360.f, std::max(-360.f, s)); }

  virtual void offset(int x, int y);
  virtual void offset(Point& pt) { offset(pt.get_x(), pt.get_y()); }
  virtual void offset(Point prev, Point curr);
  virtual void offset_point(int n, int x, int y);
  virtual void offset_point(int n, Point& prev, Point& curr);
  
  virtual bool falloff_can_be_offset(int n, int x, int y) { return true; }
  virtual bool falloff_can_be_offset(int n, Point& prev, Point& curr);

  virtual void offset_falloff_point(int n, int x, int y);
  virtual void offset_falloff_point(int n, Point& prev, Point& curr);
  virtual void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);

  virtual void build_mask(SplineCurve& scurve) { std::cout<<"PF::Shape::build_mask()"<<std::endl; }
  float *get_mask() { return mask; }
  void free_mask() { if (mask != NULL) free(mask); mask = NULL; }

  virtual int hit_test(Point& pt, int& additional) { return hit_none; }
  virtual bool hit_test_node(Point& pt_test, Point& pt_node);
  virtual int hit_test_node(int n, Point& pt, int& additional);
  
  bool hit_test_rect(Point& pt, Point& pt1, Point& pt2)
  {
    VipsRect rc;
    get_segment_bounding_rect(pt1, pt2, &rc);
    return hit_test_rect(pt, &rc);
  }
  bool hit_test_rect(Point& pt, VipsRect* rc)
  {
    return ( pt.get_x() >= rc->left-PF::Shape::hit_test_delta && pt.get_x() <= rc->left+rc->width+PF::Shape::hit_test_delta && 
        pt.get_y() >= rc->top-PF::Shape::hit_test_delta && pt.get_y() <= rc->top+rc->height+PF::Shape::hit_test_delta );
  }


  virtual void add_point(Point pt);
  virtual void insert_point(Point& pt, int position);
  virtual void insert_point(Point& pt, int hit_t, int additional) { insert_point(pt, additional+1); }
  virtual void remove_point(int n);
  virtual void remove_point(int hit_t, int additional) { remove_point(additional); }

  virtual void get_center(Point& pt, VipsRect& rc);
  virtual void get_center(Point& pt);
  virtual void get_rect(VipsRect* rc);
  virtual void get_falloff_rect(VipsRect* rc);
  virtual void get_mask_rect(VipsRect* rc) { get_falloff_rect(rc); rotate(rc); }

  virtual void expand(int n);
  virtual void expand_falloff(int n);
  virtual void expand_opacity(float f);
  
  virtual void scale(int scale);
  
  float get_triangle_area(Point& a, Point& b, Point& c);
  bool point_in_triangle(Point& pt, Point& a, Point& b, Point& c);
  bool point_in_rectangle(Point& pt, Point& a, Point& b, Point& c, Point& d);
  float point_in_ellipse(int x, int y, int h, int v, float rx2, float ry2);
  static void get_intersect_point(Point& A, Point& B, Point& C, Point& pt_out);
  bool lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt_intersection);
  bool segments_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt_intersection);
  bool segments_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4);

  static void get_segment_bounding_rect(Point& pt1, Point& pt2, VipsRect* rc);
  static void get_segment_bounding_rect(Point& pt1, Point& pt2, int expand, VipsRect* rc);
  static void get_expanded_rec_from_line(Point& pt1, Point& pt2, int amount, Point& a, Point& b, Point& c, Point& d);
  static void get_pt_proyected_from_segment(Point& pt1, Point& pt2, int amount, Point& a);
  static void get_pt1_proyected_from_segment(Point& pt1, Point& pt2, int amount, Point& a, Point& d);
  static void get_pt2_proyected_from_segment(Point& pt1, Point& pt2, int amount, Point& b, Point& c);

  void fill_falloff_ellipse_square(float* mask, VipsRect& rc, SplineCurve& scurve, Point& pt1, Point& pt2, Point& pt3, Point& pt4);
  void fill_falloff_paralelogram(float* mask, VipsRect& rc, SplineCurve& scurve, Point& pt1, Point& pt2, Point& pt3, Point& pt4);
  void fill_falloff_rectangle(float* mask, VipsRect& rc, SplineCurve& scurve, Point& pt1, Point& pt2, Point& pt3, Point& pt4);
  void fill_polygon(VipsRect& rc);
  
  void rotate(VipsRect* rc);
  void rotate(VipsRect* rc, float _angle);
  void rotate(VipsRect* rc, float sin_angle, float cos_angle);
  void rotate(VipsRect* rc, float sin_angle, float cos_angle, Point& center);

};

inline
bool operator ==(const Shape& l, const Shape& r)
{
  if( l.get_type() != r.get_type() ) return false;
  if( l.get_pen_size() != r.get_pen_size() ) return false;
  if( l.get_opacity() != r.get_opacity() ) return false;
  if( l.get_falloff() != r.get_falloff() ) return false;
  if( l.get_points() != r.get_points() ) return false;
  if( l.get_falloff_points() != r.get_falloff_points() ) return false;
  if( l.get_has_source() != r.get_has_source() ) return false;
  if( l.get_source_point() != r.get_source_point() ) return false;
  if( l.get_color() != r.get_color() ) return false;
  if( l.get_angle() != r.get_angle() ) return false;
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
  int pen_size;
  float opacity;
  int falloff;
  bool has_source;
  float angle;

  str>>s_type>>pen_size>>opacity>>falloff>>shape.get_points()>>shape.get_falloff_points()>>has_source>>shape.get_source_point()>>shape.get_color()>>angle;
  shape.set_type( s_type );
  shape.set_pen_size( pen_size );
  shape.set_opacity( opacity );
  shape.set_falloff( falloff );
  shape.set_has_source( has_source );
  shape.set_angle( angle );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Shape& shape )
{
  str<<shape.get_type()<<" "<<shape.get_pen_size()<<" "<<shape.get_opacity()<<" "<<shape.get_falloff()<<" "<<shape.get_points()<<shape.get_falloff_points()<<shape.get_has_source()<<" "<<shape.get_source_point()<<shape.get_color()<<shape.get_angle()<<" ";
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
// Circle
// -----------------------------------
class Circle: public Shape
{
  int radius;
  
public:
  Circle():
      Shape(), radius(1)
  {
    set_type(circle);
    PF::Shape::add_point(Point(0, 0));
  }
  Circle(const Circle& other): Shape(other), radius(other.radius)
  {
    set_type(circle);
  }
  Circle(Point& d_init_pos, int d_pen_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_pen_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(circle);
    set_radius(1);
    PF::Shape::add_point(d_init_pos);
  }

  friend void swap(Circle& first, Circle& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      const Shape& pf = static_cast<const Shape&>(first);
      const Shape& ps = static_cast<const Shape&>(second);
      swap(pf, ps);
      
      swap(first.radius, second.radius);
  }
  
  void set_angle(float s) { }
  void scale(int scale) { PF::Shape::scale(scale); radius /= scale; }

  int get_radius() const { return radius; }
  void set_radius(int s) { radius = std::max(1, s); }

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand(int n) { set_radius(get_radius()+n); }
  void expand_falloff(int n) { set_falloff(get_falloff()+n); }
  
  int hit_test(Point& pt, int& additional);
  
  void get_center(Point& pt) { pt = PF::Shape::get_point(0); }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }

  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void build_mask(SplineCurve& scurve);
};


inline
bool operator ==(const Circle& l, const Circle& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
  if( l.get_radius() != r.get_radius() ) return false;
  return true;      
}

inline
bool operator !=(const Circle& l, const Circle& r)
{
  return( !(l==r) );
}

inline std::istream& operator >>( std::istream& str, Circle& shape )
{
  assert(typeid(l) == typeid(r));

  Shape& pshape = static_cast<Shape&>(shape);
  int radius;
  
  str>>pshape>>radius;
  shape.set_radius(radius);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Circle& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<shape.get_radius()<<" ";
  return str;
}

template<> inline
void set_gobject_property< Circle >(gpointer object, const std::string name,
    const Circle& value)
{
}

template<> inline
void set_gobject_property< std::vector<Circle> >(gpointer object, const std::string name,
    const std::vector<Circle>& value)
{
}


// -----------------------------------
// Ellipse
// -----------------------------------
class Ellipse: public Shape
{
  int radius_x;
  int radius_y;
  
public:
  Ellipse():
      Shape()
  {
    set_type(ellipse);
    set_radius_x(1);
    set_radius_y(1);
    PF::Shape::add_point(Point(0, 0));
  }
  Ellipse(const Ellipse& other): Shape(other), radius_x(other.radius_x), radius_y(other.radius_y)
  {
    set_type(ellipse);
  }
  Ellipse(Point& d_init_pos, int d_pen_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_pen_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(ellipse);
    set_radius_x(1);
    set_radius_y(1);
    PF::Shape::add_point(d_init_pos);
  }

  friend void swap(Ellipse& first, Ellipse& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      const Shape& pf = static_cast<const Shape&>(first);
      const Shape& ps = static_cast<const Shape&>(second);
      swap(pf, ps);
      
      swap(first.radius_x, second.radius_x);
      swap(first.radius_y, second.radius_y);
  }
  
  int get_radius_x() const { return radius_x; }
  void set_radius_x(int s) { radius_x = std::max(0, s); }

  int get_radius_y() const { return radius_y; }
  void set_radius_y(int s) { radius_y = std::max(0, s); }

  int get_falloff_x() const { return get_falloff(); }
//  void set_falloff_x(int s) { set_falloff(s); }

  int get_falloff_y();

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand(int n);
//  void set_size(int n);
//  void expand_falloff(int n);
  void expand_falloff(int n) { set_falloff(get_falloff()+n); }

  int hit_test(Point& pt, int& additional);
  int hit_test(Point& pt, float angle_1, int& additional);
  
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

  void get_center(Point& pt, VipsRect& rc) { PF::Shape::get_center(pt, rc); }
  void get_center(Point& pt) { pt = PF::Shape::get_point(0); }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }

  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void scale(int scale) { PF::Shape::scale(scale); radius_x /= scale; radius_y /= scale; }
  
  void build_mask(SplineCurve& scurve);
  void build_mask(SplineCurve& scurve, float angle_1);
};

inline
bool operator ==(const Ellipse& l, const Ellipse& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
  if( l.get_radius_x() != r.get_radius_x() ) return false;
  if( l.get_radius_y() != r.get_radius_y() ) return false;
  return true;      
}

inline
bool operator !=(const Ellipse& l, const Ellipse& r)
{
  return( !(l==r) );
}

inline std::istream& operator >>( std::istream& str, Ellipse& shape )
{
  assert(typeid(l) == typeid(r));

  Shape& pshape = static_cast<Shape&>(shape);
  int radius_x;
  int radius_y;
  
  str>>pshape>>radius_x>>radius_y;
  shape.set_radius_x(radius_x);
  shape.set_radius_y(radius_y);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Ellipse& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<shape.get_radius_x()<<" "<<shape.get_radius_y()<<" ";
  return str;
}

template<> inline
void set_gobject_property< Ellipse >(gpointer object, const std::string name,
    const Ellipse& value)
{
}

template<> inline
void set_gobject_property< std::vector<Ellipse> >(gpointer object, const std::string name,
    const std::vector<Ellipse>& value)
{
}

#if 0
// -----------------------------------
// Rect1
// -----------------------------------
class Rect1: public Shape
{

public:
  Rect1():
      Shape()
  {
    set_type(rectangle);
    PF::Shape::add_point(Point(0, 0));
    PF::Shape::add_point(Point(1, 1));
  }
  Rect1(const Rect1& other): Shape(other)
  {
    set_type(rectangle);
  }
  Rect1(Point& d_init_pos, int d_pen_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_pen_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(rectangle);
    PF::Shape::add_point(d_init_pos);
    Point pt(d_init_pos.get_x()+1, d_init_pos.get_y()+1);
    PF::Shape::add_point(pt);
  }

  void set_angle(float s) { }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }


  void offset_falloff_point(int n, int x, int y);
  void offset_point(int n, int x, int y);
  void offset_point(int n, Point& prev, Point& curr) { PF::Shape::offset_point(n, prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

  int hit_test(Point& pt, int& additional);
  
  void build_mask(SplineCurve& scurve);
//  void build_mask(SplineCurve& scurve, float _angle);
  
  void lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt);
  
};
#endif

// -----------------------------------
// SegmentInfo
// -----------------------------------

class SegmentInfo
{
  int segment_type;
  PF::Point control_pt1;
  PF::Point control_pt2;

public:
  SegmentInfo(): segment_type(line)
  {
  }
  SegmentInfo(int st, Point pt1, Point pt2): segment_type(st), control_pt1(pt1), control_pt2(pt2)
  {
  }
  SegmentInfo(const SegmentInfo& other): segment_type(other.segment_type), control_pt1(other.control_pt1), control_pt2(other.control_pt2)
  {
  }
  virtual ~SegmentInfo() {}
  
  void swap(SegmentInfo& first)
  {
    std::swap(segment_type, first.segment_type);
    control_pt1.swap(first.control_pt1);
    control_pt2.swap(first.control_pt2);
  }
  
  SegmentInfo& operator=(SegmentInfo other)
  {
    this->swap(other);
    return *this;
  }
  
  enum controlPointType
  {
    free = 0,
    locked = 1,
    in_sinch = 2,
  };
  
  enum segmentType
  {
    line = 0,
    bezier3_l = 1, // point is linked to the segment on the left
    bezier3_r = 2, // point is linked to the segment on the right
    bezier4 = 3,
  };
  
  int get_segment_type() const { return segment_type; }
  void set_segment_type(int s) { segment_type = s; }

  Point& get_control_pt1() { return control_pt1; }
  const Point& get_control_pt1() const { return control_pt1; }
  void set_control_pt1(Point& pt) { control_pt1 = pt; }
  
  Point& get_control_pt2() { return control_pt2; }
  const Point& get_control_pt2() const { return control_pt2; }
  void set_control_pt2(Point& pt) { control_pt2 = pt; }
  
  void offset_control_pt1(int x, int y) { control_pt1.offset(x, y); }
  void offset_control_pt2(int x, int y) { control_pt2.offset(x, y); }
  void offset_control_pt1(Point& prev, Point& curr) { control_pt1.offset(prev, curr); }
  void offset_control_pt2(Point& prev, Point& curr) { control_pt2.offset(prev, curr); }
  
  void scale(int sf);
};

inline
bool operator ==(const SegmentInfo& l, const SegmentInfo& r)
{
  if( l.get_segment_type() != r.get_segment_type() ) return false;
  if( l.get_control_pt1() != r.get_control_pt1() ) return false;
  if( l.get_control_pt2() != r.get_control_pt2() ) return false;
  return true;
}

inline
bool operator !=(const SegmentInfo& l, const SegmentInfo& r)
{
  return( !(l==r) );
}


inline std::istream& operator >>( std::istream& str, SegmentInfo& si )
{
  int s;

  str>>s>>si.get_control_pt1()>>si.get_control_pt2();
  si.set_segment_type( s );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const SegmentInfo& si )
{
  str<<si.get_segment_type()<<" "<<si.get_control_pt1()<<si.get_control_pt2();
  return str;
}

template<> inline
void set_gobject_property< SegmentInfo >(gpointer object, const std::string name,
    const SegmentInfo& value)
{
}

template<> inline
void set_gobject_property< std::vector<SegmentInfo> >(gpointer object, const std::string name,
    const std::vector<SegmentInfo>& value)
{
}


// -----------------------------------
// Polygon
// -----------------------------------
class Polygon: public Shape
{
  std::vector<SegmentInfo> s_info;

public:
  Polygon(): Shape()
  {
    set_type(polygon);
    
    Point pt;
    add_point(pt);
    add_point(pt);
  }
  Polygon(const Polygon& other): Shape(other), s_info(other.s_info)
  {
    set_type(polygon);
  }
  Polygon(Point& d_init_pos, int d_pen_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_pen_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(polygon);
    add_point(d_init_pos);
    add_point(d_init_pos);
  }

  friend void swap(Polygon& first, Polygon& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      const Shape& pf = static_cast<const Shape&>(first);
      const Shape& ps = static_cast<const Shape&>(second);
      swap(pf, ps);
      
      first.s_info.swap(second.s_info);
  }

  std::vector<SegmentInfo>& get_segments_info() { return s_info; }
  const std::vector<SegmentInfo>& get_segments_info() const { return s_info; }

  virtual int get_pen_size() const { return 1; }
  virtual int get_min_segments() { return 3; }
  
  void set_angle(float s) { }
  void scale(int sf);

  virtual int get_segments_count() { return get_points_count(); }
  
  Point& get_point(int n) { return PF::Shape::get_point( circ_idx(n,get_points_count()) ); }
  
  virtual void add_point(Point pt);
  virtual void insert_point(Point& pt, int _position);
  virtual void insert_point(Point& pt, int hit_t, int additional);
  virtual void remove_point(int _n);
  virtual void remove_point(int hit_t, int additional);

  static int circ_idx(int i, int N) { return (i % N + N) % N; }
  
  SegmentInfo& get_segment_info(int n) { return s_info[ circ_idx(n,get_segments_count()) ]; }
  void set_segments_info(int n, SegmentInfo& si) { s_info[ circ_idx(n,get_segments_count()) ] = si; }

  void set_segment_type(int segment, int t) { get_segment_info(segment).set_segment_type(t); }
  int get_segment_type(int segment) { return get_segment_info(segment).get_segment_type(); }
  Point& get_control_pt1(int segment) { return get_segment_info(segment).get_control_pt1(); }
  void set_control_pt1(int segment, Point& pt) { get_segment_info(segment).set_control_pt1(pt); }
  Point& get_control_pt2(int segment) { return get_segment_info(segment).get_control_pt2(); }
  void set_control_pt2(int segment, Point& pt) { get_segment_info(segment).set_control_pt2(pt); }

  void synch_control_pt1(int n, Point& pt, Point& anchor);
  void synch_control_pt2(int n, Point& pt, Point& anchor);

  void get_segment(int n, Point& pt1, Point& pt2);
  void get_segment(int n, Point& pt1, Point& pt2, SegmentInfo& si);
  
//  void expand(int n) { set_size(get_size()+n); }
  void expand_falloff(int n);
  
  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  virtual void offset_point(int _n, int x, int y);
  virtual void offset_point(int n, Point& prev, Point& curr) { PF::Shape::offset_point( circ_idx(n,get_points_count()), prev, curr ); }

  virtual void offset_control_pt1(int segment, Point& prev, Point& curr, int options);
  virtual void offset_control_pt2(int segment, Point& prev, Point& curr, int options);

  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }
  void offset(Point prev, Point curr) { PF::Shape::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);

  bool point_in_polygon(Point& pt);
  
  virtual int hit_test(Point& pt, int& additional);
  void build_mask(SplineCurve& scurve);
  
  static void inter_bezier3(Point& anchor1, Point& anchor2, Point& control, std::vector<Point>& points);
  static void inter_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, std::vector<Point>& points);
  void inter_segments(std::vector<Point>& points);
  
  static void fill_polygon(std::vector<Point>& points, float* buffer, VipsRect& rc);

};

inline
bool operator ==(const Polygon& l, const Polygon& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
  if( l.get_segments_info() != r.get_segments_info() ) return false;
  return true;      
}

inline
bool operator !=(const Polygon& l, const Polygon& r)
{
  return( !(l==r) );
}

inline std::istream& operator >>( std::istream& str, Polygon& shape )
{
  assert(typeid(l) == typeid(r));

  Shape& pshape = static_cast<Shape&>(shape);
  
  str>>pshape>>shape.get_segments_info();
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Polygon& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<shape.get_segments_info();
  return str;
}

template<> inline
void set_gobject_property< Polygon >(gpointer object, const std::string name,
    const Polygon& value)
{
}

template<> inline
void set_gobject_property< std::vector<Polygon> >(gpointer object, const std::string name,
    const std::vector<Polygon>& value)
{
}

// -----------------------------------
// Line
// -----------------------------------

class Line: public Polygon
{

public:
  Line(): Polygon()
  {
    set_type(line);
  }
  Line(const Line& other): Polygon(other)
  {
    set_type(line);
  }
  Line(Point& d_init_pos, int d_pen_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Polygon(d_init_pos, d_pen_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(line);
  }

//  int get_size() const { return PF::Shape::get_size(); }
  virtual int get_min_segments() { return 1; }

  int get_segments_count() { return get_points_count()-1; }

  void offset_control_pt1(int segment, Point& prev, Point& curr, int options);
  void offset_control_pt2(int segment, Point& prev, Point& curr, int options);

  void offset_point(int n, int x, int y);
  void offset_point(int n, Point& prev, Point& curr) { PF::Polygon::offset_point( n, prev, curr ); }

  void build_mask(SplineCurve& scurve);

};

// -----------------------------------
// Rectangle
// -----------------------------------

class Rectangle: public Polygon
{

public:
  Rectangle(): Polygon()
  {
    set_type(rectangle);
    
    Point pt;
    PF::Polygon::add_point(pt);
    PF::Polygon::add_point(pt);
  }
  Rectangle(const Rectangle& other): Polygon(other)
  {
    set_type(rectangle);
  }
  Rectangle(Point& d_init_pos, int d_pen_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Polygon(d_init_pos, d_pen_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(rectangle);
    
    PF::Polygon::add_point(d_init_pos);
    PF::Polygon::add_point(d_init_pos);
  }

  virtual int get_min_segments() { return 4; }

  void offset_point(int n, int x, int y);
  void offset_point(int n, Point& prev, Point& curr) { PF::Polygon::offset_point( n, prev, curr ); }

  void offset(int x, int y); // { PF::Polygon::offset(x, y); }
  void offset(Point& pt) { PF::Polygon::offset(pt); }
  void offset(Point prev, Point curr) { PF::Polygon::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);

  void add_point(Point pt) { }
  void insert_point(Point& pt, int _position) { }
  void insert_point(Point& pt, int hit_t, int additional) { }
  void remove_point(int _n) { }
  void remove_point(int hit_t, int additional) { }

  void set_segment_type(int segment, int t) { }

};

// -----------------------------------
// ShapesGroup
// -----------------------------------

class ShapesGroup
{
  std::vector<Line> lines;
  std::vector<Circle> circles;
  std::vector<Ellipse> ellipses;
  std::vector<Rectangle> rectangles;
  std::vector<Polygon> polygons;
  std::vector< std::pair<int,int> > shapes_order; // shape_type - shape_index

  ShapeColor current_color;

public:
  ShapesGroup()
  {
  }
  ShapesGroup(const ShapesGroup& other): lines(other.lines), circles(other.circles), ellipses(other.ellipses), 
      rectangles(other.rectangles), polygons(other.polygons), shapes_order(other.shapes_order), current_color(other.current_color)
  {
  }
  virtual ~ShapesGroup() { }
  
  
  friend void swap(ShapesGroup& first, ShapesGroup& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      first.lines.swap(second.lines);
      first.circles.swap(second.circles);
      first.ellipses.swap(second.ellipses);
      first.rectangles.swap(second.rectangles);
      first.polygons.swap(second.polygons);
      first.shapes_order.swap(second.shapes_order);
      first.current_color.swap(second.current_color);
  }
  
  ShapesGroup& operator=(ShapesGroup other)
  {
      swap(*this, other);
      return *this;
  }
  
  std::vector<Line>& get_lines() { return lines; }
  const std::vector<Line>& get_lines() const { return lines; }

  std::vector<Circle>& get_circles() { return circles; }
  const std::vector<Circle>& get_circles() const { return circles; }

  std::vector<Ellipse>& get_ellipses() { return ellipses; }
  const std::vector<Ellipse>& get_ellipses() const { return ellipses; }

  std::vector<Polygon>& get_polygons() { return polygons; }
  const std::vector<Polygon>& get_polygons() const { return polygons; }

  std::vector<Rectangle>& get_rectangles() { return rectangles; }
  const std::vector<Rectangle>& get_rectangles() const { return rectangles; }

  std::vector< std::pair<int,int> >& get_shapes_order() { return shapes_order; }
  const std::vector< std::pair<int,int> >& get_shapes_order() const { return shapes_order; }

  void set_current_color(ShapeColor& c) { current_color = c; }
  ShapeColor& get_current_color() { return current_color; }
  const ShapeColor& get_current_color() const { return current_color; }
  
  int size() { return get_shapes_order().size(); }
  
  int get_shape_type(int n) { return get_shapes_order().at(n).first; }

  PF::Shape* get_shape(int index);
  
  int add(Shape* shape_source);
  void remove(int n);
  
  void clear();
  
  void scale(int sf);
  void build_mask(SplineCurve& scurve);

};

inline
bool operator ==(const ShapesGroup& l, const ShapesGroup& r)
{
  if( l.get_lines() != r.get_lines() ) return false;
  if( l.get_circles() != r.get_circles() ) return false;
  if( l.get_ellipses() != r.get_ellipses() ) return false;
  if( l.get_rectangles() != r.get_rectangles() ) return false;
  if( l.get_polygons() != r.get_polygons() ) return false;
  if( l.get_shapes_order() != r.get_shapes_order() ) return false;
  if( l.get_current_color() != r.get_current_color() ) return false;
  return true;
      }

inline
bool operator !=(const ShapesGroup& l, const ShapesGroup& r)
      {
  return( !(l==r) );
      }


inline std::istream& operator >>( std::istream& str, ShapesGroup& shape )
{
  str>>shape.get_lines()>>shape.get_circles()>>shape.get_ellipses()>>shape.get_rectangles()>>shape.get_polygons()>>shape.get_shapes_order()>>shape.get_current_color();

  return str;
}

inline std::ostream& operator <<( std::ostream& str, const ShapesGroup& shape )
{
  str<<shape.get_lines()<<shape.get_circles()<<shape.get_ellipses()<<shape.get_rectangles()<<shape.get_polygons()<<shape.get_shapes_order()<<shape.get_current_color();
  return str;
}

template<> inline
void set_gobject_property< ShapesGroup >(gpointer object, const std::string name,
    const ShapesGroup& value)
{
}

template<> inline
void set_gobject_property< std::vector<ShapesGroup> >(gpointer object, const std::string name,
    const std::vector<ShapesGroup>& value)
{
}


}

#endif 


