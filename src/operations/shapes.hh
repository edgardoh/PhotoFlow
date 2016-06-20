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
  Point(int x1, int y1): x(x1), y(y1)
  {
  }
  Point(const Point& other): x(other.x), y(other.y)
  {
  }
  virtual ~Point() {}
  
/*  friend void swap(Point& first, Point& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      swap(first.x, second.x);
      swap(first.y, second.y);
  }
  */
  void swap(Point& first)
  {
    std::swap(x, first.x);
    std::swap(y, first.y);
  }
  
  Point& operator=(Point other)
  {
//    swap(*this, other);
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

protected:
  float *mask;

public:
  Shape(): shape_type(shape), size(0), opacity(1.f), falloff(0), has_source(false), mask(NULL)
  {
  }
  Shape(const Shape& other):
    shape_type(shape), size(other.size), opacity(other.opacity), falloff(other.falloff), 
    points(other.points), falloff_points(other.falloff_points), has_source(other.has_source), source_point(other.source_point), mask(NULL)
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
  }
  virtual ~Shape() { if (mask != NULL) { free(mask); std::cout<<"PF::Shapes::~Shape() free(mask)"<<std::endl; } }
  
  enum shapeType
  {
    shape = 0,
    line = 1,
    circle = 2,
    ellipse = 3,
    rectangle = 4,
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
//      first.source_point.swap(second.source_point);
      first.source_point.swap(second.source_point);
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

  virtual Point& point_back() { return points.back(); }
  
  bool get_has_source() const { return has_source; }
  void set_has_source(bool s) { has_source = s; }

  Point& get_source_point() { return source_point; }
  const Point& get_source_point() const { return source_point; }
  void set_source_point(Point s) { source_point = s; }

  virtual void offset(int x, int y);
  virtual void offset(Point& pt) { offset(pt.get_x(), pt.get_y()); }
  virtual void offset(Point prev, Point curr);
  virtual void offset_point(int n, int x, int y);
  virtual void offset_point(int n, Point prev, Point curr);
  virtual void offset_falloff_point(int n, int x, int y);
  virtual void offset_falloff_point(int n, Point prev, Point curr);
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


  void add_point(Point pt);
  void insert_point(Point& pt, int position);
  void remove_point(int n);
  
  virtual void get_center(Point& pt, VipsRect& rc);
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
  float point_in_ellipse(int x, int y, int h, int v, float rx2, float ry2);

  static void get_segment_bounding_rect(Point& pt1, Point& pt2, VipsRect* rc);
  static void get_segment_bounding_rect(Point& pt1, Point& pt2, int expand, VipsRect* rc);
  static void get_expanded_rec_from_line(Point& pt1, Point& pt2, int amount, Point& a, Point& b, Point& c, Point& d);

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

  str>>s_type>>size>>opacity>>falloff>>shape.get_points()>>shape.get_falloff_points()>>has_source>>shape.get_source_point();
  shape.set_type( s_type );
  shape.set_size( size );
  shape.set_opacity( opacity );
  shape.set_falloff( falloff );
  shape.set_has_source( has_source );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Shape& shape )
{
  str<<shape.get_type()<<" "<<shape.get_size()<<" "<<shape.get_opacity()<<" "<<shape.get_falloff()<<" "<<shape.get_points()<<shape.get_falloff_points()<<shape.get_has_source()<<" "<<shape.get_source_point();
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
  void set_radius_x(int s) { set_size(s); }

  int get_radius_y() const { return radius_y; }
  void set_radius_y(int s) { radius_y = std::max(0, s); }

  int get_falloff_x() const { return get_falloff(); }
  void set_falloff_x(int s) { set_falloff(s); }

  int get_falloff_y();

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand(int n);
  void expand_falloff(int n);
  
  int hit_test(Point& pt, int& additional);
  
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

  void get_center(Point& pt) { pt = PF::Shape::get_point(0); }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }

  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void scale(int scale) { PF::Shape::scale(scale); radius_y /= scale; }
  
  void build_mask(SplineCurve& scurve);
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

  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }


  void offset_point(int n, int x, int y);
  void offset_point(int n, Point prev, Point curr) { PF::Shape::offset_point(n, prev, curr); }
  void offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source);
  void offset(int x, int y) { PF::Shape::offset(x, y); }
  void offset(Point& pt) { PF::Shape::offset(pt); }

  int hit_test(Point& pt, int& additional);
  
  void build_mask(SplineCurve& scurve);
  
  void lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt);
  
};

