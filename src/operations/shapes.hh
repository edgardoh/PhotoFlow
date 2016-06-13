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
  Shape(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    shape_type(shape), mask(NULL)
  {
    set_size(d_size);
    set_opacity(d_opacity);
    set_falloff(d_falloff);
    set_has_source(d_has_source);
    if ( get_has_source() ) set_source_point(d_source_point);
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

  virtual void build_mask(SplineCurve& scurve) { }
  float *get_mask() { return mask; }
  void free_mask() { if (mask != NULL) free(mask); mask = NULL; }

  virtual int hit_test(Point& pt, int& additional) { return hit_none; }
  virtual bool hit_test_node(Point& pt_test, Point& pt_node);
  virtual int hit_test_node(int n, Point& pt, int& additional);
  
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
  static void get_segment_bounding_rect(Point& pt1, Point& pt2, VipsRect* rc);
  static void get_segment_bounding_rect(Point& pt1, Point& pt2, int expand, VipsRect* rc);
  static void get_expanded_rec_from_line(Point& pt1, Point& pt2, int amount, Point& a, Point& b, Point& c, Point& d);
  
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
  Circle1(Point& d_init_pos, int d_size, float d_opacity, int d_falloff, bool d_has_source, Point& d_source_point): 
    Shape(d_init_pos, d_size, d_opacity, d_falloff, d_has_source, d_source_point)
  {
    set_type(circle);
    PF::Shape::add_point(d_init_pos);
  }

  Point& get_point() { return PF::Shape::get_point(0); }
  void set_point(Point& pt) { PF::Shape::set_point(0, pt); }
  
  void expand_falloff(int n);
  
  int hit_test(Point& pt, int& additional);
  
  void get_center(Point& pt) { pt = PF::Shape::get_point(0); }
  void expand(int n) { set_size(get_size()+n); }
  
  void add_point(Point pt) { }
  void insert_point(Point& pt, int position) { }

  void get_rect(VipsRect* rc);
  void get_falloff_rect(VipsRect* rc);
  
  void build_mask(SplineCurve& scurve);
};


// -----------------------------------
// Rect1
// -----------------------------------
class Rect1: public Shape
{

public:
//  Rect1();
  Rect1():
      Shape()
  {
    set_type(rectangle);
    PF::Shape::add_point(Point(0, 0));
    PF::Shape::add_point(Point(1, 1));
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

  bool hit_test_rect(Point& pt, Point& pt1, Point& pt2)
  {
    VipsRect rc;
    PF::Line::get_segment_bounding_rect(pt1, pt2, &rc);
    return hit_test_rect(pt, &rc);
  }
  bool hit_test_rect(Point& pt, VipsRect* rc)
  {
    return ( pt.get_x() >= rc->left-PF::Shape::hit_test_delta && pt.get_x() <= rc->left+rc->width+PF::Shape::hit_test_delta && 
        pt.get_y() >= rc->top-PF::Shape::hit_test_delta && pt.get_y() <= rc->top+rc->height+PF::Shape::hit_test_delta );
  }

  int hit_test(Point& pt, int& additional);
  
  void build_mask(SplineCurve& scurve);
  
  void lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt);
  
};


class ShapesPar: public OpParBase
{
  Property<SplineCurve> falloff_curve;
  Property< std::vector<Line> > lines;
  Property< std::vector<Circle1> > circles;
  Property< std::vector<Rect1> > rectangles1;
  Property< std::vector< std::pair<int,int> > > shapes_order; // shape_type - shape_index

//  ProcessorBase* curve;

public:

  ShapesPar();

  enum shape_type
  {
    type_none = 0,
    type_line = 1,
    type_circle = 2,
    type_rectangle = 3
  };


  SplineCurve& get_shapes_falloff_curve() { return falloff_curve.get(); }

  void shapes_modified() { shapes_order.modified();  }
  void add_shape(Line& shape);
  void add_shape(Circle1& shape);
  void add_shape(Rect1& shape);

  PF::Shape* get_shape(int index);
  int get_shapes_count() { return get_shapes_order().size(); }
  int get_shape_type(int n) { return get_shapes_order().at(n).first; }
  
  std::vector<Line>& get_lines() { return lines.get(); }
  std::vector<Circle1>& get_circles() { return circles.get(); }
  std::vector<Rect1>& get_rectangles1() { return rectangles1.get(); }
  std::vector< std::pair<int,int> >& get_shapes_order() { return shapes_order.get(); }
  
  bool needs_input() { return false; }
  bool has_intensity() { return false; }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};



template < OP_TEMPLATE_DEF >
class Shapes: public IntensityProc<T, has_imap>
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, ShapesPar* par);

};


template< OP_TEMPLATE_DEF >
void Shapes< OP_TEMPLATE_IMP >::
render(VipsRegion** ir, int n, int in_first,
    VipsRegion* imap, VipsRegion* omap,
    VipsRegion* oreg, ShapesPar* par)
    {

    };


  ProcessorBase* new_shapes();
}

#endif 


