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

#include "shapes.hh"
#include "../base/processor.hh"

const int PF::Shape::hit_test_delta = 8;


// -----------------------------------
// Point
// -----------------------------------

void PF::Point::offset(Point prev, Point curr)
{
  x += curr.get_x()-prev.get_x();
  y += curr.get_y()-prev.get_y();
}

void PF::Point::offset_pos(Point prev, Point curr)
{
  x = std::max(0, x+curr.get_x()-prev.get_x());
  y = std::max(0, y+curr.get_y()-prev.get_y());
}

void PF::Point::scale(int scale)
{
  x /= scale;
  y /= scale;
}

// Find the distance squared from this to a segment (l1, l2)
float PF::Point::distance2segment2(Point& l1, Point& l2)
{
  float A = this->x - l1.x;
  float B = this->y - l1.y;
  float C = l2.x - l1.x;
  float D = l2.y - l1.y;

  float dot = A * C + B * D;
  float len_sq = C * C + D * D;
  float param = dot / len_sq;

  float xx, yy;

  if (param < 0.f || (l1.x == l2.x && l1.y == l2.y)) {
    xx = l1.x;
    yy = l1.y;
  }
  else if (param > 1) {
    xx = l2.x;
    yy = l2.y;
  }
  else {
    xx = ((float)l1.x) + param * C;
    yy = ((float)l1.y) + param * D;
  }

  float dx = ((float)this->x) - xx;
  float dy = ((float)this->y) - yy;

  return (dx * dx + dy * dy);
}
/*
// Find the distance squared from this to a bezier curve (l1, l2, l3)
float PF::Point::distance2bezier32(Point& l1, Point& l2, Point& l3)
{
  float A = this->x - l1.x;
  float B = this->y - l1.y;
  float C = l2.x - l1.x;
  float D = l2.y - l1.y;

  float dot = A * C + B * D;
  float len_sq = C * C + D * D;
  float param = dot / len_sq;

  float xx, yy;

  if (param < 0.f || (l1.x == l2.x && l1.y == l2.y)) {
    xx = l1.x;
    yy = l1.y;
  }
  else if (param > 1) {
    xx = l2.x;
    yy = l2.y;
  }
  else {
    xx = ((float)l1.x) + param * C;
    yy = ((float)l1.y) + param * D;
  }

  float dx = ((float)this->x) - xx;
  float dy = ((float)this->y) - yy;

  return (dx * dx + dy * dy);
}
*/
int PF::Point::get_quadrant(Point& pt)
{
  int q = 0;
  if ( pt.get_x() >= get_x() ) {
    if ( pt.get_y() < get_y() )
      q = 0;
    else
      q = 1;
  }
  else {
    if ( pt.get_y() < get_y() )
      q = 3;
    else
      q = 2;
  }
  return q;
}

void PF::Point::rotate(Point& pivot, float angle)
{
  const float _angle = angle*M_PI/180.f;
  const float sin_angle = sinf(_angle);
  const float cos_angle = cosf(_angle);
  rotate(pivot, sin_angle, cos_angle);
}

void PF::Point::rotate(Point& pivot, float sin_angle, float cos_angle)
{
  set( cos_angle * (float)(get_x() - pivot.get_x()) - sin_angle * (float)(get_y() - pivot.get_y()) + (float)pivot.get_x(),
                  sin_angle * (float)(get_x() - pivot.get_x()) + cos_angle * (float)(get_y() - pivot.get_y()) + (float)pivot.get_y() );
}

// true if the distance sqared to any point in the curve is <= dist
bool PF::Point::in_bezier3(Point& anchor1, Point& anchor2, Point& control, float dist)
{
  std::vector<Point> points;
  
  PF::Polygon::inter_bezier3(anchor1, anchor2, control, points);
  if (points.size() > 0) {
    Point& pt1 = points[0];
    for (int i = 1; i < points.size(); i++) {
      Point& pt2 = points[i];
      if (distance2segment2(pt1, pt2) <= dist)
        return true;
      pt1 = pt2;
    }
  }
  return false;
}

bool PF::Point::in_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, float dist)
{
  std::vector<Point> points;
  
  PF::Polygon::inter_bezier4(anchor1, anchor2, control1, control2, points);
  if (points.size() > 0) {
    Point& pt1 = points[0];
    for (int i = 1; i < points.size(); i++) {
      Point& pt2 = points[i];
      if (distance2segment2(pt1, pt2) <= dist)
        return true;
      pt1 = pt2;
    }
  }
  return false;
}

bool PF::Point::in_polygon(std::vector<Point>& points)
{
  const int polyCorners = points.size();
  int i, j = polyCorners-1;
  bool oddNodes = false;

  for (i = 0; i < polyCorners; i++) {
    if ( ( points[i].y< get_y() && points[j].y>=get_y() || points[j].y< get_y() && points[i].y>=get_y() ) && 
        ( points[i].x<=get_x() || points[j].x<=get_x() ) ) {
      oddNodes ^= ( (float)points[i].x + (float)(get_y()-points[i].y) / (float)(points[j].y-points[i].y) * (float)(points[j].x-points[i].x) < (float)get_x() );
    }
    j=i;
  }

  return oddNodes;
}

// -----------------------------------
// Shape
// -----------------------------------

void PF::Shape::get_falloff_point_absolute(int n, /*Point& center,*/ Point& absolute)
{
/*  Point& pt = get_point(n);
  Point& pt_fall = get_falloff_point(n);
  int x, y;
  
  if (pt.get_x() < center.get_x())
    x = pt.get_x() - pt_fall.get_x();
  else
    x = pt.get_x() + pt_fall.get_x();
  
  if (pt.get_y() < center.get_y())
    y = pt.get_y() - pt_fall.get_y();
  else
    y = pt.get_y() + pt_fall.get_y();
  
  absolute.set(x, y);*/
  
  absolute = get_point(n);
  absolute.offset( get_falloff_point(n) );
}
/*
void PF::Shape::get_falloff_point_absolute(int n, VipsRect& rc, Point& absolute)
{
  Point center;
  get_center(center, rc);
  
  get_falloff_point_absolute(n, center, absolute);
}

void PF::Shape::get_falloff_point_absolute(int n, Point& absolute)
{
  Point center;
  get_center(center);
  
  get_falloff_point_absolute(n, center, absolute);
}
*/
void PF::Shape::offset(int x, int y)
{
  for (int i = 0; i < get_points_count(); i++) {
    offset_point(i, x, y);
  }
}