// -----------------------------------
// ShapesGroup
// -----------------------------------

class ShapesGroup
{
  std::vector<Line> lines;
  std::vector<Circle1> circles;
  std::vector<Ellipse> ellipses;
  std::vector<Rect1> rectangles1;
  std::vector< std::pair<int,int> > shapes_order; // shape_type - shape_index


public:
  ShapesGroup()
  {
  }
  ShapesGroup(const ShapesGroup& other): lines(other.lines), circles(other.circles), ellipses(other.ellipses), 
      rectangles1(other.rectangles1), shapes_order(other.shapes_order)
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
      first.shapes_order.swap(second.shapes_order);

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

  std::vector<Rect1>& get_rectangles() { return rectangles1; }
  const std::vector<Rect1>& get_rectangles() const { return rectangles1; }

  std::vector< std::pair<int,int> >& get_shapes_order() { return shapes_order; }
  const std::vector< std::pair<int,int> >& get_shapes_order() const { return shapes_order; }

  int size() { return get_shapes_order().size(); }
  
  int get_shape_type(int n) { return get_shapes_order().at(n).first; }

//  PF::Shape& get_shape(int index);
  PF::Shape* get_shape(int index);
  
  int add(Shape* shape_source);
  
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
  if( l.get_shapes_order() != r.get_shapes_order() ) return false;
  return true;
      }

inline
bool operator !=(const ShapesGroup& l, const ShapesGroup& r)
      {
  return( !(l==r) );
      }


inline std::istream& operator >>( std::istream& str, ShapesGroup& shape )
{
  str>>shape.get_lines()>>shape.get_circles()>>shape.get_ellipses()>>shape.get_rectangles()>>shape.get_shapes_order();

  return str;
}

inline std::ostream& operator <<( std::ostream& str, const ShapesGroup& shape )
{
  str<<shape.get_lines()<<shape.get_circles()<<shape.get_ellipses()<<shape.get_rectangles()<<shape.get_shapes_order();
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


class ShapesPar: public OpParBase
{
  Property<SplineCurve> falloff_curve;
/*  Property< std::vector<Line> > lines;
  Property< std::vector<Circle1> > circles;
  Property< std::vector<Ellipse> > ellipses;
  Property< std::vector<Rect1> > rectangles1;
  Property< std::vector< std::pair<int,int> > > shapes_order; // shape_type - shape_index
*/
  Property<ShapesGroup> shapes;
  
  ProcessorBase* shapes_algo;

//  int scale_factor;


public:

  ShapesPar();

/*  enum shape_type
  {
    type_none = 0,
    type_line = 1,
    type_circle = 2,
    type_ellipse = 3,
    type_rectangle = 4,
  };
*/
  ShapesGroup& get_ShapesGroup() { return shapes.get(); }
  ProcessorBase* get_shapes_algo() { return shapes_algo; }
  
//  int get_scale_factor() { return scale_factor; }
//  void set_scale_factor(int s) { scale_factor = s; }

  SplineCurve& get_shapes_falloff_curve() { return falloff_curve.get(); }

  void shapes_modified() { shapes.modified(); }
  
  int add_shape(Shape* shape) { return shapes.get().add(shape); }
/*  void add_shape(Line& shape) { shapes.get().add(shape); }
  void add_shape(Circle1& shape) { shapes.get().add(shape); }
  void add_shape(Ellipse& shape) { shapes.get().add(shape); }
  void add_shape(Rect1& shape) { shapes.get().add(shape); }
*/
//  PF::Shape& get_shape(int index) { return shapes.get().get_shape(index); }
  PF::Shape* get_shape(int index) 
  { 
    PF::Shape* shape = shapes.get().get_shape(index);
    if (shape == NULL)
        std::cout<<"PF::ShapesPar::get_shape() (shape == NULL) "<<std::endl;
    return  shape;
  }
  int get_shapes_count() { return shapes.get().size(); }
//  int get_shape_type(int n) { return get_shapes_order().at(n).first; }
  
//  void build_masks(unsigned int level);

/*  std::vector<Line>& get_lines() { return lines.get(); }
  std::vector<Circle1>& get_circles() { return circles.get(); }
  std::vector<Ellipse>& get_ellipses() { return ellipses.get(); }
  std::vector<Rect1>& get_rectangles1() { return rectangles1.get(); }
  std::vector< std::pair<int,int> >& get_shapes_order() { return shapes_order.get(); }
  */
  bool needs_input() { return false; }
  bool has_intensity() { return false; }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};

template < OP_TEMPLATE_DEF >
class ShapesProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
    std::cout<<"PF::ShapesProc::render()"<<std::endl;
  }
};



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class ShapesProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    std::cout<<"PF::ShapesProc::render() 2"<<std::endl;
  }
  
};


