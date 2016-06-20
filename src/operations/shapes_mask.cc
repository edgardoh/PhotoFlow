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

#include "shapes_mask.hh"
#include "../base/processor.hh"


PF::ShapesMaskPar::ShapesMaskPar():
ShapesPar()
{
  set_type( "shapes_mask" );
  set_default_name( _("shapes mask") );
}

PF::ShapesMaskPar::~ShapesMaskPar()
{
}


VipsImage* PF::ShapesMaskPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  std::cout<<"PF::ShapesPar::build()"<<std::endl;
  
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;

  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

/*  set_scale_factor( 1 );
  for(unsigned int l = 0; l < level; l++ ) {
    set_scale_factor( get_scale_factor() * 2 );
  }
*/
//  build_masks(level);
  
  if (get_shapes_algo() == NULL) {
    std::cout<<"PF::ShapesPar::build() (shapes_algo == NULL)"<<std::endl;
    return NULL;
  }
  if (get_shapes_algo()->get_par() == NULL) {
    std::cout<<"PF::ShapesPar::build() (shapes_algo->get_par() == NULL)"<<std::endl;
    return NULL;
  }
  ShapesAlgoPar* shapes_algo_par = dynamic_cast<ShapesAlgoPar*>( get_shapes_algo()->get_par() );
  if (shapes_algo_par == NULL) {
    std::cout<<"PF::ShapesPar::build() (shapes_algo_par == NULL)"<<std::endl;
    return NULL;
  }

  shapes_algo_par->build_masks(get_ShapesGroup(), get_shapes_falloff_curve(), level); 
  
//  Shape* shape = NULL;
//  Shape* shape_new = NULL;
/*  std::cout<<"PF::ShapesPar::build() shapes.get().size(): "<<get_ShapesGroup().size()<<std::endl;
  shapes_algo_par->clear_shapes();
  shapes_algo_par->set_shapes(get_ShapesGroup());
  std::cout<<"PF::ShapesPar::build() shapes.get().size(): "<<get_ShapesGroup().size()<<std::endl;
  std::cout<<"PF::ShapesPar::build() shapes_algo_par->get_shapes_count(): "<<shapes_algo_par->get_shapes_count()<<std::endl;
  shapes_algo_par->scale_shapes(get_scale_factor());
  shapes_algo_par->build_mask_shapes(get_shapes_falloff_curve());
*/
#if 0
  for( unsigned int shape_index = 0; shape_index < get_shapes_order().size(); shape_index++) {
    int shape_type = get_shapes_order()[shape_index].first;
//    int shape_index = get_shapes_order()[shape_index].second;
//    shape = get_shape(shape_index);
//    *shape_new = *shape;
//    int shape_new_index = shapes_algo_par->add_new_shape(get_shape(shape_index));
    int shape_new_index = shapes_algo_par->add_shape(get_shape(shape_index));
    std::cout<<"PF::ShapesPar::build() shape_new_index: "<<shape_new_index<<std::endl;
    Shape* shape_new = shapes_algo_par->get_shape(shape_new_index);
//    shape = get_shape(shape_index);
//    *shape_new = *get_shape(shape_index);
/*    Line line;
    Circle1 circ;
    Ellipse ellip;
    Rect1 rect;
   
    switch (shape_type)
    {
    case PF::Shape::line:
      {
//        Line* line = new Line();
        line = get_lines()[shape_index];
        shape_new = &line;
      }
      break;
    case PF::Shape::circle:
      {
//        Circle1* circ = new Circle1();
        circ = get_circles()[shape_index];
        shape_new = &circ;
  
      }
      break;
    case PF::Shape::ellipse:
      {
//        Ellipse* ellip = new Ellipse();
        ellip = get_ellipses()[shape_index];
        shape_new = &ellip;
  
      }
      break;
    case PF::Shape::rectangle:
      {
//        Rect1* rect = new Rect1();
        rect = get_rectangles1()[shape_index];
        shape_new = &rect;
      }
      break;
    }
    */
    shape_new->scale(scale_factor);
    shape_new->build_mask(get_shapes_falloff_curve());
    
    if (shape_new->get_mask() == NULL)
      std::cout<<"PF::ShapesPar::build() shape_new->get_mask() == NULL)"<<std::endl;
    if (shapes_algo_par->get_shape(shape_new_index)->get_mask() == NULL)
      std::cout<<"PF::ShapesPar::build() (shapes_algo_par->get_shape(shape_new_index)->get_mask() == NULL)"<<std::endl;
    if (get_shape(shape_index)->get_mask() != NULL)
      std::cout<<"PF::ShapesPar::build() (get_shape(shape_index)->get_mask() != NULL)"<<std::endl;

  }
#endif
  
  shapes_algo_par->set_image_hints( srcimg );
  shapes_algo_par->set_format( get_format() );
  in2.clear();
  in2.push_back( srcimg );
  VipsImage* out = shapes_algo_par->build( in2, 0, NULL, NULL, level );
//  PF_UNREF( srcimg, "ShapesPar::build(): srcimg unref" );


  set_image_hints( out );

  return out;
  
/*  VipsImage* out = PF::OpParBase::build( in, first, imap, omap, level );

  int tw = 64, th = 64, nt = out->Xsize*2/tw;

  VipsImage* cached;
  VipsAccess acc = VIPS_ACCESS_RANDOM;
  bool threaded = true, persistent = false;
  if( vips_tilecache(out, &cached,
      "tile_width", tw, "tile_height", th, "max_tiles", nt,
      "access", acc, "threaded", threaded, "persistent", persistent, NULL) ) {
    std::cout<<"ShapesPar::build(): vips_tilecache() failed."<<std::endl;
    return NULL;
  }
  PF_UNREF( out, "PF::ShapesPar::build(): out unref" );

  return cached;*/

  /*
//  VipsImage* out = PF::ShapesPar::build( in, first, imap, omap, level );
  std::cout<<"PF::ShapesMaskPar::build()"<<std::endl;

  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;

  VipsImage* srcimg = PF::ShapesPar::build(in, first,NULL, NULL, level);
  
  std::vector<VipsImage*> in2;

  if (get_shapes_algo() == NULL) {
    std::cout<<"PF::ShapesMaskPar::build() (get_shapes_algo() == NULL)"<<std::endl;
    return NULL;
  }
  std::cout<<"PF::ShapesMaskPar::build() 2"<<std::endl;
  if (get_shapes_algo()->get_par() == NULL) {
    std::cout<<"PF::ShapesMaskPar::build() (get_shapes_algo()->get_par() == NULL)"<<std::endl;
    return NULL;
  }
  std::cout<<"PF::ShapesMaskPar::build() 3"<<std::endl;
  ShapesAlgoPar* shapes_algo_ = dynamic_cast<ShapesAlgoPar*>( get_shapes_algo()->get_par() );
  if (shapes_algo_ == NULL) {
    std::cout<<"PF::ShapesMaskPar::build() (shapes_algo_ == NULL)"<<std::endl;
    return NULL;
  }
  std::cout<<"PF::ShapesMaskPar::build() 4"<<std::endl;
  shapes_algo_->set_image_hints( srcimg );
  shapes_algo_->set_format( get_format() );
  in2.clear();
    in2.push_back(srcimg);
  VipsImage* out = shapes_algo_->build(in, first, NULL, NULL, level);
  std::cout<<"PF::ShapesMaskPar::build() 5"<<std::endl;
  
  return out;
*/
}


PF::ProcessorBase* PF::new_shapes_mask()
{
  return( new PF::Processor<PF::ShapesMaskPar,PF::ShapesMask>() );
}


