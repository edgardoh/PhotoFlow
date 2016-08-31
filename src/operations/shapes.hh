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
  float x, y;

  Point(): x(0), y(0)
  {
  }
  Point(float x1, float y1): x(x1), y(y1)
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
  
  float get_x() const { return x; }
  void set_x(float s) { x = s; }
  float get_y() const { return y; }
  void set_y(float s) { y = s; }

  void set(float x1, float y1) { x = x1; y = y1; }
  void set(Point& pt) { x = pt.get_x(); y = pt.get_y(); }
  
  void offset(float x1, float y1) { x += x1; y += y1; }
  void offset(Point& pt) { offset(pt.get_x(), pt.get_y()); }
  void offset(Point prev, Point curr);
  void offset(float n, Point& center);
  void offset_pos(float x1, float y1) { x = std::max(0.f, x+x1); y = std::max(0.f, y+y1); }
  void offset_pos(Point prev, Point curr);
  
  void scale(float scale);
  
  // Distance from pt to pt1, squared
  float distance2(float x, float y)
  {
    const float dx = x - this->get_x();
    const float dy = y - this->get_y();
     return dx * dx + dy * dy;
  };
  float distance2(Point& pt) { return distance2(pt.get_x(), pt.get_y()); };
  float distance2segment2(Point& l1, Point& l2); // distance from point to segment (l1, l2) squared
  float distance2polygon2(std::vector<Point>& poly_pt);
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
  float x, y;

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
  float pen_size;
  float opacity;
  float falloff;
  std::vector<Point> points;
  std::vector<Point> falloff_points;
  bool has_source;
  Point source_point;
  ShapeColor color;
  float angle;
  bool fill_shape;

protected:
  float *mask;

