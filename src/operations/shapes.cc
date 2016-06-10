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
/*
void PF::Shape::circle_falloff(float* mask, VipsRect& rc, Point& center, int radius, int falloff)
{
  int center_x = center.get_x();
  int center_y = center.get_y();
  int radius_2 = radius;
  int radius_2f = radius+falloff;
  radius_2 *= radius_2;
  radius_2f *= radius_2f;
  
  for(int y = 0; y < rc.height; y++) {
    for(int x = 0; x < rc.width; x++) {
      float l2 = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
      if(l2 < radius_2) {
        mask[y * rc.width + x] = 1.0f;
      }
      else if(l2 < radius_2f) {
        float f = (radius_2f - l2) / (radius_2f - radius_2);
        mask[y * rc.width + x] = f * f;
      }
    }
  }

}
*/
void PF::Shape::offset(Point prev, Point curr)
{
  offset(curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}

void PF::Shape::offset(int x, int y)
{
  for (int i = 0; i < points.size(); i++) {
    points[i].offset(x, y);
  }
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
/*
int PF::Shape::hit_test_falloff_node(Point& pt_node, Point& pt_test)
{
 if (hit_test_node(pt_node, pt_test) != PF::Shape::hit_none)
   return PF::Shape::hit_falloff_node;
  
  return PF::Shape::hit_none;
}

int PF::Shape::hit_test_falloff_node(int n, Point& pt, int& additional)
{
  additional = n;
  return hit_test_falloff_node(falloff_points[n], pt);
}
*/
void PF::Shape::scale(int scale)
{
  for (int i = 0; i < points.size(); i++) {
    points[i].scale(scale);
    falloff_points[i].scale(scale);
  }
  source_point.scale(scale);
  
  falloff /= scale;

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

void PF::Shape::get_center(Point& pt)
{
  VipsRect rc;
  get_rect(&rc);
  pt.set(rc.left+rc.width/2, rc.top+rc.height/2);
  return;
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
  
  for (int i = 0; i < points.size(); i++) {
    rc->left = std::min(rc->left, points[i].get_x());
    rc->top = std::min(rc->top, points[i].get_y());
    rc->width = std::max(rc->width, points[i].get_x());
    rc->height = std::max(rc->height, points[i].get_y());
  }
  if (points.size() > 0) {
    rc->width -= rc->left;
    rc->height -= rc->top;
  }
}

void PF::Shape::get_falloff_rect(VipsRect* rc)
{
  Point center;
  get_center(center);
  
  int x, y;
  rc->left = rc->top = INT_MAX;
  rc->width = rc->height = INT_MIN;
  
  for (int i = 0; i < points.size(); i++) {
    x = get_point(i).get_x();
    if (x < center.get_x())
      x -= get_falloff_point(i).get_x();
    else
      x += get_falloff_point(i).get_x();
    y = get_point(i).get_y();
    if (y < center.get_y())
      y -= get_falloff_point(i).get_y();
    else
      y += get_falloff_point(i).get_y();
    
    rc->left = std::min(rc->left, x);
    rc->top = std::min(rc->top, y);
    rc->width = std::max(rc->width, x);
    rc->height = std::max(rc->height, y);
  }
  if (falloff_points.size() > 0) {
    rc->width -= rc->left;
    rc->height -= rc->top;
  }
}

void PF::Line::expand_falloff(int n)
{
  set_falloff(get_falloff()+n);
}

// returns a rect, result of expanding a given line by amount
void PF::Line::get_expanded_rec_from_line(Point& pt1, Point& pt2, int amount, Point& a, Point& b, Point& c, Point& d)
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
//  int hit_t = PF::Shape::hit_test(pt, additional);
  int hit_t = hit_none;
  // check the nodes
//  if ( hit_t == hit_none ) {
    int i = 0;
    while ( i < get_points().size() && hit_t == hit_none ) {
      hit_t = hit_test_node(i, pt, additional);
      i++;
    }
//  }
  // check shape & falloff
  if ( hit_t == hit_none ) {
    const int delta2 = hit_test_delta*hit_test_delta;
    const int fall2 = (get_falloff()+hit_test_delta)*(get_falloff()+hit_test_delta);
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

void PF::Line::get_falloff_rect(VipsRect* rc)
{
  get_rect(rc);
  
  rc->left -= get_falloff();
  rc->top -= get_falloff();
  rc->width += get_falloff()*2;
  rc->height += get_falloff()*2;
}

void PF::Line::get_segment_rect(Point& pt1, Point& pt2, VipsRect* rc)
{
  rc->left = std::min(pt1.get_x(), pt2.get_x());
  rc->top = std::min(pt1.get_y(), pt2.get_y());
  rc->width = std::max(pt1.get_x(), pt2.get_x()) - rc->left;
  rc->height = std::max(pt1.get_y(), pt2.get_y()) - rc->top;
}

void PF::Line::get_segment_falloff_rect(Point& pt1, Point& pt2, int falloff, VipsRect* rc)
{
  get_segment_rect(pt1, pt2, rc);
  rc->left -= falloff;
  rc->top -= falloff;
  rc->width += falloff*2;
  rc->height += falloff*2;
}

void PF::Line::build_mask()
{
  // free mask if already exists
  if (mask != NULL) {
    free(mask);
    mask = NULL;
  }
 
  // if no falloff there's no point in returning a mask
  if (get_falloff() <= 0) return;
  
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
  const float border2 = get_falloff() * get_falloff(); // square distance to the border (falloff)
  
  // go through all segments
  for (int i=0; i < get_points().size()-1; i++) {
    pt1 = get_point(i);
    pt1.offset(pt_offset);
    pt2 = get_point(i+1);
    pt2.offset(pt_offset);

    // get segment bounding rect
    get_segment_falloff_rect(pt1, pt2, get_falloff(), &rcs);
    
    // get the falloff for each point
    for(int y = rcs.top; y <  rcs.top+rcs.height; y++) {
      float* pmask = mask + y * rc.width;
      for(int x = rcs.left; x < rcs.left+rcs.width ; x++) {
        pt.set(x, y);
        
        // square distance to the segment
        const float dist2 = pt.distance2segment2(pt1, pt2);
        if (dist2 < border2) {
          const float f = (border2 - dist2) / border2;
          const float f2 = f * f * get_opacity();
          pmask[x] = std::max(pmask[x], f2);
        }
      }
    }
  }
/*  Point pt;
  Point pt1;
  Point pt2;
  const float border2 = get_falloff() * get_falloff(); // square distance to the border (falloff)
  
  for(int y = 0; y <  rc.height; y++) {
    float* pmask = mask + y * rc.width;
    for(int x = 0; x < rc.width ; x++) {
      pt.set(x, y);
      // check if belongs to a segment
      // must check all because they can overlap
      for (int i=0; i < get_points().size()-1; i++) {
        pt1 = get_point(i);
        pt1.offset(pt_offset);
        pt2 = get_point(i+1);
        pt2.offset(pt_offset);

        // square distance to the segment
        const float dist2 = pt.distance2segment2(pt1, pt2);
        if (dist2 < border2) {
          const float f = (border2 - dist2) / border2;
          const float f2 = f * f * get_opacity();
          pmask[x] = std::max(pmask[x], f2);
        }
      }
    }
  }
*/  
}


PF::Circle1::Circle1():
    Shape()
{
  set_type(circle);
  radius = 1;
  PF::Shape::add_point(Point(0, 0));
}

void PF::Circle1::expand_falloff(int n)
{
  set_falloff(get_falloff()+n);
}

int PF::Circle1::hit_test(Point& pt, int& additional)
{
  Point& point = get_point();
  
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
/*
void PF::Circle1::expand(int n)
{
  radius = std::max(0, radius+n);
}
*/
void PF::Circle1::get_rect(VipsRect* rc)
{
  const int R = get_radius();
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

void PF::Circle1::scale(int scale)
{
  Shape::scale(scale);
  
  radius /= scale;
}

void PF::Circle1::build_mask()
{
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
//  int center_x = center.get_x();
//  int center_y = center.get_y();
  const float radius_2 = get_radius()*get_radius();
  const float radiusf_2 = (get_radius()+get_falloff())*(get_radius()+get_falloff());
//  radius_2 *= radius_2;
//  radius_2f *= radius_2f;
  
  for(int y = 0; y < rc.height; y++) {
    float* pmask = mask + y * rc.width;
    for(int x = 0; x < rc.width; x++) {
      const float dist2 = center.distance2(x, y);
      if (dist2 < radius_2) {
        pmask[x] = get_opacity();
      }
      else if (dist2 < radiusf_2) {
        const float f = (radiusf_2 - dist2) / (radiusf_2 - radius_2);
        pmask[x] = f * f * get_opacity();
      }
    }
  }

  // we populate the buffer
/*  int wi = piece->pipe->iwidth, hi = piece->pipe->iheight;
  float center[2] = { circle->center[0] * wi, circle->center[1] * hi };
  float radius2 = circle->radius * MIN(wi, hi) * circle->radius * MIN(wi, hi);
  float total2 = (circle->radius + circle->border) * MIN(wi, hi) * (circle->radius + circle->border)
                 * MIN(wi, hi);
  */
//  circle_falloff(mask, rc, cir.get_point(), cir.get_radius(), cir.get_falloff());
  
//  std::cout<<"PF::Circle1::build_mask() end"<<std::endl;

}

PF::Rect1::Rect1():
    Shape()
{
  set_type(rectangle);
  PF::Shape::add_point(Point(0, 0));
  PF::Shape::add_point(Point(1, 1));
}
/*
void PF::Rect1::offset_point(int n, int x, int y)
{
  if (n == 0)
    get_top_left().set(std::min(get_bottom_right().get_x(), get_top_left().get_x()+x), std::min(get_bottom_right().get_y(), get_top_left().get_y()+y));
  else
    get_bottom_right().set(std::max(get_top_left().get_x(), get_bottom_right().get_x()+x), std::max(get_top_left().get_y(), get_bottom_right().get_y()+y));
}

void PF::Rect1::offset_point(int n, Point prev, Point curr)
{
  offset_point(n, curr.get_x()-prev.get_x(), curr.get_y()-prev.get_y());
}
*/
void PF::Rect1::offset(int hit_t, Point& prev, Point& curr, int additional, bool lock_source)
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
      offset_point(1, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 3) {
      offset_point(0, curr.get_x()-prev.get_x(), 0);
    }
    break;
  case PF::Shape::hit_falloff_segment:
    if (additional == 0) {
      offset_falloff_point(0, 0, prev.get_y()-curr.get_y());
    }
    else if (additional == 1) {
      offset_falloff_point(1, curr.get_x()-prev.get_x(), 0);
    }
    else if (additional == 2) {
      offset_falloff_point(1, 0, curr.get_y()-prev.get_y());
    }
    else if (additional == 3) {
      offset_falloff_point(0, prev.get_x()-curr.get_x(), 0);
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
    if (additional == 0) {
      offset_falloff_point(0, curr, prev);
    }
    else if (additional == 1) {
      offset_falloff_point(1, curr.get_x()-prev.get_x(), 0);
      offset_falloff_point(0, 0, prev.get_y()-curr.get_y());
    }
    else if (additional == 2) {
      offset_falloff_point(1, prev, curr);
    }
    else if (additional == 3) {
      offset_falloff_point(0, prev.get_x()-curr.get_x(), 0);
      offset_falloff_point(1, 0, curr.get_y()-prev.get_y());
    }
    break;
    //    case PF::Shape::hit_shape:
      //    case PF::Shape::hit_falloff:
        //    case PF::Shape::hit_source:
  default:
    PF::Shape::offset(hit_t, prev, curr, additional, lock_source);
    break;
  }

}
/*
void PF::Rect1::get_falloff_rect(VipsRect* rc)
{
  rc->left = get_point(0).get_x()-get_falloff_point(0).get_x();
  rc->top = get_point(0).get_y()-get_falloff_point(0).get_y();
  rc->width = get_point(1).get_x()+get_falloff_point(1).get_x()-rc->left;
  rc->height = get_point(1).get_y()+get_falloff_point(1).get_y()-rc->top;
}
*/

int PF::Rect1::hit_test(Point& pt, int& additional)
{
  int hit_t = hit_none;
  
//  Point pt1, pt2;
  
//  pt1.set( std::min(get_point(0).get_x(), get_point(1).get_x()), std::min(get_point(0).get_y(), get_point(1).get_y()) );
//  pt2.set( std::max(get_point(0).get_x(), get_point(1).get_x()), std::max(get_point(0).get_y(), get_point(1).get_y()) );
  VipsRect rc;
  get_falloff_rect(&rc);
  
  // inside the falloff
  if ( hit_test_rect(pt, &rc) ) {
    VipsRect rcs;
    get_rect(&rcs);

    Point pt1;
    
    // falloff nodes
    pt1.set(rc.left, rc.top);
    if ( hit_test_node(pt, pt1) ) {
      additional = 0;
      return hit_falloff_node;
    }
    pt1.set(rc.left+rc.width, rc.top);
    if ( hit_test_node(pt, pt1) ) {
      additional = 1;
      return hit_falloff_node;
    }
    pt1.set(rc.left+rc.width, rc.top+rc.height);
    if ( hit_test_node(pt, pt1) ) {
      additional = 2;
      return hit_falloff_node;
    }
    pt1.set(rc.left, rc.top+rc.height);
    if ( hit_test_node(pt, pt1) ) {
      additional = 3;
      return hit_falloff_node;
    }

    // shape nodes
    pt1.set(rcs.left, rcs.top);
    if ( hit_test_node(pt, pt1) ) {
      additional = 0;
      return hit_node;
    }
    pt1.set(rcs.left+rcs.width, rcs.top);
    if ( hit_test_node(pt, pt1) ) {
      additional = 1;
      return hit_node;
    }
    pt1.set(rcs.left+rcs.width, rcs.top+rcs.height);
    if ( hit_test_node(pt, pt1) ) {
      additional = 2;
      return hit_node;
    }
    pt1.set(rcs.left, rcs.top+rcs.height);
    if ( hit_test_node(pt, pt1) ) {
      additional = 3;
      return hit_node;
    }

    Point pt2;
    
    // falloff segments
    pt1.set(rc.left, rc.top);
    pt2.set(rc.left+rc.width, rc.top);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 0;
      return hit_falloff_segment;
    }
    pt1.set(rc.left+rc.width, rc.top);
    pt2.set(rc.left+rc.width, rc.top+rc.height);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 1;
      return hit_falloff_segment;
    }
    pt2.set(rc.left+rc.width, rc.top+rc.height);
    pt1.set(rc.left, rc.top+rc.height);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 2;
      return hit_falloff_segment;
    }
    pt2.set(rc.left, rc.top+rc.height);
    pt1.set(rc.left, rc.top);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 3;
      return hit_falloff_segment;
    }
    
    // shape segments
    pt1.set(rcs.left, rcs.top);
    pt2.set(rcs.left+rcs.width, rcs.top);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 0;
      return hit_segment;
    }
    pt1.set(rcs.left+rcs.width, rcs.top);
    pt2.set(rcs.left+rcs.width, rcs.top+rcs.height);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 1;
      return hit_segment;
    }
    pt2.set(rcs.left+rcs.width, rcs.top+rcs.height);
    pt1.set(rcs.left, rcs.top+rcs.height);
    if ( hit_test_rect(pt, pt1, pt2) ) {
      additional = 2;
      return hit_segment;
    }
    pt2.set(rcs.left, rcs.top+rcs.height);
    pt1.set(rcs.left, rcs.top);
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
/*
void PF::Rect1::expand(int n)
{
  Point center;
  get_center(center);
  
  get_top_left().set(std::min(center.get_x(), get_top_left().get_x()-n), std::min(center.get_y(), get_top_left().get_y()-n));
  get_bottom_right().set(std::max(center.get_x(), get_bottom_right().get_x()+n), std::max(center.get_y(), get_bottom_right().get_y()+n));
  
//  get_top_left().set(get_top_left().get_x()-n, get_top_left().get_y()-n);
//  get_bottom_right().set(get_bottom_right().get_x()+n, get_bottom_right().get_y()+n);
}
*/
/*
bool intpoint_inside_trigon(int sx, int sy, int ax, int ay, int bx, int by, int cx, int cy)
{
    int as_x = sx-ax;
    int as_y = sy-ay;

    bool s_ab = (bx-ax)*as_y-(by-ay)*as_x > 0;

    if((cx-ax)*as_y-(cy-ay)*as_x > 0 == s_ab) return false;

    if((cx-bx)*(sy-by)-(cy-by)*(sx-bx) > 0 != s_ab) return false;

    return true;
}
*/

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

void PF::Rect1::build_mask()
{
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
      pmask[x] = f * f * get_opacity(); \
    } \
  } \

  FALLOFF_REC_SIDES(rcs.left, (rcs.left+rcs.width), rc.top, rcs.top, (rcs.top-rc.top), (y-rcs.top));
  FALLOFF_REC_SIDES(rcs.left, (rcs.left+rcs.width), (rcs.top+rcs.height), (rc.top+rc.height), ((rc.top+rc.height)-(rcs.top+rcs.height)), (y-(rcs.top+rcs.height)));
  FALLOFF_REC_SIDES(rc.left, rcs.left, rcs.top, (rcs.top+rcs.height), (rcs.left-rc.left), (x-rcs.left));
  FALLOFF_REC_SIDES((rcs.left+rcs.width), (rc.left+rc.width), rcs.top, (rcs.top+rcs.height), ((rc.left+rc.width)-(rcs.left+rcs.width)), (x-(rcs.left+rcs.width)));
  