void PF::Shape::offset(Point prev, Point curr)
{
  offset(curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset_point(int n, int x, int y)
{
  get_point(n).offset(x, y);
}

void PF::Shape::offset_point(int n, Point& prev, Point& curr)
{
  offset_point(n, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

bool PF::Shape::falloff_can_be_offset(int n, Point& prev, Point& curr)
{
  return falloff_can_be_offset(n, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset_falloff_point(int n, int x, int y)
{
//  falloff_points[n].offset_pos(x, y);
  if ( falloff_can_be_offset(n, x, y) )
    falloff_points[n].offset(x, y);
}
/*
void PF::Shape::offset_falloff_point(int n, Point& prev, Point& curr)
{
  Point center;
  get_center(center);
  offset_falloff_point(n, center, prev, curr);
}
*/
void PF::Shape::offset_falloff_point(int n, /*Point& center,*/ Point& prev, Point& curr)
{
/*  if ( get_point(n).get_x() < center.get_x() ) {
    if ( get_point(n).get_y() < center.get_y() ) {
      offset_falloff_point(n, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
    }
    else
    {
      offset_falloff_point(n, curr.get_x()-prev.get_x(), prev.get_y()-curr.get_y());
    }
  }
  else {
    if ( get_point(n).get_y() < center.get_y() ) {
      offset_falloff_point(n, prev.get_x()-curr.get_x(), curr.get_y()-prev.get_y());
    }
    else
    {
      offset_falloff_point(n, prev.get_x()-curr.get_x(), prev.get_y()-curr.get_y());
    }
  }
  */
  offset_falloff_point(n, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options)
{
  switch (hit_t)
  {
  case PF::Shape::hit_shape:
  case PF::Shape::hit_falloff:
    offset(prev, curr);
    if (get_has_source() && lock_source) {
      get_source_point().offset(curr, prev);
    }
    break;
  case PF::Shape::hit_source:
    get_source_point().offset(prev, curr);
    break;
  case PF::Shape::hit_node:
    offset_point(additional, prev, curr);
    break;
  case PF::Shape::hit_falloff_node:
    offset_falloff_point(additional, prev, curr);
    break;
  case PF::Shape::hit_segment:
//    offset_point(additional, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
//    offset_point(additional+1, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
    offset_point(additional, prev, curr);
    offset_point(additional+1, prev, curr);
    break;
  case PF::Shape::hit_falloff_segment:
//    offset_falloff_point(additional, curr.get_x()-prev.get_x(), prev.get_y()-curr.get_y());
//    offset_falloff_point(additional+1, curr.get_x()-prev.get_x(), prev.get_y()-curr.get_y());
    if (falloff_can_be_offset(additional, prev, curr) && falloff_can_be_offset(additional+1, prev, curr)) {
      offset_falloff_point(additional, prev, curr);
      offset_falloff_point(additional+1, prev, curr);
    }
    break;
  }
}

bool PF::Shape::hit_test_node(Point& pt_test, Point& pt_node)
{
 return (pt_test.get_x() >= pt_node.get_x()-hit_test_delta && pt_test.get_x() <= pt_node.get_x()+hit_test_delta &&
     pt_test.get_y() >= pt_node.get_y()-hit_test_delta && pt_test.get_y() <= pt_node.get_y()+hit_test_delta);
}

int PF::Shape::hit_test_node(int n, Point& pt, int& additional)
{
  additional = n;
  if ( hit_test_node(pt, points[n]) )
    return hit_node;
  return hit_none;
}

void PF::Shape::scale(int scale)
{
  for (int i = 0; i < points.size(); i++) {
    points[i].scale(scale);
    falloff_points[i].scale(scale);
  }
  source_point.scale(scale);
  
  falloff /= scale;
  pen_size /= scale;

}

void PF::Shape::add_point(Point pt)
{
  points.push_back(pt);
  falloff_points.push_back(Point(get_falloff(), get_falloff()));
}

void PF::Shape::insert_point(Point& pt, int position)
{
  std::vector<Point>::iterator it;
  it = points.begin();
  it = points.insert( it+position, pt );

  it = falloff_points.begin();
  it = falloff_points.insert( it+position, Point(get_falloff(), get_falloff()) );
}

void PF::Shape::remove_point(int n)
{
  points.erase(points.begin()+n);
  falloff_points.erase(falloff_points.begin()+n);
}

void PF::Shape::get_center(Point& pt, VipsRect& rc)
{
  pt.set(rc.left+rc.width/2, rc.top+rc.height/2);
}

void PF::Shape::get_center(Point& pt)
{
  VipsRect rc;
  get_rect(&rc);
  get_center(pt, rc);
}

void PF::Shape::expand(int n)
{
  Point center;
  get_center(center);
  int x, y;
  for (int i = 0; i < points.size(); i++) {
    if ( get_point(i).get_x() < center.get_x() )
      x = get_point(i).get_x() - n;
    else
      x = get_point(i).get_x() + n;
    if ( get_point(i).get_y() < center.get_y() )
      y = get_point(i).get_y() - n;
    else
      y = get_point(i).get_y() + n;
    get_point(i).set(x, y);
  }
}

void PF::Shape::expand_falloff(int n)
{
/*  for (int i = 0; i < falloff_points.size(); i++) {
    falloff_points[i].set(std::max(0, falloff_points[i].get_x()+n), std::max(0, falloff_points[i].get_y()+n));
  }*/
  Point center;
  get_center(center);
  int x, y;
  for (int i = 0; i < points.size(); i++) {
    if ( get_point(i).get_x() < center.get_x() )
      x = - n;
    else
      x =  n;
    if ( get_point(i).get_y() < center.get_y() )
      y = - n;
    else
      y =  n;
    offset_falloff_point(i, x, y);
  }
}

void PF::Shape::expand_opacity(float f)
{
  opacity = std::min(1.f, std::max(0.f, opacity + f));
}

void PF::Shape::get_rect(VipsRect* rc)
{
  rc->left = rc->top = INT_MAX;
  rc->width = rc->height = INT_MIN;
  
  if (points.size() > 0) {
    for (int i = 0; i < points.size(); i++) {
      rc->left = std::min(rc->left, points[i].get_x());
      rc->top = std::min(rc->top, points[i].get_y());
      rc->width = std::max(rc->width, points[i].get_x());
      rc->height = std::max(rc->height, points[i].get_y());
    }
  
    rc->width = rc->width-rc->left+1;
    rc->height = rc->height-rc->top+1;
  }
}

void PF::Shape::get_falloff_rect(VipsRect* rc)
{
  rc->left = rc->top = INT_MAX;
  rc->width = rc->height = INT_MIN;
  
  if (points.size() > 0) {
    Point absolute;
    Point center;
    
    get_center(center);
    
    for (int i = 0; i < points.size(); i++) {
      get_falloff_point_absolute(i, /*center,*/ absolute);
      
      rc->left = std::min(rc->left, absolute.get_x());
      rc->top = std::min(rc->top, absolute.get_y());
      rc->width = std::max(rc->width, absolute.get_x());
      rc->height = std::max(rc->height, absolute.get_y());
    }
    
    rc->width = rc->width-rc->left+1;
    rc->height = rc->height-rc->top+1;
  }
}

float PF::Shape::get_triangle_area(Point& a, Point& b, Point& c)
{
  return ((float)abs(a.get_x()*b.get_y()+b.get_x()*c.get_y()+c.get_x()*a.get_y()-a.get_x()*c.get_y()-c.get_x()*b.get_y()-b.get_x()*a.get_y()))/2.f;
}

bool PF::Shape::point_in_triangle(Point& pt, Point& a, Point& b, Point& c)
{
  return (get_triangle_area(pt, a, b) + get_triangle_area(pt, b, c) + get_triangle_area(pt, c, a) == get_triangle_area(a, b, c));
}

bool PF::Shape::point_in_rectangle(Point& pt, Point& a, Point& b, Point& c, Point& d)
{
  if (point_in_triangle(pt, a, b, c)) {
    return true;
  }
  return point_in_triangle(pt, a, c, d);
}

// return <= 1 if (x,y) in ellipse
float PF::Shape::point_in_ellipse(int x, int y, int h, int v, float rx2, float ry2)
{
  return ( ((float)((x-h)*(x-h)))/rx2 ) + ( ((float)((y-v)*(y-v)))/ry2 );
}

// returns the intersection point from segment (A, B) and point C
void PF::Shape::get_intersect_point(Point& A, Point& B, Point& C, Point& pt_out)
{
/*  float x1=A.get_x(), y1=A.get_y(), x2=B.get_x(), y2=B.get_y(), x3=C.get_x(), y3=C.get_y();
  float px = x2-x1, py = y2-y1, dAB = px*px + py*py;
  float u = (float)((x3 - x1) * px + (y3 - y1) * py) / dAB;
    pt_out.set( x1 + u * px, y1 + u * py);
  */
/*  float k = (float)((B.get_y()-A.get_y()) * (C.get_x()-A.get_x()) - (B.get_x()-A.get_x()) * (C.get_y()-A.get_y())) / 
     (float) ( (B.get_y()-A.get_y())*(B.get_y()-A.get_y()) + (B.get_x()-A.get_x())*(B.get_x()-A.get_x()) );
  pt_out.set( C.get_x() - k * (float)(B.get_y()-A.get_y()), 
  C.get_y() + k * (float)(B.get_x()-A.get_x()) );
  */
  double x1=A.get_x(), y1=A.get_y(), x2=B.get_x(), y2=B.get_y(), x3=C.get_x(), y3=C.get_y();
  double dx = x2 - x1;
  double dy = y2 - y1;
  double mag = sqrtf(dx*dx + dy*dy);
  dx /= mag;
  dy /= mag;

  // translate the point and get the dot product
  double lambda = (dx * (x3 - x1)) + (dy * (y3 - y1));
  pt_out.set( (dx * lambda) + x1,
   (dy * lambda) + y1);
/*  
//  if (A.x == B.x && A.y == B.y) A.x -= 0.00001;

  float m = ((float)(A.y - B.y) / (float)(A.x - B.x));
  float b = (float)C.y - (-1.f/m) * (float)C.x;
  
//  y = m*(x-B.x)+B.y
  float x = ( - (-1/m)*C.x + C.y - B.y + m*B.x)/2.f;
   float y = -(1/m)*(x-C.x)+C.y;
   pt_out.set(x,y);*/
}

void PF::Shape::get_segment_bounding_rect(Point& pt1, Point& pt2, VipsRect* rc)
{
  rc->left = std::min(pt1.get_x(), pt2.get_x());
  rc->top = std::min(pt1.get_y(), pt2.get_y());
  rc->width = std::max(pt1.get_x(), pt2.get_x()) - rc->left + 1;
  rc->height = std::max(pt1.get_y(), pt2.get_y()) - rc->top + 1;
}

void PF::Shape::get_segment_bounding_rect(Point& pt1, Point& pt2, int expand, VipsRect* rc)
{
  get_segment_bounding_rect(pt1, pt2, rc);
  rc->left -= expand;
  rc->top -= expand;
  rc->width += expand*2;
  rc->height += expand*2;
}

// returns a rect, result of expanding a given line by amount
void PF::Shape::get_expanded_rec_from_line(Point& pt1, Point& pt2, int amount, Point& a, Point& b, Point& c, Point& d)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = sqrtf(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;
/*  a.set( pt1.get_x() + amount_y + 1, pt1.get_y() - amount_x + 1);
  b.set( pt1.get_x() - amount_y + 1, pt1.get_y() + amount_x + 1);
  d.set( pt2.get_x() + amount_y + 1, pt2.get_y() - amount_x + 1);
  c.set( pt2.get_x() - amount_y + 1, pt2.get_y() + amount_x + 1);
*/
  a.set( pt1.get_x() - amount_y + 1, pt1.get_y() + amount_x + 1);
  c.set( pt2.get_x() + amount_y + 1, pt2.get_y() - amount_x + 1);
  b.set( pt2.get_x() - amount_y + 1, pt2.get_y() + amount_x + 1);
  d.set( pt1.get_x() + amount_y + 1, pt1.get_y() - amount_x + 1);

}

// returns a point results of proyecting pt1 from segment pt1-pt2 by amount
void PF::Shape::get_pt_proyected_from_segment(Point& pt1, Point& pt2, int amount, Point& a)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = sqrtf(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;
  a.set( pt1.get_x() + amount_y + 1, pt1.get_y() - amount_x + 1);
/*  b.set( pt1.get_x() - amount_y + 1, pt1.get_y() + amount_x + 1);
  d.set( pt2.get_x() + amount_y + 1, pt2.get_y() - amount_x + 1);
  c.set( pt2.get_x() - amount_y + 1, pt2.get_y() + amount_x + 1);
*/
}

void PF::Shape::get_pt1_proyected_from_segment(Point& pt1, Point& pt2, int amount, Point& a, Point& d)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = sqrtf(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;
/*  a.set( pt1.get_x() + amount_y + 1, pt1.get_y() - amount_x + 1);
  b.set( pt1.get_x() - amount_y + 1, pt1.get_y() + amount_x + 1);
  d.set( pt2.get_x() + amount_y + 1, pt2.get_y() - amount_x + 1);
  c.set( pt2.get_x() - amount_y + 1, pt2.get_y() + amount_x + 1);
*/
  a.set( pt1.get_x() - amount_y/* + 1*/, pt1.get_y() + amount_x/* + 1*/);
//  c.set( pt2.get_x() + amount_y + 1, pt2.get_y() - amount_x + 1);
//  b.set( pt2.get_x() - amount_y + 1, pt2.get_y() + amount_x + 1);
  d.set( pt1.get_x() + amount_y/* + 1*/, pt1.get_y() - amount_x /*+ 1*/);

}

void PF::Shape::get_pt2_proyected_from_segment(Point& pt1, Point& pt2, int amount, Point& b, Point& c)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = sqrtf(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;
/*  a.set( pt1.get_x() + amount_y + 1, pt1.get_y() - amount_x + 1);
  b.set( pt1.get_x() - amount_y + 1, pt1.get_y() + amount_x + 1);
  d.set( pt2.get_x() + amount_y + 1, pt2.get_y() - amount_x + 1);
  c.set( pt2.get_x() - amount_y + 1, pt2.get_y() + amount_x + 1);
*/
//  a.set( pt1.get_x() - amount_y + 1, pt1.get_y() + amount_x + 1);
  c.set( pt2.get_x() + amount_y + 1, pt2.get_y() - amount_x + 1);
  b.set( pt2.get_x() - amount_y + 1, pt2.get_y() + amount_x + 1);
//  d.set( pt1.get_x() + amount_y + 1, pt1.get_y() - amount_x + 1);

}


void PF::Shape::rotate(VipsRect* rc)
{
  if ( get_angle() != 0 ) rotate(rc, get_angle());
}

void PF::Shape::rotate(VipsRect* rc, float _angle)
{
  const float angle = _angle*M_PI/180.f;
  rotate(rc, sin(angle), cos(angle));
}

void PF::Shape::rotate(VipsRect* rc, float sin_angle, float cos_angle)
{
  Point center;
  get_center(center, *rc);
  rotate(rc, sin_angle, cos_angle, center);
}

void PF::Shape::rotate(VipsRect* rc, float sin_angle, float cos_angle, Point& center)
{
  Point pt1, pt2, pt3, pt4;
  
  pt1.set(rc->left, rc->top);
  pt2.set(rc->left + rc->width, rc->top);
  pt3.set(rc->left + rc->width, rc->top + rc->height);
  pt4.set(rc->left, rc->top + rc->height);
  
  pt1.rotate(center, sin_angle, cos_angle);
  pt2.rotate(center, sin_angle, cos_angle);
  pt3.rotate(center, sin_angle, cos_angle);
  pt4.rotate(center, sin_angle, cos_angle);
  
  rc->left =   std::min(pt1.get_x(), std::min(pt2.get_x(), std::min(pt3.get_x(), pt4.get_x())));
  rc->top =    std::min(pt1.get_y(), std::min(pt2.get_y(), std::min(pt3.get_y(), pt4.get_y())));
  rc->width =  std::max(pt1.get_x(), std::max(pt2.get_x(), std::max(pt3.get_x(), pt4.get_x()))) - rc->left;
  rc->height = std::max(pt1.get_y(), std::max(pt2.get_y(), std::max(pt3.get_y(), pt4.get_y()))) - rc->top;
}

// -----------------------------------
// SegmentInfo
// -----------------------------------

void PF::SegmentInfo::scale(int sf)
{
  control_pt1.scale(sf);
  control_pt2.scale(sf);
}

// -----------------------------------
// Circle
// -----------------------------------

int PF::Circle::hit_test(Point& pt, int& additional)
{
  Point& point = get_point();
  const int radius = get_radius();
  int dx = abs(pt.get_x()-point.get_x());
  int dy = abs(pt.get_y()-point.get_y());
  int R = radius+get_falloff()+2;
  
  // point is outside the radius
  if (!(dx+PF::Shape::hit_test_delta>R || dy+PF::Shape::hit_test_delta>R)) {
    // point is inside the shape
    if (dx + dy + (PF::Shape::hit_test_delta*2) <= radius) 
      return hit_shape;
    // point is inside the falloff
    if (dx + dy + (PF::Shape::hit_test_delta*2) <= R) 
      return hit_falloff;
    // now test a more exact formula for shape and falloff
    if (dx*dx + dy*dy <= (radius+2)*(radius+2))
      return hit_shape;
    if (dx*dx + dy*dy <= R*R)
      return hit_falloff;
  }
  // test for the source
  if ( get_has_source() ) {
    dx = abs(pt.get_x()-(point.get_x()+get_source_point().get_x()));
    dy = abs(pt.get_y()-(point.get_y()+get_source_point().get_y()));
    // point is outside the radius
    if (!(dx+PF::Shape::hit_test_delta>R || dy+PF::Shape::hit_test_delta>R)) {
      // point is inside the falloff
      if (dx + dy + (PF::Shape::hit_test_delta*2) <= R) 
        return hit_source;
      // now test a more exact formula for falloff+shape
      if (dx*dx + dy*dy <= R*R)
        return hit_source;
    }
  }
  return hit_none;
}

void PF::Circle::get_rect(VipsRect* rc)
{
  const int R = get_radius();
  rc->left = get_point().get_x()-R;
  rc->top = get_point().get_y()-R;
  rc->width = rc->height = R*2;
}

void PF::Circle::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff();
  rc->top -= get_falloff();
  rc->width += get_falloff()*2;
  rc->height += get_falloff()*2;
}

void PF::Circle::build_mask(SplineCurve& scurve)
{
//  std::cout<<"PF::Circle::build_mask()"<<std::endl;
  
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }
  
  // get bounding rect
  VipsRect rc;
  get_falloff_rect(&rc);

  // nothing to return
  if (rc.width < 1 || rc.height < 1) return;
  
  // save offset
  Point pt_offset(-rc.left, -rc.top);
  
  // offset to (0, 0)
  rc.left = rc.top = 0;
  
  // alloc mask
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  memset(mask, 0, rc.width * rc.height * sizeof(float));

  // populate the buffer
  Point center(get_point());
  center.offset(pt_offset);
  const float radius_2 = get_radius()*get_radius();
  const float radiusf_2 = (get_radius()+get_falloff())*(get_radius()+get_falloff());
  
  for(int y = 0; y < rc.height; y++) {
    float* pmask = mask + y * rc.width;
    for(int x = 0; x < rc.width; x++) {
      const float dist2 = center.distance2(x, y);
      if (dist2 <= radius_2) {
        pmask[x] = get_opacity();
      }
      else if (dist2 <= radiusf_2) {
        const float f = (radiusf_2 - dist2) / (radiusf_2 - radius_2);
        const float f2 = f * f * get_opacity();
        const float f2s = scurve.get_value(f2);
        pmask[x] = f2s;
      }
    }
  }

}

// -----------------------------------
// Ellipse
// -----------------------------------

int PF::Ellipse::get_falloff_y() 
{
  int falloff = get_radius_y() * ((float)get_falloff_x()/(float)get_radius_x());
  
  if (falloff <= 0 && get_falloff_x() > 0) falloff = 1;
  
  return falloff; 
//  return get_falloff();
}
/*
void PF::Ellipse::set_size(int n)
{ 
  int ny = n * ((float)get_radius_y()/(float)get_radius_x());
  
  if (ny <= 0 && n > 0) ny = 1;
  
  set_radius_y(ny); 
  set_radius_x(n); 
}

void PF::Ellipse::expand_falloff(int n)
{
  set_falloff(get_falloff()+n);
}
*/

void PF::Ellipse::expand(int n)
{
  int ny = n * ((float)get_radius_y()/(float)get_radius_x());
  
  if (ny <= 0 && n > 0) ny = 1;
  
  set_radius_y(ny+get_radius_y()); 
  set_radius_x(n+get_radius_x()); 
}

int PF::Ellipse::hit_test(Point& pt, int& additional)
{
  if ( get_angle() != 0 ) {
    return hit_test(pt, get_angle(), additional);
  }
  
  VipsRect rc;  
  get_falloff_rect(&rc);
  
  // inside the falloff rect
  if ( hit_test_rect(pt, &rc) ) {
    const int radius_x = get_radius_x();
    const int radius_y = get_radius_y();
    Point& center = get_point();
    Point pt1;
    
    const float Rfx2 = ( radius_x + get_falloff_x() + PF::Shape::hit_test_delta ) * ( radius_x + get_falloff_x() + PF::Shape::hit_test_delta );
    const float Rfy2 = ( radius_y + get_falloff_y() + PF::Shape::hit_test_delta ) * ( radius_y + get_falloff_y() + PF::Shape::hit_test_delta );
    const float dx2 = ( pt.get_x() - center.get_x() ) * ( pt.get_x() - center.get_x() );
    const float dy2 = ( pt.get_y() - center.get_y() ) * ( pt.get_y() - center.get_y() );
    const float fp = dx2/Rfx2 + dy2/Rfy2;

    // inside falloff perimeter
    if ( fp <= 1.f ) {
      // check nodes
      pt1.set( center.get_x(), (center.get_y() - radius_y) );
      if ( hit_test_node(pt, pt1) ) {
        additional = 0;
        return hit_node;
      }
      pt1.set( center.get_x(), (center.get_y() + radius_y) );
      if ( hit_test_node(pt, pt1) ) {
        additional = 2;
        return hit_node;
      }
      pt1.set( (center.get_x() + radius_x), center.get_y() );
      if ( hit_test_node(pt, pt1) ) {
        additional = 1;
        return hit_node;
      }
      pt1.set( (center.get_x() - radius_x), center.get_y() );
      if ( hit_test_node(pt, pt1) ) {
        additional = 3;
        return hit_node;
      }

      const float Rx2 = (radius_x - PF::Shape::hit_test_delta) * (radius_x - PF::Shape::hit_test_delta);
      const float Ry2 = (radius_y - PF::Shape::hit_test_delta) * (radius_y - PF::Shape::hit_test_delta);
      const float dxs2 = ( pt.get_x() - center.get_x() ) * ( pt.get_x() - center.get_x() );
      const float dys2 = ( pt.get_y() - center.get_y() ) * ( pt.get_y() - center.get_y() );
      const float fs = dxs2/Rx2 + dys2/Ry2;

      // inside shape 
      if ( fs <= 1.f )
        return hit_shape;

      const float Rxg2 = (radius_x + PF::Shape::hit_test_delta) * (radius_x + PF::Shape::hit_test_delta);
      const float Ryg2 = (radius_y + PF::Shape::hit_test_delta) * (radius_y + PF::Shape::hit_test_delta);

      // inside shape perimeter
      if ( dx2/Rxg2 + dy2/Ryg2 <= 1.f ) {
        additional = center.get_quadrant(pt);
        
        return hit_segment;
      }
      
      // default is falloff
      return hit_falloff;
    }
  }
  
  // check the source
  if ( get_has_source() ) {
    Ellipse shape = *this;
    shape.set_has_source(false);
    shape.offset(get_source_point());
    int hit_t = shape.hit_test(pt, additional);
    if ( hit_t != hit_none) hit_t = hit_source;
    
    return hit_t;
  }

  return hit_none;
}

int PF::Ellipse::hit_test(Point& _pt, float angle_1, int& additional)
{
  VipsRect rc;  
  get_falloff_rect(&rc);
  
  const float _angle = angle_1*M_PI/180.f;
  const float sin_angle = sinf(_angle);
  const float cos_angle = cosf(_angle);

  VipsRect rc_r = rc;
  Point& center = get_point();
  rotate(&rc_r, -sin_angle, -cos_angle, center);
  Point pt(_pt);
  pt.rotate(center, sin_angle, cos_angle);

  // inside the falloff rect
  if ( hit_test_rect(_pt, &rc_r) ) {
    const int radius_x = get_radius_x();
    const int radius_y = get_radius_y();
    
    Point center_r;
    get_center(center_r, rc_r);
    Point pt1;
    
    const float Rfx2 = ( radius_x + get_falloff_x() + PF::Shape::hit_test_delta ) * ( radius_x + get_falloff_x() + PF::Shape::hit_test_delta );
    const float Rfy2 = ( radius_y + get_falloff_y() + PF::Shape::hit_test_delta ) * ( radius_y + get_falloff_y() + PF::Shape::hit_test_delta );
    const float dx2 = ( pt.get_x() - center.get_x() ) * ( pt.get_x() - center.get_x() );
    const float dy2 = ( pt.get_y() - center.get_y() ) * ( pt.get_y() - center.get_y() );
    const float fp = dx2/Rfx2 + dy2/Rfy2;

    // inside falloff perimeter
    if ( fp <= 1.f ) {
      // check nodes
      pt1.set( center.get_x(), (center.get_y() - radius_y) );
      if ( hit_test_node(pt, pt1) ) {
        additional = 0;
        return hit_node;
      }
      pt1.set( center.get_x(), (center.get_y() + radius_y) );
      if ( hit_test_node(pt, pt1) ) {
        additional = 2;
        return hit_node;
      }
      pt1.set( (center.get_x() + radius_x), center.get_y() );
      if ( hit_test_node(pt, pt1) ) {
        additional = 1;
        return hit_node;
      }
      pt1.set( (center.get_x() - radius_x), center.get_y() );
      if ( hit_test_node(pt, pt1) ) {
        additional = 3;
        return hit_node;
      }

      const float Rx2 = (radius_x - PF::Shape::hit_test_delta) * (radius_x - PF::Shape::hit_test_delta);
      const float Ry2 = (radius_y - PF::Shape::hit_test_delta) * (radius_y - PF::Shape::hit_test_delta);
      const float dxs2 = ( pt.get_x() - center.get_x() ) * ( pt.get_x() - center.get_x() );
      const float dys2 = ( pt.get_y() - center.get_y() ) * ( pt.get_y() - center.get_y() );
      const float fs = dxs2/Rx2 + dys2/Ry2;

      // inside shape 
      if ( fs <= 1.f )
        return hit_shape;

      const float Rxg2 = (radius_x + PF::Shape::hit_test_delta) * (radius_x + PF::Shape::hit_test_delta);
      const float Ryg2 = (radius_y + PF::Shape::hit_test_delta) * (radius_y + PF::Shape::hit_test_delta);

      // inside shape perimeter
      if ( dx2/Rxg2 + dy2/Ryg2 <= 1.f ) {
        additional = center.get_quadrant(pt);
        
        return hit_segment;
      }
      
      // default is falloff
      return hit_falloff;
    }
  }
  
  // check the source
  if ( get_has_source() ) {
    Ellipse shape = *this;
    shape.set_has_source(false);
    shape.offset(get_source_point());
    int hit_t = shape.hit_test(_pt, additional);
    if ( hit_t != hit_none) hit_t = hit_source;
    
    return hit_t;
  }

  return hit_none;
}

void PF::Ellipse::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options)
{
  switch (hit_t)
  {
  case PF::Shape::hit_node:
    if (additional == 0) {
      set_radius_y(get_radius_y() + (prev.get_y() - curr.get_y()));
    }
    else if (additional == 1) {
      set_radius_x(get_radius_x() + (curr.get_x() - prev.get_x()));
    }
    else if (additional == 2) {
      set_radius_y(get_radius_y() + (curr.get_y() - prev.get_y()));
    }
    else if (additional == 3) {
      set_radius_x(get_radius_x() + (prev.get_x() - curr.get_x()));
    }
    break;
  case PF::Shape::hit_segment:
  {
    float atn1=0, atn2=0;
    Point center;
    
    get_center(center);
    
    if (curr.get_x()-center.get_x()) atn1 = atanf( (float)(curr.get_y()-center.get_y()) / (float)(curr.get_x()-center.get_x()) ) * 180.f / (float)M_PI;
    if (prev.get_x()-center.get_x()) atn2 = atanf( (float)(prev.get_y()-center.get_y()) / (float)(prev.get_x()-center.get_x()) ) * 180.f / (float)M_PI;
    set_angle( get_angle() + (atn2 - atn1) );
  }
    break;
  case PF::Shape::hit_falloff_segment:
  case PF::Shape::hit_falloff_node:
  default:
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source, options);
    break;
  }

}

void PF::Ellipse::get_rect(VipsRect* rc)
{
  const int Rx = get_radius_x();
  const int Ry = get_radius_y();
  rc->left = get_point().get_x()-Rx;
  rc->top = get_point().get_y()-Ry;
  rc->width = Rx*2;
  rc->height = Ry*2;
}

void PF::Ellipse::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff_x();
  rc->top -= get_falloff_y();
  rc->width += get_falloff_x()*2;
  rc->height += get_falloff_y()*2;
}

void PF::Ellipse::build_mask(SplineCurve& scurve)
{
//  std::cout<<"PF::Ellipse::build_mask()"<<std::endl;
  if ( get_angle() != 0.f) {
    build_mask(scurve, get_angle());
    return;
  }
  
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }
  
  // get bounding rect
  VipsRect rc;
  get_falloff_rect(&rc);
  
  // nothing to return
  if (rc.width < 1 || rc.height < 1) return;
  
  // save offset
  Point pt_offset(-rc.left, -rc.top);
  
  // offset to (0, 0)
  rc.left = rc.top = 0;
  
  // alloc mask
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  memset(mask, 0, rc.width * rc.height * sizeof(float));

  // populate the buffer
  Point center(get_point());
  center.offset(pt_offset);
  
  // n Defines the bounds of the horizontal lines which fill the ellipse.
  const int w = get_radius_x();
  const int h = get_radius_y();
  const int wf = w+get_falloff_x();
  const int hf = h+get_falloff_y();
  int n = w;
  int nf = n+get_falloff_x();
  const float w2 = w*w;
  const float h2 = h*h;
  const float wf2 = wf*wf;
  const float hf2 = hf*hf;
  const float scale = ( w2 / wf2 );
  const float scale_1 = 1.f / (1.f-scale);

  // expand from the center
  for (int y = 0; y < hf; y++) {
    // The current top and bottom rows.
    const int ra = center.get_y() + y;
    const int rb = center.get_y() - y;
  
    // This loop removes 1 from n until it is within the shape
    const int y2 = y*y;
    const float y2_hf2 = (float)(y2) / hf2;
    while ( wf2*(hf2-y2) < hf2*nf*nf && nf>0 ) --nf;
    while ( w2*(h2-y2) < h2*n*n && n>0 ) --n;
  
    // Draws horizontal line from -n to n across the ellipse
    float* pmask1 = mask + ra * rc.width;
    float* pmask2 = mask + rb * rc.width;
    
    for (int x = center.get_x() - nf, x2 = center.get_x() + nf; x <= center.get_x(); x++, x2--) {
      if (x >= center.get_x() - n && y <= h) {
        pmask1[x] = pmask2[x] = pmask1[x2] = pmask2[x2] = get_opacity();
     }
      else {
        const float f = ( (float)((x-center.get_x())*(x-center.get_x())) / wf2 + y2_hf2 ) - scale;
        const float f1 = 1.f - ( f * scale_1 );
        if (f1 > 1.f || f1 < 0.f) {
          std::cout<<"PF::Ellipse::build_mask(): f out of range!!! f: "<<f<<", x: "<<x<<", n: "<<n<<", nf: "<<nf<<std::endl;
        }
        else {
          const float f2 = f1 * f1 * get_opacity();
          const float f2s = scurve.get_value(f2);
          pmask1[x] = pmask2[x] = pmask1[x2] = pmask2[x2] = f2s;
        }
      }
    }
  }

}

void PF::Ellipse::build_mask(SplineCurve& scurve, float angle_1)
{
  //  std::cout<<"PF::Ellipse::build_mask()"<<std::endl;
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }

  const float _angle = angle_1*M_PI/180.f;
  const float sin_angle = sinf(_angle);
  const float cos_angle = cosf(_angle);

  Point center(get_point());

  // get bounding rect
  VipsRect rc;
  get_falloff_rect(&rc);

  VipsRect rc_r = rc;
  rotate(&rc_r, sin_angle, cos_angle, center);

  Point center_r;
  get_center(center_r, rc_r);

  // nothing to return
  if (rc.width < 1 || rc.height < 1) return;

  mask = (float*)malloc(rc_r.width * rc_r.height * sizeof(float));
  memset(mask, 0, rc_r.width * rc_r.height * sizeof(float));

  // populate the buffer

  const int w = get_radius_x();
  const int h = get_radius_y();
  const int wf = w+get_falloff_x();
  const int hf = h+get_falloff_y();
  int n = w; // n Defines the bounds of the horizontal lines which fill the ellipse.
  int nf = n+get_falloff_x();
  const float w2 = w*w;
  const float h2 = h*h;
  const float wf2 = wf*wf;
  const float hf2 = hf*hf;
  const float scale = ( w2 / wf2 );
  const float scale_1 = 1.f / (1.f-scale);

  for (int y = 0; y < rc_r.height/2; y++) {
    float* pmask1 = mask + ((rc_r.height/2) + y) * rc_r.width;
    float* pmask2 = mask + ((rc_r.height/2) - y) * rc_r.width;
    for (int x = 0; x < rc_r.width; x++) {
      const int x1 = /*center_r.get_x() +*/ x;
      const int x2 = ( rc_r.width - 1 ) - x;
      Point pt(x-(rc_r.width/2), y);
      Point pt_0;
      pt.rotate(pt_0, sin_angle, cos_angle);

      const float fs = ( (float)((pt.get_x())*(pt.get_x())) / w2 + 
          (float)((pt.get_y())*(pt.get_y())) / h2 );
      if ( fs <= 1.f ) {
        pt.set(x, y);
        if (pt.get_y() < 0 || pt.get_y() >= rc_r.height || pt.get_x() < 0 || pt.get_x() >= rc_r.width) {
          std::cout<<"PF::Ellipse::build_mask(): pt.get_x(): "<<pt.get_x()<<", pt.get_y(): "<<pt.get_y()<<std::endl;
          std::cout<<"PF::Ellipse::build_mask(): rc_r.width: "<<rc_r.width<<", rc_r.height: "<<rc_r.height<<std::endl;
          std::cout<<"PF::Ellipse::build_mask(): x: "<<x<<", y: "<<y<<std::endl;
        }
        else {
          pmask1[x1] = get_opacity(); // q 1
          pmask2[x2] = get_opacity(); // q 1
        }
      }
      else {
        const float f = ( (float)((pt.get_x())*(pt.get_x())) / wf2 + 
            (float)((pt.get_y())*(pt.get_y())) / hf2 );
        if ( f <= 1.f ) {
          pt.set(x, y);
          if (pt.get_y() < 0 || pt.get_y() >= rc_r.height || pt.get_x() < 0 || pt.get_x() >= rc_r.width) {
            std::cout<<"PF::Ellipse::build_mask(): pt.get_x(): "<<pt.get_x()<<", pt.get_y(): "<<pt.get_y()<<std::endl;
            std::cout<<"PF::Ellipse::build_mask(): rc_r.width: "<<rc_r.width<<", rc_r.height: "<<rc_r.height<<std::endl;
            std::cout<<"PF::Ellipse::build_mask(): x: "<<x<<", y: "<<y<<std::endl;
          }
          else {
            const float f1 = 1.f - ( (f - scale) * scale_1 );
            if (f1 > 1.f || f1 < 0.f) {
              std::cout<<"PF::Ellipse::build_mask(): f out of range!!! f: "<<f<<", x: "<<x<<", n: "<<n<<", nf: "<<nf<<std::endl;
            }
            else {
              const float f2 = f1 * f1 * get_opacity();
              const float f2s = scurve.get_value(f2);

              pmask1[x1] = f2s; // q 1
              pmask2[x2] = f2s; // q 1
            }
          }
        }
      }
    }
  }

}