public:
  Shape(): shape_type(shape), pen_size(1), opacity(1.f), falloff(0), has_source(false), angle(0), fill_shape(true), mask(NULL)
  {
  }
  Shape(const Shape& other):
    shape_type(shape), pen_size(other.pen_size), opacity(other.opacity), falloff(other.falloff), 
    points(other.points), falloff_points(other.falloff_points), has_source(other.has_source), 
    source_point(other.source_point), color(other.color), angle(other.angle), fill_shape(other.fill_shape), mask(NULL)
  {
  }
  Shape(Point& d_init_pos, float d_pen_size, float d_opacity, float d_falloff, bool d_fill_shape, bool d_has_source, Point& d_source_point): 
    shape_type(shape), mask(NULL)
  {
    set_pen_size(d_pen_size);
    set_opacity(d_opacity);
    set_falloff(d_falloff);
    set_has_source(d_has_source);
    if ( get_has_source() ) set_source_point(d_source_point);
    set_angle(0);
    set_fill_shape(d_fill_shape);
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
  
  static const float hit_test_delta;
  
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
      swap(first.fill_shape, second.fill_shape);
  }
  
  Shape& operator=(Shape other)
  {
      swap(*this, other);
      return *this;
  }
  
  int get_type() const { return shape_type; }
  void set_type(int s) { shape_type = s; }

  virtual float get_pen_size() const { return pen_size; }
  virtual void set_pen_size(float s) { pen_size = std::max(1.f, s); }

  float get_opacity() const { return opacity; }
  void set_opacity(float s) { opacity = std::min(1.f, std::max(0.f, s)); }

  virtual float get_falloff() const { return falloff; }
  virtual void set_falloff(float f) { falloff = std::max(0.f, f); }

  virtual int get_points_count() { return get_points().size(); }
  virtual int get_falloff_points_count() { return get_falloff_points().size(); }
  
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
  virtual Point get_falloff_point_absolute(int n, Point& center);
  virtual Point get_falloff_point_absolute(int n);

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

  virtual bool get_fill_shape() const { return fill_shape; }
  virtual void set_fill_shape(bool b) { fill_shape = b; }

  virtual void offset(float x, float y);
  virtual void offset(Point& pt) { offset(pt.get_x(), pt.get_y()); }
  virtual void offset(Point prev, Point curr);
  virtual void offset_point(int n, float x, float y);
  virtual void offset_point(int n, Point& prev, Point& curr);
  
  virtual bool falloff_can_be_offset(int n, float x, float y) { return true; }
  virtual bool falloff_can_be_offset(int n, Point& prev, Point& curr);

  virtual void offset_falloff_point(int n, float x, float y);
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
  virtual int insert_point(Point& pt, int position);
  virtual int insert_point(Point& pt, int hit_t, int additional) { return insert_point(pt, additional+1); }
  virtual void remove_point(int n);
  virtual void remove_point(int hit_t, int additional) { remove_point(additional); }

  static void get_center(std::vector<Point>& poly_pt, Point& center, float& surf);
  virtual void get_center(Point& center, float& surf);
  virtual Point get_center(VipsRect& rc);
  virtual Point get_center();
  virtual void get_rect(Point& pt1, Point& pt2);
  virtual void get_falloff_rect(Point& pt1, Point& pt2);
  virtual void get_mask_rect(VipsRect* rc);

  virtual void expand(float n);
  virtual void expand_falloff(float n) { set_falloff(get_falloff()+n); }
  virtual void expand_opacity(float f);
  virtual void expand_pen_size(float f);
  
  virtual void scale(float scale);
  
  float get_triangle_area(Point& a, Point& b, Point& c);
  bool point_in_triangle(Point& pt, Point& a, Point& b, Point& c);
  bool point_in_rectangle(Point& pt, Point& a, Point& b, Point& c, Point& d);
  float point_in_ellipse(float x, float y, float h, float v, float rx2, float ry2);
  static void get_intersect_point(Point& A, Point& B, Point& C, Point& pt_out);
  static bool lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt_intersection);
  static bool segments_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt_intersection);
  static bool segments_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4);

  static void get_segment_bounding_rect(Point& pt1, Point& pt2, VipsRect* rc);
  static void get_segment_bounding_rect(Point& pt1, Point& pt2, float expand, VipsRect* rc);
  static void get_expanded_rec_from_line(Point& pt1, Point& pt2, float amount, Point& a, Point& b, Point& c, Point& d);
  static void get_pt_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& a);
  static void get_pt1_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& a, Point& d);
  static void get_pt2_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& b, Point& c);
  static void get_pt1_2_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& a, Point& b);
  static void get_pt3_4_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& d, Point& c);

  void rotate(VipsRect* rc);
  void rotate(VipsRect* rc, float _angle);
  void rotate(VipsRect* rc, float sin_angle, float cos_angle);
  void rotate(VipsRect* rc, float sin_angle, float cos_angle, Point& center);

  static void bubble_sort(std::vector<int>& vec);
  void test_mask(float* buffer, VipsRect& rc);
  
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
  if( l.get_fill_shape() != r.get_fill_shape() ) return false;
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
  float pen_size;
  float opacity;
  float falloff;
  bool has_source;
  float angle;
  bool fill_shape;

  str>>s_type>>pen_size>>opacity>>falloff>>shape.get_points()>>shape.get_falloff_points()>>has_source>>shape.get_source_point()>>shape.get_color()>>angle>>fill_shape;
  shape.set_type( s_type );
  shape.set_pen_size( pen_size );
  shape.set_opacity( opacity );
  shape.set_falloff( falloff );
  shape.set_has_source( has_source );
  shape.set_angle( angle );
  shape.set_fill_shape( fill_shape );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Shape& shape )
{
  str<<shape.get_type()<<" "<<shape.get_pen_size()<<" "<<shape.get_opacity()<<" "<<shape.get_falloff()<<" "<<shape.get_points()<<shape.get_falloff_points()<<shape.get_has_source()<<" "<<shape.get_source_point()<<shape.get_color()<<shape.get_angle()<<" "<<shape.get_fill_shape()<<" ";
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
  float radius;
  
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
  Circle(Point& d_init_pos, float d_pen_size, float d_opacity, float d_falloff, bool d_fill_shape, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_pen_size, d_opacity, d_falloff, d_fill_shape, d_has_source, d_source_point)
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
  void scale(float scale) { PF::Shape::scale(scale); radius /= scale; }

  float get_radius() const { return radius; }
  void set_radius(float s) { radius = std::max(1.f, s); }

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand(float n) { set_radius(get_radius()+n); }
  
  int hit_test(Point& pt, int& additional);
  
  Point get_center() { return PF::Shape::get_point(0); }
  
  void add_point(Point pt) { }
  int insert_point(Point& pt, int position) { return -1; }

  void get_rect(Point& pt1, Point& pt2);
  void get_falloff_rect(Point& pt1, Point& pt2);
  void get_mask_rect(VipsRect* rc);

  void fill_circle(Point& center, float radius, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);
  void draw_circle(Point& center, float radius, float pen_size, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);

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
  float radius;
  
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
  float radius_x;
  float radius_y;
  
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
  Ellipse(Point& d_init_pos, float d_pen_size, float d_opacity, float d_falloff, bool d_fill_shape, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_pen_size, d_opacity, d_falloff, d_fill_shape, d_has_source, d_source_point)
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
  
  float get_radius_x() const { return radius_x; }
  void set_radius_x(float s) { radius_x = std::max(1.f, s); }

  float get_radius_y() const { return radius_y; }
  void set_radius_y(float s) { radius_y = std::max(1.f, s); }

  float get_falloff_x() const { return get_falloff(); }

  float get_falloff_y();

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand(float n);

  int hit_test(Point& pt, int& additional);
  int hit_test(Point& pt, float angle_1, int& additional);
  
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);
  void offset(float x, float y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

  Point get_center(VipsRect& rc) { return PF::Shape::get_center(rc); }
  Point get_center() { return PF::Shape::get_point(0); }
  
  void add_point(Point pt) { }
  int insert_point(Point& pt, int position) { return -1; }

  void get_falloff_rect(VipsRect* rc);
  void get_rect(Point& pt1, Point& pt2);
  void get_falloff_rect(Point& pt1, Point& pt2);
  
  void scale(float scale) { PF::Shape::scale(scale); radius_x /= scale; radius_y /= scale; }
  
  static void fill_ellipse(Point& center, float radius_x, float radius_y, float falloff_x, float falloff_y, float angle, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);
  static void draw_ellipse(Point& center, float radius_x, float radius_y, float pen_size, float falloff_x, float falloff_y, float angle, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);

  void build_mask(SplineCurve& scurve);
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
  float radius_x;
  float radius_y;
  
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
  
  void offset_control_pt1(float x, float y) { control_pt1.offset(x, y); }
  void offset_control_pt2(float x, float y) { control_pt2.offset(x, y); }
  void offset_control_pt1(Point& prev, Point& curr) { control_pt1.offset(prev, curr); }
  void offset_control_pt2(Point& prev, Point& curr) { control_pt2.offset(prev, curr); }
  
  void scale(float sf);
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
  bool m_closed_shape;

public:
  Polygon(): Shape()
  {
    set_type(polygon);
    set_closed_shape(true);
    
    Point pt;
    add_point(pt);
    add_point(pt);
  }
  Polygon(const Polygon& other): Shape(other), s_info(other.s_info), m_closed_shape(other.m_closed_shape)
  {
    set_type(polygon);
  }
  Polygon(Point& d_init_pos, float d_pen_size, float d_opacity, float d_falloff, bool d_fill_shape, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_pen_size, d_opacity, d_falloff, d_fill_shape, d_has_source, d_source_point)
  {
    set_type(polygon);
    set_closed_shape(true);
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
      std::swap(first.m_closed_shape, second.m_closed_shape);
  }

  std::vector<SegmentInfo>& get_segments_info() { return s_info; }
  const std::vector<SegmentInfo>& get_segments_info() const { return s_info; }

  virtual int get_min_segments() { return 3; }
  
  virtual bool get_closed_shape() const { return m_closed_shape; }
  virtual void set_closed_shape(bool b) { m_closed_shape = b; }
  
  void set_angle(float s) { }
  void scale(float sf);

  virtual int get_segments_count() { return get_points_count(); }
  
  virtual void add_point(Point pt);
  virtual int insert_point(Point& pt, int _position);
  virtual int insert_point(Point& pt, int hit_t, int additional);
  virtual void remove_point(int _n);
  virtual void remove_point(int hit_t, int additional);

  virtual int insert_control_point(Point& pt, int hit_t, int additional, float t);

  static int circ_idx(int i, int N) { return (i % N + N) % N; }
  
  SegmentInfo& get_segment_info(int n) { return s_info[n]; }
  void set_segments_info(int n, SegmentInfo& si) { s_info[n] = si; }

  void set_segment_type(int segment, int t) { get_segment_info(segment).set_segment_type(t); }
  int get_segment_type(int segment) { return get_segment_info(segment).get_segment_type(); }
  Point& get_control_pt1(int segment) { return get_segment_info(segment).get_control_pt1(); }
  void set_control_pt1(int segment, Point& pt) { get_segment_info(segment).set_control_pt1(pt); }
  Point& get_control_pt2(int segment) { return get_segment_info(segment).get_control_pt2(); }
  void set_control_pt2(int segment, Point& pt) { get_segment_info(segment).set_control_pt2(pt); }

  void mirror_control_pt1(int segment, Point& pt, Point& anchor);
  void mirror_control_pt2(int segment, Point& pt, Point& anchor);
  static void synch_control_point(Point& anchor, Point& cntrl_src, Point& cntrl_synch);

  void get_segment(int n, Point& pt1, Point& pt2);
  void get_segment(int n, Point& pt1, Point& pt2, SegmentInfo& si);
  
  virtual void expand(float n);
  
  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  void get_rect(Point& pt1, Point& pt2);
  void get_falloff_rect(Point& pt1, Point& pt2);

  virtual void offset_point(int _n, float x, float y);
  virtual void offset_point(int n, Point& prev, Point& curr) { PF::Shape::offset_point(n, prev, curr ); }

  virtual void offset_control_pt1(int segment, Point& prev, Point& curr, int options);
  virtual void offset_control_pt2(int segment, Point& prev, Point& curr, int options);

  void offset(float x, float y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }
  void offset(Point prev, Point curr) { PF::Shape::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);

  bool point_in_polygon(Point& pt);
  bool point_in_polygon_falloff(Point& pt);

  static void default_node_2control_points(Point& anchor1, Point& anchor2, Point& anchor3, Point& control1, Point& control2, const float t);
  static void default_segment_control_point(Point& anchor1, Point& anchor2, Point& control, const float t, bool is_clockwise);
  void default_node_controls_point(int node, const float t);
  
  static void mirror_control_point(Point& anchor, Point& control_src, Point& control_pt);
  virtual void default_control_pt1(int segment, float t);
  virtual void default_control_pt2(int segment, float t);

  virtual int hit_test(Point& pt, int& additional);
  void build_mask(SplineCurve& scurve);
  
  static void inter_bezier3(Point& anchor1, Point& anchor2, Point& control, std::vector<Point>& points);
  static void inter_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, std::vector<Point>& points);
  void inter_segments(std::vector<Point>& points);
  
  bool is_clockwise();
  static int distance2(int x1, int y1, int x2, int y2)
  {
    const int dx = x2 - x1;
    const int dy = y2 - y1;
     return dx * dx + dy * dy;
  };
  static int get_nearest_pixel_distance2(int pixelX, int pixelY, float* buffer, VipsRect& rc);
  static int rasterCircle(int x0, int y0, int radius, float* buffer, VipsRect& rc);

  static bool generate_arc_points(Point& cmax, Point& bmin, /*Point& bmin2,*/ Point& bmax, std::vector<Point>& points, bool clockwise);
  void get_falloff_points(std::vector<Point>& points);
  
  static void fill_polygon(std::vector<Point>& points, float opacity, float* buffer, VipsRect& rc);
  static void fill_polygon_falloff3(std::vector<Point>& points, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);

  static void fill_segment_falloff(std::vector<Point>& poly_pt, Point& pt1, Point& pt2, float falloff2, float falloff2_1, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);
  static void fill_point_falloff(std::vector<Point>& poly_pt, Point& pt1, float falloff2, float falloff2_1, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);
  static void fill_polygon_falloff(std::vector<Point>& poly_pt, bool clockwise, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);
  static void draw_polygon_segments(std::vector<Point>& points, float pen_size, float falloff, float opacity, bool closed_shape, SplineCurve& scurve, float* buffer, VipsRect& rc);
  
  void fill_polygon_falloff2(SplineCurve& scurve, float* buffer, VipsRect& rc);
  static void fill_polygon_segment_falloff(std::vector<Point>& points, Point& pt3_first, Point& pt4_prev, bool clockwise, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);

  static void expand_segment(Point& pt1, Point& pt2, Point& pt3, float length2);
  static void expand_segment_to(Point& pt1, Point& pt2, Point& pt3, float length_to);
  static void draw_segment(std::vector<Point>& poly_pt_in, std::vector<Point>& poly_pt_out, Point& pt1, Point& pt2, 
      float pen_size2, float falloff2, float falloff2_1, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc);

  int FindLR( std::vector<Point>& poly );
  int Ccw( std::vector<Point>& poly, int m );

};

