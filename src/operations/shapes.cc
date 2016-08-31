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

const float PF::Shape::hit_test_delta = 8;


// -----------------------------------
// Point
// -----------------------------------

void PF::Point::offset(Point prev, Point curr)
{
  x += curr.get_x()-prev.get_x();
  y += curr.get_y()-prev.get_y();
}

void PF::Point::offset(float n, Point& center)
{
    if ( x < center.x )
      x -= n;
    else
      x += n;
    if ( y < center.y )
      y -= n;
    else
      y += n;

}

void PF::Point::offset_pos(Point prev, Point curr)
{
  x = std::max(0.f, x+curr.get_x()-prev.get_x());
  y = std::max(0.f, y+curr.get_y()-prev.get_y());
}

void PF::Point::scale(float scale)
{
  x /= scale;
  y /= scale;
}

// Find the distance squared from this to a segment (l1, l2)
/*
float PF::Point::distance2segment2(Point& l1, Point& l2)
{
  float l = l1.distance2(l2);
   if (l == 0.f) return distance2(l1);
   float t = ((this->x - l1.x) * (l2.x - l1.x) + (this->y - l1.y) * (l2.y - l1.y)) / l;
   t = std::max(0.f, std::min(1.f, t));
   return distance2( l1.x + t * (l2.x - l1.x),
                     l1.y + t * (l2.y - l1.y) );
}
*/

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

PF::Point PF::Shape::get_falloff_point_absolute(int n, Point& center)
{
  Point pt_fall = get_point(n);
  return pt_fall;
  
}

PF::Point PF::Shape::get_falloff_point_absolute(int n)
{
  Point center = get_center();
  return get_falloff_point_absolute(n, center);
}

void PF::Shape::offset(float x, float y)
{
  for (int i = 0; i < get_points_count(); i++) {
    offset_point(i, x, y);
  }
}

void PF::Shape::offset(Point prev, Point curr)
{
  offset(curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset_point(int n, float x, float y)
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

void PF::Shape::offset_falloff_point(int n, float x, float y)
{
  if ( falloff_can_be_offset(n, x, y) )
    falloff_points[n].offset(x, y);
}

void PF::Shape::offset_falloff_point(int n, Point& prev, Point& curr)
{
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
    offset_point(additional, prev, curr);
    offset_point(additional+1, prev, curr);
    break;
  case PF::Shape::hit_falloff_segment:
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

void PF::Shape::scale(float scale)
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

int PF::Shape::insert_point(Point& pt, int position)
{
  std::vector<Point>::iterator it;
  it = points.begin();
  it = points.insert( it+position, pt );

  it = falloff_points.begin();
  it = falloff_points.insert( it+position, Point(get_falloff(), get_falloff()) );
  
  return position;
}

void PF::Shape::remove_point(int n)
{
  points.erase(points.begin()+n);
  falloff_points.erase(falloff_points.begin()+n);
}

PF::Point PF::Shape::get_center(VipsRect& rc)
{
  Point pt;
  pt.set(rc.left+rc.width/2, rc.top+rc.height/2);
  return pt;
}

PF::Point PF::Shape::get_center()
{
  Point center;
  float surf;
  
  get_center(center, surf);
  
  return center;
}

void PF::Shape::get_center(std::vector<Point>& poly_pt, Point& center, float& surf)
{
  float bx = 0.0f;
  float by = 0.0f;
  surf = 0.0f;
  int nb = poly_pt.size();
  
  for (int k = 0; k < nb; k++)
  {
    int k2 = (k + 1) % nb;
    Point& point1 = poly_pt[k];
    Point& point2 = poly_pt[k2];
    surf += point1.x * point2.y - point2.x * point1.y;

    bx += (point1.x + point2.x)
          * (point1.x * point2.y - point2.x * point1.y);
    by += (point1.y + point2.y)
          * (point1.x * point2.y - point2.x * point1.y);
  }
  if (surf != 0.f) {
    bx /= 3.0 * surf;
    by /= 3.0 * surf;
  }
  center.set(bx, by);
}

void PF::Shape::get_center(Point& center, float& surf)
{
  get_center(get_points(), center, surf);
}

void PF::Shape::expand(float n)
{
  Point center, expand;
  float surf;
  float amount = 1.03;
  if(n<0) amount = 0.97;
  
  get_center(center, surf);
  if(amount < 1.0 && surf < 0.00001 && surf > -0.00001) return;

  for (int i = 0; i < get_points_count(); i++) {
    Point& point = get_point(i);
    
    expand.x = (point.x - center.x) * amount;
    expand.y = (point.y - center.y) * amount;

    // and we set the new points
    point.x = center.x + expand.x;
    point.y = center.y + expand.y;
  }
}

void PF::Shape::expand_opacity(float f)
{
  opacity = std::min(1.f, std::max(0.f, opacity + f));
}

void PF::Shape::expand_pen_size(float f)
{
  set_pen_size( get_pen_size() + f );
}

void PF::Shape::get_rect(Point& pt1, Point& pt2)
{
  if (get_points_count() > 0) {
    pt1.x = pt1.y = (float)INT_MAX;
    pt2.x = pt2.y = (float)INT_MIN;
    
   for (int i = 0; i < points.size(); i++) {
     pt1.x = std::min(pt1.x, get_point(i).x);
     pt1.y = std::min(pt1.y, get_point(i).y);
     pt2.x = std::max(pt2.x, get_point(i).x);
     pt2.y = std::max(pt2.y, get_point(i).y);
    }
   
   float pen_size = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
   
   pt1.x -= pen_size;
   pt1.y -= pen_size;
   pt2.x += pen_size;
   pt2.y += pen_size;
  }
  else {
    pt1.x = pt1.y = pt2.x = pt2.y = 0.f;
  }
}

void PF::Shape::get_falloff_rect(Point& pt1, Point& pt2)
{
  if (get_points_count() > 0) {
    get_rect(pt1, pt2);
    
    pt1.x -= get_falloff();
    pt1.y -= get_falloff();
    pt2.x += get_falloff();
    pt2.y += get_falloff();
  }
  else {
    pt1.x = pt1.y = pt2.x = pt2.y = 0.f;
  }
}

void PF::Shape::get_mask_rect(VipsRect* rc)
{
  Point pt1, pt2, pt3, pt4;
  
  get_rect(pt1, pt2);
  get_falloff_rect(pt3, pt4);
  
  rc->left = std::min(pt1.x, pt3.x);
  rc->top = std::min(pt1.y, pt3.y);
  rc->width = std::max(pt2.x, pt4.x);
  rc->height = std::max(pt2.y, pt4.y);
  
  rc->width = rc->width-rc->left+1;
  rc->height = rc->height-rc->top+1;
  
  rotate(rc);
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
float PF::Shape::point_in_ellipse(float x, float y, float h, float v, float rx2, float ry2)
{
  return ( ((float)((x-h)*(x-h)))/rx2 ) + ( ((float)((y-v)*(y-v)))/ry2 );
}

// returns the intersection point from segment (A, B) and point C
void PF::Shape::get_intersect_point(Point& A, Point& B, Point& C, Point& pt_out)
{
  float x1=A.get_x(), y1=A.get_y(), x2=B.get_x(), y2=B.get_y(), x3=C.get_x(), y3=C.get_y();
  float dx = x2 - x1;
  float dy = y2 - y1;
  float mag = sqrtf(dx*dx + dy*dy);
  dx /= mag;
  dy /= mag;

  // translate the point and get the dot product
  float lambda = (dx * (x3 - x1)) + (dy * (y3 - y1));
  pt_out.set( (dx * lambda) + x1,
   (dy * lambda) + y1);
}

void PF::Shape::get_segment_bounding_rect(Point& pt1, Point& pt2, VipsRect* rc)
{
  rc->left = std::min(pt1.get_x(), pt2.get_x());
  rc->top = std::min(pt1.get_y(), pt2.get_y());
  rc->width = std::max(pt1.get_x(), pt2.get_x()) - rc->left + 1;
  rc->height = std::max(pt1.get_y(), pt2.get_y()) - rc->top + 1;
}

void PF::Shape::get_segment_bounding_rect(Point& pt1, Point& pt2, float expand, VipsRect* rc)
{
  get_segment_bounding_rect(pt1, pt2, rc);
  rc->left -= expand;
  rc->top -= expand;
  rc->width += expand*2;
  rc->height += expand*2;
}

// returns a rect, result of expanding a given line by amount
void PF::Shape::get_expanded_rec_from_line(Point& pt1, Point& pt2, float amount, Point& a, Point& b, Point& c, Point& d)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = std::sqrt(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;

  a.set( pt1.get_x() - amount_y + 1.f, pt1.get_y() + amount_x + 1.f);
  c.set( pt2.get_x() + amount_y + 1.f, pt2.get_y() - amount_x + 1.f);
  b.set( pt2.get_x() - amount_y + 1.f, pt2.get_y() + amount_x + 1.f);
  d.set( pt1.get_x() + amount_y + 1.f, pt1.get_y() - amount_x + 1.f);

}

void PF::Shape::get_pt1_2_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& a, Point& b)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = std::sqrt(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;

  a.set( pt1.get_x() - amount_y + 1.f, pt1.get_y() + amount_x + 1.f);
  b.set( pt2.get_x() - amount_y + 1.f, pt2.get_y() + amount_x + 1.f);

}

void PF::Shape::get_pt3_4_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& d, Point& c)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = std::sqrt(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;

  c.set( pt2.get_x() + amount_y + 1.f, pt2.get_y() - amount_x + 1.f);
  d.set( pt1.get_x() + amount_y + 1.f, pt1.get_y() - amount_x + 1.f);

}

// returns a point results of proyecting pt1 from segment pt1-pt2 by amount
void PF::Shape::get_pt_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& a)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = sqrtf(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;
  
  a.set( pt1.get_x() - amount_y + 1.f, pt1.get_y() + amount_x + 1.f);

}

void PF::Shape::get_pt1_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& a, Point& d)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = sqrtf(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;

  a.set( pt1.get_x() - amount_y + 1.f, pt1.get_y() + amount_x + 1.f);
  d.set( pt1.get_x() + amount_y + 1.f, pt1.get_y() - amount_x + 1.f);

}

void PF::Shape::get_pt2_proyected_from_segment(Point& pt1, Point& pt2, float amount, Point& b, Point& c)
{
  float dx = pt1.get_x()-pt2.get_x();
  float dy = pt1.get_y()-pt2.get_y();
  const float dist = sqrtf(dx*dx + dy*dy);
  dx /= dist;
  dy /= dist;
  const float amount_x = ((float)amount)*dx;
  const float amount_y = ((float)amount)*dy;

  c.set( pt2.get_x() + amount_y + 1.f, pt2.get_y() - amount_x + 1.f);
  b.set( pt2.get_x() - amount_y + 1.f, pt2.get_y() + amount_x + 1.f);

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
  Point center = get_center(*rc);
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

void PF::Shape::bubble_sort(std::vector<int>& vec)
{
  int i = 0;
  const int nodes = vec.size();
  while ( i < nodes-1 ) {
    if ( vec[i] > vec[i+1] ) {
      std::swap( vec[i], vec[i+1] );
      if (i) i--; 
    }
    else {
      i++; 
    }
  }

}


void PF::Shape::test_mask(float* buffer, VipsRect& rc)
{
#if 0
  int  pixelX, pixelY;
  float f;

  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  int y_from = IMAGE_TOP;
  int y_to = IMAGE_BOT;
  
  int x_from = IMAGE_LEFT;
  int x_to = IMAGE_RIGHT;
 
  //  Loop through the rows of the image.
  for (pixelY=y_from; pixelY<y_to; pixelY++) {
    float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);
    
    for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
       f = *buff_y;
       
       if ( f < 0.f || f > 1.f || isnan(f) ) {
         std::cout<<"PF::Shape::test_mask(): f: "<<f<<std::endl;
       }
    }
  }
#endif
}


// -----------------------------------
// SegmentInfo
// -----------------------------------

void PF::SegmentInfo::scale(float sf)
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
  const float radius = get_radius();
  float dx = abs(pt.get_x()-point.get_x());
  float dy = abs(pt.get_y()-point.get_y());
  float R = radius+get_falloff()+2;
  
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

void PF::Circle::get_rect(Point& pt1, Point& pt2)
{
  const float R = get_radius();
  const float P = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
  pt1.x = get_point().get_x()-R-P;
  pt1.y = get_point().get_y()-R-P;
  pt2.x = get_point().get_x()+R+P;
  pt2.y = get_point().get_y()+R+P;
}

void PF::Circle::get_falloff_rect(Point& pt1, Point& pt2)
{
  get_rect(pt1, pt2);
  
  pt1.x -= get_falloff();
  pt1.y -= get_falloff();
  pt2.x += get_falloff();
  pt2.y += get_falloff();
}

void PF::Circle::get_mask_rect(VipsRect* rc)
{
  Point pt1, pt2;
  
  get_falloff_rect(pt1, pt2);
  
  rc->left = pt1.x;
  rc->top = pt1.y;
  rc->width = pt2.x-pt1.x+1.f;
  rc->height = pt2.y-pt1.y+1.f;
}

void PF::Circle::fill_circle(Point& center, float radius, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY;
  float dist2, f;
  const float R2 = radius*radius;
  const float Rf2 = (radius+falloff)*(radius+falloff);
  const float R_fall = (1.f / (Rf2-R2));

  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  int y_from = center.y-radius-falloff;
  int y_to = center.y+radius+falloff+1;
  y_from = std::max(y_from, IMAGE_TOP);
  y_to = std::min(y_to, IMAGE_BOT);
  
  int x_from = center.x-radius-falloff;
  int x_to = center.x+radius+falloff+1;
  x_from = std::max(x_from, IMAGE_LEFT);
  x_to = std::min(x_to, IMAGE_RIGHT);
 
  //  Loop through the rows of the image.
  for (pixelY=y_from; pixelY<y_to; pixelY++) {
    float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);
    
    for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
       dist2 =  center.distance2(pixelX, pixelY);
       
       if ( dist2 <= R2 ) {
         *buff_y = opacity;
       }
       else if ( dist2 <= Rf2 ) {
         f = (dist2-R2) * R_fall;
         *buff_y = scurve.get_value( 1.f - f ) * opacity;
       }
    }
  }

}