#if 0
// -----------------------------------
// Rect1
// -----------------------------------

void PF::Rect1::offset_falloff_point(int n, int x, int y)
{
  get_falloff_point(n).offset_pos(x, y);
}

void PF::Rect1::offset_point(int n, int x, int y)
{
  Point pt = get_point(n);
  PF::Shape::offset_point(n, x, y);
  if (get_point(0).get_x() == get_point(1).get_x() || get_point(0).get_y() == get_point(1).get_y()) {
    set_point(n, pt);
  }
}

void PF::Rect1::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options)
{
  int x, y;
  Point center;
  
  switch (hit_t)
  {
  case PF::Shape::hit_segment:
    if (additional == 0) {
      offset_point(0, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 1) {
      offset_point(1, curr.get_x()-prev.get_x(), 0);
    }
    else if (additional == 2) {
      offset_point(1, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 3) {
      offset_point(0, curr.get_x()-prev.get_x(), 0);
    }
    break;
  case PF::Shape::hit_falloff_segment:
    get_center(center);
    if (additional == 0) {
      if (get_point(0).get_y() < center.get_y())
        offset_falloff_point(0, 0, prev.get_y()-curr.get_y());
      else
        offset_falloff_point(0, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 1) {
      if (get_point(1).get_x() < center.get_x())
        offset_falloff_point(1, prev.get_x()-curr.get_x(), 0);
      else
        offset_falloff_point(1, curr.get_x()-prev.get_x(), 0);
    }
    else if (additional == 2) {
      if (get_point(1).get_y() < center.get_y())
        offset_falloff_point(1, 0, prev.get_y()-curr.get_y());
      else
        offset_falloff_point(1, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 3) {
      if (get_point(0).get_x() < center.get_x())
        offset_falloff_point(0, prev.get_x()-curr.get_x(), 0);
      else
        offset_falloff_point(0, curr.get_x()-prev.get_x(), 0);
    }
    break;
  case PF::Shape::hit_node:
    if (additional == 0) {
      offset_point(0, prev, curr);
    }
    else if (additional == 1) {
      offset_point(1, curr.get_x()-prev.get_x(), 0);
      offset_point(0, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 2) {
      offset_point(1, prev, curr);
    }
    else if (additional == 3) {
      offset_point(0, curr.get_x()-prev.get_x(), 0);
      offset_point(1, 0, curr.get_y()-prev.get_y());
    }
    break;
  case PF::Shape::hit_falloff_node:
    get_center(center);
    if (additional == 0) {
      if (get_point(0).get_y() < center.get_y())
        offset_falloff_point(0, 0, prev.get_y()-curr.get_y());
      else
        offset_falloff_point(0, 0, curr.get_y()-prev.get_y());
      if (get_point(0).get_x() < center.get_x())
        offset_falloff_point(0, prev.get_x()-curr.get_x(), 0);
      else
        offset_falloff_point(0, curr.get_x()-prev.get_x(), 0);
    }
    else if (additional == 1) {
      if (get_point(0).get_y() < center.get_y())
        offset_falloff_point(0, 0, prev.get_y()-curr.get_y());
      else
        offset_falloff_point(0, 0, curr.get_y()-prev.get_y());
      if (get_point(1).get_x() < center.get_x())
        offset_falloff_point(1, prev.get_x()-curr.get_x(), 0);
      else
        offset_falloff_point(1, curr.get_x()-prev.get_x(), 0);
    }
    else if (additional == 2) {
      if (get_point(1).get_y() < center.get_y())
        offset_falloff_point(1, 0, prev.get_y()-curr.get_y());
      else
        offset_falloff_point(1, 0, curr.get_y()-prev.get_y());
      if (get_point(1).get_x() < center.get_x())
        offset_falloff_point(1, prev.get_x()-curr.get_x(), 0);
      else
        offset_falloff_point(1, curr.get_x()-prev.get_x(), 0);
    }
    else if (additional == 3) {
      if (get_point(1).get_y() < center.get_y())
        offset_falloff_point(1, 0, prev.get_y()-curr.get_y());
      else
        offset_falloff_point(1, 0, curr.get_y()-prev.get_y());
      if (get_point(0).get_x() < center.get_x())
        offset_falloff_point(0, prev.get_x()-curr.get_x(), 0);
      else
        offset_falloff_point(0, curr.get_x()-prev.get_x(), 0);
    }
    break;
  default:
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source, options);
    break;
  }

}

int PF::Rect1::hit_test(Point& pt, int& additional)
{
  int hit_t = hit_none;
  
  VipsRect rc;
  get_falloff_rect(&rc);

  // inside the falloff
  if ( hit_test_rect(pt, &rc) ) {
    Point pt1, pt2;
    Point pt_fall0, pt_fall1;
    Point center;
    VipsRect rcs;
    get_rect(&rcs);
    get_center(center, rcs);

    // falloff nodes
    get_falloff_point_absolute(0, /*center,*/ pt_fall0);
    if ( hit_test_node(pt, pt_fall0) ) {
      additional = 0;
      return hit_falloff_node;
    }

    get_falloff_point_absolute(1, /*center,*/ pt_fall1);
    pt1.set( pt_fall1.get_x(), pt_fall0.get_y() );
    if ( hit_test_node(pt, pt1) ) {
      additional = 1;
      return hit_falloff_node;
    }

    if ( hit_test_node(pt, pt_fall1) ) {
      additional = 2;
      return hit_falloff_node;
    }

    pt1.set( pt_fall0.get_x(), pt_fall1.get_y() );
    if ( hit_test_node(pt, pt1) ) {
      additional = 3;
      return hit_falloff_node;
    }

    // shape nodes
    if ( hit_test_node(pt, get_point(0)) ) {
      additional = 0;
      return hit_node;
    }
    pt1.set( get_point(1).get_x(), get_point(0).get_y() );
    if ( hit_test_node(pt, pt1) ) {
      additional = 1;
      return hit_node;
    }
    if ( hit_test_node(pt, get_point(1)) ) {
      additional = 2;
      return hit_node;
    }
    pt1.set( get_point(0).get_x(), get_point(1).get_y() );
    if ( hit_test_node(pt, pt1) ) {
      additional = 3;
      return hit_node;
    }

    // falloff segments
    pt2.set( pt_fall1.get_x(), pt_fall0.get_y() );
    if ( hit_test_rect(pt, pt_fall0, pt2) ) {
      additional = 0;
      return hit_falloff_segment;
    }
    pt1 = pt2;

    if ( hit_test_rect(pt, pt1, pt_fall1) ) {
      additional = 1;
      return hit_falloff_segment;
    }

    pt2.set( pt_fall0.get_x(), pt_fall1.get_y() );
    if ( hit_test_rect(pt, pt_fall1, pt2) ) {
      additional = 2;
      return hit_falloff_segment;
    }
    pt1 = pt2;

    if ( hit_test_rect(pt, pt1, pt_fall0) ) {
      additional = 3;
      return hit_falloff_segment;
    }
    
    // shape segments
    pt1 = get_point(0);
    pt2.set( get_point(1).get_x(), get_point(0).get_y() );
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 0;
      return hit_segment;
    }
    pt1 = pt2;
    pt2 = get_point(1);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 1;
      return hit_segment;
    }
    pt1 = pt2;
    pt2.set( get_point(0).get_x(), get_point(1).get_y() );
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 2;
      return hit_segment;
    }
    pt1 = pt2;
    pt2 = get_point(0);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 3;
      return hit_segment;
    }
    
    // inside the shape
    if ( hit_test_rect(pt, &rcs) )
      return hit_shape;
    
    return hit_falloff;
  }
  
  // check the source
  if ( hit_t == hit_none && get_has_source()) {
    Rect1 shape = *this;
    shape.set_has_source(false);
    shape.offset(get_source_point());
    hit_t = shape.hit_test(pt, additional);
    if ( hit_t != hit_none) hit_t = hit_source;
  }

  return hit_t;
}

