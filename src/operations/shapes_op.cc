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

#include "shapes_op.hh"
#include "../base/processor.hh"


PF::ShapesPar::ShapesPar():
                OpParBase(),
                falloff_curve( "falloff_curve", this ), 
                shapes("shapes_grp", this)/*,
                hide_outline( "hide_outline", this, false ),
                lock_source( "lock_source", this, true )*/
{
  shapes_algo = new PF::Processor<PF::ShapesAlgoPar,PF::ShapesAlgoProc>();
  
  set_type( "shapes" );
  set_default_name( _("shapes") );
}

PF::ShapesAlgoPar::ShapesAlgoPar():
                OpParBase()
{
  set_type( "shapes_algo" );
  set_default_name( _("shapes_algo") );
}

VipsImage* PF::ShapesPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;

  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  if (shapes_algo == NULL) {
    std::cout<<"PF::ShapesPar::build() (shapes_algo == NULL)"<<std::endl;
    return NULL;
  }
  if (shapes_algo->get_par() == NULL) {
    std::cout<<"PF::ShapesPar::build() (shapes_algo->get_par() == NULL)"<<std::endl;
    return NULL;
  }
  ShapesAlgoPar* shapes_algo_par = dynamic_cast<ShapesAlgoPar*>( shapes_algo->get_par() );
  if (shapes_algo_par == NULL) {
    std::cout<<"PF::ShapesPar::build() (shapes_algo_par == NULL)"<<std::endl;
    return NULL;
  }

  shapes_algo_par->build_masks(get_shapes_group(), get_shapes_falloff_curve(), level); 
  
  shapes_algo_par->set_image_hints( srcimg );
  shapes_algo_par->set_format( get_format() );
  in2.clear();
  in2.push_back( srcimg );
  VipsImage* out = shapes_algo_par->build( in2, 0, NULL, NULL, level );
  PF_UNREF( srcimg, "ShapesPar::build(): srcimg unref" );

  set_image_hints( out );

  return out;
}

void PF::ShapesAlgoPar::build_masks(ShapesGroup& sg, SplineCurve& scurve, unsigned int level)
{
  int scale_factor = 1;
  for(unsigned int l = 0; l < level; l++ ) {
    scale_factor *= 2;
  }
  
  shapes.clear();
  shapes = sg;
  shapes.scale(scale_factor);
  shapes.build_mask(scurve);

}

VipsImage* PF::ShapesAlgoPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  VipsImage* out = PF::OpParBase::build(in, first, NULL, NULL, level);
  
  return out;
}

PF::ProcessorBase* PF::new_shapes()
{
  return( new PF::Processor<PF::ShapesPar,PF::ShapesProc>() );
}