inline
bool operator ==(const Polygon& l, const Polygon& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
  if( l.get_segments_info() != r.get_segments_info() ) return false;
  if( l.get_closed_shape() != r.get_closed_shape() ) return false;
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
  bool closed_s;
  
  str>>pshape>>shape.get_segments_info()>>closed_s;
  shape.set_closed_shape(closed_s);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Polygon& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<shape.get_segments_info()<<shape.get_closed_shape()<<" ";
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
    set_closed_shape(false);
  }
  Line(const Line& other): Polygon(other)
  {
    set_type(line);
    set_closed_shape(false);
  }
  Line(Point& d_init_pos, float d_pen_size, float d_opacity, float d_falloff, bool d_fill_shape, bool d_has_source, Point& d_source_point): 
    Polygon(d_init_pos, d_pen_size, d_opacity, d_falloff, d_fill_shape, d_has_source, d_source_point)
  {
    set_type(line);
    set_closed_shape(false);
  }

  virtual int get_min_segments() { return 1; }

  virtual bool get_fill_shape() const { return false; }

  virtual void expand(float n) { set_pen_size( get_pen_size() + n ); }
  
  virtual int get_segments_count() { return get_points_count()-1; }
  virtual int get_falloff_segments_count() { return get_falloff_points_count()-1; }

  void offset_control_pt1(int segment, Point& prev, Point& curr, int options);
  void offset_control_pt2(int segment, Point& prev, Point& curr, int options);

  void offset_point(int n, float x, float y);
  void offset_point(int n, Point& prev, Point& curr) { PF::Polygon::offset_point( n, prev, curr ); }

  int insert_control_point(Point& pt, int hit_t, int additional, float t);

  void default_control_pt1(int segment, float t);
  void default_control_pt2(int segment, float t);

  int hit_test(Point& pt, int& additional);

};


