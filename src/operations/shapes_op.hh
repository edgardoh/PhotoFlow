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
//  Property<bool> hide_outline;
//  Property<bool> lock_source;
  
protected:
  ProcessorBase* shapes_algo;

public:

  ShapesPar();

  ShapesGroup& get_shapes_group() { return shapes.get(); }
  ProcessorBase* get_shapes_algo() { return shapes_algo; }
  SplineCurve& get_shapes_falloff_curve() { return falloff_curve.get(); }
//  bool get_hide_outline() { return hide_outline.get(); }
//  bool get_lock_source() { return lock_source.get(); }

  void shapes_modified() { shapes.modified(); }
  
  int add_shape(Shape* shape) { return shapes.get().add(shape); }
  void remove_shape(int n) { return shapes.get().remove(n); }

  PF::Shape* get_shape(int index) { return shapes.get().get_shape(index); }
  
  int get_shapes_count() { return shapes.get().size(); }

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
  ShapesGroup shapes;
  
public:

  ShapesAlgoPar();

  ~ShapesAlgoPar()
  {
  }

  ShapesGroup& get_shapes_group() { return shapes; }

  void build_masks(ShapesGroup& sg, SplineCurve& scurve, unsigned int level);

  PF::Shape* get_shape(int index) { return shapes.get_shape(index); }
  int get_shapes_count() { return shapes.size(); }

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
    Rect *r = &oreg->valid;
    Rect *ir = &ireg[0]->valid;
    const int ch = oreg->im->Bands;
//    const int bands = oreg->im->Bands;
    float* pout;

    for( int y = 0; y < r->height; y++ ) {
      pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );
      
      memset(pout, 1.0f, r->width*ch*sizeof(float));
/*      for( int x = 0; x < r->width; x++, pout += bands ) {
        for( int b = 0; b < bands; b++ )
          pout[b] = 1.f;
      }*/
    }

    ShapesAlgoPar* opar = dynamic_cast<ShapesAlgoPar*>( par );
    if (opar == NULL) {
      std::cout<<"PF::ShapesAlgoProc::render() (opar == NULL)"<<std::endl;
      return;
    }

    for (int i = 0; i < opar->get_shapes_count(); i++) {
      PF::Shape* shape = opar->get_shape(i);
      if (shape == NULL) {
        std::cout<<"PF::ShapesAlgoProc::render() (shape == NULL) "<<std::endl;
        return;
      }

      float *mask = shape->get_mask();
      if (mask == NULL) {
        std::cout<<"PF::ShapesAlgoProc::render() (mask == NULL) "<<std::endl;
        return;
      }

      VipsRect mask_area, out_area;
      shape->get_mask_rect(&mask_area);

      vips_rect_intersectrect( r, &mask_area, &out_area );

      for( int y = 0; y < out_area.height; y++ ) {
        pout = (float*)VIPS_REGION_ADDR( oreg, out_area.left, out_area.top + y );
        float* pmask = mask + ( y + out_area.top-mask_area.top) * mask_area.width;

        for( int x = 0; x < out_area.width; x++, pout+=ch ) {
          const float f = pmask[x + out_area.left - mask_area.left];
          if (f > 0.) {
            for (int c = 0; c < ch; c++)
              pout[c] = (1.f - f);
          }
        }
      }
    }
  }

};


  ProcessorBase* new_shapes();
}

#endif 


