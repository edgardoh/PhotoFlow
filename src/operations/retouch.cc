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

#include "retouch.hh"

int
vips_retouch( VipsImage* in, VipsImage **out, PF::ProcessorBase* proc, /*int group_num,*/ PF::Shape* shape_obj, ...);


PF::RetouchPar::RetouchPar(): 
    ShapesPar()
{
  set_type( "retouch" );

  set_default_name( _("retouch") );
}


PF::RetouchPar::~RetouchPar()
{
}


VipsImage* PF::RetouchPar::build(std::vector<VipsImage*>& in, int first, 
				VipsImage* imap, VipsImage* omap, unsigned int& level)
{
  if( (in.size() < 1) || (in[0] == NULL) )
    return NULL;

  VipsImage* outnew = in[0];
  PF_REF( outnew, "RetouchPar::build(): initial outnew ref" );

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
  
  for( unsigned int i = 0; i < shapes_algo_par->get_shapes_count(); i++) {
    Shape* shape = shapes_algo_par->get_shape(i);
    
    VipsImage* tempimg = outnew;
    if( vips_retouch( tempimg, &outnew, get_processor(), /*shape_type,*/ /*shape_index*/shape, NULL ) )
      return NULL;
    PF_UNREF( tempimg, "RetouchPar::build(): tempimg unref" );
      
  }

  return outnew;
}


PF::ProcessorBase* PF::new_retouch()
{
  return( new PF::Processor<PF::RetouchPar,PF::RetouchProc>() );
}
