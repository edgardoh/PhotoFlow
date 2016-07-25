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

#ifndef PF_SHAPES_MASK_H
#define PF_SHAPES_MASK_H
/*
#include <math.h>

#include <iostream>

#include "../base/format_info.hh"
#include "../base/color.hh"
#include "../base/processor.hh"
#include "../base/splinecurve.hh"

#include "curves.hh"
*/
#include "shapes.hh"
#include "shapes_op.hh"

namespace PF 
{

class ShapesMaskPar: public ShapesPar
{
  Property<bool> invert;

public:

  ShapesMaskPar();

  bool get_invert() { return invert.get(); }

  bool needs_input() { return false; }
  bool has_intensity() { return false; }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};


template < OP_TEMPLATE_DEF >
class ShapesMaskProc: public IntensityProc<T, has_imap>
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, ShapesMaskPar* par);

};

template< OP_TEMPLATE_DEF >
void ShapesMaskProc< OP_TEMPLATE_IMP >::
render(VipsRegion** ir, int n, int in_first,
    VipsRegion* imap, VipsRegion* omap,
    VipsRegion* oreg, ShapesMaskPar* par)
{

  ShapesMaskPar* opar = par;
  if (opar == NULL) {
    std::cout<<"PF::ShapesMaskProc::render() (opar == NULL)"<<std::endl;
    return;
  }

  Rect *r = &oreg->valid;
  int bands = oreg->im->Bands;
  int line_size = r->width * bands;
  bool invert = opar->get_invert();

  T* pout;

  if ( invert ) {
    for(int y = 0; y < r->height; y++ ) {
      pout = (T*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );
      for(int x = 0; x < line_size; x++) pout[x] = FormatInfo<T>::MAX;
    }
  }
  else {
    for(int y = 0; y < r->height; y++ ) {
      pout = (T*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );
      for(int x = 0; x < line_size; x++) pout[x] = FormatInfo<T>::MIN;
    }
  }


  for (int i = 0; i < opar->get_shapes_scaled_count(); i++) {
    PF::Shape* shape = opar->get_shape_scaled(i);
    if (shape == NULL) {
      std::cout<<"PF::ShapesMaskProc::render() (shape == NULL) "<<std::endl;
      return;
    }

    float *mask = shape->get_mask();
    if (mask == NULL) {
      std::cout<<"PF::ShapesMaskProc::render() (mask == NULL) "<<std::endl;
      return;
    }

    VipsRect mask_area, out_area;
    float f;
    shape->get_mask_rect(&mask_area);

    vips_rect_intersectrect( r, &mask_area, &out_area );

    for( int y = 0; y < out_area.height; y++ ) {
      pout = (T*)VIPS_REGION_ADDR( oreg, out_area.left, out_area.top + y );
      float* pmask = mask + ( y + out_area.top-mask_area.top) * mask_area.width;

      for( int x = 0; x < out_area.width; x++, pout+=bands ) {
        f = pmask[x + out_area.left - mask_area.left];
        if (f > 0.f) {
          for (int c = 0; c < bands; c++) {
            if ( invert ) f = (1.f - f);
            pout[c] = static_cast<T>(f);
          }
        }
      }
    }
  }

}




  ProcessorBase* new_shapes_mask();
}

#endif 