void PF::Circle::draw_circle(Point& center, float radius, float pen_size, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY;
  float dist2, f;
  const float pen_size_2 = pen_size / 2.f;
  //  const float R2 = radius*radius;
  const float R2_in = std::max(0.f, (radius-pen_size_2))*std::max(0.f, (radius-pen_size_2));
  const float R2_out = (radius+pen_size_2)*(radius+pen_size_2);

  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;

  int y_from = center.y-radius-falloff-pen_size_2;
  int y_to = center.y+radius+falloff+pen_size_2+1;
  y_from = std::max(y_from, IMAGE_TOP);
  y_to = std::min(y_to, IMAGE_BOT);

  int x_from = center.x-radius-falloff-pen_size_2;
  int x_to = center.x+radius+falloff+pen_size_2+1;
  x_from = std::max(x_from, IMAGE_LEFT);
  x_to = std::min(x_to, IMAGE_RIGHT);

  if ( falloff > 0.f ) {
    const float R2_fin = std::max(0.f, (radius-falloff-pen_size_2))*std::max(0.f, (radius-falloff-pen_size_2));
    const float R2_fout = (radius+falloff+pen_size_2)*(radius+falloff+pen_size_2);
    const float R_fall_out = (1.f / std::max(.00001f, (R2_fout-R2_out)));
    const float R_fall_in = (1.f / std::max(.00001f, (R2_in-R2_fin)));

    //  Loop through the rows of the image.
    for (pixelY=y_from; pixelY<y_to; pixelY++) {
      float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);

      for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
        dist2 =  center.distance2(pixelX, pixelY);

        if ( R2_fin <= dist2 && dist2 < R2_in ) {
          f = (R2_in-dist2) * R_fall_in;
          *buff_y = scurve.get_value( 1.f - f ) * opacity;
        }
        else if ( R2_in <= dist2 && dist2 <= R2_out ) {
          *buff_y = opacity;
        }
        else if ( R2_out < dist2 && dist2 <= R2_fout ) {
          f = (dist2-R2_out) * R_fall_out;
          *buff_y = scurve.get_value( 1.f - f ) * opacity;
        }
      }
    }
  }
  else {
    //  Loop through the rows of the image.
    for (pixelY=y_from; pixelY<y_to; pixelY++) {
      float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);

      for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
        dist2 =  center.distance2(pixelX, pixelY);

        if ( R2_in <= dist2 && dist2 <= R2_out ) {
          *buff_y = opacity;
        }
      }
    }
  }
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
  get_mask_rect(&rc);

  // nothing to return
  if (rc.width < 1 || rc.height < 1) return;
  
  // alloc mask
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  memset(mask, 0, rc.width * rc.height * sizeof(float));

  Point center = get_center();
  
  if ( get_fill_shape() ) {
    fill_circle(center, get_radius(), get_falloff(), get_opacity(), scurve, mask, rc);
  }
  else {
    draw_circle(center, get_radius(), get_pen_size(), get_falloff(), get_opacity(), scurve, mask, rc);
  }

  test_mask(mask, rc);
  
}

// -----------------------------------
// Ellipse
// -----------------------------------

float PF::Ellipse::get_falloff_y() 
{
  float falloff = get_radius_y() * (get_falloff_x()/get_radius_x());
  
  if (falloff <= 0.f && get_falloff_x() > 0.f) falloff = 1.f;
  
  return falloff;
}

void PF::Ellipse::expand(float n)
{
  float ny = n * (get_radius_y()/get_radius_x());
  
  if (ny <= 0.f && n > 0.f) ny = 1.f;
  
  set_radius_y(ny+get_radius_y()); 
  set_radius_x(n+get_radius_x()); 
}

int PF::Ellipse::hit_test(Point& pt, int& additional)
{
  if ( get_angle() != 0 ) {
    return hit_test(pt, get_angle(), additional);
  }
  
  VipsRect rc;  
  get_mask_rect(&rc);
  
  // inside the falloff rect
  if ( hit_test_rect(pt, &rc) ) {
    const float radius_x = get_radius_x();
    const float radius_y = get_radius_y();
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
  get_mask_rect(&rc);
  
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
    const float radius_x = get_radius_x();
    const float radius_y = get_radius_y();
    
    Point center_r;
    center_r = get_center(rc_r);
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
  {
    Point center = get_center();
    float dist_c = sqrt(center.distance2(curr));
    float dist_p = sqrt(center.distance2(prev));

    if (additional == 0 || additional == 2) {
      set_radius_y(get_radius_y() + (dist_c - dist_p));
    }
    else if (additional == 1 || additional == 3) {
      set_radius_x(get_radius_x() + (dist_c - dist_p));
   }
  }
    break;
  case PF::Shape::hit_segment:
  {
    float atn1=0, atn2=0, new_angle;
    Point center;
    
    center = get_center();
    
    if (curr.get_x()-center.get_x()) atn1 = atanf( (float)(curr.get_y()-center.get_y()) / (float)(curr.get_x()-center.get_x()) ) * 180.f / (float)M_PI;
    if (prev.get_x()-center.get_x()) atn2 = atanf( (float)(prev.get_y()-center.get_y()) / (float)(prev.get_x()-center.get_x()) ) * 180.f / (float)M_PI;
    new_angle = get_angle() + (atn2 - atn1);
    while ( new_angle < 0.f ) new_angle += 360.f;
    while ( new_angle > 360.f ) new_angle -= 360.f;
    set_angle( new_angle );
  }
    break;
  case PF::Shape::hit_falloff_segment:
  case PF::Shape::hit_falloff_node:
  default:
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source, options);
    break;
  }

}

void PF::Ellipse::get_falloff_rect(VipsRect* rc)
{
  Point pt1, pt2;
  
  get_falloff_rect(pt1, pt2);
  
  rc->left = pt1.x;
  rc->top = pt1.y;
  rc->width = pt2.x-pt1.x;
  rc->height = pt2.y-pt1.y;
}

void PF::Ellipse::get_rect(Point& pt1, Point& pt2)
{
  const float Rx = get_radius_x();
  const float Ry = get_radius_y();
  const float P = ( get_fill_shape() ) ? 0.f: (get_pen_size()/2.f);
  pt1.x = get_point().get_x()-Rx-P;
  pt1.y = get_point().get_y()-Ry-P;
  pt2.x = get_point().get_x()+Rx+P;
  pt2.y = get_point().get_y()+Ry+P;
}

void PF::Ellipse::get_falloff_rect(Point& pt1, Point& pt2)
{
  get_rect(pt1, pt2);
  
  pt1.x -= get_falloff_x();
  pt1.y -= get_falloff_y();
  pt2.x += get_falloff_x();
  pt2.y += get_falloff_y();
}

void PF::Ellipse::fill_ellipse(Point& center, float radius_x, float radius_y, float falloff_x, float falloff_y, float angle, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY;
  float dist2, dist2f, f;
  Point pt;
  const float Rx2 = radius_x*radius_x;
  const float Rx2_1 = (Rx2>0.f) ? 1.f / Rx2: 1.f;
  const float Ry2 = radius_y*radius_y;
  const float Ry2_1 = (Ry2>0.f) ? 1.f / Ry2: 1.f;

  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  const int y_from = IMAGE_TOP;
  const int y_to = IMAGE_BOT;
  
  const int x_from = IMAGE_LEFT;
  const int x_to = IMAGE_RIGHT;
 
  const float _angle = angle*M_PI/180.f;
  const float sin_angle = sinf(_angle);
  const float cos_angle = cosf(_angle);

  if ( falloff_x > 0.f || falloff_y > 0.f ) {
    const float Rx2f = (radius_x+falloff_x)*(radius_x+falloff_x);
    const float Rx2f_1 = (Rx2f>0.f) ? 1.f / Rx2f: 1.f;
    const float Ry2f = (radius_y+falloff_y)*(radius_y+falloff_y);
    const float Ry2f_1 = (Ry2f>0.f) ? 1.f / Ry2f: 1.f;

    float tot_dist = (Rx2>0 && Rx2 != Rx2f) ? (1.f / ( (Rx2f/Rx2) - 1.f )): 1.f;
    
    //  Loop through the rows of the image.
    for (pixelY=y_from; pixelY<y_to; pixelY++) {
      float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);
      
      for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
        pt.set(pixelX, pixelY);
        pt.rotate(center, sin_angle, cos_angle);
        const float dist_x2 = ((pt.x-center.x)*(pt.x-center.x));
        const float dist_y2 = ((pt.y-center.y)*(pt.y-center.y));
        dist2 = ( dist_x2 * Rx2_1 + dist_y2 * Ry2_1 );
  
         if ( dist2 <= 1.f ) {
           *buff_y = opacity;
         }
         else {
           dist2f = ( dist_x2 * Rx2f_1 + dist_y2 * Ry2f_1 );
           if ( dist2f <= 1.f ) {
             f = ( ( dist2 - 1.f ) * tot_dist ) ;
             if ( f >= 0.f && f <= 1.f ) {
               *buff_y = scurve.get_value( 1.f - f ) * opacity;
             }
             else {
               std::cout<<"dist2f: "<<dist2f<<", dist2: "<<dist2<<", f: "<<f<<std::endl;
             }
           }
         }
      }
    }
  }
  else {
    //  Loop through the rows of the image.
    for (pixelY=y_from; pixelY<y_to; pixelY++) {
      float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);
      
      for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
        pt.set(pixelX, pixelY);
        pt.rotate(center, sin_angle, cos_angle);
        const float dist_x2 = ((pt.x-center.x)*(pt.x-center.x));
        const float dist_y2 = ((pt.y-center.y)*(pt.y-center.y));
        dist2 = ( dist_x2 * Rx2_1 + dist_y2 * Ry2_1 );
  
         if ( dist2 <= 1.f ) {
           *buff_y = opacity;
         }
      }
    }
  }
}

void PF::Ellipse::draw_ellipse(Point& center, float radius_x, float radius_y, float pen_size, float falloff_x, float falloff_y, float angle, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY;
  Point pt;
  float dist2_in, dist2_out, dist2f_in, dist2f_out, f;
  const float pen_size_2 = pen_size / 2.f;
  
  const float Rx2_in = std::max(0.f, (radius_x-pen_size_2))*std::max(0.f, (radius_x-pen_size_2));
  const float Ry2_in = std::max(0.f, (radius_y-pen_size_2))*std::max(0.f, (radius_y-pen_size_2));
  const float Rx2_in_1 = (Rx2_in>0.f) ? 1.f / Rx2_in: 1.f;
  const float Ry2_in_1 = (Ry2_in>0.f) ? 1.f / Ry2_in: 1.f;

  const float Rx2_out = (radius_x+pen_size_2)*(radius_x+pen_size_2);
  const float Ry2_out = (radius_y+pen_size_2)*(radius_y+pen_size_2);
  const float Rx2_out_1 = (Rx2_out>0.f) ? 1.f / Rx2_out: 1.f;
  const float Ry2_out_1 = (Ry2_out>0.f) ? 1.f / Ry2_out: 1.f;
  
  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  const int y_from = IMAGE_TOP;
  const int y_to = IMAGE_BOT;
  
  const int x_from = IMAGE_LEFT;
  const int x_to = IMAGE_RIGHT;

  const float _angle = angle*M_PI/180.f;
  const float sin_angle = sinf(_angle);
  const float cos_angle = cosf(_angle);

  if ( falloff_x > 0.f || falloff_y > 0.f ) {
    const float Rx2_fin = std::max(0.f, (radius_x-falloff_x-pen_size_2))*std::max(0.f, (radius_x-falloff_x-pen_size_2));
    const float Ry2_fin = std::max(0.f, (radius_y-falloff_y-pen_size_2))*std::max(0.f, (radius_y-falloff_y-pen_size_2));
    const float Rx2_fin_1 = (Rx2_fin>0.f) ? 1.f / Rx2_fin: 1.f;
    const float Ry2_fin_1 = (Ry2_fin>0.f) ? 1.f / Ry2_fin: 1.f;
    
    const float Rx2_fout = (radius_x+falloff_x+pen_size_2)*(radius_x+falloff_x+pen_size_2);
    const float Ry2_fout = (radius_y+falloff_y+pen_size_2)*(radius_y+falloff_y+pen_size_2);
    const float Rx2_fout_1 = (Rx2_fout>0.f) ? 1.f / Rx2_fout: 1.f;
    const float Ry2_fout_1 = (Ry2_fout>0.f) ? 1.f / Ry2_fout: 1.f;

    float tot_dist;
    if ( radius_x+falloff_x < radius_y+falloff_y )
      tot_dist = (Ry2_out>0.f && Ry2_fout!=Ry2_out) ? 1.f/((Ry2_fout/Ry2_out)-1.f): 1.f;
    else
      tot_dist = (Rx2_out>0.f && Rx2_fout!=Rx2_out) ? 1.f/((Rx2_fout/Rx2_out)-1.f): 1.f;
    
    float tot_dist_in;
    if ( radius_x+falloff_x > radius_y+falloff_y )
      tot_dist_in = (Ry2_in>0.f) ? (Ry2_fin/Ry2_in): 0.f;
    else
      tot_dist_in = (Rx2_in>0.f) ? (Rx2_fin/Rx2_in): 0.f;
    
    float tot_dist_in_1 = ((1.f-tot_dist_in)!=0.f) ? 1.f / (1.f-tot_dist_in): 1.f;

    //  Loop through the rows of the image.
    for (pixelY=y_from; pixelY<y_to; pixelY++) {
      float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);
      
      for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
        pt.set(pixelX, pixelY);
        pt.rotate(center, sin_angle, cos_angle);
        const float dist_x2 = ((pt.x-center.x)*(pt.x-center.x));
        const float dist_y2 = ((pt.y-center.y)*(pt.y-center.y));
        dist2f_in = ( dist_x2 * Rx2_fin_1 + dist_y2 * Ry2_fin_1 );
  
        if ( dist2f_in > 1.f ) {
          dist2_in = ( dist_x2 * Rx2_in_1 + dist_y2 * Ry2_in_1 );
          if ( dist2_in < 1.f ) {
            f = ( ( dist2_in - tot_dist_in ) * tot_dist_in_1 ) ;
            if ( f >= 0.f && f <= 1.f )
              *buff_y = scurve.get_value( /*1.f -*/ f ) * opacity;
            else {
              std::cout<<"dist2f_in: "<<dist2f_in<<", dist2_in: "<<dist2_in<<", f: "<<f<<std::endl;
            }
          }
          else {
            dist2_out = ( dist_x2 * Rx2_out_1 + dist_y2 * Ry2_out_1 );
      
             if ( dist2_out <= 1.f ) {
               *buff_y = opacity;
             }
             else {
               dist2f_out = ( dist_x2 * Rx2_fout_1 + dist_y2 * Ry2_fout_1 );
               if ( dist2f_out <= 1.f ) {
                 f = ( ( dist2_out - 1.f ) * tot_dist ) ;
                 if ( f >= 0.f && f <= 1.f )
                   *buff_y = scurve.get_value( 1.f - f ) * opacity;
                 else
                   std::cout<<"2 dist2f_out: "<<dist2f_out<<", dist2_in: "<<dist2_in<<", dist2_out: "<<dist2_out<<", f: "<<f<<std::endl;
               }
             }
          }
        }
      }
    }
  }
  else {
    //  Loop through the rows of the image.
    for (pixelY=y_from; pixelY<y_to; pixelY++) {
      float *buff_y = buffer + ((pixelY-IMAGE_TOP)*rc.width) + (x_from-IMAGE_LEFT);
      
      for (pixelX=x_from; pixelX<x_to; pixelX++, buff_y++) {
        pt.set(pixelX, pixelY);
        pt.rotate(center, sin_angle, cos_angle);
        const float dist_x2 = ((pt.x-center.x)*(pt.x-center.x));
        const float dist_y2 = ((pt.y-center.y)*(pt.y-center.y));
  
        dist2_in = ( dist_x2 * Rx2_in_1 + dist_y2 * Ry2_in_1 );
        if ( dist2_in >= 1.f ) {
          dist2_out = ( dist_x2 * Rx2_out_1 + dist_y2 * Ry2_out_1 );
    
           if ( dist2_out <= 1.f ) {
             *buff_y = opacity;
           }
        }
      }
    }
  }
}

