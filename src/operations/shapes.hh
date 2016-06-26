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
  int x, y;

public:
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
  int size;
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
  Shape(): shape_type(shape), size(0), opacity(1.f), falloff(0), has_source(false), angle(0), mask(NULL)
  {
  }
  Shape(const Shape& other):
    shape_type(shape), size(other.size), opacity(other.opacity), falloff(other.falloff), 
    points(other.points), falloff_points(other.falloff_points), has_source(other.has_source), 
    source_point(other.source_point), color(other.color), angle(other.angle), mask(NULL)
  {
  }
  Shape(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    shape_type(shape), mask(NULL)
  {
    set_size(d_size);
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
    bpath = 6,
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
//      swap(first.shape_type, second.mSize);
      swap(first.size, second.size);
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

  virtual int get_size() const { return size; }
  virtual void set_size(int s) { size = std::max(0, s); }

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
  virtual void get_falloff_point_absolute(int n, Point& center, Point& absolute);
  virtual void get_falloff_point_absolute(int n, VipsRect& rc, Point& absolute);
  virtual void get_falloff_point_absolute(int n, Point& absolute);

//  virtual Point& point_back() { return points.back(); }
  
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
  virtual void offset_point(int n, Point prev, Point curr);
  virtual void offset_falloff_point(int n, int x, int y);
  virtual void offset_falloff_point(int n, Point& prev, Point& curr);
  virtual void offset_falloff_point(int n, Point& center, Point& prev, Point& curr);
  virtual void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);

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
  void insert_point(Point& pt, int position);
  void remove_point(int n);
  
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
//  bool point_in_rectangle(Point& pt, Point& pt1, Point& pt2, Point& pt3);
  
  static void get_segment_bounding_rect(Point& pt1, Point& pt2, VipsRect* rc);
  static void get_segment_bounding_rect(Point& pt1, Point& pt2, int expand, VipsRect* rc);
  static void get_expanded_rec_from_line(Point& pt1, Point& pt2, int amount, Point& a, Point& b, Point& c, Point& d);

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
  if( l.get_size() != r.get_size() ) return false;
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
  int size;
  float opacity;
  int falloff;
  bool has_source;
  float angle;

  str>>s_type>>size>>opacity>>falloff>>shape.get_points()>>shape.get_falloff_points()>>has_source>>shape.get_source_point()>>shape.get_color()>>angle;
  shape.set_type( s_type );
  shape.set_size( size );
  shape.set_opacity( opacity );
  shape.set_falloff( falloff );
  shape.set_has_source( has_source );
  shape.set_angle( angle );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Shape& shape )
{
  str<<shape.get_type()<<" "<<shape.get_size()<<" "<<shape.get_opacity()<<" "<<shape.get_falloff()<<" "<<shape.get_points()<<shape.get_falloff_points()<<shape.get_has_source()<<" "<<shape.get_source_point()<<shape.get_color()<<shape.get_angle()<<" ";
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
  Line(const Line& other): Shape(other)
  {
    set_type(line);
  }
  Line(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(line);
    add_point(d_init_pos);
    add_point(d_init_pos);
  }

  void set_angle(float s) { }

  void expand(int n) { set_size(get_size()+n); }
  void expand_falloff(int n);
  
  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }
  void offset(Point prev, Point curr) { PF::Shape::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);

  int hit_test(Point& pt, int& additional);
  void build_mask(SplineCurve& scurve);
  
};


// -----------------------------------
// Circle1
// -----------------------------------
class Circle1: public Shape
{

public:
  Circle1():
      Shape()
  {
    set_type(circle);
    PF::Shape::add_point(Point(0, 0));
  }
  Circle1(const Circle1& other): Shape(other)
  {
    set_type(circle);
  }
  Circle1(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(circle);
    PF::Shape::add_point(d_init_pos);
  }

  void set_angle(float s) { }
  
  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand(int n) { set_size(get_size()+n); }
  void expand_falloff(int n);
  
  int hit_test(Point& pt, int& additional);
  
  void get_center(Point& pt) { pt = PF::Shape::get_point(0); }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }

  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void build_mask(SplineCurve& scurve);
};


