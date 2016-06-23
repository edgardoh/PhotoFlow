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

#include "../base/processor.hh"
#include "shapes.hh"
#include "shapes_op.hh"

namespace PF 
{

class ShapesMaskPar: public ShapesPar
{

public:
  ShapesMaskPar();
  ~ShapesMaskPar();

  bool has_intensity() { return false; }
  bool needs_input() { return true; }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};

template < OP_TEMPLATE_DEF >
class ShapesMaskProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
    std::cout<<"PF::ShapesMaskProc::render()"<<std::endl;
  }
};


template < OP_TEMPLATE_DEF_TYPE_SPEC >
class ShapesMaskProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    std::cout<<"PF::ShapesMaskProc::render() 2"<<std::endl;
  }
  
};

class ShapesMaskAlgoPar: public ShapesAlgoPar
{
  
public:

  ShapesMaskAlgoPar();

  ~ShapesMaskAlgoPar()
  {
  }
  
  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);

};

template < OP_TEMPLATE_DEF >
class ShapesMaskAlgoProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
    std::cout<<"PF::ShapesMaskAlgoProc::render()"<<std::endl;
  }
};



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class ShapesMaskAlgoProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
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

    ShapesMaskAlgoPar* opar = dynamic_cast<ShapesMaskAlgoPar*>( par );
    if (opar == NULL) {
      std::cout<<"PF::ShapesMaskAlgoProc::render() (opar == NULL)"<<std::endl;
      return;
    }

    for (int i = 0; i < opar->get_shapes_count(); i++) {
      PF::Shape* shape = opar->get_shape(i);
      if (shape == NULL) {
        std::cout<<"PF::ShapesMaskAlgoProc::render() (shape == NULL) "<<std::endl;
        return;
      }

      float *mask = shape->get_mask();
      if (mask == NULL) {
        std::cout<<"PF::ShapesMaskAlgoProc::render() (mask == NULL) "<<std::endl;
        return;
      }

      VipsRect mask_area, out_area/*, in_area*/;
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


  ProcessorBase* new_shapes_mask();
}

#endif 