// returns in pt the intersection of the two lines defined by (pt1, pt2) - (pt3, pt4)
void PF::Rect1::lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt)
{
  const float a = ( ( ((pt1.get_x()*pt2.get_y()) - (pt1.get_y()*pt2.get_x())) * (pt3.get_x()-pt4.get_x()) ) - 
      ( (pt1.get_x()-pt2.get_x()) * ( (pt3.get_x()*pt4.get_y()) - (pt3.get_y()*pt4.get_x()) ) ) );
  const float b = ( (pt1.get_x()-pt2.get_x()) * (pt3.get_y()-pt4.get_y()) - (pt1.get_y()-pt2.get_y()) * (pt3.get_x()-pt4.get_x()) );
  const float c = ( ( ((pt1.get_x()*pt2.get_y()) - (pt1.get_y()*pt2.get_x())) * (pt3.get_y()-pt4.get_y()) ) - 
      ( (pt1.get_y()-pt2.get_y()) * ( (pt3.get_x()*pt4.get_y()) - (pt3.get_y()*pt4.get_x()) ) ) );
  const float d = ( (pt1.get_x()-pt2.get_x()) * (pt3.get_y()-pt4.get_y()) - (pt1.get_y()-pt2.get_y()) * (pt3.get_x()-pt4.get_x()) );
  
  pt.set( a / b, c / d );
  
}

void PF::Rect1::build_mask(SplineCurve& scurve)
{
//  std::cout<<"PF::Rect1::build_mask()"<<std::endl;
  
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }
  
  // get bounding rect
  VipsRect rc;
  get_falloff_rect(&rc);
  
  // nothing to return
  if (rc.width <= 1 || rc.height <= 1) return;
  
  // save offset
  Point pt_offset(-rc.left, -rc.top);
  
  // offset to (0, 0)
  rc.left = rc.top = 0;
  
  // alloc mask
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  memset(mask, 0, rc.width * rc.height * sizeof(float));

  // populate the buffer
  
  // shape
  VipsRect rcs;
  get_rect(&rcs);
  
  rcs.left += pt_offset.get_x();
  rcs.top += pt_offset.get_y();

  for(int y = rcs.top; y < rcs.top+rcs.height; y++) {
    float* pmask = mask + y * rc.width;
    for(int x = rcs.left; x < rcs.left+rcs.width; x++) {
      pmask[x] = get_opacity();
    }
  }

  float dist_t2;
  
#define FALLOFF_REC_SIDES(x_from, x_to, y_from, y_to, dist_t, dist) \
  dist_t2 = dist_t * dist_t; \
  for(int y = y_from; y < y_to; y++) { \
    float* pmask = mask + y * rc.width; \
    for(int x = x_from; x < x_to; x++) { \
      const float dist2 = dist * dist; \
      const float f = (dist_t2-dist2) / dist_t2; \
      const float f2 = f * f * get_opacity(); \
      pmask[x] = scurve.get_value(f2); \
    } \
  } \

  FALLOFF_REC_SIDES(rcs.left, (rcs.left+rcs.width), rc.top, rcs.top, (rcs.top-rc.top), (y-rcs.top));
  FALLOFF_REC_SIDES(rcs.left, (rcs.left+rcs.width), (rcs.top+rcs.height), (rc.top+rc.height), ((rc.top+rc.height)-(rcs.top+rcs.height)), (y-(rcs.top+rcs.height)));
  FALLOFF_REC_SIDES(rc.left, rcs.left, rcs.top, (rcs.top+rcs.height), (rcs.left-rc.left), (x-rcs.left));
  FALLOFF_REC_SIDES((rcs.left+rcs.width), (rc.left+rc.width), rcs.top, (rcs.top+rcs.height), ((rc.left+rc.width)-(rcs.left+rcs.width)), (x-(rcs.left+rcs.width)));
  
#undef FALLOFF_REC_SIDES
  
  if (true) { // round corners
#define FALLOFF_REC_CORNERS_R(x_from, x_to, y_from, y_to, h, v) \
  if (x_to-x_from > 0 && y_to-y_from > 0) { \
    const int rx2 = (x_to-x_from)*(x_to-x_from); \
    const int ry2 = (y_to-y_from)*(y_to-y_from); \
    for(int y = y_from; y < y_to; y++) { \
      float* pmask = mask + y * rc.width; \
      for(int x = x_from; x < x_to; x++) { \
        const float f = point_in_ellipse(x, y, h, v, rx2, ry2); \
          if (f <= 1.f) { \
            const float f2 = (1.f-f) * (1.f-f) * get_opacity(); \
        pmask[x] = scurve.get_value(f2); \
          } \
      } \
    } \
  } \

    FALLOFF_REC_CORNERS_R(rc.left, rcs.left, rc.top, rcs.top, rcs.left, rcs.top);
    FALLOFF_REC_CORNERS_R((rcs.left+rcs.width), (rc.left+rc.width), rc.top, rcs.top, (rcs.left+rcs.width), rcs.top);
    FALLOFF_REC_CORNERS_R((rcs.left+rcs.width), (rc.left+rc.width), (rcs.top+rcs.height), (rc.top+rc.height), (rcs.left+rcs.width), (rcs.top+rcs.height));
    FALLOFF_REC_CORNERS_R(rc.left, rcs.left, (rcs.top+rcs.height), (rc.top+rc.height), rcs.left, (rcs.top+rcs.height));

#undef FALLOFF_REC_CORNERS_R
  }
  else { // straight corners
  Point pt, a, b, c, d, pt_int;

#define FALLOFF_REC_CORNERS(x_from, x_to, y_from, y_to) \
  for(int y = y_from; y < y_to; y++) { \
    float* pmask = mask + y * rc.width; \
    for(int x = x_from; x < x_to; x++) { \
      pt.set(x, y); \
      if ( point_in_triangle(pt, a, c, d) )  \
        lines_intersects(c, pt, a, d, pt_int); /* get the intersection of (c, pt) and (a, d) */ \
      else \
        lines_intersects(c, pt, a, b, pt_int); /* get the intersection of (c, pt) and (a, b) */ \
    \
      const float dist_t2 = pt_int.distance2(c); /* total distance == distance from intersection to c */ \
      const float dist2 = c.distance2(pt); /* distance == current point to c */ \
      const float f = (dist_t2-dist2) / dist_t2; \
      const float f2 = f * f * get_opacity(); \
      pmask[x] = scurve.get_value(f2); \
    } \
  } \

  a.set(rc.left, rc.top);
  b.set(rcs.left, rc.top);
  c.set(rcs.left, rcs.top);
  d.set(rc.left, rcs.top);
  FALLOFF_REC_CORNERS(rc.left, rcs.left, rc.top, rcs.top);
  
  a.set((rc.left+rc.width), rc.top);
  b.set((rcs.left+rcs.width), rc.top);
  c.set((rcs.left+rcs.width), rcs.top);
  d.set((rc.left+rc.width), rcs.top);
  FALLOFF_REC_CORNERS((rcs.left+rcs.width), (rc.left+rc.width), rc.top, rcs.top);
  
  a.set((rc.left+rc.width), (rc.top+rc.height));
  b.set((rcs.left+rcs.width), (rc.top+rc.height));
  c.set((rcs.left+rcs.width), (rcs.top+rcs.height));
  d.set((rc.left+rc.width), (rcs.top+rcs.height));
  FALLOFF_REC_CORNERS((rcs.left+rcs.width), (rc.left+rc.width), (rcs.top+rcs.height), (rc.top+rc.height));
  
  a.set(rc.left, (rc.top+rc.height));
  b.set(rc.left, (rcs.top+rcs.height));
  c.set(rcs.left, (rcs.top+rcs.height));
  d.set(rcs.left, (rc.top+rc.height));
  FALLOFF_REC_CORNERS(rc.left, rcs.left, (rcs.top+rcs.height), (rc.top+rc.height));
  
#undef FALLOFF_REC_CORNERS
  }
  
}
#endif

// -----------------------------------
// Polygon
// -----------------------------------

void PF::Polygon::scale(int scale)
{
  PF::Shape::scale(scale);
  
  for (int i = 0; i < get_segments_count(); i++) {
    get_segment_info(i).scale(scale);
  }

}

void PF::Polygon::fill_polygon(std::vector<Point>& points, float* buffer, VipsRect& rc)
{
  int  nodes, pixelX, pixelY, i, j, swap ;
  int polyCorners = points.size();
  int* nodeX = (int*)malloc(polyCorners * sizeof(int));
  
  int IMAGE_TOP = rc.top;
  int IMAGE_LEFT = rc.left;
  int IMAGE_BOT = IMAGE_TOP + rc.height;
  int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  //  Loop through the rows of the image.
  for (pixelY=IMAGE_TOP; pixelY<IMAGE_BOT; pixelY++) {

    //  Build a list of nodes.
    nodes=0;
    j=polyCorners-1;
    for (i=0; i<polyCorners; i++) {
      if ( (points[i].y<(double) pixelY && points[j].y>=(double) pixelY)
      ||  (points[j].y<(double) pixelY && points[i].y>=(double) pixelY) ) {
        nodeX[nodes++]=(int) ( points[i].x + ((float)(pixelY-points[i].y) / (float)(points[j].y-points[i].y)) 
        * (float)(points[j].x-points[i].x) ); 
      }
      j=i; 
    }

    //  Sort the nodes, via a simple “Bubble” sort.
    i=0;
    while (i<nodes-1) {
      if (nodeX[i]>nodeX[i+1]) {
        swap=nodeX[i]; nodeX[i]=nodeX[i+1]; nodeX[i+1]=swap; if (i) i--; 
      }
      else {
        i++; 
      }
    }

    //  Fill the pixels between node pairs.
    for (i=0; i<nodes; i+=2) {
      if   (nodeX[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX[i+1]> IMAGE_LEFT ) {
        if (nodeX[i  ]< IMAGE_LEFT ) nodeX[i  ]=IMAGE_LEFT ;
        if (nodeX[i+1]> IMAGE_RIGHT) nodeX[i+1]=IMAGE_RIGHT;
        for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) {
//          fillPixel(pixelX,pixelY);
          buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = 1.f;
        }
      }
    }
  }

  if (nodeX) free(nodeX);
}

// returns a vector with bezier curve points
void PF::Polygon::inter_bezier3(Point& anchor1, Point& anchor2, Point& control, std::vector<Point>& points)
{
  Point pt1, pt2;
  float step = 1.f/100.f;
  
  pt1 = anchor1;
  points.push_back(pt1);
  
  //This loops draws each step of the curve
  for (float t = 0.f; t <= 1.f; t += step) {
    pt2.set( (1.f - t) * (1.f - t) * (float)anchor1.x + 2.f * (1.f - t) * t * (float)control.x + t * t * (float)anchor2.x, 
             (1.f - t) * (1.f - t) * (float)anchor1.y + 2.f * (1.f - t) * t * (float)control.y + t * t * (float)anchor2.y );
    points.push_back(pt2);
    
    pt1 = pt2;
  } 
  
  //As a final step, make sure the curve ends on the second anchor
//  points.push_back(anchor2);
  
}

void PF::Polygon::inter_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, std::vector<Point>& points)
{
  Point pt1, pt2;
  float step = 1.f/100.f;
  
  pt1 = anchor1;
  points.push_back(pt1);

  //This loops draws each step of the curve
  for (float u = 0.f; u <= 1.f; u += step) { 
    pt2.set( powf(u,3)*(anchor2.get_x()+3*(control1.get_x()-control2.get_x())-anchor1.get_x())+3*powf(u,2)*(anchor1.get_x()-2*control1.get_x()+control2.get_x())+3*u*(control1.get_x()-anchor1.get_x())+anchor1.get_x(),
    powf(u,3)*(anchor2.get_y()+3*(control1.get_y()-control2.get_y())-anchor1.get_y())+3*powf(u,2)*(anchor1.get_y()-2*control1.get_y()+control2.get_y())+3*u*(control1.get_y()-anchor1.get_y())+anchor1.get_y() );
    points.push_back(pt2);

    pt1 = pt2;
  } 
  
  //As a final step, make sure the curve ends on the second anchor
//  points.push_back(anchor2);
  
}