// -----------------------------------
// Rectangle
// -----------------------------------

class Rectangle: public Polygon
{
  bool rounded_corners;
  
public:
  Rectangle(): Polygon()
  {
    set_type(rectangle);
    set_rounded_corners(false);
    
    Point pt;
    PF::Polygon::add_point(pt);
    PF::Polygon::add_point(pt);
  }
  Rectangle(const Rectangle& other): Polygon(other), rounded_corners(other.rounded_corners)
  {
    set_type(rectangle);
  }
  Rectangle(Point& d_init_pos, float d_pen_size, float d_opacity, float d_falloff, bool d_fill_shape, bool d_has_source, Point& d_source_point): 
    Polygon(d_init_pos, d_pen_size, d_opacity, d_falloff, d_fill_shape, d_has_source, d_source_point)
  {
    set_type(rectangle);
    set_rounded_corners(false);
    
    PF::Polygon::add_point(d_init_pos);
    PF::Polygon::add_point(d_init_pos);
  }

  friend void swap(Rectangle& first, Rectangle& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      const Polygon& pf = static_cast<const Polygon&>(first);
      const Polygon& ps = static_cast<const Polygon&>(second);
      swap(pf, ps);
      
      std::swap(first.rounded_corners, second.rounded_corners);
  }

  virtual int get_min_segments() { return 4; }

