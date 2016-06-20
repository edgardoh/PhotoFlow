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
/*  set_scale_factor(1);
  for(unsigned int l = 0; l < level; l++ ) {
    set_scale_factor( get_scale_factor() * 2 );
  }
*/
  if( (in.size() < 1) || (in[0] == NULL) )
    return NULL;

  VipsImage* outnew = in[0];
  PF_REF( outnew, "RetouchPar::build(): initial outnew ref" );

  Shape* shape = NULL;
  
  for( unsigned int i = 0; i < get_shapes_count(); i++) {
/*    int shape_type = get_shapes_order()[i].first;
    int shape_index = get_shapes_order()[i].second;
    
    switch (shape_type)
    {
    case PF::Shape::line:
      {
        Line* line = new Line();
        *line = get_lines()[shape_index];
        shape = line;
      }
      break;
    case PF::Shape::circle:
      {
        Circle1* circ = new Circle1();
        *circ = get_circles()[shape_index];
        shape = circ;
  
      }
      break;
    case PF::Shape::ellipse:
      {
        Ellipse* ellip = new Ellipse();
        *ellip = get_ellipses()[shape_index];
        shape = ellip;
  
      }
      break;
    case PF::Shape::rectangle:
      {
        Rect1* rect = new Rect1();
        *rect = get_rectangles1()[shape_index];
        shape = rect;
      }
      break;
    }
    
    shape->scale(get_scale_factor());
    shape->build_mask(get_shapes_falloff_curve());
*/          
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