void PF::Ellipse::build_mask(SplineCurve& scurve)
{
//  std::cout<<"PF::Circle::build_mask()"<<std::endl;
  
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

  Point center = get_center();
  
  if ( get_fill_shape() ) {
    fill_ellipse(center, get_radius_x(), get_radius_y(), get_falloff_x(), get_falloff_y(), get_angle(), get_opacity(), scurve, mask, rc);
  }
  else {
    draw_ellipse(center, get_radius_x(), get_radius_y(), get_pen_size(), get_falloff_x(), get_falloff_y(), get_angle(), get_opacity(), scurve, mask, rc);
  }

  test_mask(mask, rc);
}

// -----------------------------------
// Polygon
// -----------------------------------

void PF::Polygon::scale(float scale)
{
  PF::Shape::scale(scale);
  
  for (int i = 0; i < get_segments_count(); i++) {
    get_segment_info(i).scale(scale);
  }

}

bool PF::Shape::lines_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt_intersection)
{
   //L1: (x1,y1)(x2,y2) L2: (x3,y3)(x4,y4)
   float d = (pt1.get_x()-pt2.get_x())*(pt3.get_y()-pt4.get_y()) - (pt1.get_y()-pt2.get_y())*(pt3.get_x()-pt4.get_x());
   if(!d) return false;
   pt_intersection.set( (float)((pt1.get_x()*pt2.get_y()-pt1.get_y()*pt2.get_x())*(pt3.get_x()-pt4.get_x()) - (pt1.get_x()-pt2.get_x())*(pt3.get_x()*pt4.get_y()-pt3.get_y()*pt4.get_x()))/d, 
   (float)((pt1.get_x()*pt2.get_y()-pt1.get_y()*pt2.get_x())*(pt3.get_y()-pt4.get_y()) - (pt1.get_y()-pt2.get_y())*(pt3.get_x()*pt4.get_y()-pt3.get_y()*pt4.get_x()))/d );
   return true;
}

bool PF::Shape::segments_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4, Point& pt_intersection)
{
    if (!lines_intersects(pt1, pt2, pt3, pt4, pt_intersection))
        return false;

    return pt_intersection.in_rectangle(pt1, pt2) && pt_intersection.in_rectangle(pt3, pt4);
}

bool PF::Point::in_rectangle(Point& pt1, Point& pt2)
{
    if ( get_x()>=pt1.get_x() && get_x()<=pt2.get_x() || get_x()>=pt2.get_x() && get_x()<=pt1.get_x() )
      if ( get_y()>=pt1.get_y() && get_y()<=pt2.get_y() || get_y()>=pt2.get_y() && get_y()<=pt1.get_y() )
        return true;
    return false;
}
/*
bool PF::Point::is_left_of_line(Point& a, Point& b)
{
  return ( ((b.get_x() - a.get_x()) * (get_y() - a.get_y())) > ((b.get_y() - a.get_y())*(get_x() - a.get_x())) );
}

bool PF::Shape::segments_intersects(Point& pt1, Point& pt2, Point& pt3, Point& pt4)
{
  return ( pt1.is_left_of_line(pt3, pt4) != pt2.is_left_of_line(pt3, pt4) && pt3.is_left_of_line(pt1, pt2) != pt4.is_left_of_line(pt1, pt2));
}

#define print_nan(pt_nan, name) \
  if (isnan(pt_nan.x) || isnan(pt_nan.y)) std::cout<<"IsNan: pt_nan"<<".x: "<<pt_nan.x<<","<<name<<".y: "<<pt_nan.y<<std::endl; \
*/

void PF::Polygon::fill_polygon(std::vector<Point>& poly_pt, float opacity, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY, i, j;
  const int polyCorners = poly_pt.size();
  std::vector<int> nodeX;

  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  int y_from = IMAGE_BOT;
  int y_to = IMAGE_TOP;
  for ( int ii = 0; ii < poly_pt.size(); ii++ ) {
    y_from = std::min(y_from, (int)poly_pt[ii].y);
    y_to = std::max(y_to, (int)poly_pt[ii].y);
  }
  y_from = std::max(y_from, IMAGE_TOP);
  y_to = std::min(y_to+1, IMAGE_BOT);
  
  //  Loop through the rows of the image.
  for (pixelY=y_from; pixelY<y_to; pixelY++) {

    //  Build a list of nodes.
        nodeX.clear();
    j=polyCorners-1;
    for (i=0; i<polyCorners; i++) {
      if ( (poly_pt[i].y<(float) pixelY && poly_pt[j].y>=(float) pixelY)
      ||  (poly_pt[j].y<(float) pixelY && poly_pt[i].y>=(float) pixelY) ) {
        nodeX.push_back( (int) ( poly_pt[i].x + ((float)(pixelY-poly_pt[i].y) / (float)(poly_pt[j].y-poly_pt[i].y)) 
        * (float)(poly_pt[j].x-poly_pt[i].x) ) ); 
      }
      j=i; 
    }

    //  Sort the nodes, via a simple “Bubble” sort.
    bubble_sort(nodeX);

    //  Fill the pixels between node pairs.
    float *buf = buffer + ((pixelY-IMAGE_TOP)*rc.width);
    for (i=0; i<nodeX.size(); i+=2) {
      if   (nodeX[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX[i+1]> IMAGE_LEFT ) {
        if (nodeX[i  ]< IMAGE_LEFT ) nodeX[i  ]=IMAGE_LEFT ;
        if (nodeX[i+1]> IMAGE_RIGHT) nodeX[i+1]=IMAGE_RIGHT;
        for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) {
//          buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = opacity;
          buf[(pixelX-IMAGE_LEFT)] = opacity;
        }
      }
    }
  }

}

// Código bresenham para círculos en C++
int PF::Polygon::rasterCircle(int x0, int y0, int radius, float* buffer, VipsRect& rc)
{
  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;

  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
  
#define TEST_PIXEL(pixelX, pixelY) \
  if ( (pixelX) >= IMAGE_LEFT && (pixelX) < IMAGE_RIGHT && \
      (pixelY) >= IMAGE_TOP && (pixelY) < IMAGE_BOT ) {  \
    if ( buffer[(((pixelY)-IMAGE_TOP)*rc.width)+((pixelX)-IMAGE_LEFT)] == 1.f ) { \
      return distance2(x0, y0, (pixelX), (pixelY)); \
    } \
  } \
         
  TEST_PIXEL(x0, y0 + radius);
  TEST_PIXEL(x0, y0 - radius);
  TEST_PIXEL(x0 + radius, y0);
  TEST_PIXEL(x0 - radius, y0);
  while(x < y) {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    TEST_PIXEL(x0 + x, y0 + y);
    TEST_PIXEL(x0 - x, y0 + y);
    TEST_PIXEL(x0 + x, y0 - y);
    TEST_PIXEL(x0 - x, y0 - y);
    TEST_PIXEL(x0 + y, y0 + x);
    TEST_PIXEL(x0 - y, y0 + x);
    TEST_PIXEL(x0 + y, y0 - x);
    TEST_PIXEL(x0 - y, y0 - x);
  }
  
  return 0;
}

int PF::Polygon::get_nearest_pixel_distance2(int pixelX, int pixelY, float* buffer, VipsRect& rc)
{
//  std::cout<<"PF::Polygon::get_nearest_pixel_distance2() "<<std::endl;
  
  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;

  float distance = 0;
  const int max_radius = std::max(rc.width, rc.height);
  int incY = 0;
  float *buf = buffer + ((pixelY-IMAGE_TOP)*rc.width);
  float *buf_top = NULL;
  float *buf_bot = NULL;
  
  for ( int radius = 1; radius < max_radius; radius++ ) {
//    std::cout<<"PF::Polygon::get_nearest_pixel_distance2() 1"<<std::endl;
    
/*    if ( pixelX+radius < IMAGE_RIGHT && buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)+radius] == 1.f ) {
      return distance2(pixelX, pixelY, pixelX+radius, pixelY);
    }
    if ( pixelX-radius > IMAGE_LEFT && buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)-radius] == 1.f ) {
      return distance2(pixelX, pixelY, pixelX-radius, pixelY);
    }
    if ( pixelY+radius < IMAGE_BOT && buffer[((pixelY-IMAGE_TOP+radius)*rc.width)+(pixelX-IMAGE_LEFT)] == 1.f ) {
      return distance2(pixelX, pixelY, pixelX, pixelY+radius);
    }
    if ( pixelY-radius > IMAGE_TOP && buffer[((pixelY-IMAGE_TOP-radius)*rc.width)+(pixelX-IMAGE_LEFT)] == 1.f ) {
      return distance2(pixelX, pixelY, pixelX, pixelY-radius);
    }
    
    if ( pixelY-radius > IMAGE_TOP )
      buf_top = buffer + ((pixelY-IMAGE_TOP-radius)*rc.width);
    else
      buf_top = NULL;
    if ( pixelY+radius < IMAGE_BOT )
      buf_bot = buffer + ((pixelY-IMAGE_TOP+radius)*rc.width);
    else
      buf_bot = NULL;
    
//    std::cout<<"PF::Polygon::get_nearest_pixel_distance2() 2"<<std::endl;
    
    for ( int i = 1; i <= radius; i++ ) {
      if ( pixelX+i < IMAGE_RIGHT ) {
        if ( pixelY-radius > IMAGE_TOP && buf_top[(pixelX-IMAGE_LEFT)+i] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX+i, pixelY);
          break;
        }
        if ( pixelY+radius < IMAGE_BOT && buf_bot[(pixelX-IMAGE_LEFT)+i] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX+i, pixelY);
          break;
        }
      }
      if ( pixelX-i > IMAGE_LEFT ) {
        if ( pixelY-radius > IMAGE_TOP && buf_top[(pixelX-IMAGE_LEFT)-i] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX-i, pixelY);
          break;
        }
        if ( pixelY+radius < IMAGE_BOT && buf_bot[(pixelX-IMAGE_LEFT)-i] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX-i, pixelY);
          break;
        }
      }
      incY += rc.width;
      if ( pixelY+incY < IMAGE_BOT ) {
        if ( pixelY+radius < IMAGE_BOT && buffer[((pixelY-IMAGE_TOP+incY)*rc.width)+(pixelX-IMAGE_LEFT)+radius] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX+radius, pixelY+incY);
          break;
        }
        if ( pixelX-radius > IMAGE_LEFT && buffer[((pixelY-IMAGE_TOP+incY)*rc.width)+(pixelX-IMAGE_LEFT)-radius] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX-radius, pixelY+incY);
          break;
        }
      }
      if ( pixelY-incY > IMAGE_TOP ) {
        if ( pixelY+radius < IMAGE_BOT && buffer[((pixelY-IMAGE_TOP-incY)*rc.width)+(pixelX-IMAGE_LEFT)+radius] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX+radius, pixelY-incY);
          break;
        }
        if ( pixelX-radius > IMAGE_LEFT && buffer[((pixelY-IMAGE_TOP-incY)*rc.width)+(pixelX-IMAGE_LEFT)-radius] == 1.f ) {
          return distance2(pixelX, pixelY, pixelX-radius, pixelY-incY);
          break;
        }
      }
    }
    */
    distance = rasterCircle(pixelX, pixelY, radius, buffer, rc);
    if ( distance > 0 ) return distance;
  }
  
//  std::cout<<"PF::Polygon::get_nearest_pixel_distance2() end"<<std::endl;
  
  return distance;
}

float PF::Point::distance2polygon2(std::vector<Point>& poly_pt)
{
  float dist2 = 999999;
  float d;
  
  Point pt1, pt2;
  
  pt1 = poly_pt[0];
  for (int i = 1; i < poly_pt.size(); i++) {
    pt2 = poly_pt[i];
    if ( pt1 != pt2 ) {
      d = distance2segment2(pt1, pt2);
      if ( d < dist2 ) dist2 = d;
      
      pt1 = pt2;
    }
  }
  pt2 = poly_pt[0];
  if ( pt1 != pt2 ) {
    d = distance2segment2(pt1, pt2);
    if ( d < dist2 ) dist2 = d;
    
    pt1 = pt2;
  }

  return dist2;
}

