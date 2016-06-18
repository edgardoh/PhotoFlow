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
}


PF::ProcessorBase* PF::new_shapes_mask()
{
  return( new PF::Processor<PF::ShapesMaskPar,PF::ShapesMask>() );
}