// return a vector with all segments points
void PF::Polygon::inter_segments(std::vector<Point>& points)
{
  if (get_points_count() > 1) {
    Point pt1, pt2;
    SegmentInfo si;
    
    for (int i = 0; i < get_segments_count(); i++) {
      get_segment(i, pt1, pt2, si);
      
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        points.push_back(pt1);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        inter_bezier3(pt1, pt2, si.get_control_pt1(), points);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        inter_bezier4(pt1, pt2, si.get_control_pt1(), si.get_control_pt2(), points);
      }
    }
    
    points.push_back(pt2);
  }
}

void PF::Polygon::add_point(Point pt)
{
  PF::Shape::add_point(pt);
  
  SegmentInfo si;
  s_info.push_back(si);
}

void PF::Polygon::insert_point(Point& pt, int _position)
{
  const int position = circ_idx(_position,get_points_count());
  
  PF::Shape::insert_point(pt, position);
  
  SegmentInfo si;
  
  std::vector<SegmentInfo>::iterator it;
  it = s_info.begin();
  it = s_info.insert( it+position, si );
}

void PF::Polygon::insert_point(Point& pt, int hit_t, int additional)
{
  if (hit_t == PF::Shape::hit_segment) {
    insert_point(pt, additional+1);
  }
  else if (hit_t == PF::Shape::hit_node) {
/*    if (additional == 0) {
      SegmentInfo& si = get_segment_info(additional);
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        set_segment_type( additional, PF::SegmentInfo::bezier3_l );
        //        default_control_point(additional, hit_control_point);
      }
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        set_segment_type( additional, PF::SegmentInfo::bezier4 );
        //        default_control_point(additional, hit_control_point);
      }
    }
    else if (additional == get_points_count()-1) {
      SegmentInfo& si = get_segment_info(additional-1);
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_l) {
        set_segment_type( additional-1, PF::SegmentInfo::bezier4 );
        //        default_control_point(additional-1, hit_control_point_end);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::line) {
        set_segment_type( additional-1, PF::SegmentInfo::bezier3_r );
        //        default_control_point(additional-1, hit_control_point);
      }
    }
    else {*/
      SegmentInfo& si = get_segment_info(additional);
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        set_segment_type( additional, PF::SegmentInfo::bezier3_l );
        //        default_control_point(additional, hit_control_point);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        set_segment_type( additional, PF::SegmentInfo::bezier4 );
        set_control_pt2( additional, get_control_pt1(additional) );
        //        default_control_point(additional, hit_control_point);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier4) {
        SegmentInfo& si_l = get_segment_info(additional-1);
        if (si_l.get_segment_type() == PF::SegmentInfo::line) {
          set_segment_type( additional-1, PF::SegmentInfo::bezier3_r );
          //          default_control_point(additional-1, hit_control_point);
        }
        else if (si_l.get_segment_type() == PF::SegmentInfo::bezier3_l) {
          set_segment_type( additional-1, PF::SegmentInfo::bezier4 );
          //          default_control_point(additional-1, hit_control_point_end);
        }
      }
//    }
  }
}

void PF::Polygon::remove_point(int _n)
{
  const int n = circ_idx(_n,get_points_count());
  
  if (get_segments_count() > get_min_segments()) {
    PF::Shape::remove_point(n);
    
    s_info.erase(s_info.begin()+n);
  }
}

void PF::Polygon::remove_point(int hit_t, int additional)
{
  if (hit_t == PF::Shape::hit_node) {
    remove_point(additional);
  }
  else if (hit_t == PF::Shape::hit_control_point_end) {
    SegmentInfo& si = get_segment_info(additional);
    if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
      std::cout<<"PF::Polygon::remove_point(): hit_control_point_end: additional: "<<additional<<std::endl;  
      si.set_segment_type( PF::SegmentInfo::bezier3_l );
    }
  }
  else if (hit_t == PF::Shape::hit_control_point) {
    SegmentInfo& si = get_segment_info(additional);
    if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
      std::cout<<"PF::Polygon::remove_point(): hit_control_point bezier4: additional: "<<additional<<std::endl;  
      si.set_control_pt1( si.get_control_pt2() );
      si.set_segment_type( PF::SegmentInfo::bezier3_r );
    }
    else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
      std::cout<<"PF::Polygon::remove_point(): hit_control_point bezier3_l/r: additional: "<<additional<<std::endl;  
      si.set_segment_type( PF::SegmentInfo::line );
    }
  }
}

void PF::Polygon::get_segment(int n, Point& pt1, Point& pt2)
{
  pt1 = get_point(n);
  pt2 = get_point(n+1);
}

void PF::Polygon::get_segment(int n, Point& pt1, Point& pt2, SegmentInfo& si)
{
  get_segment(n, pt1, pt2);
  si = get_segment_info(n);
}

void PF::Polygon::expand_falloff(int n)
{
  set_falloff(get_falloff()+n);
}

void PF::Polygon::offset_point(int _n, int x, int y)
{
  const int n = circ_idx(_n,get_points_count());
  
  PF::Shape::offset_point(n, x, y);
  
  if (get_segment_info(n).get_segment_type() == PF::SegmentInfo::bezier3_l || get_segment_info(n).get_segment_type() == PF::SegmentInfo::bezier4)
    get_segment_info(n).offset_control_pt1(x, y);
  if (get_segment_info(n-1).get_segment_type() == PF::SegmentInfo::bezier3_r)
    get_segment_info(n-1).offset_control_pt1(x, y);
  else if (get_segment_info(n-1).get_segment_type() == PF::SegmentInfo::bezier4)
    get_segment_info(n-1).offset_control_pt2(x, y);
}

void PF::Polygon::synch_control_pt1(int n, Point& pt, Point& anchor)
{
  Point synch(anchor);
  synch.offset(anchor.x-pt.x, anchor.y-pt.y);
  set_control_pt1(n, synch);
}

void PF::Polygon::synch_control_pt2(int n, Point& pt, Point& anchor)
{
  Point synch(anchor);
  synch.offset(anchor.x-pt.x, anchor.y-pt.y);
  set_control_pt2(n, synch);
}

void PF::Polygon::offset_control_pt1(int segment, Point& prev, Point& curr, int options)
{
  get_segment_info(segment).offset_control_pt1(prev, curr);

  if (options != 0) {
    int opt = 0;
    int opt_n = -1;
    Point anchor;

    if (get_segment_type(segment) == PF::SegmentInfo::bezier3_l || get_segment_type(segment) == PF::SegmentInfo::bezier4) {
      if (get_segment_type(segment-1) == PF::SegmentInfo::bezier4) {
        opt = 2;
        opt_n = segment-1;
        anchor = get_point(segment);
      }
      else if (get_segment_type(segment-1) == PF::SegmentInfo::bezier3_r) {
        opt = 1;
        opt_n = segment-1;
        anchor = get_point(segment);
      }
    }
    else if (get_segment_type(segment) == PF::SegmentInfo::bezier3_r) {
      if (get_segment_type(segment+1) == PF::SegmentInfo::bezier4) {
        opt = 1;
        opt_n = segment+1;
        anchor = get_point(segment+1);
      }
      else if (get_segment_type(segment+1) == PF::SegmentInfo::bezier3_l) {
        opt = 1;
        opt_n = segment+1;
        anchor = get_point(segment+1);
      }
    }

    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_pt1(opt_n, get_control_pt1(segment), anchor);
      else if (opt == 2)
        synch_control_pt2(opt_n, get_control_pt1(segment), anchor);
    }
    else if (options == PF::SegmentInfo::locked) {
      if (opt == 1)
        get_segment_info(opt_n).offset_control_pt1(prev, curr);
      else if (opt == 2)
        get_segment_info(opt_n).offset_control_pt2(prev, curr);
    }
  }
}

void PF::Polygon::offset_control_pt2(int segment, Point& prev, Point& curr, int options)
{
  get_segment_info(segment).offset_control_pt2(prev, curr);
  
  if (options != 0) {
    int opt = 0;
    int opt_n = -1;
    Point anchor;

    if (get_segment_type(segment+1) == PF::SegmentInfo::bezier3_l || get_segment_type(segment+1) == PF::SegmentInfo::bezier4) {
      opt = 1;
      opt_n = segment+1;
      anchor = get_point(segment+1);
    }
    
    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_pt1(opt_n, get_control_pt2(segment), anchor);
      else if (opt == 2)
        synch_control_pt2(opt_n, get_control_pt2(segment), anchor);
    }
    else if (options == PF::SegmentInfo::locked) {
      if (opt == 1)
        get_segment_info(opt_n).offset_control_pt1(prev, curr);
      else if (opt == 2)
        get_segment_info(opt_n).offset_control_pt2(prev, curr);
    }
  }
  
}

void PF::Polygon::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options)
{
  switch (hit_t)
  {
  case PF::Shape::hit_control_point:
    offset_control_pt1(additional, prev, curr, options);
    break;
  case PF::Shape::hit_control_point_end:
    offset_control_pt2(additional, prev, curr, options);
    break;
  default:
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source, options);
    break;
  }
}

bool PF::Polygon::point_in_polygon(Point& pt)
{
  std::vector<Point> points;
  inter_segments(points);
  return pt.in_polygon(points);
}

int PF::Polygon::hit_test(Point& pt, int& additional)
{
  int hit_t = hit_none;
  
  // check control points
  for ( int i = 0; i < get_segments_count(); i++ ) {
    SegmentInfo& si = get_segment_info(i);
    
    if ( si.get_segment_type() != PF::SegmentInfo::line ) {
      if ( hit_test_node(pt, si.get_control_pt1()) ) {
        additional = i;
        return hit_control_point;
      }
    }
    if ( si.get_segment_type() == PF::SegmentInfo::bezier4 ) {
      if ( hit_test_node(pt, si.get_control_pt2()) ) {
        additional = i;
        return hit_control_point_end;
      }
    }
  }

  // check the nodes
  Point pt1, pt2;
  const int delta2 = (get_pen_size()+hit_test_delta)*(get_pen_size()+hit_test_delta);

  for ( int i = 0; i < get_segments_count(); i++ ) {
    get_segment(i, pt1, pt2);
    
    if ( hit_test_node(pt, pt1) ) {
      additional = i;
      return hit_node;
    }
    if ( hit_test_node(pt, pt2) ) {
      additional = i+1;
      return hit_node;
    }
  }
    
  // check the segments
  SegmentInfo si;
  
  for ( int i = 0; i < get_segments_count(); i++ ) {
    get_segment(i, pt1, pt2, si);
    
    if ( si.get_segment_type() == PF::SegmentInfo::line ) {
      const float dist2 = pt.distance2segment2(pt1, pt2);
      if (dist2 <= delta2) {
        additional = i;
        return hit_segment;
      }
    }
    else if ( si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r ) {
      if (pt.in_bezier3(pt1, pt2, si.get_control_pt1(), delta2)) {
        additional = i;
        return hit_segment;
      }
    }
    else if ( si.get_segment_type() == PF::SegmentInfo::bezier4 ) {
      if (pt.in_bezier4(pt1, pt2, si.get_control_pt1(), si.get_control_pt2(), delta2)) {
        additional = i;
        return hit_segment;
      }
    }
  }
    
  // check shape
  if ( hit_t == hit_none ) {
    if (point_in_polygon(pt)) {
      return hit_shape;
    }
  }
  
  // check the source
  if ( get_has_source() ) {
    Polygon shape = *this;
    shape.set_has_source(false);
    shape.offset(get_source_point());
    hit_t = shape.hit_test(pt, additional);
    if ( hit_t != hit_none) hit_t = hit_source;
  }

  return hit_t;
}

void PF::Polygon::get_rect(VipsRect* rc)
{
  rc->left = rc->top = INT_MAX;
  rc->width = rc->height = INT_MIN;
  
  if (get_points_count() > 0) {
    for (int i = 0; i < get_points_count(); i++) {
      rc->left = std::min(rc->left, get_point(i).x);
      rc->top = std::min(rc->top, get_point(i).y);
      rc->width = std::max(rc->width, get_point(i).x);
      rc->height = std::max(rc->height, get_point(i).y);
    }
  
    for (int i = 0; i < get_segments_count(); i++) {
      SegmentInfo& si = get_segment_info(i);
      if (si.get_segment_type() != PF::SegmentInfo::line) {
        rc->left = std::min(rc->left, si.get_control_pt1().x);
        rc->top = std::min(rc->top, si.get_control_pt1().y);
        rc->width = std::max(rc->width, si.get_control_pt1().x);
        rc->height = std::max(rc->height, si.get_control_pt1().y);
      }
      if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        rc->left = std::min(rc->left, si.get_control_pt2().x);
        rc->top = std::min(rc->top, si.get_control_pt2().y);
        rc->width = std::max(rc->width, si.get_control_pt2().x);
        rc->height = std::max(rc->height, si.get_control_pt2().y);
      }
    }
  
    rc->width = rc->width-rc->left+1;
    rc->height = rc->height-rc->top+1;
  }
  
  rc->left -= get_pen_size();
  rc->top -= get_pen_size();
  rc->width += get_pen_size()*2;
  rc->height += get_pen_size()*2;
  
}

void PF::Polygon::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff();
  rc->top -= get_falloff();
  rc->width += get_falloff()*2;
  rc->height += get_falloff()*2;
  
}

void PF::Polygon::build_mask(SplineCurve& scurve)
{
  // free mask if already exists
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }
 
  // get bounding rect
  VipsRect rc;
  get_mask_rect(&rc);

  // nothing to return
  if (rc.width < 1 || rc.height < 1) return;
  
  // alloc mask
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  memset(mask, 0, rc.width * rc.height * sizeof(float));

  // populate the buffer
  std::vector<Point> points;
  
  inter_segments(points);
  fill_polygon(points, mask, rc);
  
}

// -----------------------------------
// Line
// -----------------------------------

void PF::Line::offset_control_pt1(int segment, Point& prev, Point& curr, int options)
{
  if (segment == 0) {
    get_segment_info(segment).offset_control_pt1(prev, curr);
  }
/*  else if (segment == get_segment_count()-1) {
    get_segment_info(segment).offset_control_pt1(prev, curr);
  }*/
  else {
    PF::Polygon::offset_control_pt1(segment, prev, curr, options);
  }
  
/*  get_segment_info(segment).offset_control_pt1(prev, curr);

  if (options != 0) {
    int opt = 0;
    int opt_n = -1;
    Point anchor;

    if (get_segment_type(segment) == PF::SegmentInfo::bezier3_l || get_segment_type(segment) == PF::SegmentInfo::bezier4) {
      if (get_segment_type(segment-1) == PF::SegmentInfo::bezier4) {
        opt = 2;
        opt_n = segment-1;
        anchor = get_point(segment);
      }
      else if (get_segment_type(segment-1) == PF::SegmentInfo::bezier3_r) {
        opt = 1;
        opt_n = segment-1;
        anchor = get_point(segment);
      }
    }
    else if (get_segment_type(segment) == PF::SegmentInfo::bezier3_r) {
      if (get_segment_type(segment+1) == PF::SegmentInfo::bezier4) {
        opt = 1;
        opt_n = segment+1;
        anchor = get_point(segment+1);
      }
      else if (get_segment_type(segment+1) == PF::SegmentInfo::bezier3_l) {
        opt = 1;
        opt_n = segment+1;
        anchor = get_point(segment+1);
      }
    }

    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_pt1(opt_n, get_control_pt1(segment), anchor);
      else if (opt == 2)
        synch_control_pt2(opt_n, get_control_pt1(segment), anchor);
    }
    else if (options == PF::SegmentInfo::locked) {
      if (opt == 1)
        get_segment_info(opt_n).offset_control_pt1(prev, curr);
      else if (opt == 2)
        get_segment_info(opt_n).offset_control_pt2(prev, curr);
    }
  }*/
}