// -----------------------------------
// Ellipse
// -----------------------------------
class Ellipse: public Shape
{
  int radius_y; // y radius, the rx is the size property
  
public:
  Ellipse():
      Shape()
  {
    set_type(ellipse);
    set_radius_y(0);
    PF::Shape::add_point(Point(0, 0));
  }
  Ellipse(const Ellipse& other): Shape(other), radius_y(other.radius_y)
  {
    set_type(ellipse);
  }
  Ellipse(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(ellipse);
    set_radius_y(d_size/2);
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
      
      swap(first.radius_y, second.radius_y);
  }
  
  int get_radius_x() const { return get_size(); }
  void set_radius_x(int s) { Shape::set_size(s); }

  int get_radius_y() const { return radius_y; }
  void set_radius_y(int s) { radius_y = std::max(0, s); }

  int get_falloff_x() const { return get_falloff(); }
  void set_falloff_x(int s) { set_falloff(s); }

  int get_falloff_y();

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand(int n) { set_size(get_size()+n); }
  void set_size(int n);
  void expand_falloff(int n);
  
  int hit_test(Point& pt, int& additional);
  int hit_test(Point& pt, float angle_1, int& additional);
  
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

  void get_center(Point& pt, VipsRect& rc) { PF::Shape::get_center(pt, rc); }
  void get_center(Point& pt) { pt = PF::Shape::get_point(0); }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }

  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void scale(int scale) { PF::Shape::scale(scale); radius_y /= scale; }
  
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
  int radius_y;
  
  str>>pshape>>radius_y;
  shape.set_radius_y(radius_y);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Ellipse& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<shape.get_radius_y()<<" ";
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
  Rect1(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_size, d_opacity, d_falloff, d_has_source, d_source_point)
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
  void offset_point(int n, Point prev, Point curr) { PF::Shape::offset_point(n, prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

  int hit_test(Point& pt, int& additional);
  
  void build_mask(SplineCurve& scurve);
//  void build_mask(SplineCurve& scurve, float _angle);
  
  void lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt);
  
};

// -----------------------------------
// Polygon
// -----------------------------------
class Polygon: public Shape
{
//  int radius_y; // y radius, the rx is the size property
  
public:
  Polygon():
      Shape()
  {
    set_type(polygon);
  }
  Polygon(const Polygon& other): Shape(other)
  {
    set_type(polygon);
  }
  Polygon(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(polygon);
    
    add_point(d_init_pos);
    add_point(d_init_pos);
  }

/*  friend void swap(Polygon& first, Polygon& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      const Shape& pf = static_cast<const Shape&>(first);
      const Shape& ps = static_cast<const Shape&>(second);
      swap(pf, ps);
      
//      swap(first.radius_y, second.radius_y);
  }
*/  
  void set_angle(float s) { }

/*  void expand(int n) { set_size(get_size()+n); }
  void expand_falloff(int n);
  
  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  */
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }
  void offset(Point prev, Point curr) { PF::Shape::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);

  int hit_test(Point& pt, int& additional);
  void build_mask(SplineCurve& scurve);
  
};

/*
inline
bool operator ==(const Polygon& l, const Polygon& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
//  if( l.get_radius_y() != r.get_radius_y() ) return false;
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
  int radius_y;
  
  str>>pshape>>radius_y;
//  shape.set_radius_y(radius_y);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Polygon& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<1<<" ";
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
*/

// -----------------------------------
// BPath
// -----------------------------------
class BPath: public Shape
{
  int radius_y; // y radius, the rx is the size property
  
public:
  BPath():
      Shape()
  {
    set_type(bpath);
  }
  BPath(const BPath& other): Shape(other), radius_y(other.radius_y)
  {
    set_type(bpath);
  }
  BPath(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(bpath);
    
    add_point(d_init_pos);
    add_point(d_init_pos);
  }

  friend void swap(BPath& first, BPath& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      const Shape& pf = static_cast<const Shape&>(first);
      const Shape& ps = static_cast<const Shape&>(second);
      swap(pf, ps);
      
      swap(first.radius_y, second.radius_y);
  }
  
  void set_angle(float s) { }
  bool lock_control_points() { return true; }
  
