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

#ifndef PF_SHAPES_OP_HH
#define PF_SHAPES_OP_HH

#include <math.h>

#include <assert.h>
#include <iostream>

#include <gtkmm.h>

#include "../base/format_info.hh"
#include "../base/color.hh"
#include "../base/processor.hh"
#include "../base/splinecurve.hh"
#include "../gui/doublebuffer.hh"

#include "shapes.hh"
#include "curves.hh"

namespace PF 
{

class ShapesPar: public OpParBase
{
  Property<SplineCurve> falloff_curve;
  Property<ShapesGroup> shapes;
  
protected:
  ShapesGroup shapes_scaled;

public:

  ShapesPar();

  ShapesGroup& get_shapes_group() { return shapes.get(); }
  SplineCurve& get_shapes_falloff_curve() { return falloff_curve.get(); }

  void shapes_modified() { shapes.modified(); }
  
  int add_shape(Shape* shape) { return shapes.get().add(shape); }
  void remove_shape(int n) { return shapes.get().remove(n); }

  PF::Shape* get_shape(int index) { return shapes.get().get_shape(index); }
  PF::Shape* get_shape_scaled(int index) { return shapes_scaled.get_shape(index); }
  
  int get_shapes_count() { return shapes.get().size(); }
  int get_shapes_scaled_count() { return shapes_scaled.size(); }

  bool needs_input() { return false; }
  bool has_intensity() { return false; }

  void build_masks(ShapesGroup& sg, SplineCurve& scurve, unsigned int level);

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

  ProcessorBase* new_shapes();
}

#endif 


