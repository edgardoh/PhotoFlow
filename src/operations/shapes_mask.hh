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

//#include <math.h>

//#include <iostream>

//#include "../base/format_info.hh"
//#include "../base/color.hh"
#include "../base/processor.hh"
//#include "../base/splinecurve.hh"

#include "shapes.hh"

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
/*
template < OP_TEMPLATE_DEF >
class ShapesMask: public IntensityProc<T, has_imap>
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, ShapesPar* par)
  {
    std::cout<<"PF::ShapesMask::render()"<<std::endl;
  }

};
*/
/*
template< OP_TEMPLATE_DEF >
void ShapesMask< OP_TEMPLATE_IMP >::
render(VipsRegion** ir, int n, int in_first,
    VipsRegion* imap, VipsRegion* omap,
    VipsRegion* oreg, ShapesPar* par)
{

  std::cout<<"PF::ShapesMask::render() 2"<<std::endl;
  
  Rect *r = &oreg->valid;
  const int bands = oreg->im->Bands;
  T* pout;

  for( int y = 0; y < r->height; y++ ) {
    pout = (T*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );

    for( int x = 0; x < r->width; x++, pout += bands ) {
      for( int b = 0; b < bands; b++ )
        pout[b] = FormatInfo<T>::MAX;
    }
  }

  if (par->get_shapes_algo() == NULL) {
    std::cout<<"PF::ShapesMask::render() (par->get_shapes_algo() == NULL)"<<std::endl;
    return;
  }
  if (par->get_shapes_algo()->get_par() == NULL) {
    std::cout<<"PF::ShapesMask::render() (par->get_shapes_algo()->get_par() == NULL)"<<std::endl;
    return;
  }
  
  ShapesAlgoPar* shapes_algo_par = dynamic_cast<ShapesAlgoPar*>( par->get_shapes_algo()->get_par() );
  if (shapes_algo_par == NULL) {
    std::cout<<"PF::ShapesMask::render() (shapes_algo_par == NULL)"<<std::endl;
    return;
  }

  if (shapes_algo_par) {
    PF::Shape* shape;
    
//    float *mask = shape->get_mask();

  }
  
};
*/

template < OP_TEMPLATE_DEF >
class ShapesMask
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
    std::cout<<"PF::ShapesMask::render()"<<std::endl;
  }
};



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class ShapesMask< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    std::cout<<"PF::ShapesMask::render() 2"<<std::endl;
    
/*    Rect *r = &oreg->valid;
    const int bands = oreg->im->Bands;
    float* pout;

    for( int y = 0; y < r->height; y++ ) {
      pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top+y );

      for( int x = 0; x < r->width; x++, pout += bands ) {
        for( int b = 0; b < bands; b++ )
          pout[b] = 1.f;
      }
    }

    ShapesMaskPar* mask_par = dynamic_cast<ShapesMaskPar*>( par );
    if (mask_par == NULL) {
      std::cout<<"PF::ShapesMask::render() (mask_par == NULL)"<<std::endl;
      return;
    }
    
    ShapesAlgoPar* shapes_algo_par = dynamic_cast<ShapesAlgoPar*>( mask_par->get_shapes_algo()->get_par() );
    if (shapes_algo_par == NULL) {
      std::cout<<"PF::ShapesMask::render() (shapes_algo_par == NULL)"<<std::endl;
      return;
    }

    if (shapes_algo_par) {
      PF::Shape* shape;
      
  //    float *mask = shape->get_mask();

    }
    */
  }
  
};



  ProcessorBase* new_shapes_mask();
}

#endif 


