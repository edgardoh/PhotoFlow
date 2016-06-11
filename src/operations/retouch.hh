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

#ifndef PF_RETOUCH_H
#define PF_RETOUCH_H

#include <iostream>

#include "../base/format_info.hh"
#include "../base/operation.hh"
#include "../base/processor.hh"
#include "../base/rawbuffer.hh"

#include "shapes.hh"

//#include "diskbuffer.hh"
//#include "blender.hh"

namespace PF 
{
/*
#define StampMask PencilMask


class Stamp
{
  unsigned int size;
  float opacity;
  float smoothness;
  StampMask mask;

public:
  Stamp(): size(0), opacity(1), smoothness(0) {}
  Stamp( const Stamp& s )
  {
    //std::cout<<"Stamp(const Stamp& s) called."<<std::endl;
    size = s.get_size();
    opacity = s.get_opacity();
    smoothness = s.get_smoothness();
    init_mask();
  }
  void set_size( unsigned int s ) { size = s; init_mask(); }
  unsigned int get_size() const { return size; }

  void set_opacity( float val ) { opacity = val; }
  float get_opacity() const { return opacity; }

  void set_smoothness( float val ) { smoothness = val; init_mask(); }
  float get_smoothness() const { return smoothness; }

  void init_mask()
  {
    mask.init( size*2+1, opacity, smoothness );
  }
  StampMask& get_mask() { return mask; }
};


inline
bool operator ==(const Stamp& l, const Stamp& r)
{
  if( l.get_opacity() != r.get_opacity() ) return false;
  if( l.get_smoothness() != r.get_smoothness() ) return false;
  if( l.get_size() != r.get_size() ) return false;
  return true;
}

inline
bool operator !=(const Stamp& l, const Stamp& r)
{
  return( !(l==r) );
}


inline std::istream& operator >>( std::istream& str, Stamp& pen )
{
  unsigned int size;
  float opacity;
  float smoothness;
  str>>size>>opacity>>smoothness;
  pen.set_size( size );
  pen.set_opacity( opacity );
  pen.set_smoothness( smoothness );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const Stamp& pen )
{
  str<<pen.get_size()<<" "<<pen.get_opacity()<<" "<<pen.get_smoothness()<<" ";
  return str;
}



class StrokesGroup
{
  int delta_row, delta_col;
  std::vector< Stroke<Stamp> > strokes;
public:
  StrokesGroup(): delta_row(0), delta_col(0)
{
}

  int get_delta_row() const { return delta_row; }
  void set_delta_row( int d ) { delta_row = d; }
  int get_delta_col() const { return delta_col; }
  void set_delta_col( int d ) { delta_col = d; }
  std::vector< Stroke<Stamp> >& get_strokes() { return strokes; }
  const std::vector< Stroke<Stamp> >& get_strokes() const { return strokes; }
};



inline
bool operator ==(const StrokesGroup& l, const StrokesGroup& r)
{
  if( l.get_delta_row() != r.get_delta_row() ) return false;
  if( l.get_delta_col() != r.get_delta_col() ) return false;
  if( l.get_strokes() != r.get_strokes() ) return false;
  return true;
      }

inline
bool operator !=(const StrokesGroup& l, const StrokesGroup& r)
      {
  return( !(l==r) );
      }


inline std::istream& operator >>( std::istream& str, StrokesGroup& group )
{
  int delta_row, delta_col;
  str>>delta_row>>delta_col;
  str>>group.get_strokes();
  group.set_delta_row( delta_row );
  group.set_delta_col( delta_col );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const StrokesGroup& group )
{
  str<<group.get_delta_row()<<" "<<group.get_delta_col()<<" ";
  str<<group.get_strokes();
  return str;
}



template<> inline
void set_gobject_property< std::vector<StrokesGroup> >(gpointer object, const std::string name,
    const std::vector<StrokesGroup>& value)
    {
    }
*/


class RetouchPar: public ShapesPar /*OpParBase*/
{
/*  Property<int> stamp_size;
  Property<float> stamp_opacity;
  Property<float> stamp_smoothness;
  Property< std::vector<StrokesGroup> > strokes;
*/
  int scale_factor;

//  Stamp pen;
//  Circle1 circle_draw;
//  Rect1 rect_draw;

public:
  RetouchPar();
  ~RetouchPar();

  bool has_intensity() { return false; }
  bool needs_input() { return true; }

//  Stamp& get_pen() { return pen; }

  int get_scale_factor() { return scale_factor; }
//  Circle1* get_circle_draw() { return &circle_draw; }
//  Rect1* get_rect_draw() { return &rect_draw; }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);

/*  void new_group( int dr, int dc );

  void start_stroke()
  {
    start_stroke( stamp_size.get(), stamp_opacity.get(), stamp_smoothness.get() );
  }
  void start_stroke( unsigned int pen_size, float opacity, float smoothness );
  void end_stroke();

  std::vector<StrokesGroup>& get_strokes() { return strokes.get(); }

  void draw_point( unsigned int x, unsigned int y, VipsRect& update );
  */
};



template < OP_TEMPLATE_DEF >
class RetouchProc
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, RetouchPar* par)
  {
  }
};


ProcessorBase* new_retouch();
}

#endif 