void PF::Polygon::fill_polygon_falloff3(std::vector<Point>& poly_pt, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  std::cout<<"PF::Polygon::fill_polygon_falloff3() "<<std::endl;
  Point pt;
  int  pixelX, pixelY, i, j;
  const int polyCorners = poly_pt.size();
  std::vector<int> nodeX;
  float f;
  float falloff2 = falloff*falloff;
  float falloff2_1 = 1.f / falloff2;

  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  int y_from = IMAGE_BOT;
  int y_to = IMAGE_TOP;
  for ( int ii = 0; ii < poly_pt.size(); ii++ ) {
    y_from = std::min(y_from, (int)poly_pt[ii].y);
    y_to = std::max(y_to, (int)poly_pt[ii].y);
  }
  y_from = std::max(y_from, IMAGE_TOP);
  y_to = std::min(y_to+1, IMAGE_BOT);
  
  //  Loop through the rows of the image.
  for (pixelY=y_from; pixelY<y_to; pixelY++) {

    //  Build a list of nodes.
        nodeX.clear();
    j=polyCorners-1;
    for (i=0; i<polyCorners; i++) {
      if ( (poly_pt[i].y<(float) pixelY && poly_pt[j].y>=(float) pixelY)
      ||  (poly_pt[j].y<(float) pixelY && poly_pt[i].y>=(float) pixelY) ) {
        nodeX.push_back( (int) ( poly_pt[i].x + ((float)(pixelY-poly_pt[i].y) / (float)(poly_pt[j].y-poly_pt[i].y)) 
        * (float)(poly_pt[j].x-poly_pt[i].x) ) ); 
      }
      j=i; 
    }

    //  Sort the nodes, via a simple “Bubble” sort.
    bubble_sort(nodeX);

    //  Fill the pixels between node pairs.
    float *buf = buffer + ((pixelY-IMAGE_TOP)*rc.width);
    
    const int vec_size = nodeX.size();
    
    std::cout<<"PF::Polygon::fill_polygon_falloff3() vec_size: "<<vec_size<<std::endl;
    
    if ( vec_size > 0 ) {
      i = 0;
      nodeX[i] = std::min(nodeX[i], IMAGE_RIGHT-1);
      for (pixelX = IMAGE_LEFT; pixelX <= nodeX[i]; pixelX++) {
        pt.set(pixelX, pixelY);
        f = pt.distance2polygon2(poly_pt);
//        f = get_nearest_pixel_distance2(pixelX, pixelY, buffer, rc);
        if ( f <= falloff2 ) {
          f = scurve.get_value( 1.f - (f*falloff2_1) ) * opacity;
          buf[(pixelX-IMAGE_LEFT)] = f;
        }
      }
    }
    else {
      for (pixelX = IMAGE_LEFT; pixelX < IMAGE_RIGHT; pixelX++) {
        pt.set(pixelX, pixelY);
        f = pt.distance2polygon2(poly_pt);
//        f = get_nearest_pixel_distance2(pixelX, pixelY, buffer, rc);
        if ( f <= falloff2 ) {
          f = scurve.get_value( 1.f - (f*falloff2_1) ) * opacity;
          buf[(pixelX-IMAGE_LEFT)] = f;
        }
      }
    }
    std::cout<<"PF::Polygon::fill_polygon_falloff3() 2"<<std::endl;
    
    for (i=1; i<vec_size-2; i+=2) {
      if   (nodeX[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX[i+1]>= IMAGE_LEFT ) {
        if (nodeX[i  ]< IMAGE_LEFT ) nodeX[i  ]=IMAGE_LEFT ;
        if (nodeX[i+1]>= IMAGE_RIGHT) nodeX[i+1]=IMAGE_RIGHT-1;
        for (pixelX=nodeX[i]; pixelX<=nodeX[i+1]; pixelX++) {
          pt.set(pixelX, pixelY);
          f = pt.distance2polygon2(poly_pt);
//          f = get_nearest_pixel_distance2(pixelX, pixelY, buffer, rc);
          if ( f <= falloff2 ) {
            f = scurve.get_value( 1.f - (f*falloff2_1) ) * opacity;
            buf[(pixelX-IMAGE_LEFT)] = f;
          }
        }
      }
    }
    
    std::cout<<"PF::Polygon::fill_polygon_falloff3() 3"<<std::endl;
    
    i = vec_size-1;
    if ( i > 0 ) {
      nodeX[i] = std::max(nodeX[i], IMAGE_LEFT);
      for (pixelX = nodeX[i]; pixelX < IMAGE_RIGHT; pixelX++) {
        pt.set(pixelX, pixelY);
        f = pt.distance2polygon2(poly_pt);
//        f = get_nearest_pixel_distance2(pixelX, pixelY, buffer, rc);
        if ( f <= falloff2 ) {
          f = scurve.get_value( 1.f - (f*falloff2_1) ) * opacity;
          buf[(pixelX-IMAGE_LEFT)] = f;
        }
      }
    }
  }
  std::cout<<"PF::Polygon::fill_polygon_falloff3() end"<<std::endl;

}

void PF::Polygon::fill_segment_falloff(std::vector<Point>& poly_pt, Point& pt1, Point& pt2, float falloff2, float falloff2_1, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY, i, j;
  float f;
  Point pt;
  const int polyCorners = poly_pt.size();
  std::vector<int> nodeX;
  
  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  int y_from = IMAGE_BOT;
  int y_to = IMAGE_TOP;
  for ( int ii = 0; ii < poly_pt.size(); ii++ ) {
    y_from = std::min(y_from, (int)poly_pt[ii].y);
    y_to = std::max(y_to, (int)poly_pt[ii].y);
  }
  y_from = std::max(y_from, IMAGE_TOP);
  y_to = std::min(y_to+1, IMAGE_BOT);
  
  //  Loop through the rows of the image.
  for (pixelY=y_from; pixelY<y_to; pixelY++) {

    //  Build a list of nodes.
        nodeX.clear();
    j=polyCorners-1;
    for (i=0; i<polyCorners; i++) {
      if ( (poly_pt[i].y<(float) pixelY && poly_pt[j].y>=(float) pixelY)
      ||  (poly_pt[j].y<(float) pixelY && poly_pt[i].y>=(float) pixelY) ) {
        nodeX.push_back( (int) ( poly_pt[i].x + ((float)(pixelY-poly_pt[i].y) / (float)(poly_pt[j].y-poly_pt[i].y)) 
        * (float)(poly_pt[j].x-poly_pt[i].x) ) ); 
      }
      j=i; 
    }

    //  Sort the nodes, via a simple “Bubble” sort.
    bubble_sort(nodeX);

    //  Fill the pixels between node pairs.
    
    float *buf = buffer + ((pixelY-IMAGE_TOP)*rc.width);
    for (i=0; i<nodeX.size(); i+=2) {
      if   (nodeX[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX[i+1]> IMAGE_LEFT ) {
        if (nodeX[i  ]< IMAGE_LEFT ) nodeX[i  ]=IMAGE_LEFT ;
        if (nodeX[i+1]> IMAGE_RIGHT) nodeX[i+1]=IMAGE_RIGHT;
        for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) {
          if ( buf[(pixelX-IMAGE_LEFT)] < 1.f ) {
            pt.set(pixelX, pixelY);
            f = pt.distance2segment2(pt1, pt2);
            if ( f <= falloff2 ) {
              f = scurve.get_value( 1.f - (f*falloff2_1) ) * opacity;
  //            f = ( 1.f - (f*falloff2_1) ) * opacity;
  //            buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = std::max( f, buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] );
              buf[(pixelX-IMAGE_LEFT)] = std::max( f, buf[(pixelX-IMAGE_LEFT)] );
            }
          }
        }
      }
    }
 
  }

}

void PF::Polygon::fill_point_falloff(std::vector<Point>& poly_pt, Point& pt1, float falloff2, float falloff2_1, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY, i, j;
  float f;
  Point pt;
  const int polyCorners = poly_pt.size();
  std::vector<int> nodeX;
  
  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;
  
  int y_from = IMAGE_BOT;
  int y_to = IMAGE_TOP;
  for ( int ii = 0; ii < poly_pt.size(); ii++ ) {
    y_from = std::min(y_from, (int)poly_pt[ii].y);
    y_to = std::max(y_to, (int)poly_pt[ii].y);
  }
  y_from = std::max(y_from, IMAGE_TOP);
  y_to = std::min(y_to+1, IMAGE_BOT);
  
  //  Loop through the rows of the image.
  for (pixelY=y_from; pixelY<y_to; pixelY++) {

    //  Build a list of nodes.
        nodeX.clear();
    j=polyCorners-1;
    for (i=0; i<polyCorners; i++) {
      if ( (poly_pt[i].y<(float) pixelY && poly_pt[j].y>=(float) pixelY)
      ||  (poly_pt[j].y<(float) pixelY && poly_pt[i].y>=(float) pixelY) ) {
        nodeX.push_back( (int) ( poly_pt[i].x + ((float)(pixelY-poly_pt[i].y) / (float)(poly_pt[j].y-poly_pt[i].y)) 
        * (float)(poly_pt[j].x-poly_pt[i].x) ) ); 
      }
      j=i; 
    }

    //  Sort the nodes, via a simple “Bubble” sort.
    bubble_sort(nodeX);

    //  Fill the pixels between node pairs.
    
    float *buf = buffer + ((pixelY-IMAGE_TOP)*rc.width);
    for (i=0; i<nodeX.size(); i+=2) {
      if   (nodeX[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX[i+1]> IMAGE_LEFT ) {
        if (nodeX[i  ]< IMAGE_LEFT ) nodeX[i  ]=IMAGE_LEFT ;
        if (nodeX[i+1]> IMAGE_RIGHT) nodeX[i+1]=IMAGE_RIGHT;
        for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) {
          if ( buf[(pixelX-IMAGE_LEFT)] < 1.f ) {
            pt.set(pixelX, pixelY);
            f = pt.distance2(pt1);
            if ( f <= falloff2 ) {
              f = scurve.get_value( 1.f - (f*falloff2_1) ) * opacity;
  //            f = ( 1.f - (f*falloff2_1) ) * opacity;
  //            buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = std::max( f, buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] );
              buf[(pixelX-IMAGE_LEFT)] = std::max( f, buf[(pixelX-IMAGE_LEFT)] );
            }
          }
        }
      }
    }
 
  }

}

void PF::Polygon::draw_segment(std::vector<Point>& poly_pt_in, std::vector<Point>& poly_pt_out, Point& pt1, Point& pt2, 
    float pen_size2, float falloff2, float falloff2_1, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  int  pixelX, pixelY, i, j;
  float f;
  Point pt;
  const int polyCorners_in = poly_pt_in.size();
  const int polyCorners_out = poly_pt_out.size();
  std::vector<int> nodeX_in;
  std::vector<int> nodeX_out;

  const int IMAGE_TOP = rc.top;
  const int IMAGE_LEFT = rc.left;
  const int IMAGE_BOT = IMAGE_TOP + rc.height;
  const int IMAGE_RIGHT = IMAGE_LEFT + rc.width;

  int y_from = IMAGE_BOT;
  int y_to = IMAGE_TOP;
  for ( int ii = 0; ii < poly_pt_out.size(); ii++ ) {
    y_from = std::min(y_from, (int)poly_pt_out[ii].y);
    y_to = std::max(y_to, (int)poly_pt_out[ii].y);
  }
  y_from = std::max(y_from, IMAGE_TOP);
  y_to = std::min(y_to+1, IMAGE_BOT);

   y_to = IMAGE_BOT;
   y_from = IMAGE_TOP;

  //  Loop through the rows of the image.
  for (pixelY=y_from; pixelY<y_to; pixelY++) {

    //  Build a list of nodes.
    nodeX_in.clear();
    nodeX_out.clear();

    j=polyCorners_in-1;
    for (i=0; i<polyCorners_in; i++) {
      if ( (poly_pt_in[i].y<(float) pixelY && poly_pt_in[j].y>=(float) pixelY)
          ||  (poly_pt_in[j].y<(float) pixelY && poly_pt_in[i].y>=(float) pixelY) ) {
        int x = (int) ( poly_pt_in[i].x + ((float)(pixelY-poly_pt_in[i].y) / (float)(poly_pt_in[j].y-poly_pt_in[i].y)) 
            * (float)(poly_pt_in[j].x-poly_pt_in[i].x) );
        nodeX_in.push_back( x ); 
        nodeX_out.push_back( x ); 
      }
      j=i; 
    }

    j=polyCorners_out-1;
    for (i=0; i<polyCorners_out; i++) {
      if ( (poly_pt_out[i].y<(float) pixelY && poly_pt_out[j].y>=(float) pixelY)
          ||  (poly_pt_out[j].y<(float) pixelY && poly_pt_out[i].y>=(float) pixelY) ) {
        nodeX_out.push_back( (int) ( poly_pt_out[i].x + ((float)(pixelY-poly_pt_out[i].y) / (float)(poly_pt_out[j].y-poly_pt_out[i].y)) 
            * (float)(poly_pt_out[j].x-poly_pt_out[i].x) ) ); 
      }
      j=i; 
    }

    //  Sort the nodes, via a simple “Bubble” sort.
    bubble_sort(nodeX_in);
    bubble_sort(nodeX_out);

    //  Fill the pixels between node pairs.
    float *buf = buffer + ((pixelY-IMAGE_TOP)*rc.width);
    for (i=0; i<nodeX_in.size(); i+=2) {
      if   (nodeX_in[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX_in[i+1]> IMAGE_LEFT ) {
        if (nodeX_in[i  ]< IMAGE_LEFT ) nodeX_in[i  ]=IMAGE_LEFT ;
        if (nodeX_in[i+1]> IMAGE_RIGHT) nodeX_in[i+1]=IMAGE_RIGHT;
        for (pixelX=nodeX_in[i]; pixelX<nodeX_in[i+1]; pixelX++) {
//          buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = opacity;
          buf[(pixelX-IMAGE_LEFT)] = opacity;
        }
      }
    }
  

    //  Fill the falloff between node pairs.
    for (i=0; i<nodeX_out.size(); i+=2) {
      if   (nodeX_out[i  ]>=IMAGE_RIGHT) break;
      if   (nodeX_out[i+1]> IMAGE_LEFT ) {
        if (nodeX_out[i  ]< IMAGE_LEFT ) nodeX_out[i  ]=IMAGE_LEFT ;
        if (nodeX_out[i+1]> IMAGE_RIGHT) nodeX_out[i+1]=IMAGE_RIGHT;
        for (pixelX=nodeX_out[i]; pixelX<nodeX_out[i+1]; pixelX++) {
          pt.set(pixelX, pixelY);
          f = pt.distance2segment2(pt1, pt2);
          if ( f <= pen_size2 ) {
//            buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = opacity;
            buf[(pixelX-IMAGE_LEFT)] = opacity;
          }
          else if ( f <= falloff2 ) {
            f = scurve.get_value( 1.f - ((f-pen_size2)*falloff2_1) ) * opacity;
//            buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] = std::max( f, buffer[((pixelY-IMAGE_TOP)*rc.width)+(pixelX-IMAGE_LEFT)] );
            buf[(pixelX-IMAGE_LEFT)] = std::max( f, buf[(pixelX-IMAGE_LEFT)] );
          }
        }
      }
    }
  }

}

void PF::Polygon::fill_polygon_falloff2(SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  if ( get_closed_shape() ) {
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
    bool clock = is_clockwise();
    float pen_size = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
    float expand = pen_size+get_falloff();
    bool first_time = true;
    float falloff2 = get_falloff()*get_falloff();
    float falloff2_1 = 1.f / falloff2;

    //    std::cout<<"PF::Polygon::get_falloff_points(): clock: "<<clock<<std::endl;

/*    get_segment(get_segments_count()-1, pts1, pts2, si);
    get_pt2_proyected_from_segment(pts1, pts2, expand, pt1, pt2);
    if ( clock )
      pt4_prev = pt1;
    else
      pt4_prev = pt2;
*/
    for (int s = 0; s < get_segments_count(); s++) {
      get_segment(s, pts1, pts2, si);
      vec_pt.clear();

      if (si.get_segment_type() == PF::SegmentInfo::line) {
        vec_pt.push_back(pts1);
        vec_pt.push_back(pts2);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        inter_bezier3(pts1, pts2, si.get_control_pt1(), vec_pt);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        inter_bezier4(pts1, pts2, si.get_control_pt1(), si.get_control_pt2(), vec_pt);
      }
      
      fill_polygon_segment_falloff(vec_pt, pt1, pt2, clock, expand, get_opacity(), scurve, buffer, rc);

      if ( !first_time ) {
        vec_pt.clear();
          if ( generate_arc_points(pts1, pt2_prev, pt1, vec_pt, clock) ) {
            vec_pt.push_back( pts1 );
            fill_point_falloff(vec_pt, pts1, falloff2, falloff2_1, get_opacity(), scurve, buffer, rc);
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
    get_segment(0, pts1, pts2, si);
      if ( generate_arc_points(pts1, pt2_prev, pt1_first, vec_pt, clock) ) {
        vec_pt.push_back( pts1 );
        fill_point_falloff(vec_pt, pts1, falloff2, falloff2_1, get_opacity(), scurve, buffer, rc);
      }

  }

}



void PF::Polygon::fill_polygon_segment_falloff(std::vector<Point>& points, Point& pt3_first, Point& pt4_prev, bool clockwise, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
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
          get_pt1_2_proyected_from_segment(pt1, pt2, expand, pt3, pt4);
        else
          get_pt3_4_proyected_from_segment(pt1, pt2, expand, pt3, pt4);
        
        vec_pt.clear();
        vec_pt.push_back( pt3 );
        vec_pt.push_back( pt4 );
        vec_pt.push_back( pt2 );
        vec_pt.push_back( pt1 );

        fill_segment_falloff(vec_pt, pt1, pt2, falloff2, falloff2_1, opacity, scurve, buffer, rc);

        if ( !first_time ) {
          vec_pt.clear();
//          if ( generate_arc_points(pt1, pt4_prev, pt3, vec_pt, clockwise) ) {
//            vec_pt.push_back( pt1 );
//            fill_point_falloff(vec_pt, pt1, falloff2, falloff2_1, opacity, scurve, buffer, rc);
//          }
          
          mid_pt.line_mid_point(pt4_prev, pt3);
          expand_segment(pt1, mid_pt, pt_expand, expand);
          
          vec_pt.push_back( pt4_prev );
          vec_pt.push_back( pt_expand );
          vec_pt.push_back( pt3 );
          vec_pt.push_back( pt1 );
            fill_point_falloff(vec_pt, pt1, falloff2, falloff2_1, opacity, scurve, buffer, rc);
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

void PF::Polygon::fill_polygon_falloff(std::vector<Point>& points, bool clockwise, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
//  std::cout<<"PF::Polygon::fill_polygon_falloff(): clockwise: "<<clockwise<<std::endl;
  
//  if ( get_closed_shape() ) {
  bool closed_shape = true;
//    bool first, intersects;
//    int i;
    std::vector<Point> vec_pt;
    Point pt1, pt2, pt3, pt4, pt4_prev, pt_expand, mid_pt;
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

    if ( closed_shape ) {
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
    
    pt1 = points[0];
    for (int i = 1; i < points.size(); i++) {
      pt2 = points[i];
      if ( pt1 != pt2 ) {
       
        if ( clockwise )
          get_pt1_2_proyected_from_segment(pt1, pt2, expand, pt3, pt4);
        else
          get_pt3_4_proyected_from_segment(pt1, pt2, expand, pt3, pt4);
        
        vec_pt.clear();
        vec_pt.push_back( pt3 );
        vec_pt.push_back( pt4 );
        vec_pt.push_back( pt2 );
        vec_pt.push_back( pt1 );

        fill_segment_falloff(vec_pt, pt1, pt2, falloff2, falloff2_1, opacity, scurve, buffer, rc);

          vec_pt.clear();
//          if ( generate_arc_points(pt1, pt4_prev, pt3, vec_pt, clockwise) ) {
//            vec_pt.push_back( pt1 );
//            fill_point_falloff(vec_pt, pt1, falloff2, falloff2_1, opacity, scurve, buffer, rc);
//          }
          
          mid_pt.line_mid_point(pt4_prev, pt3);
          expand_segment_to(pt1, mid_pt, pt_expand, expand);
          
          vec_pt.push_back( pt4_prev );
          vec_pt.push_back( pt_expand );
          vec_pt.push_back( pt3 );
          vec_pt.push_back( pt1 );
            fill_point_falloff(vec_pt, pt1, falloff2, falloff2_1, opacity, scurve, buffer, rc);
          

        pt1 = pt2;
        pt4_prev = pt4;
      }
    }



    if ( closed_shape ) {
      points.pop_back();
    }

}


/*
void PF::Polygon::fill_polygon_falloff(std::vector<Point>& points, float falloff, float opacity, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  Point pt1, pt2, pt3, b, c;
  std::vector<Point> vec_pt;
  float falloff2 = falloff*falloff;
  float falloff2_1 = 1.f / falloff2;
  
  pt1 = points[0];
  for (int i = 1; i < points.size(); i++) {
    pt2 = points[i];
    if ( pt1 != pt2 ) {
      vec_pt.clear();
      
      expand_segment(pt1, pt2, pt3, falloff);
      get_pt2_proyected_from_segment(pt2, pt3, falloff, b, c);
      vec_pt.push_back( b );
      vec_pt.push_back( c );
      
      expand_segment(pt2, pt1, pt3, falloff);
      get_pt1_proyected_from_segment(pt3, pt1, falloff, b, c);
      vec_pt.push_back( c );
      vec_pt.push_back( b );
      
      fill_segment_falloff(vec_pt, pt1, pt2, falloff2, falloff2_1, opacity, scurve, buffer, rc);
      
      pt1 = pt2;
    }
  }
  
  pt2 = points[0];
  if ( pt1 != pt2 ) {
    vec_pt.clear();
    
    expand_segment(pt1, pt2, pt3, falloff);
    get_pt2_proyected_from_segment(pt2, pt3, falloff, b, c);
    vec_pt.push_back( b );
    vec_pt.push_back( c );
    
    expand_segment(pt2, pt1, pt3, falloff);
    get_pt1_proyected_from_segment(pt3, pt1, falloff, b, c);
    vec_pt.push_back( c );
    vec_pt.push_back( b );
    
    fill_segment_falloff(vec_pt, pt1, pt2, falloff2, falloff2_1, opacity, scurve, buffer, rc);
  }
  
}
*/
void PF::Polygon::draw_polygon_segments(std::vector<Point>& points, float pen_size, float falloff, float opacity, bool closed_shape, SplineCurve& scurve, float* buffer, VipsRect& rc)
{
  Point pt1, pt2, pt3, a, b, c, d;
  std::vector<Point> vec_pt_in;
  std::vector<Point> vec_pt_out;

  pen_size /= 2.f;
  float falloff2 = (falloff+pen_size)*(falloff+pen_size);
  float pen_size2 = pen_size*pen_size;
  float falloff2_1 = (falloff2!=pen_size2) ? 1.f / (falloff2-pen_size2): 0.f;

  pt1 = points[0];
  for (int i = 1; i < points.size(); i++) {
    pt2 = points[i];
    if ( pt1 != pt2 ) {
      vec_pt_out.clear();
      vec_pt_in.clear();

      expand_segment(pt1, pt2, pt3, falloff+pen_size);
      get_pt2_proyected_from_segment(pt2, pt3, falloff+pen_size, b, c);
      vec_pt_out.push_back( b );
      vec_pt_out.push_back( c );

      expand_segment(pt2, pt1, pt3, falloff+pen_size);
      get_pt1_proyected_from_segment(pt3, pt1, falloff+pen_size, b, c);
      vec_pt_out.push_back( c );
      vec_pt_out.push_back( b );

      get_expanded_rec_from_line(pt1, pt2, pen_size, a, b, c, d);
      vec_pt_in.push_back( a );
      vec_pt_in.push_back( b );
      vec_pt_in.push_back( c );
      vec_pt_in.push_back( d );

      draw_segment(vec_pt_in, vec_pt_out, pt1, pt2, pen_size2, falloff2, falloff2_1, opacity, scurve, buffer, rc);

      pt1 = pt2;
    }
  }

  if ( closed_shape ) {
    pt2 = points[0];
    if ( pt1 != pt2 ) {
      vec_pt_out.clear();
      vec_pt_in.clear();

      expand_segment(pt1, pt2, pt3, falloff+pen_size);
      get_pt2_proyected_from_segment(pt2, pt3, falloff+pen_size, b, c);
      vec_pt_out.push_back( b );
      vec_pt_out.push_back( c );

      expand_segment(pt2, pt1, pt3, falloff+pen_size);
      get_pt1_proyected_from_segment(pt3, pt1, falloff+pen_size, b, c);
      vec_pt_out.push_back( c );
      vec_pt_out.push_back( b );

      get_expanded_rec_from_line(pt1, pt2, pen_size, a, b, c, d);
      vec_pt_in.push_back( a );
      vec_pt_in.push_back( b );
      vec_pt_in.push_back( c );
      vec_pt_in.push_back( d );

      draw_segment(vec_pt_in, vec_pt_out, pt1, pt2, pen_size2, falloff2, falloff2_1, opacity, scurve, buffer, rc);
    }

  }
}


// returns a vector with bezier curve points
void PF::Polygon::inter_bezier3(Point& anchor1, Point& anchor2, Point& control, std::vector<Point>& points)
{
  
  curve3_div curve3;
  curve3.init(anchor1.x, anchor1.y, control.x, control.y, anchor2.x, anchor2.y);
  for (int i = 0; i < curve3.m_points.size(); i++) points.push_back(curve3.m_points[i]);
/*  return;
  
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
  */
}

void PF::Polygon::inter_bezier4(Point& anchor1, Point& anchor2, Point& control1, Point& control2, std::vector<Point>& points)
{
  curve4_div curve4;
  curve4.init(anchor1.x, anchor1.y, control1.x, control1.y, control2.x, control2.y, anchor2.x, anchor2.y);
  for (int i = 0; i < curve4.m_points.size(); i++) points.push_back(curve4.m_points[i]);
/*  return;

  
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
  */
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
    if ( !get_closed_shape() )
      points.push_back(pt2);
  }
}

// From http://cs.smith.edu/~orourke/Code/polyorient.C
// FindLR finds the lowest, rightmost vertex of poly.
int PF::Polygon::FindLR( std::vector<Point>& poly )
{
  int m;
  Point min;

  min.x = poly[0].x;
  min.y = poly[0].y;
  m = 0;

  for( int i = 0; i < poly.size(); i++ ) {
    if( (poly[i].y < min.y) ||
        ( (poly[i].y == min.y) && (poly[i].x > min.x) )
      ) {
      m = i;
      min.x = poly[m].x;
      min.y = poly[m].y;
    }
  }

  return m;
}

int PF::Polygon::Ccw( std::vector<Point>& poly, int m )
{
  int m1;
  float area2;
  int i;
  Point a, b, c; //Just renaming

  m1 = circ_idx(m-1, poly.size()); //= m - 1

  a.x = poly[m1].x;
  b.x = poly[m].x;
  c.x = poly[circ_idx(m+1, poly.size())].x;
  
  a.y = poly[m1].y;
  b.y = poly[m].y;
  c.y = poly[circ_idx(m+1, poly.size())].y;


  area2 =
                a.x * b.y - a.y * b.x +
                a.y * c.x - a.x * c.y +
                b.x * c.y - c.x * b.y;

  if ( area2 > 0 )
    return 1;
  else if ( area2 < 0 )
    return -1;
  else
    return 0;
}

bool PF::Polygon::is_clockwise()
{
  return ( Ccw(get_points(), FindLR(get_points())) == 1 );
}

// From Darktable path.c _path_points_recurs_border_gaps()
bool PF::Polygon::generate_arc_points(Point& cmax, Point& bmin, /*Point& bmin2,*/ Point& bmax, std::vector<Point>& points, bool clockwise)
{
  Point pt;
  // we want to find the start and end angles
  double a1 = atan2(bmin.y - cmax.y, bmin.x - cmax.x);
  double a2 = atan2(bmax.y - cmax.y, bmax.x - cmax.x);
  if(a1 == a2) {
    points.push_back(bmin);
    points.push_back(bmax);
    return true;
  }

  // we have to be sure that we turn in the correct direction
  if(a2 < a1 && clockwise)
  {
    a2 += 2 * M_PI;
  }
  if(a2 > a1 && !clockwise)
  {
    a1 += 2 * M_PI;
  }


//  std::cout<<"PF::Polygon::generate_arc_points(): a1: "<<a1<<", a2: "<<a2<<std::endl;
  if ( std::abs(a2-a1) > (M_PI) ) {
//    std::cout<<"PF::Polygon::generate_arc_points(): abs(a2-a1) > (M_PI*1.f): "<<abs(a2-a1)<<", > (M_PI*1.f): "<<(M_PI*1.f)<<std::endl;
    points.push_back(bmin);
    points.push_back(bmax);
//    return true;
    return false;
  }

  
  // we determine start and end radius too
  float r1 = sqrtf((bmin.y - cmax.y) * (bmin.y - cmax.y) + (bmin.x - cmax.x) * (bmin.x - cmax.x));
  float r2 = sqrtf((bmax.y - cmax.y) * (bmax.y - cmax.y) + (bmax.x - cmax.x) * (bmax.x - cmax.x));

  // and the max length of the circle arc
  int l;
  if(a2 > a1)
    l = (a2 - a1) * fmaxf(r1, r2);
  else
    l = (a1 - a2) * fmaxf(r1, r2);
  if(l < 2) // return false;
  {
    points.push_back(bmin);
    points.push_back(bmax);
    return true;
  }

  // and now we add the points
  float incra = (a2 - a1) / l;
  float incrr = (r2 - r1) / l;
  float rr = r1 + incrr;
  float aa = a1 + incra;
  
  points.push_back(bmin);
  for(int i = 1; i < l; i++)
  {
    pt.set( cmax.x + rr * cosf(aa), cmax.y + rr * sinf(aa));
    points.push_back(pt);
    rr += incrr;
    aa += incra;
  }
  points.push_back(bmax);
  
  return true;
}

void PF::Polygon::get_falloff_points(std::vector<Point>& points)
{
  if ( get_closed_shape() ) {
    bool first, intersects;
    int i;
    std::vector<Point> vec_pt;
    Point pt1, pt2;
    Point pts1, pts2;
    Point a, b, c, d;
    Point pt_end, pt_start, pt_intersection;
    SegmentInfo si;
    bool clock = is_clockwise();
    float pen_size = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
    float expand = pen_size+get_falloff();

    //    std::cout<<"PF::Polygon::get_falloff_points(): clock: "<<clock<<std::endl;

    for (int s = 0; s < get_segments_count(); s++) {
      get_segment(s, pts1, pts2, si);
      vec_pt.clear();

      if (si.get_segment_type() == PF::SegmentInfo::line) {
        vec_pt.push_back(pts1);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l || si.get_segment_type() == PF::SegmentInfo::bezier3_r) {
        inter_bezier3(pts1, pts2, si.get_control_pt1(), vec_pt);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        inter_bezier4(pts1, pts2, si.get_control_pt1(), si.get_control_pt2(), vec_pt);
      }
      vec_pt.push_back(pts2);

      i = 0;
      pt1 = vec_pt[i++];
      if ( points.size() >= 2 ) {
        pt2 = vec_pt[i++];
        while ( (i < vec_pt.size()) && (pt1 == pt2) ) pt2 = vec_pt[i++];
        if ( pt1 != pt2 ) {
          PF::Line::get_expanded_rec_from_line(pt1, pt2, expand, a, b, c, d);

          if ( clock )
            intersects = segments_intersects(points[points.size()-1], points[points.size()-2], a, b, pt_intersection);
          else
            intersects = segments_intersects(points[points.size()-1], points[points.size()-2], c, d, pt_intersection);
          if ( intersects ) {
            points.back() = pt_intersection;

            if ( clock )
              points.push_back(b);
            else
              points.push_back(c);
          }
          else {
            if ( clock ) {
              if ( !generate_arc_points(pt1, points.back(), a, points, clock) )
                points.push_back(a);
              points.push_back(b);
            }
            else {
              if ( !generate_arc_points(pt1, points.back(), d, points, clock) )
                points.push_back(d);
              points.push_back(c);
            }
          }

          pt1 = pt2;
        }
      }

      for (; i < vec_pt.size(); i++) {
        pt2 = vec_pt[i];

        if ( pt1 != pt2 ) {
          PF::Line::get_expanded_rec_from_line(pt1, pt2, expand, a, b, c, d);

          if ( clock ) {
            points.push_back(a);
            points.push_back(b);
          }
          else {
            points.push_back(d);
            points.push_back(c);
          }

          pt1 = pt2;
        }
      }  
    }

    if ( points.size() > 2 ) {
      intersects = segments_intersects(points[points.size()-1], points[points.size()-2], points[0], points[1], pt_intersection);
      if ( intersects ) {
        points.front() = pt_intersection;
        points.pop_back();
      }
      else {
        generate_arc_points(get_point(0), points.back(), points.front(), points, clock);
      }
    }

  }

}

void PF::Polygon::add_point(Point pt)
{
  PF::Shape::add_point(pt);
  
  SegmentInfo si;
  s_info.push_back(si);
}

int PF::Polygon::insert_point(Point& pt, int position)
{
  int new_point = PF::Shape::insert_point(pt, position);
  
  SegmentInfo si;
  
  std::vector<SegmentInfo>::iterator it;
  it = s_info.begin();
  it = s_info.insert( it+position, si );
  
  return new_point;
}

int PF::Polygon::insert_point(Point& pt, int hit_t, int additional)
{
  int new_point = -1;
  
  if (hit_t == PF::Shape::hit_segment) {
    new_point = insert_point(pt, additional+1);
  }
  else {
    std::cout<<"PF::Polygon::insert_point(): invalid hit_test: "<<hit_t<<std::endl;  
  }
  
  return new_point;
}

int PF::Polygon::insert_control_point(Point& pt, int hit_t, int additional, float t)
{
  int new_hit = -1;
  const int node = additional;
  const int segment = node;
  const int segment_prev = (segment>0) ? segment-1: get_segments_count()-1;
  SegmentInfo& si = get_segment_info(segment);
  SegmentInfo& si_prev = get_segment_info(segment_prev);
  
  if (hit_t == PF::Shape::hit_node) {
      if (si.get_segment_type() == PF::SegmentInfo::line) { // add a right control pt to the node
        set_segment_type( segment, PF::SegmentInfo::bezier3_l );
        new_hit = PF::SegmentInfo::bezier3_l;
        default_control_pt1(segment, t);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) { // add a right control pt to the node
        set_segment_type( segment, PF::SegmentInfo::bezier4 );
        set_control_pt2( segment, get_control_pt1(segment) );
        new_hit = PF::SegmentInfo::bezier4;
        default_control_pt1(segment, t);
      }
      else if (si_prev.get_segment_type() == PF::SegmentInfo::bezier3_l) { // add a left control pt to the node
        set_segment_type( segment_prev, PF::SegmentInfo::bezier4 );
        new_hit = PF::SegmentInfo::bezier4;
        default_control_pt2(segment_prev, t);
      }
      else if (si_prev.get_segment_type() == PF::SegmentInfo::line) { // add a left control pt to the node
        set_segment_type( segment_prev, PF::SegmentInfo::bezier3_r );
        new_hit = PF::SegmentInfo::bezier3_r;
        default_control_pt1(segment_prev, t);
      }
  }
  else {
    std::cout<<"PF::Polygon::insert_control_point(): invalid hit_test: "<<hit_t<<std::endl;  
  }

  return new_hit;
}

void PF::Polygon::remove_point(int n)
{
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
  if ( n < get_points_count()-1 )
    pt2 = get_point(n+1);
  else
    pt2 = get_point(0);
}

void PF::Polygon::get_segment(int n, Point& pt1, Point& pt2, SegmentInfo& si)
{
  get_segment(n, pt1, pt2);
  si = get_segment_info(n);
}

void PF::Polygon::offset_point(int n, float x, float y)
{
  PF::Shape::offset_point(n, x, y);
  
  if (get_segment_info(n).get_segment_type() == PF::SegmentInfo::bezier3_l || get_segment_info(n).get_segment_type() == PF::SegmentInfo::bezier4)
    get_segment_info(n).offset_control_pt1(x, y);
  const int segment = (n > 0) ? n-1: get_segments_count()-1;
  if (get_segment_info(segment).get_segment_type() == PF::SegmentInfo::bezier3_r)
    get_segment_info(segment).offset_control_pt1(x, y);
  else if (get_segment_info(segment).get_segment_type() == PF::SegmentInfo::bezier4)
    get_segment_info(segment).offset_control_pt2(x, y);
}

void PF::Polygon::offset_control_pt1(int segment, Point& prev, Point& curr, int options)
{
  get_segment_info(segment).offset_control_pt1(prev, curr);

  if (options != 0) {
    int opt = 0;
    int opt_n = -1;
    Point anchor;

    if (get_segment_type(segment) == PF::SegmentInfo::bezier3_l || get_segment_type(segment) == PF::SegmentInfo::bezier4) {
      const int segment_prev = (segment > 0) ? segment-1: get_segments_count()-1;
      if (get_segment_type(segment_prev) == PF::SegmentInfo::bezier4) {
        opt = 2;
        opt_n = segment_prev;
        anchor = get_point(segment);
      }
      else if (get_segment_type(segment_prev) == PF::SegmentInfo::bezier3_r) {
        opt = 1;
        opt_n = segment_prev;
        anchor = get_point(segment);
      }
    }
    else if (get_segment_type(segment) == PF::SegmentInfo::bezier3_r) {
      const int segment_next = (segment < get_segments_count()-1) ? segment+1: 0;
      if (get_segment_type(segment_next) == PF::SegmentInfo::bezier4) {
        opt = 1;
        opt_n = segment_next;
        anchor = get_point(segment_next);
      }
      else if (get_segment_type(segment_next) == PF::SegmentInfo::bezier3_l) {
        opt = 1;
        opt_n = segment_next;
        anchor = get_point(segment_next);
      }
    }

    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_point(anchor, get_control_pt1(segment), get_control_pt1(opt_n));
      else if (opt == 2)
        synch_control_point(anchor, get_control_pt1(segment), get_control_pt2(opt_n));
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
    const int segment_next = (segment < get_segments_count()-1) ? segment+1: 0;
    
    if (get_segment_type(segment_next) == PF::SegmentInfo::bezier3_l || get_segment_type(segment_next) == PF::SegmentInfo::bezier4) {
      opt = 1;
      opt_n = segment_next;
      anchor = get_point(segment_next);
    }
    
    if (options == PF::SegmentInfo::in_sinch) {
      if (opt == 1)
        synch_control_point(anchor, get_control_pt2(segment), get_control_pt1(opt_n));
      else if (opt == 2)
        synch_control_point(anchor, get_control_pt2(segment), get_control_pt2(opt_n));
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

void PF::Polygon::expand(float n)
{
  PF::Shape::expand(n);
  
  Point center, expand;
  float surf;
  float amount = 1.03;
  if(n<0) amount = 0.97;
  
  get_center(center, surf);
  if(amount < 1.0 && surf < 0.00001 && surf > -0.00001) return;

  for (int i = 0; i < get_segments_count(); i++) {
    SegmentInfo& si = get_segment_info(i);
    
    Point& point = si.get_control_pt1();
    
    expand.x = (point.x - center.x) * amount;
    expand.y = (point.y - center.y) * amount;

    // and we set the new points
    point.x = center.x + expand.x;
    point.y = center.y + expand.y;
    
    Point& point2 = si.get_control_pt2();
    
    expand.x = (point2.x - center.x) * amount;
    expand.y = (point2.y - center.y) * amount;

    // and we set the new points
    point2.x = center.x + expand.x;
    point2.y = center.y + expand.y;
    
  }
  
}

bool PF::Polygon::point_in_polygon(Point& pt)
{
  std::vector<Point> points;
  inter_segments(points);
  return pt.in_polygon(points);
}

bool PF::Polygon::point_in_polygon_falloff(Point& pt)
{
  std::vector<Point> points;
  get_falloff_points(points);
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
  float pen_size = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
  const float delta2 = (pen_size+hit_test_delta)*(pen_size+hit_test_delta);

  for ( int i = 0; i < get_points_count(); i++ ) {
    pt1 = get_point(i);
    
    if ( hit_test_node(pt, pt1) ) {
      additional = i;
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
  
  // check falloff
  if ( hit_t == hit_none ) {
    if (point_in_polygon_falloff(pt)) {
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
      rc->left = std::min(rc->left, (int)get_point(i).x);
      rc->top = std::min(rc->top, (int)get_point(i).y);
      rc->width = std::max(rc->width, (int)get_point(i).x);
      rc->height = std::max(rc->height, (int)get_point(i).y);
    }
  
    for (int i = 0; i < get_segments_count(); i++) {
      SegmentInfo& si = get_segment_info(i);
      if (si.get_segment_type() != PF::SegmentInfo::line) {
        rc->left = std::min(rc->left, (int)si.get_control_pt1().x);
        rc->top = std::min(rc->top, (int)si.get_control_pt1().y);
        rc->width = std::max(rc->width, (int)si.get_control_pt1().x);
        rc->height = std::max(rc->height, (int)si.get_control_pt1().y);
      }
      if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
        rc->left = std::min(rc->left, (int)si.get_control_pt2().x);
        rc->top = std::min(rc->top, (int)si.get_control_pt2().y);
        rc->width = std::max(rc->width, (int)si.get_control_pt2().x);
        rc->height = std::max(rc->height, (int)si.get_control_pt2().y);
      }
    }
  
    rc->width = rc->width-rc->left+1;
    rc->height = rc->height-rc->top+1;
  }
  
  float pen_size = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
  
  rc->left -= pen_size;
  rc->top -= pen_size;
  rc->width += pen_size*2;
  rc->height += pen_size*2;
  
}

void PF::Polygon::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff();
  rc->top -= get_falloff();
  rc->width += get_falloff()*2;
  rc->height += get_falloff()*2;
  
}

void PF::Polygon::get_falloff_rect(Point& pt1, Point& pt2)
{
  get_rect(pt1, pt2);
  
  pt1.x -= get_falloff();
  pt1.y -= get_falloff();
  pt2.x += get_falloff();
  pt2.y += get_falloff();
  
}

void PF::Polygon::get_rect(Point& pt1, Point& pt2)
{
  PF::Shape::get_rect(pt1, pt2);
  
  for (int i = 0; i < get_segments_count(); i++) {
    SegmentInfo& si = get_segment_info(i);
    
    if (si.get_segment_type() != PF::SegmentInfo::line) {
      pt1.x = std::min(pt1.x, si.get_control_pt1().x);
      pt1.y = std::min(pt1.y, si.get_control_pt1().y);
      pt2.x = std::max(pt2.x, si.get_control_pt1().x);
      pt2.y = std::max(pt2.y, si.get_control_pt1().y);
     }
    if (si.get_segment_type() == PF::SegmentInfo::bezier4) {
      pt1.x = std::min(pt1.x, si.get_control_pt2().x);
      pt1.y = std::min(pt1.y, si.get_control_pt2().y);
      pt2.x = std::max(pt2.x, si.get_control_pt2().x);
      pt2.y = std::max(pt2.y, si.get_control_pt2().y);
   }
    
    float pen_size = (get_fill_shape()) ? 0.f: (get_pen_size()/2.f);
    
    pt1.x -= pen_size;
    pt1.y -= pen_size;
    pt2.x += pen_size;
    pt2.y += pen_size;
  }
}

// From http://scaledinnovation.com/analytics/splines/aboutSplines.html
void PF::Polygon::default_node_2control_points(Point& anchor1, Point& anchor2, Point& anchor3, Point& control1, Point& control2, const float t)
{
  const float d01 = std::sqrt((anchor2.x-anchor1.x)*(anchor2.x-anchor1.x)+(anchor2.y-anchor1.y)*(anchor2.y-anchor1.y));
  const float d12 = std::sqrt((anchor3.x-anchor2.x)*(anchor3.x-anchor2.x)+(anchor3.y-anchor2.y)*(anchor3.y-anchor2.y));
  const float fa = t*d01/(d01+d12);   // scaling factor for triangle Ta
  const float fb = t*d12/(d01+d12);   // ditto for Tb, simplifies to fb=t-fa
  
  control1.x=anchor2.x-fa*(anchor3.x-anchor1.x);    // x2-x0 is the width of triangle T
  control1.y=anchor2.y-fa*(anchor3.y-anchor1.y);    // y2-y0 is the height of T
  control2.x=anchor2.x+fb*(anchor3.x-anchor1.x);
  control2.y=anchor2.y+fb*(anchor3.y-anchor1.y);  
}

void PF::Polygon::default_segment_control_point(Point& anchor1, Point& anchor2, Point& control, const float t, bool is_clockwise)
{
  control.line_mid_point(anchor1, anchor2);
  float amount = std::sqrt(control.distance2(anchor1)) * t;
  if ( is_clockwise ) amount *= -1;
  if (control != anchor1)
    PF::Shape::get_pt_proyected_from_segment(control, anchor1, amount, control);
  else
    control.offset(amount, amount);
}

void PF::Polygon::default_node_controls_point(int node, const float t)
{
  const int segment_left = (node>0) ? node-1: get_points_count()-1;
  const int segment_right = node;
  PF::SegmentInfo& sil = get_segment_info(segment_left);
  PF::SegmentInfo& sir = get_segment_info(segment_right);
  int point1, point2, point3;
  point2 = node;
  point1 = (point2>0) ? point2-1: get_points_count()-1;
  point3 = (point2<get_points_count()-1) ? point2+1: 0;
  
  // node has two control points
  if ( (sil.get_segment_type() == PF::SegmentInfo::bezier3_r || sil.get_segment_type() == PF::SegmentInfo::bezier4) &&
      (sir.get_segment_type() == PF::SegmentInfo::bezier3_l || sir.get_segment_type() == PF::SegmentInfo::bezier4) ) {
    if ( sil.get_segment_type() == PF::SegmentInfo::bezier3_r )
      PF::Polygon::default_node_2control_points(get_point(point1), get_point(point2), get_point(point3), get_control_pt1(segment_left), get_control_pt1(segment_right), t);
    else
      PF::Polygon::default_node_2control_points(get_point(point1), get_point(point2), get_point(point3), get_control_pt2(segment_left), get_control_pt1(segment_right), t);
  }
  // node has one control point
  else if ( (sil.get_segment_type() == PF::SegmentInfo::line || sil.get_segment_type() == PF::SegmentInfo::bezier3_l) &&
      (sir.get_segment_type() == PF::SegmentInfo::bezier3_l || sir.get_segment_type() == PF::SegmentInfo::bezier4) ) {
    PF::Polygon::default_segment_control_point(get_point(point2), get_point(point3), get_control_pt1(segment_right), t, is_clockwise());
  }
}

void PF::Polygon::mirror_control_pt1(int segment, Point& pt, Point& anchor)
{
  Point synch(anchor);
  synch.offset(anchor.x-pt.x, anchor.y-pt.y);
  set_control_pt1(segment, synch);
}

void PF::Polygon::mirror_control_point(Point& anchor, Point& control_src, Point& control_pt)
{
  control_pt = anchor;
  control_pt.offset(anchor.x-control_src.x, anchor.y-control_src.y);
}

void PF::Polygon::default_control_pt1(int segment, float t)
{
  const int segment_prev = (segment > 0) ? segment-1: get_segments_count()-1;
  const int segment_next = (segment < get_segments_count()-1) ? segment+1: 0;
  const int node = segment;
  const int node_next = (node < get_points_count()-1) ? node+1: 0;
  
  if ( get_segment_type(segment) == PF::SegmentInfo::bezier3_l || get_segment_type(segment) == PF::SegmentInfo::bezier4 ) {
    // if prev segment has a control point, mirror against it
    if ( get_segment_type(segment_prev) == PF::SegmentInfo::bezier3_r ) {
      mirror_control_point(get_point(node), get_control_pt1(segment_prev), get_control_pt1(segment));
    }
    else if ( get_segment_type(segment_prev) == PF::SegmentInfo::bezier4 ) {
      mirror_control_point(get_point(node), get_control_pt2(segment_prev), get_control_pt1(segment));
    }
    else { // if not, just default it
      Point pt1, pt2;
      get_segment(segment, pt1, pt2);
      default_segment_control_point(pt1, pt2, get_control_pt1(segment), t, is_clockwise());
    }
  }
  else if ( get_segment_type(segment) == PF::SegmentInfo::bezier3_r ) {
    // if next segment has a control point, mirror against it
    if ( get_segment_type(segment_next) == PF::SegmentInfo::bezier3_l || get_segment_type(segment_next) == PF::SegmentInfo::bezier4 ) {
      mirror_control_point(get_point(node_next), get_control_pt1(segment_next), get_control_pt1(segment));
    }
    else { // if not, just default it
      Point pt1, pt2;
      get_segment(segment, pt1, pt2);
      default_segment_control_point(pt1, pt2, get_control_pt1(segment), t, is_clockwise());
    }
  }
  else {
    std::cout<<"PF::Polygon::default_control_pt1(): invalid segment type: "<<get_segment_type(segment)<<std::endl;
  }
}

void PF::Polygon::default_control_pt2(int segment, float t)
{
  const int segment_next = (segment < get_segments_count()-1) ? segment+1: 0;
  const int node = segment;
  const int node_next = (node < get_points_count()-1) ? node+1: 0;

  if ( get_segment_type(segment_next) == PF::SegmentInfo::bezier3_l || get_segment_type(segment_next) == PF::SegmentInfo::bezier4 ) {
    mirror_control_point(get_point(node_next), get_control_pt1(segment_next), get_control_pt2(segment));
  }
  else {
    Point pt1, pt2;
    get_segment(segment, pt1, pt2);
    default_segment_control_point(pt1, pt2, get_control_pt2(segment), t, is_clockwise());
  }
}

void PF::Polygon::mirror_control_pt2(int segment, Point& pt, Point& anchor)
{
  Point synch(anchor);
  synch.offset(anchor.x-pt.x, anchor.y-pt.y);
  set_control_pt2(segment, synch);
}

void PF::Polygon::expand_segment(Point& pt1, Point& pt2, Point& pt3, float length)
{
  float lenAB = std::sqrt( ((pt1.x-pt2.x)*(pt1.x-pt2.x)) + ((pt1.y-pt2.y)*(pt1.y-pt2.y)) );
  float f = length / lenAB;
  float x =  (float)(pt2.x - pt1.x) * f;
  float y =  (float)(pt2.y - pt1.y) * f;
  pt3.x = pt2.x + x;
  pt3.y = pt2.y + y;
}

void PF::Polygon::expand_segment_to(Point& pt1, Point& pt2, Point& pt3, float length_to)
{
  float lenAB = std::sqrt( ((pt1.x-pt2.x)*(pt1.x-pt2.x)) + ((pt1.y-pt2.y)*(pt1.y-pt2.y)) );
  length_to -= lenAB;
  float f = length_to / lenAB;
  float x =  (float)(pt2.x - pt1.x) * f;
  float y =  (float)(pt2.y - pt1.y) * f;
  pt3.x = pt2.x + x;
  pt3.y = pt2.y + y;
}

void PF::Polygon::synch_control_point(Point& anchor, Point& cntrl_src, Point& cntrl_synch)
{
  float lenAB = std::sqrt( ((anchor.x-cntrl_src.x)*(anchor.x-cntrl_src.x)) + ((anchor.y-cntrl_src.y)*(anchor.y-cntrl_src.y)) );
  float length = std::sqrt( ((anchor.x-cntrl_synch.x)*(anchor.x-cntrl_synch.x)) + ((anchor.y-cntrl_synch.y)*(anchor.y-cntrl_synch.y)) );
  float f = length / lenAB;
  float x =  (float)(anchor.x - cntrl_src.x) * f;
  float y =  (float)(anchor.y - cntrl_src.y) * f;
  cntrl_synch.x = anchor.x + x;
  cntrl_synch.y = anchor.y + y;
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

  std::vector<Point> points;
  
  inter_segments(points);
  
  if ( get_fill_shape() ) {
    fill_polygon(points, get_opacity(), mask, rc);
    
    if ( get_falloff() > 0.f ) {
//      points.push_back(points[0]);
//      fill_polygon_falloff(points, is_clockwise(), get_falloff(), get_opacity(), scurve, mask, rc);
      fill_polygon_falloff2(scurve, mask, rc);
//      fill_polygon_falloff3(points, get_falloff(), get_opacity(), scurve, mask, rc);
    }
  }
  else {
    draw_polygon_segments(points, get_pen_size(), get_falloff(), get_opacity(), get_closed_shape(), scurve, mask, rc);
  }
  
  test_mask(mask, rc);
  
}

// -----------------------------------
// Line
// -----------------------------------

void PF::Line::offset_control_pt1(int segment, Point& prev, Point& curr, int options)
{
  if (segment == 0) {
    if (get_segment_type(segment) == PF::SegmentInfo::bezier3_r)
      PF::Polygon::offset_control_pt1(segment, prev, curr, options);
    else
      get_segment_info(segment).offset_control_pt1(prev, curr);
  }
  else {
    PF::Polygon::offset_control_pt1(segment, prev, curr, options);
  }
}

void PF::Line::offset_control_pt2(int segment, Point& prev, Point& curr, int options)
{
  if (segment == get_segments_count()-1) {
    get_segment_info(segment).offset_control_pt2(prev, curr);
  }
  else {
    PF::Polygon::offset_control_pt2(segment, prev, curr, options);
  }
}

void PF::Line::offset_point(int n, float x, float y)
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

void PF::Line::default_control_pt1(int segment, float t)
{
  if ( segment > 0 ) {
    PF::Polygon::default_control_pt1(segment, t);
  }
  else if ( segment == 0 ) {
    if ( get_segment_type(segment) == PF::SegmentInfo::bezier3_l || get_segment_type(segment) == PF::SegmentInfo::bezier4 ) {
      // just default it
        Point pt1, pt2;
        get_segment(segment, pt1, pt2);
        default_segment_control_point(pt1, pt2, get_control_pt1(segment), t, is_clockwise());
    }
    else if ( get_segment_type(segment) == PF::SegmentInfo::bezier3_r ) {
      PF::Polygon::default_control_pt1(segment, t);
    }
    else {
      std::cout<<"PF::Line::default_control_pt1(): invalid segment type: "<<get_segment_type(segment)<<std::endl;
    }
  }
  else {
    std::cout<<"PF::Line::default_control_pt1(): invalid segment: "<<segment<<std::endl;
  }
}

void PF::Line::default_control_pt2(int segment, float t)
{
  if ( segment < get_segments_count()-1 ) {
    PF::Polygon::default_control_pt2(segment, t);
  }
  else if ( segment == get_segments_count()-1 ) {
    Point pt1, pt2;
    get_segment(segment, pt1, pt2);
    default_segment_control_point(pt1, pt2, get_control_pt2(segment), t, is_clockwise());
  }
  else {
    std::cout<<"PF::Line::default_control_pt2(): invalid segment: "<<segment<<std::endl;
  }
}

int PF::Line::insert_control_point(Point& pt, int hit_t, int additional, float t)
{
  int new_hit = -1;
  
  if ( additional > 0 && additional < get_points_count()-1 ) {
    new_hit = PF::Polygon::insert_control_point(pt, hit_t, additional, t);
  }
  else if (hit_t == PF::Shape::hit_node) {
    if (additional == 0) {
      const int segment = 0;
      SegmentInfo& si = get_segment_info(segment);
      if (si.get_segment_type() == PF::SegmentInfo::line) { // add a right control pt to the node
        set_segment_type( segment, PF::SegmentInfo::bezier3_l );
        new_hit = PF::SegmentInfo::bezier3_l;
        default_control_pt1(segment, t);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_r) { // add a right control pt to the node
        set_segment_type( segment, PF::SegmentInfo::bezier4 );
        set_control_pt2( segment, get_control_pt1(segment) );
        new_hit = PF::SegmentInfo::bezier4;
        default_control_pt1(segment, t);
      }
    }
    else if (additional == get_points_count()-1) {
      const int segment = get_points_count()-2;
      SegmentInfo& si = get_segment_info(segment);
      if (si.get_segment_type() == PF::SegmentInfo::line) { // add a left control pt to the node
        set_segment_type( segment, PF::SegmentInfo::bezier3_r );
        new_hit = PF::SegmentInfo::bezier3_r;
        default_control_pt1(segment, t);
      }
      else if (si.get_segment_type() == PF::SegmentInfo::bezier3_l) { // add a left control pt to the node
        set_segment_type( segment, PF::SegmentInfo::bezier4 );
        new_hit = PF::SegmentInfo::bezier4;
        default_control_pt2(segment, t);
      }
    }
    else {
      std::cout<<"PF::Line::insert_control_point(): invalid node: "<<additional<<std::endl;
    }
  }
  else {
    std::cout<<"PF::Line::insert_control_point(): invalid hit_test: "<<hit_t<<std::endl;  
  }

  return new_hit;
}

int PF::Line::hit_test(Point& pt, int& additional)
{
  int hit_t = PF::Polygon::hit_test(pt, additional);
  
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
  const float delta2 = (get_pen_size()+hit_test_delta)*(get_pen_size()+hit_test_delta);

  for ( int i = 0; i < get_points_count(); i++ ) {
    pt1 = get_point(i);
    
    if ( hit_test_node(pt, pt1) ) {
      additional = i;
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

// -----------------------------------
// Rectangle
// -----------------------------------

void PF::Rectangle::offset(float x, float y)
{
  for (int i = 0; i < get_points_count(); i++) {
    get_point(i).offset(x, y);
  }
}

void PF::Rectangle::offset_point(int n, float x, float y)
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
//  std::cout<<"PF::Rectangle::offset(): additional: "<<additional<<std::endl;
  
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

void PF::Rectangle::build_mask(SplineCurve& scurve)
{
  if ( get_fill_shape() || get_rounded_corners() ) {
    PF::Polygon::build_mask(scurve);
    return;
  }
  
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

  std::vector<Point> points1, points2;
  Point center = get_center();;
  float pen_size = get_pen_size() / 2.f;
  
  points1.resize(4);
  points2.resize(4);
  
  for ( int i = 0; i < 4; i++) {
    if ( get_point(i).x < center.x ) {
      points1[i].x = get_point(i).x - pen_size;
      points2[i].x = get_point(i).x + pen_size;
    }
    else {
      points1[i].x = get_point(i).x + pen_size;
      points2[i].x = get_point(i).x - pen_size;
    }
    
    if ( get_point(i).y < center.y ) {
      points1[i].y = get_point(i).y - pen_size;
      points2[i].y = get_point(i).y + pen_size;
    }
    else {
      points1[i].y = get_point(i).y + pen_size;
      points2[i].y = get_point(i).y - pen_size;
    }
    
  }
  
  if ( get_falloff() > 0.f ) {
    fill_polygon_falloff(points1, is_clockwise(), get_falloff(), get_opacity(), scurve, mask, rc);
    fill_polygon_falloff(points2, is_clockwise(), get_falloff(), get_opacity(), scurve, mask, rc);
  }

  points1.push_back(points1[0]);
  for ( int i = 0; i < 4; i++) points1.push_back(points2[i]);
  points1.push_back(points2[0]);
  
  fill_polygon(points1, get_opacity(), mask, rc);
    
  
  test_mask(mask, rc);
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

void PF::ShapesGroup::scale(float sf)
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


//------------------------------------------------------------------------
// From http://www.antigrain.com/research/adaptive_bezier/index.html
//------------------------------------------------------------------------


// -----------------------------------
// curve3_div
// -----------------------------------

void curve3_div::init(double x1, double y1, 
                         double x2, double y2, 
                         double x3, double y3)
   {
  curve_collinearity_epsilon = 0.01f;
  curve_angle_tolerance_epsilon = 0.01f;
  
       m_points.clear();
       m_distance_tolerance_square = 0.5 / m_approximation_scale;
       m_distance_tolerance_square *= m_distance_tolerance_square;
       bezier(x1, y1, x2, y2, x3, y3);
       m_count = 0;
   }

   //------------------------------------------------------------------------
   void curve3_div::recursive_bezier(double x1, double y1, 
                                     double x2, double y2, 
                                     double x3, double y3,
                                     unsigned level)
   {
       if(level > curve_recursion_limit) 
       {
           return;
       }

       // Calculate all the mid-points of the line segments
       //----------------------
       double x12   = (x1 + x2) / 2;                
       double y12   = (y1 + y2) / 2;
       double x23   = (x2 + x3) / 2;
       double y23   = (y2 + y3) / 2;
       double x123  = (x12 + x23) / 2;
       double y123  = (y12 + y23) / 2;

       double dx = x3-x1;
       double dy = y3-y1;
       double d = fabs(((x2 - x3) * dy - (y2 - y3) * dx));
       double da;

       if(d > curve_collinearity_epsilon)
       { 
           // Regular case
           //-----------------
           if(d * d <= m_distance_tolerance_square * (dx*dx + dy*dy))
           {
               // If the curvature doesn't exceed the distance_tolerance value
               // we tend to finish subdivisions.
               //----------------------
               if(m_angle_tolerance < curve_angle_tolerance_epsilon)
               {
                   m_points.push_back(PF::Point(x123, y123));
                   return;
               }

               // Angle & Cusp Condition
               //----------------------
               da = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
               if(da >= M_PI) da = 2*M_PI - da;

               if(da < m_angle_tolerance)
               {
                   // Finally we can stop the recursion
                   //----------------------
                   m_points.push_back(PF::Point(x123, y123));
                   return;                 
               }
           }
       }
       else
       {
           // Collinear case
           //------------------
           da = dx*dx + dy*dy;
           if(da == 0)
           {
               d = calc_sq_distance(x1, y1, x2, y2);
           }
           else
           {
               d = ((x2 - x1)*dx + (y2 - y1)*dy) / da;
               if(d > 0 && d < 1)
               {
                   // Simple collinear case, 1---2---3
                   // We can leave just two endpoints
                   return;
               }
                    if(d <= 0) d = calc_sq_distance(x2, y2, x1, y1);
               else if(d >= 1) d = calc_sq_distance(x2, y2, x3, y3);
               else            d = calc_sq_distance(x2, y2, x1 + d*dx, y1 + d*dy);
           }
           if(d < m_distance_tolerance_square)
           {
               m_points.push_back(PF::Point(x2, y2));
               return;
           }
       }

       // Continue subdivision
       //----------------------
       recursive_bezier(x1, y1, x12, y12, x123, y123, level + 1); 
       recursive_bezier(x123, y123, x23, y23, x3, y3, level + 1); 
   }

   //------------------------------------------------------------------------
   void curve3_div::bezier(double x1, double y1, 
                           double x2, double y2, 
                           double x3, double y3)
   {
       m_points.push_back(PF::Point(x1, y1));
       recursive_bezier(x1, y1, x2, y2, x3, y3, 0);
       m_points.push_back(PF::Point(x3, y3));
   }

   

   // -----------------------------------
   // curve4_div
   // -----------------------------------

   //------------------------------------------------------------------------
    void curve4_div::init(double x1, double y1, 
                          double x2, double y2, 
                          double x3, double y3,
                          double x4, double y4)
    {
      curve_collinearity_epsilon = 0.01f;
      curve_angle_tolerance_epsilon = 0.01f;
      
        m_points.clear();
        m_distance_tolerance_square = 0.5 / m_approximation_scale;
        m_distance_tolerance_square *= m_distance_tolerance_square;
        bezier(x1, y1, x2, y2, x3, y3, x4, y4);
        m_count = 0;
    }

    //------------------------------------------------------------------------
    void curve4_div::recursive_bezier(double x1, double y1, 
                                      double x2, double y2, 
                                      double x3, double y3, 
                                      double x4, double y4,
                                      unsigned level)
    {
        if(level > curve_recursion_limit) 
        {
            return;
        }

        // Calculate all the mid-points of the line segments
        //----------------------
        double x12   = (x1 + x2) / 2;
        double y12   = (y1 + y2) / 2;
        double x23   = (x2 + x3) / 2;
        double y23   = (y2 + y3) / 2;
        double x34   = (x3 + x4) / 2;
        double y34   = (y3 + y4) / 2;
        double x123  = (x12 + x23) / 2;
        double y123  = (y12 + y23) / 2;
        double x234  = (x23 + x34) / 2;
        double y234  = (y23 + y34) / 2;
        double x1234 = (x123 + x234) / 2;
        double y1234 = (y123 + y234) / 2;


        // Try to approximate the full cubic curve by a single straight line
        //------------------
        double dx = x4-x1;
        double dy = y4-y1;

        double d2 = fabs(((x2 - x4) * dy - (y2 - y4) * dx));
        double d3 = fabs(((x3 - x4) * dy - (y3 - y4) * dx));
        double da1, da2, k;

        switch((int(d2 > curve_collinearity_epsilon) << 1) +
                int(d3 > curve_collinearity_epsilon))
        {
        case 0:
            // All collinear OR p1==p4
            //----------------------
            k = dx*dx + dy*dy;
            if(k == 0)
            {
                d2 = calc_sq_distance(x1, y1, x2, y2);
                d3 = calc_sq_distance(x4, y4, x3, y3);
            }
            else
            {
                k   = 1 / k;
                da1 = x2 - x1;
                da2 = y2 - y1;
                d2  = k * (da1*dx + da2*dy);
                da1 = x3 - x1;
                da2 = y3 - y1;
                d3  = k * (da1*dx + da2*dy);
                if(d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
                {
                    // Simple collinear case, 1---2---3---4
                    // We can leave just two endpoints
                    return;
                }
                     if(d2 <= 0) d2 = calc_sq_distance(x2, y2, x1, y1);
                else if(d2 >= 1) d2 = calc_sq_distance(x2, y2, x4, y4);
                else             d2 = calc_sq_distance(x2, y2, x1 + d2*dx, y1 + d2*dy);

                     if(d3 <= 0) d3 = calc_sq_distance(x3, y3, x1, y1);
                else if(d3 >= 1) d3 = calc_sq_distance(x3, y3, x4, y4);
                else             d3 = calc_sq_distance(x3, y3, x1 + d3*dx, y1 + d3*dy);
            }
            if(d2 > d3)
            {
                if(d2 < m_distance_tolerance_square)
                {
                    m_points.push_back(PF::Point(x2, y2));
                    return;
                }
            }
            else
            {
                if(d3 < m_distance_tolerance_square)
                {
                    m_points.push_back(PF::Point(x3, y3));
                    return;
                }
            }
            break;

        case 1:
            // p1,p2,p4 are collinear, p3 is significant
            //----------------------
            if(d3 * d3 <= m_distance_tolerance_square * (dx*dx + dy*dy))
            {
                if(m_angle_tolerance < curve_angle_tolerance_epsilon)
                {
                    m_points.push_back(PF::Point(x23, y23));
                    return;
                }

                // Angle Condition
                //----------------------
                da1 = fabs(atan2(y4 - y3, x4 - x3) - atan2(y3 - y2, x3 - x2));
                if(da1 >= M_PI) da1 = 2*M_PI - da1;

                if(da1 < m_angle_tolerance)
                {
                    m_points.push_back(PF::Point(x2, y2));
                    m_points.push_back(PF::Point(x3, y3));
                    return;
                }

                if(m_cusp_limit != 0.0)
                {
                    if(da1 > m_cusp_limit)
                    {
                        m_points.push_back(PF::Point(x3, y3));
                        return;
                    }
                }
            }
            break;

        case 2:
            // p1,p3,p4 are collinear, p2 is significant
            //----------------------
            if(d2 * d2 <= m_distance_tolerance_square * (dx*dx + dy*dy))
            {
                if(m_angle_tolerance < curve_angle_tolerance_epsilon)
                {
                    m_points.push_back(PF::Point(x23, y23));
                    return;
                }

                // Angle Condition
                //----------------------
                da1 = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
                if(da1 >= M_PI) da1 = 2*M_PI - da1;

                if(da1 < m_angle_tolerance)
                {
                    m_points.push_back(PF::Point(x2, y2));
                    m_points.push_back(PF::Point(x3, y3));
                    return;
                }

                if(m_cusp_limit != 0.0)
                {
                    if(da1 > m_cusp_limit)
                    {
                        m_points.push_back(PF::Point(x2, y2));
                        return;
                    }
                }
            }
            break;

        case 3: 
            // Regular case
            //-----------------
            if((d2 + d3)*(d2 + d3) <= m_distance_tolerance_square * (dx*dx + dy*dy))
            {
                // If the curvature doesn't exceed the distance_tolerance value
                // we tend to finish subdivisions.
                //----------------------
                if(m_angle_tolerance < curve_angle_tolerance_epsilon)
                {
                    m_points.push_back(PF::Point(x23, y23));
                    return;
                }

                // Angle & Cusp Condition
                //----------------------
                k   = atan2(y3 - y2, x3 - x2);
                da1 = fabs(k - atan2(y2 - y1, x2 - x1));
                da2 = fabs(atan2(y4 - y3, x4 - x3) - k);
                if(da1 >= M_PI) da1 = 2*M_PI - da1;
                if(da2 >= M_PI) da2 = 2*M_PI - da2;

                if(da1 + da2 < m_angle_tolerance)
                {
                    // Finally we can stop the recursion
                    //----------------------
                    m_points.push_back(PF::Point(x23, y23));
                    return;
                }

                if(m_cusp_limit != 0.0)
                {
                    if(da1 > m_cusp_limit)
                    {
                        m_points.push_back(PF::Point(x2, y2));
                        return;
                    }

                    if(da2 > m_cusp_limit)
                    {
                        m_points.push_back(PF::Point(x3, y3));
                        return;
                    }
                }
            }
            break;
        }

        // Continue subdivision
        //----------------------
        recursive_bezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1); 
        recursive_bezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1); 
    }

    //------------------------------------------------------------------------
    void curve4_div::bezier(double x1, double y1, 
                            double x2, double y2, 
                            double x3, double y3, 
                            double x4, double y4)
    {
        m_points.push_back(PF::Point(x1, y1));
        recursive_bezier(x1, y1, x2, y2, x3, y3, x4, y4, 0);
        m_points.push_back(PF::Point(x4, y4));
    }

    