class ShapesAlgoPar: public OpParBase
{
/*  std::vector<Line> lines;
  std::vector<Circle1> circles;
  std::vector<Ellipse> ellipses;
  std::vector<Rect1> rectangles1;
  std::vector< std::pair<int,int> > shapes_order; // shape_type - shape_index
*/
  ShapesGroup shapes;
  
//  int scale_factor;

public:

  ShapesAlgoPar();

  ~ShapesAlgoPar()
  {
  }

//  void clear_shapes() { shapes.clear(); }
//  void set_shapes(ShapesGroup& sg) { shapes = sg; }
//  void scale_shapes(int sf) { shapes.scale(sf); }
//  void build_mask_shapes(SplineCurve& scurve) { shapes.build_mask(scurve); }
  
  void build_masks(ShapesGroup& sg, SplineCurve& scurve, unsigned int level);

//  int add_new_shape(Shape* shape_source);
  PF::Shape* get_shape(int index) { return shapes.get_shape(index); }
  int get_shapes_count() { return shapes.size(); }

/*  std::vector<Line>& get_lines() { return lines; }
  std::vector<Circle1>& get_circles() { return circles; }
  std::vector<Ellipse>& get_ellipses() { return ellipses; }
  std::vector<Rect1>& get_rectangles1() { return rectangles1; }
  std::vector< std::pair<int,int> >& get_shapes_order() { return shapes_order; }
*/
  
  
  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);

};

template < OP_TEMPLATE_DEF >
class ShapesAlgoProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
    std::cout<<"PF::ShapesAlgoProc::render()"<<std::endl;
  }
};



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class ShapesAlgoProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    std::cout<<"PF::ShapesAlgoProc::render() 2"<<std::endl;
    
    Rect *r = &oreg->valid;
    Rect *ir = &ireg[0]->valid;
    const int ch = oreg->im->Bands;
    const int bands = oreg->im->Bands;
    float* pout;

    for( int y = 0; y < r->height; y++ ) {
      pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );

      for( int x = 0; x < r->width; x++, pout += bands ) {
        for( int b = 0; b < bands; b++ )
          pout[b] = 1.f;
      }
    }

/*    if (par->get_shapes_algo() == NULL) {
      std::cout<<"PF::ShapesMask::render() (par->get_shapes_algo() == NULL)"<<std::endl;
      return;
    }*/
    ShapesAlgoPar* opar = dynamic_cast<ShapesAlgoPar*>( par );
    if (opar == NULL) {
      std::cout<<"PF::ShapesAlgoProc::render() (opar == NULL)"<<std::endl;
      return;
    }
    