#undef FALLOFF_REC_SIDES
  
/*    int x_from, x_to, y_from, y_to;
    float dist_t, dist, dist_t2_1;
    float dist2;

    x_from = rc.left;
    x_to = rcs.left;
    y_from = rc.top;
    y_to = rcs.top;
  */
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
      pmask[x] = f2; \
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
  
  // top fallof
/*  dist_t2 = (rcs.top-rc.top) * (rcs.top-rc.top);
  for(int y = rc.top; y < rcs.top; y++) {
    float* pmask = mask + y * rc.width;
    for(int x = rcs.left; x < rcs.left+rcs.width; x++) {
      const float dist2 = (y-rcs.top) * (y-rcs.top);
      const float f = (dist_t2-dist2) / dist_t2;
      pmask[x] = f * f * get_opacity();
    }
  }

  // bottom fallof
  dist_t2 = ((rc.top+rc.height)-(rcs.top+rcs.height)) * ((rc.top+rc.height)-(rcs.top+rcs.height));
  for(int y = rcs.top+rcs.height; y < rc.top+rc.height; y++) {
    float* pmask = mask + y * rc.width;
    for(int x = rcs.left; x < rcs.left+rcs.width; x++) {
      const float dist2 = (y-(rcs.top+rcs.height)) * (y-(rcs.top+rcs.height));
      const float f = (dist_t2-dist2) / dist_t2;
      pmask[x] = f * f * get_opacity();
    }
  }
*/
  std::cout<<"PF::Rect1::build_mask() end"<<std::endl;

}
#if 0
void PF::Rect1::build_mask()
{
  Rect1 rec = *this;
  rec.offset(-(get_point(0).get_x()-get_falloff_point(0).get_x()), -(get_point(0).get_y()-get_falloff_point(0).get_y()));
  VipsRect rc;
  rec.get_rect(&rc);
  
  std::cout<<"PF::Rect1::build_mask() get_top_left().get_x(): "<<rec.get_top_left().get_x()<<", get_top_left().get_y(): "<<rec.get_top_left().get_y()<<std::endl;
  std::cout<<"PF::Rect1::build_mask() get_top_left_falloff().get_x(): "<<rec.get_top_left_falloff().get_x()<<", get_top_left_falloff().get_y(): "<<rec.get_top_left_falloff().get_y()<<std::endl;

  std::cout<<"PF::Rect1::build_mask() get_bottom_right().get_x(): "<<rec.get_bottom_right().get_x()<<", get_bottom_right().get_y(): "<<rec.get_bottom_right().get_y()<<std::endl;
  std::cout<<"PF::Rect1::build_mask() get_bottom_right_falloff().get_x(): "<<rec.get_bottom_right_falloff().get_x()<<", get_bottom_right_falloff().get_y(): "<<rec.get_bottom_right_falloff().get_y()<<std::endl;

  std::cout<<"PF::Rect1::build_mask() rc.left: "<<rc.left<<", rc.top: "<<rc.top<<std::endl;
  std::cout<<"PF::Rect1::build_mask() rc.width: "<<rc.width<<", rc.height: "<<rc.height<<std::endl;

  if (rc.width < 1 || rc.height < 1) return;
  if (mask != NULL) free(mask);
  
  mask = (float*)malloc(rc.width * rc.height * sizeof(float));
  if (mask == NULL) {
    std::cout<<"PF::Rect1::build_mask() rc.width: could not create mask, not enought memory"<<std::endl;
    return;
  }
  memset(mask, 0, rc.width * rc.height * sizeof(float));
  std::cout<<"PF::Rect1::build_mask() 1 "<<std::endl;

  // we populate the buffer
  VipsRect rcs;
  
  // first we populate the shape
  rcs.left = rec.get_point(0).get_x();
  rcs.top = rec.get_point(0).get_y();
  rcs.width = rec.get_point(1).get_x()-rcs.left;
  rcs.height = rec.get_point(1).get_y()-rcs.top;
  
  for(int y = rcs.top; y < rcs.height+rcs.top; y++) {
    for(int x = rcs.left; x < rcs.width+rcs.left; x++) {
      mask[y * rc.width + x] = 1.0f;
    }
  }
  std::cout<<"PF::Rect1::build_mask() 2 "<<std::endl;
  
  float f;
  std::cout<<"PF::Rect1::build_mask() 3 "<<std::endl;

  // now deal with the falloff
  
  // top/left
  for(int y = rc.top; y < rcs.top; y++) {
    for(int x = rc.left; x < rcs.left; x++) {
      {
        const float x_width = ((float)rcs.width)+(float)(rc.width-rcs.width)*(1.f-((float)y/(float)rcs.top));
        const float xf = ((float)rc.width)*((float)x/(float)x_width);
        const float xs = ((float)rcs.width)*((float)x/(float)x_width);
        const float dist_t = ((float)xs-(float)xf)*((float)xs-(float)xf) + (float)rcs.top*(float)rcs.top;
        const float dist = ((float)rcs.top-(float)y)*((float)rcs.top-(float)y) + ((float)xs-(float)x)*((float)xs-(float)x); 
        
        if (dist_t > 0.f)
          f = 1.f - (dist/dist_t);
        else
          f = 1.f;
        mask[y * rc.width + x] = f*f;
      }
      if (intpoint_inside_trigon(x, y, rc.left, rc.top, rcs.left, rcs.top, rc.left, rcs.top)) { // left
        const float x_width = ((float)rcs.height)+(float)(rc.height-rcs.height)*(1.f-((float)x/(float)rcs.left));
        const float xf = ((float)rc.height)*((float)y/(float)x_width);
        const float xs = ((float)rcs.height)*((float)y/(float)x_width);
        const float dist_t = ((float)xs-(float)xf)*((float)xs-(float)xf) + (float)rcs.left*(float)rcs.left;
        const float dist = ((float)rcs.left-(float)x)*((float)rcs.left-(float)x) + ((float)xs-(float)y)*((float)xs-(float)y); 
        
        if (dist_t > 0.f)
          f = 1.f - (dist/dist_t);
        else
          f = 1.f;
        mask[y * rc.width + x] = std::min(f*f, mask[y * rc.width + x]);
      }
      else { // top
    }
    }
  }

  // top
  for(int y = rc.top; y < rcs.top; y++) {
    for(int x = rcs.left; x < rcs.left+rcs.width; x++) {
      const float x_width = ((float)rcs.width)+(float)(rc.width-rcs.width)*(1.f-((float)y/(float)rcs.top));
      const float xf = ((float)rc.width)*((float)x/(float)x_width);
      const float xs = ((float)rcs.width)*((float)x/(float)x_width);
      const float dist_t = ((float)xs-(float)xf)*((float)xs-(float)xf) + (float)rcs.top*(float)rcs.top;
      const float dist = ((float)rcs.top-(float)y)*((float)rcs.top-(float)y) + ((float)xs-(float)x)*((float)xs-(float)x); 
      
      if (dist_t > 0.f)
        f = 1.f - (dist/dist_t);
      else
        f = 1.f;
      mask[y * rc.width + x] = f*f;
    }
  }

  // left
  for(int y = rcs.top; y < rcs.top+rcs.height; y++) {
    for(int x = rc.left; x < rcs.left; x++) {
      const float x_width = ((float)rcs.height)+(float)(rc.height-rcs.height)*(1.f-((float)x/(float)rcs.left));
      const float xf = ((float)rc.height)*((float)y/(float)x_width);
      const float xs = ((float)rcs.height)*((float)y/(float)x_width);
      const float dist_t = ((float)xs-(float)xf)*((float)xs-(float)xf) + (float)rcs.left*(float)rcs.left;
      const float dist = ((float)rcs.left-(float)x)*((float)rcs.left-(float)x) + ((float)xs-(float)y)*((float)xs-(float)y); 
      
      if (dist_t > 0.f)
        f = 1.f - (dist/dist_t);
      else
        f = 1.f;
      mask[y * rc.width + x] = f*f;
    }
  }

  // bottom
  for(int y = rcs.height+rcs.top; y < rc.height; y++) {
    for(int x = rcs.left; x < rcs.left+rcs.width; x++) {
      const float x_width = ((float)rcs.width)+(float)(rc.width-rcs.width)*(((float)(y-(rcs.height+rcs.top))/(float)(rc.height-(rcs.height+rcs.top))));
      const float xf = ((float)rc.width)*((float)x/(float)x_width);
      const float xs = ((float)rcs.width)*((float)x/(float)x_width);
      const float dist_t = ((float)xs-(float)xf)*((float)xs-(float)xf) + (float)(rc.height-(rcs.height+rcs.top))*(float)(rc.height-(rcs.height+rcs.top));
      const float dist = ((float)y-(float)(rcs.height+rcs.top))*((float)y-(float)(rcs.height+rcs.top)) + ((float)xs-(float)x)*((float)xs-(float)x); 
      
      if (dist_t > 0.f)
        f = 1.f - (dist/dist_t);
      else
        f = 1.f;
      mask[y * rc.width + x] = f*f;
    }
  }

  // right
  for(int y = rcs.top; y < rcs.top+rcs.height; y++) {
    for(int x = rcs.left+rcs.width; x < rc.width; x++) {
      const float x_width = ((float)rcs.height)+(float)(rc.height-rcs.height)*(((float)(x-(rcs.width+rcs.left))/(float)(rc.width-(rcs.width+rcs.left))));
      const float xf = ((float)rc.height)*((float)y/(float)x_width);
      const float xs = ((float)rcs.height)*((float)y/(float)x_width);
      const float dist_t = ((float)xs-(float)xf)*((float)xs-(float)xf) + (float)(rc.width-(rcs.width+rcs.left))*(float)(rc.width-(rcs.width+rcs.left));
      const float dist = ((float)x-(float)(rcs.width+rcs.left))*((float)x-(float)(rcs.width+rcs.left)) + ((float)xs-(float)y)*((float)xs-(float)y); 
      
      if (dist_t > 0.f)
        f = 1.f - (dist/dist_t);
      else
        f = 1.f;
      mask[y * rc.width + x] = f*f;
    }
  }

  
  std::cout<<"PF::Rect1::build_mask() end"<<std::endl;

}
#endif

