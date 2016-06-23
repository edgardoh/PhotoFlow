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

#include "draw_shapes.hh"
#include "../base/processor.hh"


PF::DrawShapesPar::DrawShapesPar():
ShapesPar(),
fgd_color( "fgd_color", this, RGBColor(1,1,1) ),
bgd_color( "bgd_color", this, RGBColor(0,0,0) ),
bgd_transparent( "bgd_transparent", this, false )
{
  shapes_algo = new PF::Processor<PF::DrawShapesAlgoPar,PF::DrawShapesAlgoProc>();

  get_shapes_group().get_current_color().set(1.f, 1.f, 1.f);

  set_type( "draw_shapes" );
  set_default_name( _("draw shapes") );
}

PF::DrawShapesPar::~DrawShapesPar()
{
}

void PF::DrawShapesPar::set_fgd_color(float r, float g, float b)
{
  get_fgd_color().get().r = r;
  get_fgd_color().get().g = g;
  get_fgd_color().get().b = b;
  
  get_shapes_group().get_current_color().set(r, g, b);
}

void PF::DrawShapesPar::set_bgd_color(float r, float g, float b)
{
  get_bgd_color().get().r = r;
  get_bgd_color().get().g = g;
  get_bgd_color().get().b = b;
}

VipsImage* PF::DrawShapesPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;

  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  if (shapes_algo == NULL) {
    std::cout<<"PF::DrawShapesPar::build() (shapes_algo == NULL)"<<std::endl;
    return NULL;
  }
  if (shapes_algo->get_par() == NULL) {
    std::cout<<"PF::DrawShapesPar::build() (shapes_algo->get_par() == NULL)"<<std::endl;
    return NULL;
  }
  DrawShapesAlgoPar* shapes_algo_par = dynamic_cast<DrawShapesAlgoPar*>( shapes_algo->get_par() );
  if (shapes_algo_par == NULL) {
    std::cout<<"PF::DrawShapesPar::build() (shapes_algo_par == NULL)"<<std::endl;
    return NULL;
  }

  shapes_algo_par->build_masks(get_shapes_group(), get_shapes_falloff_curve(), level); 
  
  shapes_algo_par->set_transparent( get_bgd_transparent().get() );
  shapes_algo_par->get_bgd_color().set( get_bgd_color().get().r, get_bgd_color().get().g, get_bgd_color().get().b );
  shapes_algo_par->set_image_hints( srcimg );
  shapes_algo_par->set_format( get_format() );
  in2.clear();
  in2.push_back( srcimg );
  VipsImage* out = shapes_algo_par->build( in2, 0, NULL, NULL, level );
//  PF_UNREF( srcimg, "DrawShapesPar::build(): srcimg unref" );

  set_image_hints( out );

  return out;
  
}

PF::DrawShapesAlgoPar::DrawShapesAlgoPar():
    ShapesAlgoPar()
{
  set_type( "shapes_algo" );
  set_default_name( _("shapes_algo") );
}

VipsImage* PF::DrawShapesAlgoPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  std::cout<<"PF::DrawShapesAlgoPar::build()"<<std::endl;
  
  VipsImage* out = PF::OpParBase::build(in, first, NULL, NULL, level);
  
  std::cout<<"PF::DrawShapesAlgoPar::build() 2"<<std::endl;
  
  return out;
}


PF::ProcessorBase* PF::new_draw_shapes()
{
  return( new PF::Processor<PF::DrawShapesPar,PF::DrawShapesProc>() );
}