  bool get_rounded_corners() const { return rounded_corners; }
  void set_rounded_corners(bool b) { rounded_corners = b; }

  void offset_point(int n, float x, float y);
  void offset_point(int n, Point& prev, Point& curr) { PF::Polygon::offset_point( n, prev, curr ); }

  void offset(float x, float y);
  void offset(Point& pt) { PF::Polygon::offset(pt); }
  void offset(Point prev, Point curr) { PF::Polygon::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options);

  void add_point(Point pt) { }
  int insert_point(Point& pt, int _position) { return -1; }
  int insert_point(Point& pt, int hit_t, int additional) { return -1; }
  void remove_point(int _n) { }
  void remove_point(int hit_t, int additional) { }

  void set_segment_type(int segment, int t) { }

  void build_mask(SplineCurve& scurve);

};

inline
bool operator ==(const Rectangle& l, const Rectangle& r)
{
  assert(typeid(l) == typeid(r));

  const Polygon& pr = static_cast<const Polygon&>(r);
  const Polygon& pl = static_cast<const Polygon&>(l);
    
  if( pl != pr ) return false;
  if( l.get_rounded_corners() != r.get_rounded_corners() ) return false;
  return true;      
}

inline
bool operator !=(const Rectangle& l, const Rectangle& r)
{
  return( !(l==r) );
}