//    if (opar) {
//      PF::Shape* shape;
      
      for (int i = 0; i < opar->get_shapes_count(); i++) {
        std::cout<<"PF::ShapesAlgoProc::render() opar->get_shape(i): "<<i<<std::endl;
        
        PF::Shape* shape = opar->get_shape(i);
        if (shape == NULL) {
          std::cout<<"PF::ShapesAlgoProc::render() (shape == NULL) "<<std::endl;
          return;
        }
        std::cout<<"PF::ShapesAlgoProc::render() opar->get_shape(i):2 "<<i<<std::endl;
        
        float *mask = shape->get_mask();
        std::cout<<"PF::ShapesAlgoProc::render() opar->get_shape(i):21 "<<i<<std::endl;
        if (mask == NULL) {
          std::cout<<"PF::ShapesAlgoProc::render() (mask == NULL) "<<std::endl;
          return;
        }
        std::cout<<"PF::ShapesAlgoProc::render() opar->get_shape(i):3 "<<i<<std::endl;
        
        VipsRect mask_area, out_area/*, in_area*/;
        shape->get_mask_rect(&mask_area);
        std::cout<<"PF::ShapesAlgoProc::render() opar->get_shape(i):4 "<<i<<std::endl;

        vips_rect_intersectrect( r, &mask_area, &out_area );
        std::cout<<"PF::ShapesAlgoProc::render() opar->get_shape(i):5 "<<i<<std::endl;

//        out_area = *r;
//        in_area = *ir;
//        const int offset_left = out_area.left /*+ (in_area.left - in_area_offset.left)*/;
//        const int offset_top = out_area.top /*+ (in_area.top - in_area_offset.top)*/;
if (out_area.width > 0 && out_area.height > 0) {
        std::cout<<"PF::ShapesAlgoProc::render() mask_area.left: "<<mask_area.left<<", mask_area.top: "<<mask_area.top<<std::endl;
        std::cout<<"PF::ShapesAlgoProc::render() mask_area.width: "<<mask_area.width<<", mask_area.height: "<<mask_area.height<<std::endl;
        
        std::cout<<"PF::ShapesAlgoProc::render() r->left: "<<r->left<<", r->top: "<<r->top<<std::endl;
        std::cout<<"PF::ShapesAlgoProc::render() r->width: "<<r->width<<", r->height: "<<r->height<<std::endl;
        
        std::cout<<"PF::ShapesAlgoProc::render() out_area.left: "<<out_area.left<<", out_area.top: "<<out_area.top<<std::endl;
        std::cout<<"PF::ShapesAlgoProc::render() out_area.width: "<<out_area.width<<", out_area.height: "<<out_area.height<<std::endl;
}
        for( int y = 0; y < out_area.height; y++ ) {
          pout = (float*)VIPS_REGION_ADDR( oreg, out_area.left, out_area.top + y );
 //         std::cout<<"PF::ShapesAlgoProc::render() 3: y = "<<y<<std::endl;
//          const int y_mask = offset_top + y;
          float* pmask = mask + ( y + out_area.top-mask_area.top) * mask_area.width;
//          std::cout<<"PF::ShapesAlgoProc::render() 3: y*mask_area.width = "<<y*mask_area.width<<std::endl;
          
          for( int x = 0; x < out_area.width; x++, pout+=ch ) {
//            std::cout<<"PF::ShapesAlgoProc::render() 4: x = "<<x<<", y: "<<y<<std::endl;
//            const int x_mask = offset_left + x;
            const float f = pmask[x + out_area.left - mask_area.left];
//            std::cout<<"PF::ShapesAlgoProc::render() 4: i = "<<i<<std::endl;
            if (f > 0.) {
              for (int c = 0; c < ch; c++)
                pout[c] = (1.f - f);
            }
          }
        }

      }
//    }
  }
  
};


/*
template < OP_TEMPLATE_DEF >
class Shapes: public IntensityProc<T, has_imap>
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, ShapesPar* par)
  {
  }

};


template< OP_TEMPLATE_DEF >
void Shapes< OP_TEMPLATE_IMP >::
render(VipsRegion** ir, int n, int in_first,
    VipsRegion* imap, VipsRegion* omap,
    VipsRegion* oreg, ShapesPar* par)
{
  std::cout<<"PF::Shapes::render()"<<std::endl;
};
*/

  ProcessorBase* new_shapes();
}

#endif 