PF::Shape* PF::ShapesPar::get_shape(int index)
{
  int s_type = get_shapes_order().at(index).first;
  int s_index = get_shapes_order().at(index).second;
  PF::Shape* shape = NULL;
  
    switch(s_type) {
    case PF::ShapesPar::type_line:
      shape = &(get_lines()[s_index]);
      break;
    case PF::ShapesPar::type_circle:
      shape = &(get_circles()[s_index]);
      break;
    case PF::ShapesPar::type_rectangle:
      shape = &(get_rectangles1()[s_index]);
      break;
    }
    return shape;
}


void PF::ShapesPar::add_shape(Line& shape)
{
  lines.get().push_back(shape);
  shapes_order.get().push_back( std::pair<int,int>(type_line, lines.get().size()-1) );
}

void PF::ShapesPar::add_shape(Circle1& shape)
{
  circles.get().push_back(shape);
  shapes_order.get().push_back( std::pair<int,int>(type_circle, circles.get().size()-1) );
}

void PF::ShapesPar::add_shape(Rect1& shape)
{
  rectangles1.get().push_back(shape);
  shapes_order.get().push_back( std::pair<int,int>(type_rectangle, rectangles1.get().size()-1) );
}
/*
void PF::ShapesPar::add_shape(Rectangle& shape)
{
  rectangles.get().push_back(shape);
}
*/
/*
void PF::get_shape_line_points(int x1, int y1, int x2, int y2, std::vector< std::pair<int,int> >& points)
{
  float xdiff = (x2 - x1);
  float ydiff = (y2 - y1);

  if( false && y1<20 )
    std::cout<<"get_shape_line_points("<<x1<<","<<y1<<","<<x2<<","<<y2<<")"<<std::endl;

  if(xdiff == 0.0f && ydiff == 0.0f) {
    points.push_back( std::make_pair(x1, y1) );
    return;
  }
  if(fabs(xdiff) > fabs(ydiff)) {
    //std::cout<<"ydiff="<<ydiff<<std::endl;
    float xmin, xmax;

    // set xmin to the lower x value given
    // and xmax to the higher value
    if(x1 < x2) {
      xmin = x1;
      xmax = x2;
    } else {
      xmin = x2;
      xmax = x1;
    }

    // draw line in terms of y slope
    float slope = ydiff / xdiff;
    float x = x1;
    float incr = (xdiff>0) ? 1.0f : -1.0f;
    while( x != x2) {
      float y = y1 + ((x - x1) * slope);
      points.push_back( std::make_pair((int)rint(x), (int)rint(y)) );
      if( false && y1<20 )
        std::cout<<"  added point "<<(int)rint(x)<<","<<(int)rint(y)<<std::endl;
      x += incr;
    }
    //points.push_back( std::make_pair(x2, y2) );
  } else {
    float ymin, ymax;

    // set ymin to the lower y value given
    // and ymax to the higher value
    if(y1 < y2) {
      ymin = y1;
      ymax = y2;
    } else {
      ymin = y2;
      ymax = y1;
    }

    // draw line in terms of x slope
    float slope = xdiff / ydiff;
    for(int y = ymin; y <= ymax; y += 1) {
      float x = x1 + ((y - y1) * slope);
      points.push_back( std::make_pair((int)rint(x), (int)rint(y)) );
    }
    //points.push_back( std::make_pair(x2, y2) );
  }
}


void PF::init_shape_segment( PF::shape_falloff_segment& seg, int x1, int y1, int x2, int y2, int ymin, int ymax )
{
  seg.x1 = x1; seg.y1 = y1; seg.x2 = x2; seg.y2 = y2;
  seg.fl = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) + 1;
  seg.l = (int)seg.fl;
  float lx = x2-x1;
  float ly = y2-y1;

  if( ly == 0 ) {
    seg.lmin = 0;
    seg.lmax = seg.l;
  } else {
    if( (y1 < ymin) ) {
      int x = x1 + ( lx * (ymin - y1) ) / ly;
      seg.xmin = x;
      seg.lmin = sqrt((x - x1) * (x - x1) + (ymin - y1) * (ymin - y1)) - 1;
    } else if( y1 > ymax ) {
      int x = x1 + ( lx * (ymax - y1) ) / ly;
      seg.xmin = x;
      seg.lmin = sqrt((x - x1) * (x - x1) + (ymax - y1) * (ymax - y1)) - 1;
    } else {
      seg.xmin = x1;
      seg.lmin = 0;
    }

    if( (y2 < ymin) ) {
      int x = x1 + ( lx * (ymin - y1) ) / ly;
      seg.xmax = x;
      seg.lmax = sqrt((x - x1) * (x - x1) + (ymin - y1) * (ymin - y1)) + 1;
    } else if( y2 > ymax ) {
      int x = x1 + ( lx * (ymax - y1) ) / ly;
      seg.xmax = x;
      seg.lmax = sqrt((x - x1) * (x - x1) + (ymax - y1) * (ymax - y1)) + 1;
    } else {
      seg.xmax = x2;
      seg.lmax = seg.l;
    }
  }
}



static void get_falloff_segments( const std::vector< std::pair<int,int> >& points,
    const std::vector< std::pair<int,int> >& border,
    std::vector< PF::shape_falloff_segment >& segments )
{
  int p0[2], p1[2];
  int last0[2] = { -100, -100 }, last1[2] = { -100, -100 };
  //nbp = 0;
  int next = 0;
  for(unsigned int i = 0; i < border.size(); i++) {
    p0[0] = points[i].first, p0[1] = points[i].second;
    if(next > 0)
      p1[0] = border[next].first, p1[1] = border[next].second;
    else
      p1[0] = border[i].first, p1[1] = border[i].second;

    // now we check p1 value to know if we have to skip a part
    if(next == (int)i) next = 0;
    while(p1[0] == -999999) {
      if(p1[1] == -999999)
        next = i - 1;
      else
        next = p1[1];
      p1[0] = border[next].first, p1[1] = border[next].second;
    }

    // and we add the segment
    if(last0[0] != p0[0] || last0[1] != p0[1] || last1[0] != p1[0] || last1[1] != p1[1]) {
      //std::cout<<"adding segment: "<<p0[0]<<","<<p0[1]<<" -> "<<p1[0]<<","<<p1[1]<<std::endl;
      //_shape_falloff(buffer, p0, p1, *posx, *posy, *width);
      PF::shape_falloff_segment s;
      s.x1 = p0[0]; s.y1 = p0[1]; s.x2 = p1[0]; s.y2 = p1[1];
      segments.push_back( s );
      last0[0] = p0[0], last0[1] = p0[1];
      last1[0] = p1[0], last1[1] = p1[1];
    }
  }
}
*/

