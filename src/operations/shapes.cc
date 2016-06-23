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

// -----------------------------------
// Shape
// -----------------------------------

void PF::Shape::get_falloff_point_absolute(int n, Point& center, Point& absolute)
{
  Point& pt = get_point(n);
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
  
  absolute.set(x, y);
}

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

void PF::Shape::offset(int x, int y)
{
  for (int i = 0; i < points.size(); i++) {
    points[i].offset(x, y);
  }
}

void PF::Shape::offset(Point prev, Point curr)
{
  offset(curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset_point(int n, int x, int y)
{
  points[n].offset(x, y);
}

void PF::Shape::offset_point(int n, Point prev, Point curr)
{
  offset_point(n, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset_falloff_point(int n, int x, int y)
{
  falloff_points[n].offset_pos(x, y);
}

void PF::Shape::offset_falloff_point(int n, Point prev, Point curr)
{
  offset_falloff_point(n, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source)
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
    offset_falloff_point(additional, curr, prev);
    break;
  case PF::Shape::hit_segment:
    offset_point(additional, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
    offset_point(additional+1, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
    break;
  case PF::Shape::hit_falloff_segment:
    offset_falloff_point(additional, curr.get_x()-prev.get_x(), prev.get_y()-curr.get_y());
    offset_falloff_point(additional+1, curr.get_x()-prev.get_x(), prev.get_y()-curr.get_y());
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
  size /= scale;

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
  for (int i = 0; i < falloff_points.size(); i++) {
    falloff_points[i].set(std::max(0, falloff_points[i].get_x()+n), std::max(0, falloff_points[i].get_y()+n));
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
      get_falloff_point_absolute(i, center, absolute);
      
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
  const float amount_x = amount*dx;
  const float amount_y = amount*dy;
  a.set( pt1.get_x() + amount_y, pt1.get_y() - amount_x);
  b.set( pt1.get_x() - amount_y, pt1.get_y() + amount_x);
  d.set( pt2.get_x() + amount_y, pt2.get_y() - amount_x);
  c.set( pt2.get_x() - amount_y, pt2.get_y() + amount_x);

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
// Line
// -----------------------------------

void PF::Line::expand_falloff(int n)
{
  set_falloff(get_falloff()+n);
}

void PF::Line::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source)
{
  switch (hit_t)
  {
  case PF::Shape::hit_falloff_segment:
    PF::Shape::offset(PF::Shape::hit_falloff, prev, curr, additional, lock_source);
    break;
  default:
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source);
    break;
  }
}

int PF::Line::hit_test(Point& pt, int& additional)
{
  int hit_t = hit_none;
  
  // check the nodes
  int i = 0;
  while ( i < get_points().size() && hit_t == hit_none ) {
    hit_t = hit_test_node(i, pt, additional);
    i++;
  }
    
  // check shape & falloff
  if ( hit_t == hit_none ) {
    const int delta2 = (get_size()+hit_test_delta)*(get_size()+hit_test_delta);
    const int fall2 = (get_size()+get_falloff()+hit_test_delta)*(get_size()+get_falloff()+hit_test_delta);
    Point pt1, pt2;
    int i = 0;
    while ( i < get_points().size()-1 && hit_t == hit_none ) {
      pt1 = get_point(i);
      pt2 = get_point(i+1);
      const float dist2 = pt.distance2segment2(pt1, pt2);
      
      if (dist2 <= delta2) {
        hit_t = hit_segment;
        additional = i;
      }
      else if (dist2 <= fall2) {
        hit_t = hit_falloff_segment;
        additional = i;
      }
      i++;
    }    
  }
  // check the source
  if ( hit_t == hit_none && get_has_source()) {
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
  PF::Shape::get_rect(rc);
  
  rc->left -= get_size();
  rc->top -= get_size();
  rc->width += get_size()*2;
  rc->height += get_size()*2;
}

void PF::Line::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff();
  rc->top -= get_falloff();
  rc->width += get_falloff()*2;
  rc->height += get_falloff()*2;
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
  rc.left = rc.top = 0;
  
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
  
  // go through all segments
  for (int i=0; i < get_points().size()-1; i++) {
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

}

// -----------------------------------
// Circle1
// -----------------------------------

void PF::Circle1::expand_falloff(int n)
{
  set_falloff(get_falloff()+n);
}

int PF::Circle1::hit_test(Point& pt, int& additional)
{
  Point& point = get_point();
  const int radius = get_size();
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

void PF::Circle1::get_rect(VipsRect* rc)
{
  const int R = get_size();
  rc->left = get_point().get_x()-R;
  rc->top = get_point().get_y()-R;
  rc->width = rc->height = R*2;
}

void PF::Circle1::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff();
  rc->top -= get_falloff();
  rc->width += get_falloff()*2;
  rc->height += get_falloff()*2;
}

void PF::Circle1::build_mask(SplineCurve& scurve)
{
//  std::cout<<"PF::Circle1::build_mask()"<<std::endl;
  
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
  const float radius_2 = get_size()*get_size();
  const float radiusf_2 = (get_size()+get_falloff())*(get_size()+get_falloff());
  
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
  int falloff = get_falloff_x() * ((float)get_radius_y()/(float)get_radius_x());
  
  if (falloff <= 0 && get_falloff_x() > 0) falloff = 1;
  
  return falloff; 
}

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

void PF::Ellipse::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source)
{
  switch (hit_t)
  {
  case PF::Shape::hit_node:
    if (additional == 0) {
      set_radius_y(get_radius_y() + (prev.get_y() - curr.get_y()));
    }
    else if (additional == 1) {
      set_size(get_radius_x() + (curr.get_x() - prev.get_x()));
    }
    else if (additional == 2) {
      set_radius_y(get_radius_y() + (curr.get_y() - prev.get_y()));
    }
    else if (additional == 3) {
      set_size(get_radius_x() + (prev.get_x() - curr.get_x()));
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
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source);
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
        const float f = ( (float)((x-center.get_x())*(x-center.get_x())) / wf2 + (float)(y*y) / hf2 ) - scale;
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

// -----------------------------------
// Rect1
// -----------------------------------

void PF::Rect1::offset_point(int n, int x, int y)
{
  Point pt = get_point(n);
  PF::Shape::offset_point(n, x, y);
  if (get_point(0).get_x() == get_point(1).get_x() || get_point(0).get_y() == get_point(1).get_y()) {
    set_point(n, pt);
  }
}

void PF::Rect1::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source)
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
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source);
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
    get_falloff_point_absolute(0, center, pt_fall0);
    if ( hit_test_node(pt, pt_fall0) ) {
      additional = 0;
      return hit_falloff_node;
    }

    get_falloff_point_absolute(1, center, pt_fall1);
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
    Circle1& sh = get_circles()[s_index];
    shape = dynamic_cast<Circle1*>(&sh);
  }
  else if ( s_type == PF::Shape::ellipse )
    shape = &(get_ellipses().data()[s_index]);

  else if ( s_type == PF::Shape::rectangle )
    shape = &(get_rectangles().data()[s_index]);
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
    PF::Circle1 shape;
    const PF::Circle1* ss = static_cast<const PF::Circle1*>(shape_source);
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
    PF::Rect1 shape;
    const PF::Rect1* ss = static_cast<const PF::Rect1*>(shape_source);
    shape = *ss;
    rectangles1.push_back(shape);
    index = rectangles1.size()-1;
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
    rectangles1.erase(rectangles1.begin()+nindex);
    break;
  default:
    std::cout<<"PF::ShapesGroup::remove(): error removing shape: index: "<<n<<std::endl;
    break;
  }
 
  for (int i = 0; i < shapes_order.size(); i++) {
    if (shapes_order.at(i).first == ntype && shapes_order.at(n).second > nindex) {
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
  rectangles1.clear();
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
  for (int i = 0; i < rectangles1.size(); i++) {
    rectangles1[i].scale(sf);
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
  for (int i = 0; i < rectangles1.size(); i++) {
    rectangles1[i].build_mask(scurve);
  }
}