inline std::istream& operator >>( std::istream& str, Rectangle& shape )
{
  assert(typeid(l) == typeid(r));

  Polygon& pshape = static_cast<Polygon&>(shape);
  bool b_rounded_corners;
  
  str>>pshape>>b_rounded_corners;
  shape.set_rounded_corners(b_rounded_corners);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Rectangle& shape )
{
  assert(typeid(l) == typeid(r));

  const Polygon& pshape = static_cast<const Polygon&>(shape);
    
  str<<pshape<<shape.get_rounded_corners()<<" ";
  return str;
}

template<> inline
void set_gobject_property< Rectangle >(gpointer object, const std::string name,
    const Rectangle& value)
{
}

template<> inline
void set_gobject_property< std::vector<Rectangle> >(gpointer object, const std::string name,
    const std::vector<Rectangle>& value)
{
}

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
  
  void scale(float sf);
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


// -----------------------------------
// curve3_div
// -----------------------------------

class curve3_div
   {
   public:
       curve3_div() : 
           m_approximation_scale(1.0),
           m_angle_tolerance(0.0),
           m_count(0),
           curve_recursion_limit(32)
       {}

       curve3_div(double x1, double y1, 
                  double x2, double y2, 
                  double x3, double y3) :
           m_approximation_scale(1.0),
           m_angle_tolerance(0.0),
           m_count(0),
           curve_recursion_limit(32)
       { 
           init(x1, y1, x2, y2, x3, y3);
       }

       enum curve_approximation_method_e
       {
           curve_inc,
           curve_div
       };
       
       void reset() { m_points.clear(); m_count = 0; }
       void init(double x1, double y1, 
                 double x2, double y2, 
                 double x3, double y3);

       void approximation_method(curve_approximation_method_e) {}
       curve_approximation_method_e approximation_method() const { return curve_div; }

       void approximation_scale(double s) { m_approximation_scale = s; }
       double approximation_scale() const { return m_approximation_scale;  }

       void angle_tolerance(double a) { m_angle_tolerance = a; }
       double angle_tolerance() const { return m_angle_tolerance;  }

       void cusp_limit(double) {}
       double cusp_limit() const { return 0.0; }

       void rewind(unsigned)
       {
           m_count = 0;
       }
       double calc_sq_distance(double x1, double y1, double x2, double y2)
           {
               double dx = x2-x1;
               double dy = y2-y1;
               return dx * dx + dy * dy;
           }

   private:
       void bezier(double x1, double y1, 
                   double x2, double y2, 
                   double x3, double y3);
       void recursive_bezier(double x1, double y1, 
                             double x2, double y2, 
                             double x3, double y3,
                             unsigned level);

       double               m_approximation_scale;
       double               m_distance_tolerance_square;
       double               m_angle_tolerance;
       unsigned             m_count;
       unsigned curve_recursion_limit;
       
   public:
       std::vector<PF::Point> m_points;
       
       float curve_collinearity_epsilon;
       float curve_angle_tolerance_epsilon;
   };