  int get_points_count() { return get_points().size() / 3; }
  
  void add_point(Point pt);
  
  PF::Point& get_point(int n) { return PF::Shape::get_point(n*3); }
  PF::Point& get_control_point(int n);
  PF::Point& get_control_point_end(int n);
  
  PF::Point get_control_point_absolute(int n);
  PF::Point get_control_point_end_absolute(int n);
  
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }
  void offset(Point prev, Point curr) { PF::Shape::offset(prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);
  
  int hit_test(Point& pt, int& additional);
  
  void build_mask(SplineCurve& scurve);
};

inline
bool operator ==(const BPath& l, const BPath& r)
{
  assert(typeid(l) == typeid(r));

  const Shape& pr = static_cast<const Shape&>(r);
  const Shape& pl = static_cast<const Shape&>(l);
    
  if( pl != pr ) return false;
//  if( l.get_radius_y() != r.get_radius_y() ) return false;
  return true;      
}

inline
bool operator !=(const BPath& l, const BPath& r)
{
  return( !(l==r) );
}

inline std::istream& operator >>( std::istream& str, BPath& shape )
{
  assert(typeid(l) == typeid(r));

  Shape& pshape = static_cast<Shape&>(shape);
  int radius_y;
  
  str>>pshape>>radius_y;
//  shape.set_radius_y(radius_y);
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const BPath& shape )
{
  assert(typeid(l) == typeid(r));

  const Shape& pshape = static_cast<const Shape&>(shape);
    
  str<<pshape<<1<<" ";
  return str;
}

template<> inline
void set_gobject_property< BPath >(gpointer object, const std::string name,
    const BPath& value)
{
}

template<> inline
void set_gobject_property< std::vector<BPath> >(gpointer object, const std::string name,
    const std::vector<BPath>& value)
{
}

// -----------------------------------
// ShapesGroup
// -----------------------------------

class ShapesGroup
{
  std::vector<Line> lines;
  std::vector<Circle1> circles;
  std::vector<Ellipse> ellipses;
  std::vector<Rect1> rectangles1;
  std::vector<Polygon> polygons;
  std::vector<BPath> bpaths;
  std::vector< std::pair<int,int> > shapes_order; // shape_type - shape_index

  ShapeColor current_color;

public:
  ShapesGroup()
  {
  }
  ShapesGroup(const ShapesGroup& other): lines(other.lines), circles(other.circles), ellipses(other.ellipses), 
      rectangles1(other.rectangles1), polygons(other.polygons), bpaths(other.bpaths), shapes_order(other.shapes_order), current_color(other.current_color)
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
      first.rectangles1.swap(second.rectangles1);
      first.polygons.swap(second.polygons);
      first.bpaths.swap(second.bpaths);
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

  std::vector<Circle1>& get_circles() { return circles; }
  const std::vector<Circle1>& get_circles() const { return circles; }

  std::vector<Ellipse>& get_ellipses() { return ellipses; }
  const std::vector<Ellipse>& get_ellipses() const { return ellipses; }

  std::vector<Polygon>& get_polygons() { return polygons; }
  const std::vector<Polygon>& get_polygons() const { return polygons; }

  std::vector<BPath>& get_bpaths() { return bpaths; }
  const std::vector<BPath>& get_bpaths() const { return bpaths; }

  std::vector<Rect1>& get_rectangles() { return rectangles1; }
  const std::vector<Rect1>& get_rectangles() const { return rectangles1; }

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
  if( l.get_bpaths() != r.get_bpaths() ) return false;
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
  str>>shape.get_lines()>>shape.get_circles()>>shape.get_ellipses()>>shape.get_rectangles()>>shape.get_polygons()>>shape.get_bpaths()>>shape.get_shapes_order()>>shape.get_current_color();

  return str;
}

inline std::ostream& operator <<( std::ostream& str, const ShapesGroup& shape )
{
  str<<shape.get_lines()<<shape.get_circles()<<shape.get_ellipses()<<shape.get_rectangles()<<shape.get_polygons()<<shape.get_bpaths()<<shape.get_shapes_order()<<shape.get_current_color();
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