PF::ShapesPar::ShapesPar():
                OpParBase(),
                /*invert("invert",this,false),
                enable_falloff("enable_falloff",this,true),
                falloff_curve( "falloff_curve", this ), // 0
                smod( "smod", this ),
                border_size( "border_size", this, 0.05 ),*/
                shape_type("shape_type", this, type_circle, "circle", "circle"),
                lines("lines", this),
                circles("circles", this),
                rectangles1("rectangles1", this),
                shapes_order("shapes_order", this)
                /*rectangles("rectangles", this),
                modvec( NULL ), edgevec( NULL ), segvec( NULL )*/
{
/*  float x1 = 0.0, y1 = 1.0, x2 = 1.0, y2 = 0.0;
  falloff_curve.get().set_point( 0, x1, y1 );
  falloff_curve.get().set_point( 1, x2, y2 );
  falloff_curve.store_default();

  smod.store_default();
*/
  //const std::vector< std::pair<float,float> >& points = smod.get().get_points();
  //std::pair<float,float> center = smod.get().get_center();

  shape_type.add_enum_value(type_rectangle, "rectangle", "rectangle");
  shape_type.add_enum_value(type_line, "line", "line");
  
  set_type( "shapes" );
  set_default_name( _("shapes") );
}

VipsImage* PF::ShapesPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  VipsImage* out = PF::OpParBase::build( in, first, imap, omap, level );

  int tw = 64, th = 64, nt = out->Xsize*2/tw;