void PF::Line::offset_control_pt2(int segment, Point& prev, Point& curr, int options)
{
  if (segment == get_segments_count()-1) {
    get_segment_info(segment).offset_control_pt2(prev, curr);
  }
  else {
    PF::Polygon::offset_control_pt2(segment, prev, curr, options);
  }
  
/*  get_segment_info(segment).offset_control_pt2(prev, curr);
  
  if (options != 0) {
    int opt = 0;
    int opt_n = -1;
    Point anchor;

    if (get_segment_type(segment+1) == PF::SegmentInfo::bezier3_l || get_segment_type(segment+1) == PF::SegmentInfo::bezier4) {
      opt = 1;
      opt_n = segment+1;
      anchor = get_point(segment+1);
    }
    
    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_pt1(opt_n, get_control_pt2(segment), anchor);
      else if (opt == 2)
        synch_control_pt2(opt_n, get_control_pt2(segment), anchor);
    }
    else if (options == PF::SegmentInfo::locked) {
      if (opt == 1)
        get_segment_info(opt_n).offset_control_pt1(prev, curr);
      else if (opt == 2)
        get_segment_info(opt_n).offset_control_pt2(prev, curr);
    }
  }
  */
}

void PF::Line::offset_point(int n, int x, int y)
{
  if (n == 0) {
    PF::Shape::offset_point(n, x, y);
    
    if (get_segment_info(n).get_segment_type() == PF::SegmentInfo::bezier3_l || get_segment_info(n).get_segment_type() == PF::SegmentInfo::bezier4)
      get_segment_info(n).offset_control_pt1(x, y);
  }
  else if (n == get_points_count()-1) {
    PF::Shape::offset_point(n, x, y);
    
    if (get_segment_info(n-1).get_segment_type() == PF::SegmentInfo::bezier3_r)
      get_segment_info(n-1).offset_control_pt1(x, y);
    else if (get_segment_info(n-1).get_segment_type() == PF::SegmentInfo::bezier4)
      get_segment_info(n-1).offset_control_pt2(x, y);
  }
  else {
    PF::Polygon::offset_point(n, x, y);
  }
}
/*
int PF::Line::hit_test(Point& pt, int& additional)
{
  int hit_t = hit_none;
  
  // check control points
  for ( int i = 0; i < get_segments_count(); i++ ) {
    SegmentInfo& si = get_segment_info(i);
    
    if ( si.get_segment_type() != PF::SegmentInfo::line ) {
      if ( hit_test_node(pt, si.get_control_pt1()) ) {
        additional = i;
        return hit_control_point;
      }
    }
    if ( si.get_segment_type() == PF::SegmentInfo::bezier4 ) {
      if ( hit_test_node(pt, si.get_control_pt2()) ) {
        additional = i;
        return hit_control_point_end;
      }
    }
  }

  // check the nodes
  Point pt1, pt2;
  const int delta2 = (get_size()+hit_test_delta)*(get_size()+hit_test_delta);

  for ( int i = 0; i < get_segments_count(); i++ ) {
    get_segment(i, pt1, pt2);
    
    if ( hit_test_node(pt, pt1) ) {
      additional = i;
      return hit_node;
    }
    if ( hit_test_node(pt, pt2) ) {
      additional = i+1;
      return hit_node;
    }
  }
    
  // check the segments
  SegmentInfo si;
  
  for ( int i = 0; i < get_segments_count(); i++ ) {
    get_segment(i, pt1, pt2, si);
    
    if ( si.get_segment_type() == PF::SegmentInfo::line ) {
      const float dist2 = pt.distance2segment2(pt1, pt2);
      if (dist2 <= delta2) {
        additional = i;
        return hit_segment;
      }
    }
    else if ( si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r ) {
      if (pt.in_bezier3(pt1, pt2, si.get_control_pt1(), delta2)) {
        additional = i;
        return hit_segment;
      }
    }
    else if ( si.get_segment_type() == PF::SegmentInfo::bezier4 ) {
      if (pt.in_bezier4(pt1, pt2, si.get_control_pt1(), si.get_control_pt2(), delta2)) {
        additional = i;
        return hit_segment;
      }
    }
  }
    
  // check shape
  if ( hit_t == hit_none ) {
    if (point_in_polygon(pt)) {
      return hit_shape;
    }
  }
  
  // check the source
  if ( get_has_source() ) {
    Polygon shape = *this;
    shape.set_has_source(false);
    shape.offset(get_source_point());
    hit_t = shape.hit_test(pt, additional);
    if ( hit_t != hit_none) hit_t = hit_source;
  }

  return hit_t;
}
*/

void PF::Line::build_mask(SplineCurve& scurve)
{
//  std::cout<<"PF::Line::build_mask()"<<std::endl;
  
  // free mask if already exists
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }
 
  // if no falloff there's no point in returning a mask
  if (get_falloff()+get_pen_size() <= 0) return;
  
  // get bounding rect
  VipsRect rc;
  get_mask_rect(&rc);

  // nothing to return
  if (rc.width < 1 || rc.height < 1) return;
  
  // save offset
  Point pt_offset(-rc.left, -rc.top);
  
  // offset to (0, 0)
//  rc.left = rc.top = 0;
  
  // alloc mask
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  memset(mask, 0, rc.width * rc.height * sizeof(float));

  // populate the buffer
  Point pt;
  Point pt1;
  Point pt2;
  VipsRect rcs;
  const float borderf2 = (get_pen_size()+get_falloff()) * (get_pen_size()+get_falloff()); // square distance to the border (falloff)
  const float border2 = get_pen_size() * get_pen_size(); // square distance to the border (shape)
  std::vector<Point> points;
  
  inter_segments(points);
//  PF::Line::fill_polygon(points, mask, rc);
  
  // go through all segments
  for (int i=1; i < points.size(); i++) {
    pt1 = points[i-1];
    pt1.offset(pt_offset);
    pt2 = points[i];
    pt2.offset(pt_offset);

    // get segment bounding rect
    get_segment_bounding_rect(pt1, pt2, get_pen_size()+get_falloff(), &rcs);
       
    // get the falloff for each point
    for(int y = rcs.top; y <  rcs.top+rcs.height; y++) {
      float* pmask = mask + y * rc.width;
      for(int x = rcs.left; x < rcs.left+rcs.width; x++) {
        pt.set(x, y);
        
        // square distance to the segment
        const float dist2 = pt.distance2segment2(pt1, pt2);
        if (dist2 < border2) {
          pmask[x] = std::max(pmask[x], get_opacity());
        }
        else if (dist2 < borderf2) {
          const float f = (borderf2 - dist2) / (borderf2-border2);
          const float f2 = f * f * get_opacity();
          const float f2s = scurve.get_value(f2);
          pmask[x] = std::max(pmask[x], f2s);
        }
      }
    }
  }

}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
#if 0

void PF::Line::scale(int scale)
{
  PF::Shape::scale(scale);
  
  for (int i = 0; i < get_segments_count(); i++) {
    get_segment_info(i).scale(scale);
  }

}

void PF::Line::fill_polygon(std::vector<Point>& points, float* buffer, VipsRect& rc)
{
  int  nodes, /*nodeX[MAX_POLY_CORNERS],*/ pixelX, pixelY, i, j, swap ;
  int polyCorners = points.size();
  int* nodeX = (int*)malloc(polyCorners * sizeof(int));
  
/*  int IMAGE_TOP = rc_s.top-rc.top;
  int IMAGE_LEFT = rc_s.left-rc.left;
  int IMAGE_BOT = IMAGE_TOP + rc_s.height;
  int IMAGE_RIGHT = IMAGE_LEFT + rc_s.width;
  */
  int IMAGE_TOP = rc.top;
  int IMAGE_LEFT = rc.left;
  int IMAGE_BOT = IMAGE_TOP + rc.height;
  int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  
//  float* polyY = (float*)malloc(polyCorners * sizeof(float));
  
  //  Loop through the rows of the image.
  for (pixelY=IMAGE_TOP; pixelY<IMAGE_BOT; pixelY++) {

    //  Build a list of nodes.
    nodes=0;
    j=polyCorners-1;
    for (i=0; i<polyCorners; i++) {
      if ( (points[i].y<(double) pixelY && points[j].y>=(double) pixelY)
      ||  (points[j].y<(double) pixelY && points[i].y>=(double) pixelY) ) {
        nodeX[nodes++]=(int) ( points[i].x + ((float)(pixelY-points[i].y) / (float)(points[j].y-points[i].y)) 
        * (float)(points[j].x-points[i].x) ); 
      }
      j=i; 
    }

    //  Sort the nodes, via a simple “Bubble” sort.
    i=0;
    while (i<nodes-1) {
      if (nodeX[i]>nodeX[i+1]) {
        swap=nodeX[i]; nodeX[i]=nodeX[i+1]; nodeX[i+1]=swap; if (i) i--; 
      }
      else {
        i++; 
      }
    }

    //  Fill the pixels between node pairs.
    for (i=0; i<nodes; i+=2) {
      if   (nodeX[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX[i+1]> IMAGE_LEFT ) {
        if (nodeX[i  ]< IMAGE_LEFT ) nodeX[i  ]=IMAGE_LEFT ;
        if (nodeX[i+1]> IMAGE_RIGHT) nodeX[i+1]=IMAGE_RIGHT;
        for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) {
//          fillPixel(pixelX,pixelY);
          buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = 1.f;
        }
      }
    }
  }

  if (nodeX) free(nodeX);
}

// returns a vector with bezier curve points
void PF::Line::inter_bezier3(Point& anchor1, Point& anchor2, Point& control, std::vector<Point>& points)
{
  Point pt1, pt2;
  float step = 1.f/100.f;
  
  pt1 = anchor1;
  points.push_back(pt1);
  
  //This loops draws each step of the curve
  for (float t = 0.f; t <= 1.f; t += step) {
    pt2.set( (1.f - t) * (1.f - t) * (float)anchor1.x + 2.f * (1.f - t) * t * (float)control.x + t * t * (float)anchor2.x, 
             (1.f - t) * (1.f - t) * (float)anchor1.y + 2.f * (1.f - t) * t * (float)control.y + t * t * (float)anchor2.y );
    points.push_back(pt2);
    
    pt1 = pt2;
  } 
  
  //As a final step, make sure the curve ends on the second anchor
//  points.push_back(anchor2);
  
}

void PF::Line::inter_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, std::vector<Point>& points)
{
  Point pt1, pt2;
  float step = 1.f/100.f;
  
  pt1 = anchor1;
  points.push_back(pt1);

  //This loops draws each step of the curve
  for (float u = 0.f; u <= 1.f; u += step) { 
    pt2.set( powf(u,3)*(anchor2.get_x()+3*(control1.get_x()-control2.get_x())-anchor1.get_x())+3*powf(u,2)*(anchor1.get_x()-2*control1.get_x()+control2.get_x())+3*u*(control1.get_x()-anchor1.get_x())+anchor1.get_x(),
    powf(u,3)*(anchor2.get_y()+3*(control1.get_y()-control2.get_y())-anchor1.get_y())+3*powf(u,2)*(anchor1.get_y()-2*control1.get_y()+control2.get_y())+3*u*(control1.get_y()-anchor1.get_y())+anchor1.get_y() );
    points.push_back(pt2);

    pt1 = pt2;
  } 
  
  //As a final step, make sure the curve ends on the second anchor
//  points.push_back(anchor2);
  
}

void PF::Line::polygon_from_line(Point& pt1, Point& pt2, std::vector<Point>& points1, std::vector<Point>& points2)
{
  Point a, b, c, d;
  const int amount = get_size();
  
  if (pt1 != pt2) {
  get_expanded_rec_from_line(pt1, pt2, amount, a, b, c, d);
  
  points1.push_back(a);
  points1.push_back(b);
  points2.push_back(d);
  points2.push_back(c);
  }
  else {
//    points1.push_back(pt1);
//    points2.push_back(pt1);
  }
  
}

void PF::Line::polygon_from_bezier3(Point& anchor1, Point& anchor2, Point& control, std::vector<Point>& points1, std::vector<Point>& points2)
{
  Point a, b, c, d, pt1, pt2;
  const int amount = get_size();
  std::vector<Point> points;
  
  inter_bezier3(anchor1, anchor2, control, points);
  
  pt1 = points[0];
  for (int i = 1; i < points.size(); i++) {
    pt2 = points[i];
    
    if (pt1 != pt2) {
    get_expanded_rec_from_line(pt1, pt2, amount, a, b, c, d);
    
    points1.push_back(a);
    points1.push_back(b);
    points2.push_back(d);
    points2.push_back(c);
    }
    else {
//      points1.push_back(pt1);
//      points2.push_back(pt1);
    }
    
    pt1 = pt2;
  }
  
}

void PF::Line::polygon_from_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, std::vector<Point>& points1, std::vector<Point>& points2)
{
  Point a, b, c, d, pt1, pt2;
  const int amount = get_size();
  std::vector<Point> points;
  
  inter_bezier4(anchor1, anchor2, control1, control2, points);
  
  pt1 = points[0];
  for (int i = 1; i < points.size(); i++) {
    pt2 = points[i];
    
    if (pt1 != pt2) {
    get_expanded_rec_from_line(pt1, pt2, amount, a, b, c, d);
    
    points1.push_back(a);
    points1.push_back(b);
    points2.push_back(d);
    points2.push_back(c);
    }
    else {
//      points1.push_back(pt1);
//      points2.push_back(pt1);
    }
    
    pt1 = pt2;
  }
  
}

// return a vector with a polygon representing the segments
void PF::Line::polygon_from_segment(std::vector<Point>& points)
{
  std::cout<<"PF::Line::polygon_from_segment()"<<std::endl;
  
  std::vector<Point> points_l;
  std::vector<Point> points_r;
  
  if (get_points_count() > 1) {
    SegmentInfo si;
    Point pt1, pt2, pt3;
    Point a1, b1, c1, d1;
    Point a2, b2;
    const int amount = get_size();
  
    std::cout<<"PF::Line::polygon_from_segment() 1"<<std::endl;
    
    // get an array of points for all the segments
    for (int i = 0; i < get_segments_count(); i++) {
      get_segment(i, pt1, pt2, si);
      
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        points_r.push_back(pt1);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        inter_bezier3(pt1, pt2, si.get_control_pt1(), points_r);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        inter_bezier4(pt1, pt2, si.get_control_pt1(), si.get_control_pt2(), points_r);
      }      
    }
    points_r.push_back(pt2);
    
    std::cout<<"PF::Line::polygon_from_segment() 2"<<std::endl;
    
    // make the corners
    pt1 = points_r[0];
    pt2 = points_r[1];
    get_pt1_proyected_from_segment(pt1, pt2, amount, a1, b1);
    points.push_back(a1);
    points_l.push_back(b1);
    for (int i = 2; i < points_r.size(); i++) {
      pt3 = points_r[i];
      
      if (pt1 != pt2 && pt2 != pt3) {
        get_pt2_proyected_from_segment(pt1, pt2, amount, a2, b2);
        get_pt1_proyected_from_segment(pt2, pt3, amount, c1, d1);
//        std::swap(a2.x, c1.x); std::swap(a2.y, c1.y);
//        std::swap(b2.x, d1.x); std::swap(b2.y, d1.y);
        get_intersect_point(a1, a2, c1, c1);
        get_intersect_point(b1, b2, d1, d1);
        
        a1 = c1;
        b1 = d1;
        
        points.push_back(a1);
        points_l.push_back(b1);
      }
      
      pt1 = pt2;
      pt2 = pt3;
    }
    get_pt2_proyected_from_segment(pt1, pt2, amount, a1, b1);
    points.push_back(a1);
    points_l.push_back(b1);
  }
  
  std::cout<<"PF::Line::polygon_from_segment() 3"<<std::endl;
  
  for (int i = points_l.size()-1; i >= 0; i--) points.push_back(points_l[i]);
  
  std::cout<<"PF::Line::polygon_from_segment() 4"<<std::endl;
}