// -----------------------------------
// curve4_div
// -----------------------------------

class curve4_div
   {
   public:
       curve4_div() : 
           m_approximation_scale(1.0),
           m_angle_tolerance(0.0),
           m_cusp_limit(0.0),
           m_count(0),
           curve_recursion_limit(32)
       {}

       curve4_div(double x1, double y1, 
                  double x2, double y2, 
                  double x3, double y3,
                  double x4, double y4) :
           m_approximation_scale(1.0),
           m_angle_tolerance(0.0),
           m_cusp_limit(0.0),
           m_count(0),
           curve_recursion_limit(32)
       { 
           init(x1, y1, x2, y2, x3, y3, x4, y4);
       }

       enum curve_approximation_method_e
       {
           curve_inc,
           curve_div
       };
       
       double calc_sq_distance(double x1, double y1, double x2, double y2)
           {
               double dx = x2-x1;
               double dy = y2-y1;
               return dx * dx + dy * dy;
           }
       void reset() { m_points.clear(); m_count = 0; }
       void init(double x1, double y1, 
                 double x2, double y2, 
                 double x3, double y3,
                 double x4, double y4);

       void approximation_method(curve_approximation_method_e) {}

       curve_approximation_method_e approximation_method() const 
       { 
           return curve_div; 
       }

       void approximation_scale(double s) { m_approximation_scale = s; }
       double approximation_scale() const { return m_approximation_scale;  }

       void angle_tolerance(double a) { m_angle_tolerance = a; }
       double angle_tolerance() const { return m_angle_tolerance;  }

       void cusp_limit(double v) 
       { 
           m_cusp_limit = (v == 0.0) ? 0.0 : M_PI - v; 
       }

       double cusp_limit() const 
       { 
           return (m_cusp_limit == 0.0) ? 0.0 : M_PI - m_cusp_limit; 
       }

       void rewind(unsigned)
       {
           m_count = 0;
       }

   private:
       void bezier(double x1, double y1, 
                   double x2, double y2, 
                   double x3, double y3, 
                   double x4, double y4);

       void recursive_bezier(double x1, double y1, 
                             double x2, double y2, 
                             double x3, double y3, 
                             double x4, double y4,
                             unsigned level);

       double               m_approximation_scale;
       double               m_distance_tolerance_square;
       double               m_angle_tolerance;
       double               m_cusp_limit;
       unsigned             m_count;
       unsigned curve_recursion_limit;

   public:
       std::vector<PF::Point> m_points;
       float curve_collinearity_epsilon;
       float curve_angle_tolerance_epsilon;
   };


#endif 


