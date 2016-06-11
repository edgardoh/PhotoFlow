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
vips_retouch( VipsImage* in, VipsImage **out, PF::ProcessorBase* proc, int group_num, /*int*/PF::Shape* stroke_num, ...);



PF::RetouchPar::RetouchPar(): 
    ShapesPar/*OpParBase*/()/*,
  stamp_size( "stamp_size", this, 5 ),
  stamp_opacity( "stamp_opacity", this, 1 ),
  stamp_smoothness( "stamp_smoothness", this, 1 ),
  strokes( "strokes", this )*/
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
  scale_factor = 1;
  for(unsigned int l = 0; l < level; l++ ) {
    scale_factor *= 2;
  }

  if( (in.size() < 1) || (in[0] == NULL) )
    return NULL;

  std::cout<<"PF::RetouchPar::build()"<<std::endl;
  
  VipsImage* outnew = in[0];
  PF_REF( outnew, "RetouchPar::build(): initial outnew ref" );

  Shape* shape = NULL;
  
  
  for( unsigned int i = 0; i < get_shapes_order().size(); i++) {
    int shape_type = get_shapes_order()[i].first;
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
//      get_circles()[shape_index].build_mask();
      Circle1* circ = new Circle1();
      *circ = get_circles()[shape_index];
      shape = circ;
/*      circle_draw = get_circles()[shape_index];
      circle_draw.scale(scale_factor);
      circle_draw.build_mask();*/
    }
      break;
    case PF::Shape::rectangle:
    {
//      get_rectangles1()[shape_index].build_mask();
      Rect1* rect = new Rect1();
      *rect = get_rectangles1()[shape_index];
      shape = rect;
/*      rect_draw = get_rectangles1()[shape_index];
      rect_draw.scale(scale_factor);
      rect_draw.build_mask();*/
    }
      break;
    }
    shape->scale(scale_factor);
    shape->build_mask();
          
    std::cout<<"PF::RetouchPar::build() call vips"<<std::endl;
      VipsImage* tempimg = outnew;
      if( vips_retouch( tempimg, &outnew, get_processor(), shape_type, /*shape_index*/shape, NULL ) )
        return NULL;
      PF_UNREF( tempimg, "RetouchPar::build(): tempimg unref" );
      
//      shape->free_mask();
//      delete shape;
  }

/*  for( unsigned int i = 0; i < strokes.get().size(); i++) {
    PF::StrokesGroup& group = strokes.get()[i];
    for( unsigned int j = 0; j < group.get_strokes().size(); j++ ) {
      VipsImage* tempimg = outnew;
      if( vips_retouch( tempimg, &outnew, get_processor(), i, j,NULL ) )
        return NULL;
      PF_UNREF( tempimg, "RetouchPar::build(): tempimg unref" );
    }
  }
*/
  /*
#ifndef NDEBUG    
  std::cout<<"OpParBase::build(): type="<<type<<"  format="<<get_format()<<std::endl
	   <<"input images:"<<std::endl;
  for(int i = 0; i < n; i++) {
    std::cout<<"  "<<(void*)invec[i]<<"   ref_count="<<G_OBJECT( invec[i] )->ref_count<<std::endl;
  }
  std::cout<<"imap: "<<(void*)imap<<std::endl<<"omap: "<<(void*)omap<<std::endl;
  std::cout<<"out: "<<(void*)outnew<<std::endl<<std::endl;
#endif
  */

#ifndef NDEBUG    
  std::cout<<"OpParBase::build(): outnew refcount ("<<(void*)outnew<<") = "<<G_OBJECT(outnew)->ref_count<<std::endl;
#endif
  return outnew;
}



/*
void PF::RetouchPar::new_group( int drow, int dcol )
{
  strokes.get().push_back( PF::StrokesGroup() );
  PF::StrokesGroup& group = strokes.get().back();

  group.set_delta_row( drow );
  group.set_delta_col( dcol );
  std::cout<<"RetouchPar::new_group(): Drow="<<drow<<"  Dcol="<<dcol<<std::endl;
}



void PF::RetouchPar::start_stroke( unsigned int pen_size, float opacity, float smoothness )
{
  if( strokes.get().empty() ) return;

  PF::StrokesGroup& group = strokes.get().back();

  group.get_strokes().push_back( PF::Stroke<PF::Stamp>() );
  PF::Stroke<PF::Stamp>& stroke = group.get_strokes().back();

  PF::Stamp& pen = stroke.get_pen();
  pen.set_size( pen_size );
  pen.set_opacity( opacity );
  pen.set_smoothness( smoothness );

  //std::cout<<"RetouchPar::start_stroke(): pen size="<<pen.get_size()<<"  opacity="<<pen.get_opacity()
  //         <<"  smoothness="<<pen.get_smoothness()<<std::endl;
}



void PF::RetouchPar::end_stroke()
{
}



void PF::RetouchPar::draw_point( unsigned int x, unsigned int y, VipsRect& update )
{
  update.width = update.height = update.left = update.top = 0;

  if( strokes.get().empty() ) return;

  PF::StrokesGroup& group = strokes.get().back();
  if( group.get_strokes().empty() ) return;

  //group.get_strokes().push_back( PF::Stroke<PF::Stamp>() );
  PF::Stroke<PF::Stamp>& stroke = group.get_strokes().back();

  if( !stroke.get_points().empty() ) {
    if( (stroke.get_points().back().first == (int)x ) &&
				(stroke.get_points().back().second == (int)y ) )
      return;
  }

  stroke.get_points().push_back( std::make_pair(x, y) );

  strokes.modified();

  PF::Stamp& pen = stroke.get_pen();

  update.left = x - pen.get_size();
  update.top = y - pen.get_size();
  update.width = update.height = pen.get_size()*2 + 1;
}
*/


PF::ProcessorBase* PF::new_retouch()
{
  return( new PF::Processor<PF::RetouchPar,PF::RetouchProc>() );
}