/*
void PF::Line::polygon_from_segment(std::vector<Point>& points)
{
  std::vector<Point> points_r;
  
  if (get_points_count() > 1) {
    SegmentInfo si;
    Point pt1, pt2;
    
    for (int i = 0; i < get_segments_count(); i++) {
      get_segment(i, pt1, pt2, si);
      
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        polygon_from_line(pt1, pt2, points, points_r);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        polygon_from_bezier3(pt1, pt2, si.get_control_pt1(), points, points_r);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        polygon_from_bezier4(pt1, pt2, si.get_control_pt1(), si.get_control_pt2(), points, points_r);
      }      
    }
  }
  
  for (int i = points_r.size()-1; i >= 0; i--) points.push_back(points_r[i]);

}
*/
// return a vector with all segments points
void PF::Line::inter_segments(std::vector<Point>& points)
{
  if (get_points_count() > 1) {
    Point pt1, pt2;
    SegmentInfo si;
    
    for (int i = 0; i < get_points_count(); i++) {
      get_segment(i, pt1, pt2, si);
      
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        points.push_back(pt1);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        inter_bezier3(pt1, pt2, si.get_control_pt1(), points);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        inter_bezier4(pt1, pt2, si.get_control_pt1(), si.get_control_pt2(), points);
      }
    }
    
    points.push_back(pt2);
  }
}

void PF::Line::default_control_point(int segment, int hit_t)
{  
  Point pt1, pt2, center, ptc;
  SegmentInfo& si = get_segment_info(segment);
  SegmentInfo si_adj;
  int segment_adj = -1;
  
  get_segment(segment, pt1, pt2);
  get_center(center);

  if ( (segment > 0 || hit_t == PF::Shape::hit_control_point_end) && (segment < get_segments_count()-1 || hit_t == PF::Shape::hit_control_point) ) {
    if (hit_t == PF::Shape::hit_control_point) {
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        si_adj = get_segment_info(segment+1);
        if (si_adj.get_segment_type() == PF::SegmentInfo::bezier3_l || si_adj.get_segment_type() == PF::SegmentInfo::bezier4) {
          ptc.set( pt2.x-(si_adj.get_control_pt1().x-pt2.x), pt2.y+(pt2.y-si_adj.get_control_pt1().y) );
          segment_adj = segment-1;
        }
      }
      else {
        si_adj = get_segment_info(segment-1);
        if (si_adj.get_segment_type() == PF::SegmentInfo::bezier4) {
          ptc.set( pt1.x-(si_adj.get_control_pt2().x-pt1.x), pt1.y+(pt1.y-si_adj.get_control_pt2().y) );
          segment_adj = segment-1;
        }
      }
    }
    else {
      si_adj = get_segment_info(segment+1);
      if (si_adj.get_segment_type() != PF::SegmentInfo::line) {
        if (si.get_segment_type() == PF::SegmentInfo::bezier3_r)
          ptc.set( pt1.x-(si_adj.get_control_pt1().x-pt1.x), pt1.y+(pt1.y-si_adj.get_control_pt1().y) );
        else
          ptc.set( pt2.x-(si_adj.get_control_pt1().x-pt2.x), pt2.y+(pt2.y-si_adj.get_control_pt1().y) );
        segment_adj = segment+1;
      }
    }
  }
  
  if ( segment_adj < 0 ) {
    ptc.set( std::min(pt1.x, pt2.x) + abs(pt1.x - pt2.x)/2, std::min(pt1.y, pt2.y) + abs(pt1.y - pt2.y)/2 );
    int amount = -50;
    if (ptc != pt2)
      get_pt_proyected_from_segment(ptc, pt2, amount, ptc);
    else
      ptc.offset(amount, amount);
  }
  
  if (hit_t == PF::Shape::hit_control_point) {
    set_control_pt1(segment, ptc);
  }
  else if (hit_t == PF::Shape::hit_control_point_end) {
    set_control_pt2(segment, ptc);
  }
}

void PF::Line::add_point(Point pt)
{
  PF::Shape::add_point(pt);
  
  SegmentInfo si;
  s_info.push_back(si);
}

void PF::Line::insert_point(Point& pt, int position)
{
  PF::Shape::insert_point(pt, position);
  
  SegmentInfo si;
  
  std::vector<SegmentInfo>::iterator it;
  it = s_info.begin();
  it = s_info.insert( it+position, si );
}

void PF::Line::insert_point(Point& pt, int hit_t, int additional)
{
  if (hit_t == PF::Shape::hit_segment) {
    insert_point(pt, additional+1);
  }
  else if (hit_t == PF::Shape::hit_node) {
    if (additional == 0) {
      SegmentInfo& si = get_segment_info(additional);
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        set_segment_type( additional, PF::SegmentInfo::bezier3_l );
        default_control_point(additional, hit_control_point);
      }
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        set_segment_type( additional, PF::SegmentInfo::bezier4 );
        default_control_point(additional, hit_control_point);
      }
    }
    else if (additional == get_points_count()-1) {
      SegmentInfo& si = get_segment_info(additional-1);
      if (si.get_segment_type() == PF::SegmentInfo::bezier3_l /*|| si.get_segment_type() == PF::SegmentInfo::bezier3_r*/) {
        set_segment_type( additional-1, PF::SegmentInfo::bezier4 );
        default_control_point(additional-1, hit_control_point_end);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::line) {
        set_segment_type( additional-1, PF::SegmentInfo::bezier3_r );
        default_control_point(additional-1, hit_control_point);
      }
    }
    else {
      SegmentInfo& si = get_segment_info(additional);
      if (si.get_segment_type() == PF::SegmentInfo::line) {
        set_segment_type( additional, PF::SegmentInfo::bezier3_l );
        default_control_point(additional, hit_control_point);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        set_segment_type( additional, PF::SegmentInfo::bezier4 );
        set_control_pt2( additional, get_control_pt1(additional) );
        default_control_point(additional, hit_control_point);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier4) {
        SegmentInfo& si_l = get_segment_info(additional-1);
        if (si_l.get_segment_type() == PF::SegmentInfo::line) {
          set_segment_type( additional-1, PF::SegmentInfo::bezier3_r );
          default_control_point(additional-1, hit_control_point);
        }
        else if (si_l.get_segment_type() == PF::SegmentInfo::bezier3_l) {
          set_segment_type( additional-1, PF::SegmentInfo::bezier4 );
          default_control_point(additional-1, hit_control_point_end);
        }
    }
    }
  }
}

void PF::Line::remove_point(int n)
{
  if (get_segments_count() > 1) {
    PF::Shape::remove_point(n);
    
    s_info.erase(s_info.begin()+n);
  }
}

void PF::Line::remove_point(int hit_t, int additional)
{
  if (hit_t == PF::Shape::hit_node) {
    remove_point(additional);
  }
  else if (hit_t == PF::Shape::hit_control_point_end) {
    SegmentInfo& si = get_segment_info(additional);
    if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
      si.set_segment_type( PF::SegmentInfo::bezier3_l );
    }
  }
  else if (hit_t == PF::Shape::hit_control_point) {
    SegmentInfo& si = get_segment_info(additional);
    if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
      si.set_control_pt1( si.get_control_pt2() );
      si.set_segment_type( PF::SegmentInfo::bezier3_r );
    }
    else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
      si.set_segment_type( PF::SegmentInfo::line );
    }
  }
}

void PF::Line::get_segment(int n, Point& pt1, Point& pt2)
{
  pt1 = get_point(n);
  pt2 = get_point(n+1);
}

void PF::Line::get_segment(int n, Point& pt1, Point& pt2, SegmentInfo& si)
{
  get_segment(n, pt1, pt2);
  si = get_segment_info(n);
}

void PF::Line::expand_falloff(int n)
{
  set_falloff(get_falloff()+n);
}

void PF::Line::offset_point(int n, int x, int y)
{
  PF::Shape::offset_point(n, x, y);
  
  if (n < get_points_count()-1)
    get_segment_info(n).offset_control_pt1(x, y);
  if (n > 0)
    get_segment_info(n-1).offset_control_pt2(x, y);
}

void PF::Line::synch_control_pt1(int n, Point& pt, Point& anchor)
{
  Point synch(anchor);
  synch.offset(anchor.x-pt.x, anchor.y-pt.y);
  set_control_pt1(n, synch);
}

void PF::Line::synch_control_pt2(int n, Point& pt, Point& anchor)
{
  Point synch(anchor);
  synch.offset(anchor.x-pt.x, anchor.y-pt.y);
  set_control_pt2(n, synch);
}

void PF::Line::offset_control_pt1(int segment, Point& prev, Point& curr, int options)
{
  get_segment_info(segment).offset_control_pt1(prev, curr);

  if (options != 0) {
    int opt = 0;
    int opt_n = -1;
    Point anchor;

    if (segment == 0) {
    }
    else if (segment == get_segments_count()-1) {
      if (get_segment_type(segment) == PF::SegmentInfo::bezier3_l || get_segment_type(segment) == PF::SegmentInfo::bezier4) {
        if (get_segment_type(segment-1) == PF::SegmentInfo::bezier4) {
          opt = 2;
          opt_n = segment-1;
          anchor = get_point(segment);
        }
        else if (get_segment_type(segment-1) == PF::SegmentInfo::bezier3_r) {
          opt = 1;
          opt_n = segment-1;
          anchor = get_point(segment);
        }
      }
    }
    else {
      if (get_segment_type(segment) == PF::SegmentInfo::bezier3_l || get_segment_type(segment) == PF::SegmentInfo::bezier4) {
        if (get_segment_type(segment-1) == PF::SegmentInfo::bezier4) {
          opt = 2;
          opt_n = segment-1;
          anchor = get_point(segment);
        }
        else if (get_segment_type(segment-1) == PF::SegmentInfo::bezier3_r) {
          opt = 1;
          opt_n = segment-1;
          anchor = get_point(segment);
        }
      }
      else if (get_segment_type(segment) == PF::SegmentInfo::bezier3_r) {
        if (get_segment_type(segment+1) == PF::SegmentInfo::bezier4) {
          opt = 1;
          opt_n = segment+1;
          anchor = get_point(segment+1);
        }
        else if (get_segment_type(segment+1) == PF::SegmentInfo::bezier3_l) {
          opt = 1;
          opt_n = segment+1;
          anchor = get_point(segment+1);
        }
      }
    }

    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_pt1(opt_n, get_control_pt1(segment), anchor);
      else if (opt == 2)
        synch_control_pt2(opt_n, get_control_pt1(segment), anchor);
    }
    else if (options == PF::SegmentInfo::locked) {
      if (opt == 1)
        get_segment_info(opt_n).offset_control_pt1(prev, curr);
      else if (opt == 2)
        get_segment_info(opt_n).offset_control_pt2(prev, curr);
    }
  }
}

void PF::Line::offset_control_pt2(int segment, Point& prev, Point& curr, int options)
{
  get_segment_info(segment).offset_control_pt2(prev, curr);
  
  if (options != 0) {
    int opt = 0;
    int opt_n = -1;
    Point anchor;

    if (segment < get_segments_count()-1) {
      if (get_segment_type(segment+1) == PF::SegmentInfo::bezier3_l || get_segment_type(segment+1) == PF::SegmentInfo::bezier4) {
        opt = 1;
        opt_n = segment+1;
        anchor = get_point(segment+1);
      }
    }
    
    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_pt1(opt_n, get_control_pt2(segment), anchor);
      else if (opt == 2)
        synch_control_pt2(opt_n, get_control_pt2(segment), anchor);
    }
    else if (options == PF::SegmentInfo::locked) {
      if (opt == 1)
        get_segment_info(opt_n).offset_control_pt1(prev, curr);
      else if (opt == 2)
        get_segment_info(opt_n).offset_control_pt2(prev, curr);
    }
  }
  
}

void PF::Line::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options)
{
  switch (hit_t)
  {
  case PF::Shape::hit_control_point:
    offset_control_pt1(additional, prev, curr, options);
    break;
  case PF::Shape::hit_control_point_end:
    offset_control_pt2(additional, prev, curr, options);
    break;
  default:
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source, options);
    break;
  }
}

int PF::Line::hit_test(Point& pt, int& additional)
{
  int hit_t = hit_none;
  
  // check control points
  for ( int i = 0; i < get_segments_count(); i++ ) {
    SegmentInfo& si = get_segment_info(i);
    
    if ( si.get_segment_type() != PF::SegmentInfo::line ) {
      if ( hit_test_node(pt, si.get_control_pt1()) ) {
        additional = i;
        return hit_control_point;
      }
    }
    if ( si.get_segment_type() == PF::SegmentInfo::bezier4 ) {
      if ( hit_test_node(pt, si.get_control_pt2()) ) {
        additional = i;
        return hit_control_point_end;
      }
    }
  }

  // check the nodes
  Point pt1, pt2;
  const int delta2 = (get_size()+hit_test_delta)*(get_size()+hit_test_delta);

  for ( int i = 0; i < get_segments_count(); i++ ) {
    get_segment(i, pt1, pt2);
    
    if ( hit_test_node(pt, pt1) ) {
      additional = i;
      return hit_node;
    }
    if ( hit_test_node(pt, pt2) ) {
      additional = i+1;
      return hit_node;
    }
  }
    
  // check the segments
  SegmentInfo si;
  
  for ( int i = 0; i < get_segments_count(); i++ ) {
    get_segment(i, pt1, pt2, si);
    
    if ( si.get_segment_type() == PF::SegmentInfo::line ) {
      const float dist2 = pt.distance2segment2(pt1, pt2);
      if (dist2 <= delta2) {
        additional = i;
        return hit_segment;
      }
    }
    else if ( si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r ) {
      if (pt.in_bezier3(pt1, pt2, si.get_control_pt1(), delta2)) {
        additional = i;
        return hit_segment;
      }
    }
    else if ( si.get_segment_type() == PF::SegmentInfo::bezier4 ) {
      if (pt.in_bezier4(pt1, pt2, si.get_control_pt1(), si.get_control_pt2(), delta2)) {
        additional = i;
        return hit_segment;
      }
    }
  }
    
  // check the source
  if ( get_has_source() ) {
    Line shape = *this;
    shape.set_has_source(false);
    shape.offset(get_source_point());
    hit_t = shape.hit_test(pt, additional);
    if ( hit_t != hit_none) hit_t = hit_source;
  }

  return hit_t;
}