/*  falloff_curve.get().lock();
  if( falloff_curve.is_modified() || shape_prop.invert.is_modified() ) {
    //std::cout<<"ShapesPar::build(): updating falloff LUT"<<std::endl;
    for(unsigned int i = 0; i <= FormatInfo<unsigned short int>::RANGE; i++) {
      float x = ((float)i)/FormatInfo<unsigned short int>::RANGE;
      float y = falloff_curve.get().get_value( x );
      falloff_vec[i] = shape_prop.invert.get() ? 1.0f-y : y;
    }
  }
  falloff_curve.get().unlock();
*/
  //std::cout<<"ShapesPar::build(): get_smod().get_border_size()="<<get_smod().get_border_size()
  //    <<"  border_size.get()="<<border_size.get()<<std::endl;
/*  bool shape_modified = smod.is_modified();
  if( get_smod().get_border_size() != border_size.get() ) {
    get_smod().set_border_size( border_size.get() );
    shape_modified = true;
  }
  if( shape_modified || (out->Xsize != get_smod().get_wd_last()) ||
      (out->Ysize != get_smod().get_ht_last()) ) {
    get_smod().update_outline( out->Xsize, out->Ysize );
    shape_modified = true;
  }

  const std::vector< std::pair<float,float> >& points = smod.get().get_points();
  int ps = points.size();

  //std::cout<<"ShapesPar::build(): shape_modified="<<shape_modified<<std::endl;
  if( shape_modified ) {
    //std::cout<<"ShapesPar::build(): updating shape"<<std::endl;
    if( edgevec ) delete[] edgevec;
    edgevec = new std::vector<shape_point>[out->Ysize];

    const std::vector< std::pair<int,int> >& spline_points  = get_smod().get_outline();
    const std::vector< std::pair<int,int> >& spline_points2 = get_smod().get_border();

    std::vector<shape_point> shape_points;
    for( unsigned int pi = 0; pi < spline_points.size(); pi++ ) {
      unsigned int pi2 = pi + 1;
      if( pi2 >= spline_points.size() ) pi2 = 0;
      double px  = spline_points[pi].first,  py  = spline_points[pi].second;
      double px2 = spline_points[pi2].first, py2 = spline_points[pi2].second;
      int ipx = px, ipy = py;
      int ipx2 = px2, ipy2 = py2;
      if( (ipx == ipx2) && (ipy == ipy2) )
        continue;
      shape_point pt;
      pt.x = ipx;
      pt.y = ipy;
      pt.state_changing = false;
      shape_points.push_back( pt );
      //std::cout<<"shape point #"<<shape_points.size()-1<<"  px="<<ipx<<"  py="<<ipy
      //    <<"  pi2="<<pi2<<"  px2="<<ipx2<<"  py2="<<ipy2<<std::endl;
    }
    //std::cout<<"ShapesPar::build(): spline_points.size()="<<spline_points.size()<<"  shape_points.size()="<<shape_points.size()<<std::endl;

    int point_start = -1;
    for( unsigned int pi = 0; pi < shape_points.size(); pi++ ) {
      unsigned int pi2 = pi + 1;
      if( pi2 >= shape_points.size() ) pi2 = 0;
      int py = shape_points[pi].y;
      int py2 = shape_points[pi2].y;
      if( py != py2 ) {
        point_start = pi2;
        break;
      }
    }
    if( point_start >= 0 ) {
      int pj = point_start;
      int yprev;
      do {
        int pjprev = pj - 1;
        if( pjprev < 0 ) pjprev = shape_points.size() - 1;
        int pjnext = pj + 1;
        if( pjnext >= (int)shape_points.size() ) pjnext = 0;

        int px = shape_points[pj].x;
        int py = shape_points[pj].y;
        int pxprev = shape_points[pjprev].x;
        int pxnext = shape_points[pjnext].x;
        int pyprev = shape_points[pjprev].y;
        int pynext = shape_points[pjnext].y;
        if( pyprev != py ) yprev = pyprev;

        int dy1 = py - yprev;
        int dy2 = py - pynext;
        int dy = dy1 * dy2;
        //std::cout<<"pj="<<pj<<"  px="<<px<<"  py="<<py<<"  yprev="<<yprev<<"  pynext="<<pynext<<" dy="<<dy<<std::endl;
        if( dy < 0 ) shape_points[pj].state_changing = true;

        pj += 1;
        if( pj >= (int)shape_points.size() ) pj = 0;
      } while( pj != point_start );
    }


    std::pair<float,float> outline_center = get_smod().get_center();
    float bx = outline_center.first * out->Xsize;
    float by = outline_center.second * out->Ysize;

    ptvec.clear();
    //std::cout<<"spline_points.size():  "<<spline_points.size()<<std::endl;
    //std::cout<<"spline_points2.size(): "<<spline_points2.size()<<std::endl;
    int xlast=-1, ylast=-1;
    int yprev = -1000000;
    for( unsigned int pi = 0; pi < shape_points.size(); pi++ ) {
      double px = shape_points[pi].x, py = shape_points[pi].y;

      int ipx = px, ipy = py;
      //if( ipx==xlast && ipy==ylast ) continue;
      xlast = ipx; ylast = ipy;
      if( yprev == -1000000 ) yprev = ipy;

      int pi2 = pi + 1;
      if( pi2 >= (int)shape_points.size() ) pi2 = 0;

      double px2 = shape_points[pi2].x, py2 = shape_points[pi2].y;

      int ipx2 = px2, ipy2 = py2;

      if( (ipx==ipx2) && (ipy==ipy2) ) continue;

      //std::cout<<"pi="<<pi<<"  pi2="<<pi2<<"  ipx="<<ipx<<"  ipy="<<ipy<<"  ipx2="<<ipx2<<"  ipy2="<<ipy2<<std::endl;

      get_shape_line_points( ipx, ipy, ipx2, ipy2, ptvec );
      std::vector< std::pair<int,int> > ptvec2;
      get_shape_line_points( ipx, ipy, ipx2, ipy2, ptvec2 );
      int ylast = -1;
      for( unsigned int i = 0; i < ptvec2.size(); i++ ) {
        int x = ptvec2[i].first;
        int y = ptvec2[i].second;
        if( x==ipx2 && y==ipy2 ) continue;
        if( (y<0) || (y>=out->Ysize) ) continue;
        if( ipy < 20 ) {
          //std::cout<<"spline point "<<x<<","<<y<<std::endl;
        }
        int idx = -1;
        for( unsigned int j = 0; j < edgevec[y].size(); j++ ) {
          if( edgevec[y][j].x >= x ) {
            idx = j; break;
          }
        }
        shape_point ppt; ppt.x = x; ppt.y = y; ppt.state_changing = false;
        //if( (i == 0) || (y != ipy) ) ppt.state_changing = shape_points[pi].state_changing;
        if( y != ylast ) ppt.state_changing = shape_points[pi].state_changing;
        //std::cout<<"pi="<<pi<<"  x="<<x<<"  y="<<y<<"  state_changing="<<ppt.state_changing<<std::endl;
        ylast = y;
        if( idx < 0 ) edgevec[y].push_back( ppt );
        else {
          if( edgevec[y][idx].x > x )
            edgevec[y].insert( edgevec[y].begin()+idx, ppt );
        }
      }
    }
    //std::cout<<"Edge vectors filling ended"<<std::endl;


    for( int y = 0; y < out->Ysize; y++ ) {
      shape_point ppt; ppt.x = out->Xsize; ppt.y = y; ppt.state_changing = false;
      if( edgevec[y].empty() ) edgevec[y].push_back( ppt );
    }

    std::vector< PF::shape_falloff_segment > segments;
    get_falloff_segments( spline_points, spline_points2, segments );

    if( segvec ) delete[] segvec;
    segvec = new std::vector< PF::shape_falloff_segment >[out->Ysize/tw+1];

    for( int y = 0, si = 0; y < out->Ysize; y+=tw, si++ ) {
      //std::cout<<"ShapesPar::build(): Filling segment "<<si<<"  ("<<y<<" -> "<<y+tw<<")"<<std::endl;
      //std::cout<<"  segments.size()="<<segments.size()<<std::endl;
      //if( segments.size()>0 ) std::cout<<"  segments[0]="<<segments[0].x1<<","<<segments[0].x1<<","<<segments[0].y1<<" -> "<<segments[0].x2<<","<<segments[0].y2<<std::endl;
      for( unsigned int i = 0; i < segments.size(); i++ ) {
        int ipx = segments[i].x1;
        int ipy = segments[i].y1;
        int ibpx = segments[i].x2;
        int ibpy = segments[i].y2;

        bool crossing = true;
        if( (ipy < y) && (ibpy < y) ) crossing = false;
        if( (ipy >= (y+tw)) && (ibpy >= (y+tw)) ) crossing = false;

        //std::cout<<"y="<<y<<"->"<<y+tw-1<<"  ipy="<<ipy<<"  ibpy="<<ibpy<<"  crossing="<<crossing<<std::endl;

        if( crossing ) {
          //std::cout<<"  crossing segment found: "<<ipx<<","<<ipy<<" -> "<<ibpx<<","<<ibpy<<std::endl;
          PF::shape_falloff_segment& seg = segments[i];
          PF::init_shape_segment( seg, ipx, ipy, ibpx, ibpy, y, y+tw-1 );
          segvec[si].push_back( seg );
        }
      }
      //std::cout<<"ShapesPar::build(): segvec["<<si<<"].size()="<<segvec[si].size()<<std::endl;
    }
  }
*/
  VipsImage* cached;
  VipsAccess acc = VIPS_ACCESS_RANDOM;
  bool threaded = true, persistent = false;
  if( vips_tilecache(out, &cached,
      "tile_width", tw, "tile_height", th, "max_tiles", nt,
      "access", acc, "threaded", threaded, "persistent", persistent, NULL) ) {
    std::cout<<"ShapesPar::build(): vips_tilecache() failed."<<std::endl;
    return NULL;
  }
  PF_UNREF( out, "GaussBlurPar::build(): iter_in unref" );

  return cached;
}


PF::ProcessorBase* PF::new_shapes()
{
  return( new PF::Processor<PF::ShapesPar,PF::Shapes>() );
}


