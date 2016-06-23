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

#ifndef PF_DRAW_SHAPES_H
#define PF_DRAW_SHAPES_H

#include "../base/processor.hh"
#include "draw.hh"
#include "shapes.hh"
#include "shapes_op.hh"

namespace PF 
{

class DrawShapesPar: public ShapesPar
{
  Property<RGBColor> fgd_color;
  Property<RGBColor> bgd_color;
  Property<bool> bgd_transparent;

public:
  DrawShapesPar();
  ~DrawShapesPar();

  Property<RGBColor>& get_fgd_color() { return fgd_color; }
  void set_fgd_color(float r, float g, float b);
  
  Property<RGBColor>& get_bgd_color() { return bgd_color; }
  void set_bgd_color(float r, float g, float b);
  
  Property<bool>& get_bgd_transparent() { return bgd_transparent; }

  bool has_intensity() { return false; }
  bool needs_input() { return false; }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};

template < OP_TEMPLATE_DEF >
class DrawShapesProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
    std::cout<<"PF::DrawShapesProc::render()"<<std::endl;
  }
};

template < OP_TEMPLATE_DEF_TYPE_SPEC >
class DrawShapesProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    std::cout<<"PF::DrawShapesProc::render() 2"<<std::endl;
  }
  
};

class DrawShapesAlgoPar: public ShapesAlgoPar
{
  ShapeColor bgd_color;
  bool transparent;
  
public:

  DrawShapesAlgoPar();

  ~DrawShapesAlgoPar()
  {
  }

  bool get_transparent() { return transparent; }
  void set_transparent(bool t) { transparent = t; }
  
  ShapeColor& get_bgd_color() { return bgd_color; }
  void set_bgd_color(ShapeColor& c) { bgd_color = c; }
  
  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);

};

template < OP_TEMPLATE_DEF >
class DrawShapesAlgoProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
    std::cout<<"PF::DrawShapesAlgoProc::render()"<<std::endl;
  }
};


template < OP_TEMPLATE_DEF_TYPE_SPEC >
class DrawShapesAlgoProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    DrawShapesAlgoPar* opar = dynamic_cast<DrawShapesAlgoPar*>( par );
    if (opar == NULL) {
      std::cout<<"PF::DrawShapesAlgoProc::render() (opar == NULL)"<<std::endl;
      return;
    }

    Rect *r = &oreg->valid;
    Rect *ir = &ireg[0]->valid;
    const int ch = oreg->im->Bands;
    const int bands = oreg->im->Bands;
    float* pout;

    if ( opar->get_transparent() ) {
      vips_region_copy (ireg[in_first], oreg, r, r->left, r->top);
    }
    else {
      ShapeColor& current_color = opar->get_bgd_color();

      for( int y = 0; y < r->height; y++ ) {
        pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );

        for( int x = 0; x < r->width; x++, pout += bands ) {
          pout[0] = current_color.get_r();
          pout[1] = current_color.get_g();
          pout[2] = current_color.get_b();
        }
      }
    }

    for (int i = 0; i < opar->get_shapes_count(); i++) {
      PF::Shape* shape = opar->get_shape(i);
      if (shape == NULL) {
        std::cout<<"PF::DrawShapesAlgoProc::render() (shape == NULL) "<<std::endl;
        return;
      }

      float *mask = shape->get_mask();
      if (mask == NULL) {
        std::cout<<"PF::DrawShapesAlgoProc::render() (mask == NULL) "<<std::endl;
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
            pout[0] = (shape->get_color().get_r() * f) + pout[0] * (1.f - f);
            pout[1] = (shape->get_color().get_g() * f) + pout[1] * (1.f - f);
            pout[2] = (shape->get_color().get_b() * f) + pout[2] * (1.f - f);
          }
        }
      }
    }
  }

};


  ProcessorBase* new_draw_shapes();
}

#endif 