void PF::Line::get_rect(VipsRect* rc)
{
  rc->left = rc->top = INT_MAX;
  rc->width = rc->height = INT_MIN;
  
  if (get_points_count() > 0) {
    for (int i = 0; i < get_points_count(); i++) {
      rc->left = std::min(rc->left, get_point(i).x);
      rc->top = std::min(rc->top, get_point(i).y);
      rc->width = std::max(rc->width, get_point(i).x);
      rc->height = std::max(rc->height, get_point(i).y);
    }
  
    for (int i = 0; i < get_segments_count(); i++) {
      SegmentInfo& si = get_segment_info(i);
      if (si.get_segment_type() != PF::SegmentInfo::line) {
        rc->left = std::min(rc->left, si.get_control_pt1().x);
        rc->top = std::min(rc->top, si.get_control_pt1().y);
        rc->width = std::max(rc->width, si.get_control_pt1().x);
        rc->height = std::max(rc->height, si.get_control_pt1().y);
      }
      if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        rc->left = std::min(rc->left, si.get_control_pt2().x);
        rc->top = std::min(rc->top, si.get_control_pt2().y);
        rc->width = std::max(rc->width, si.get_control_pt2().x);
        rc->height = std::max(rc->height, si.get_control_pt2().y);
      }
    }
  
    rc->width = rc->width-rc->left+1;
    rc->height = rc->height-rc->top+1;
  }
  
  rc->left -= get_size();
  rc->top -= get_size();
  rc->width += get_size()*2;
  rc->height += get_size()*2;
  
//  std::cout<<"PF::Line::get_rect(): rc->left: "<<rc->left<<" ,rc->top; "<<rc->top<<" ,rc->width; "<<rc->width<<" ,rc->height; "<<rc->height<<std::endl;
}

void PF::Line::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff();
  rc->top -= get_falloff();
  rc->width += get_falloff()*2;
  rc->height += get_falloff()*2;
  
//  std::cout<<"PF::Line::get_falloff_rect(): rc->left: "<<rc->left<<" ,rc->top; "<<rc->top<<" ,rc->width; "<<rc->width<<" ,rc->height; "<<rc->height<<std::endl;
}

void PF::Line::build_mask(SplineCurve& scurve)
{
//  std::cout<<"PF::Line::build_mask()"<<std::endl;
  
  // free mask if already exists
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }
 
  // if no falloff there's no point in returning a mask
  if (get_falloff()+get_size() <= 0) return;
  
  // get bounding rect
  VipsRect rc;
  get_falloff_rect(&rc);

  // nothing to return
  if (rc.width < 1 || rc.height < 1) return;
  
  // save offset
  Point pt_offset(-rc.left, -rc.top);
  
  // offset to (0, 0)
//  rc.left = rc.top = 0;
  
  // alloc mask
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  memset(mask, 0, rc.width * rc.height * sizeof(float));

  // populate the buffer
  Point pt;
  Point pt1;
  Point pt2;
  VipsRect rcs;
  const float borderf2 = (get_size()+get_falloff()) * (get_size()+get_falloff()); // square distance to the border (falloff)
  const float border2 = get_size() * get_size(); // square distance to the border (shape)
  std::vector<Point> points;
  
  polygon_from_segment(points);
//  PF::Line::fill_polygon(points, mask, rc);
  
  // go through all segments
/*  for (int i=0; i < get_points_count()-1; i++) {
    pt1 = get_point(i);
    pt1.offset(pt_offset);
    pt2 = get_point(i+1);
    pt2.offset(pt_offset);

    // get segment bounding rect
    get_segment_bounding_rect(pt1, pt2, get_size()+get_falloff(), &rcs);
       
    // get the falloff for each point
    for(int y = rcs.top; y <  rcs.top+rcs.height; y++) {
      float* pmask = mask + y * rc.width;
      for(int x = rcs.left; x < rcs.left+rcs.width; x++) {
        pt.set(x, y);
        
        // square distance to the segment
        const float dist2 = pt.distance2segment2(pt1, pt2);
        if (dist2 < border2) {
          pmask[x] = std::max(pmask[x], get_opacity());
        }
        else if (dist2 < borderf2) {
          const float f = (borderf2 - dist2) / (borderf2-border2);
          const float f2 = f * f * get_opacity();
          const float f2s = scurve.get_value(f2);
          pmask[x] = std::max(pmask[x], f2s);
        }
      }
    }
  }
*/
}


#endif
//----------------------------------------------------------------

void PF::Shape::fill_falloff_rectangle(float* mask, VipsRect& rc, SplineCurve& scurve, Point& pt1, Point& pt2, Point& pt3, Point& pt4)
{
  const int x_from = std::min(pt1.get_x(), pt4.get_x());
  const int y_from = std::min(pt1.get_y(), pt2.get_y());
  const int x_to = std::max(pt2.get_x(), pt3.get_x());
  const int y_to = std::max(pt3.get_y(), pt4.get_y());
  
  for(int y = y_from; y < y_to; y++) { 
    float* pmask = mask + ((y - rc.top) * rc.width);
    for(int x = x_from; x < x_to; x++) {
      Point pt(x, y);
      if ( point_in_rectangle(pt, pt1, pt2, pt3, pt4) ) {
        Point pt_43;
        Point pt_12( x, pt2.get_y() );
        
        get_intersect_point(pt4, pt3, pt, pt_12);
        get_intersect_point(pt1, pt2, pt_12, pt_43);
        
        const float dist_t2 = pt_43.distance2(pt_12); 
        const float dist2 = pt.distance2(pt_12);
        const float f = (dist_t2-dist2) / dist_t2; 
        const float f2 = f * f * get_opacity(); 
        pmask[x-rc.left] = scurve.get_value(f2);
      }
    } 
  } 
}

void PF::Shape::fill_falloff_ellipse_square(float* mask, VipsRect& rc, SplineCurve& scurve, 
    Point& pt1, Point& pt2, Point& pt3, Point& pt4)
{
  const int x_from = std::min(pt1.get_x(), std::min(pt2.get_x(), std::min(pt3.get_x(), pt4.get_x())));
  const int y_from = std::min(pt1.get_y(), std::min(pt2.get_y(), std::min(pt3.get_y(), pt4.get_y())));
  const int x_to = std::max(pt1.get_x(), std::max(pt2.get_x(), std::max(pt3.get_x(), pt4.get_x())));
  const int y_to = std::max(pt1.get_y(), std::max(pt2.get_y(), std::max(pt3.get_y(), pt4.get_y())));
//  const int v = y_to - y_from;
//  const int h = x_to - x_from;
  const int h = (pt1.get_y()-pt2.get_y());
  const int w = (pt1.get_x()-pt4.get_x());
  
//  const int rx2 = (x_to-x_from)*(x_to-x_from); 
//  const int ry2 = (y_to-y_from)*(y_to-y_from); 
  const int rx2 = w*w; 
  const int ry2 = h*h; 
  for(int y = y_from; y < y_to; y++) { 
    float* pmask = mask + ((y - rc.top) * rc.width); 
    for(int x = x_from; x < x_to; x++) {
      Point pt(x, y);
      if ( point_in_rectangle(pt, pt1, pt2, pt3, pt4) ) {
      const float f = point_in_ellipse(x, y, pt1.get_x(), pt1.get_y(), rx2, ry2); 
        if (f>0.f && f <= 1.f) { 
          const float f2 = (1.f-f) * (1.f-f) * get_opacity(); 
          pmask[x-rc.left] = scurve.get_value(f2); 
        }
      }
    } 
  } 

}

void PF::Shape::fill_falloff_paralelogram(float* mask, VipsRect& rc, SplineCurve& scurve, Point& pt1, Point& pt2, Point& pt3, Point& pt4)
{
  Point corner;
  Point ptr1, ptr2, ptr3/*, ptr4*/;
  
//  corner.set(pt3.get_x(), pt2.get_y());
//  fill_falloff_triangle(mask, rc, scurve, pt3, corner, pt2);
  
/*  ptr1.set(pt4.get_x(), pt1.get_y());
  ptr2.set(pt3.get_x(), pt2.get_y());
  ptr3.set(pt3.get_x(), pt3.get_y());
  ptr4.set(pt4.get_x(), pt4.get_y());
  */
//  get_intersect_point(pt1, pt2, pt4, ptr1);
//  get_intersect_point(pt1, pt2, pt3, ptr2);
//  fill_falloff_rectangle(mask, rc, scurve, ptr1, ptr2, pt3, pt4);

//  get_intersect_point(pt4, pt3, pt2, ptr3);
//  fill_falloff_ellipse_square(mask, rc, scurve, pt3, ptr2, pt2, ptr3);
  
  fill_falloff_rectangle(mask, rc, scurve, pt1, pt2, pt3, pt4);
}

// -----------------------------------
// Rectangle
// -----------------------------------

void PF::Rectangle::offset(int x, int y)
{
  for (int i = 0; i < get_points_count(); i++) {
    get_point(i).offset(x, y);
  }
}

void PF::Rectangle::offset_point(int n, int x, int y)
{
  PF::Polygon::offset_point(n, x, y);
  
  if (n == 0) {
    get_point(1).set(get_point(1).x, get_point(n).y);
    get_point(3).set(get_point(n).x, get_point(3).y);
  }
  if (n == 1) {
    get_point(0).set(get_point(0).x, get_point(n).y);
    get_point(2).set(get_point(n).x, get_point(2).y);
  }
  if (n == 2) {
    get_point(1).set(get_point(n).x, get_point(1).y);
    get_point(3).set(get_point(3).x, get_point(n).y);
  }
  if (n == 3) {
    get_point(0).set(get_point(n).x, get_point(0).y);
    get_point(2).set(get_point(2).x, get_point(n).y);
  }
}

void PF::Rectangle::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source, int options)
{
  switch (hit_t)
  {
  case PF::Shape::hit_segment:
    if (additional == 0) {
      offset_point(0, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 1) {
      offset_point(1, curr.get_x()-prev.get_x(), 0);
    }
    else if (additional == 2) {
      offset_point(2, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 3) {
      offset_point(0, curr.get_x()-prev.get_x(), 0);
    }
    break;
  case PF::Shape::hit_falloff_segment:
    if (falloff_can_be_offset(additional, prev, curr) && falloff_can_be_offset(additional+1, prev, curr)) {
      if (additional == 3)
        offset_falloff_point(additional+1, prev, curr);
      else
        offset_falloff_point(additional, prev, curr);
    }
    break;
  case PF::Shape::hit_shape:
  case PF::Shape::hit_falloff:
    offset(prev, curr);
    if (get_has_source() && lock_source) {
      get_source_point().offset(curr, prev);
    }
    break;
  default:
    PF::Polygon::offset(hit_t, prev, curr, additional, lock_source, options);
    break;
  }
}


// -----------------------------------
// ShapesGroup
// -----------------------------------

PF::Shape* PF::ShapesGroup::get_shape(int index)
{
  PF::Shape* shape = NULL;

  int s_type = get_shapes_order().at(index).first;
  int s_index = get_shapes_order().at(index).second;

  if ( s_type == PF::Shape::line )
    shape = &(get_lines().data()[s_index]);

  else if ( s_type == PF::Shape::circle ) {
    Circle& sh = get_circles()[s_index];
    shape = dynamic_cast<Circle*>(&sh);
  }
  else if ( s_type == PF::Shape::ellipse )
    shape = &(get_ellipses().data()[s_index]);

  else if ( s_type == PF::Shape::rectangle )
    shape = &(get_rectangles().data()[s_index]);
  
  else if ( s_type == PF::Shape::polygon )
    shape = &(get_polygons().data()[s_index]);
  
  else
    std::cout<<"PF::ShapesGroup::get_shape(): invalid shape index: "<<index<<std::endl;

  if (shape == NULL)
    std::cout<<"PF::ShapesGroup::get_shape() (shape == NULL) "<<std::endl;

  return shape;
}

int PF::ShapesGroup::add(Shape* shape_source)
{
  int index = -1;
  
  switch (shape_source->get_type())
  {
  case PF::Shape::line:
  {
    PF::Line shape;
    const PF::Line* ss = static_cast<const PF::Line*>(shape_source);
    shape = *ss;
    lines.push_back(shape);
    index = lines.size()-1;
  }
    break;
  case PF::Shape::circle:
  {
    PF::Circle shape;
    const PF::Circle* ss = static_cast<const PF::Circle*>(shape_source);
    shape = *ss;
    circles.push_back(shape);
    index = circles.size()-1;
  }
    break;
  case PF::Shape::ellipse:
  {
    PF::Ellipse shape;
    const PF::Ellipse* ss = static_cast<const PF::Ellipse*>(shape_source);
    shape = *ss;
    ellipses.push_back(shape);
    index = ellipses.size()-1;
  }
    break;
  case PF::Shape::rectangle:
  {
    PF::Rectangle shape;
    const PF::Rectangle* ss = static_cast<const PF::Rectangle*>(shape_source);
    shape = *ss;
    rectangles.push_back(shape);
    index = rectangles.size()-1;
  }
    break;
  case PF::Shape::polygon:
  {
    std::cout<<"PF::ShapesGroup::add(): adding polygon "<<std::endl;
    
    PF::Polygon shape;
    const PF::Polygon* ss = static_cast<const PF::Polygon*>(shape_source);
    shape = *ss;
    polygons.push_back(shape);
    index = polygons.size()-1;
  }
    break;
  default:
    std::cout<<"PF::ShapesGroup::add(): invalid shape type: "<<shape_source->get_type()<<std::endl;
    break;
  }
  
  if (index >= 0) {
    shapes_order.push_back( std::pair<int,int>(shape_source->get_type(),index) );
    index = shapes_order.size()-1;
  }
  else {
    std::cout<<"PF::ShapesGroup::add(): error adding shape: index: "<<index<<std::endl;
  }
  
  return index;
}

void PF::ShapesGroup::remove(int n)
{
  int ntype = shapes_order.at(n).first;
  int nindex = shapes_order.at(n).second;
  
  switch (ntype)
  {
  case PF::Shape::line:
    lines.erase(lines.begin()+nindex);
    break;
  case PF::Shape::circle:
    circles.erase(circles.begin()+nindex);
    break;
  case PF::Shape::ellipse:
    ellipses.erase(ellipses.begin()+nindex);
    break;
  case PF::Shape::rectangle:
    rectangles.erase(rectangles.begin()+nindex);
    break;
  case PF::Shape::polygon:
    polygons.erase(polygons.begin()+nindex);
    break;
  default:
    std::cout<<"PF::ShapesGroup::remove(): error removing shape: index: "<<n<<std::endl;
    break;
  }
 
  for (int i = 0; i < shapes_order.size(); i++) {
    if (shapes_order.at(i).first == ntype && shapes_order.at(i).second > nindex) {
      shapes_order.at(i).second--;
    }
  }

  shapes_order.erase(shapes_order.begin()+n);
}

void PF::ShapesGroup::clear()
{
  lines.clear();
  circles.clear();
  ellipses.clear();
  rectangles.clear();
  polygons.clear();
  shapes_order.clear();

}

void PF::ShapesGroup::scale(int sf)
{
  for (int i = 0; i < lines.size(); i++) {
    lines[i].scale(sf);
  }
  for (int i = 0; i < circles.size(); i++) {
    circles[i].scale(sf);
  }
  for (int i = 0; i < ellipses.size(); i++) {
    ellipses[i].scale(sf);
  }
  for (int i = 0; i < rectangles.size(); i++) {
    rectangles[i].scale(sf);
  }
  for (int i = 0; i < polygons.size(); i++) {
    polygons[i].scale(sf);
  }
}

void PF::ShapesGroup::build_mask(SplineCurve& scurve)
{
  for (int i = 0; i < lines.size(); i++) {
    lines[i].build_mask(scurve);
  }
  for (int i = 0; i < circles.size(); i++) {
    circles[i].build_mask(scurve);
  }
  for (int i = 0; i < ellipses.size(); i++) {
    ellipses[i].build_mask(scurve);
  }
  for (int i = 0; i < rectangles.size(); i++) {
    rectangles[i].build_mask(scurve);
  }
  for (int i = 0; i < polygons.size(); i++) {
    polygons[i].build_mask(scurve);
  }
}


