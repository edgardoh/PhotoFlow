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



/* Turn on ADDR() range checks.
#define DEBUG 1
 */

//#ifdef HAVE_CONFIG_H
//#include <config.h>
//#endif /*HAVE_CONFIG_H*/
//#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <lcms2.h>

#include <iostream>


#include <vips/dispatch.h>

#include "../base/array2d.hh"
#include "../base/processor.hh"
#include "../base/layer.hh"
//#include "../operations/shapes.hh"
#include "../operations/retouch.hh"

#define PF_MAX_INPUT_IMAGES 10

static GObject* object_in;

using namespace PF;

//#undef NDEBUG

/**/
#define VIPS_TYPE_RETOUCH (vips_retouch_get_type())
#define VIPS_RETOUCH( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), \
		VIPS_TYPE_RETOUCH, VipsRetouch ))
#define VIPS_RETOUCH_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		VIPS_TYPE_RETOUCH, VipsRetouchClass))
#define VIPS_IS_RETOUCH( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), VIPS_TYPE_RETOUCH ))
#define VIPS_IS_RETOUCH_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), VIPS_TYPE_RETOUCH ))
#define VIPS_RETOUCH_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
		VIPS_TYPE_RETOUCH, VipsRetouchClass ))
/**/
typedef struct _VipsRetouch {
  VipsOperation parent_instance;
  
  /* The vector of input images.
   */
  VipsImage* in;

  /* The vector of input images.
   */
  VipsImage* out;

  /* Pointer to the object which does the actual image processing
   */
  PF::ProcessorBase* processor;

  int group_num;

//  int stroke_num;
  PF::Shape* stroke_num;
} VipsRetouch;

/*
typedef struct _VipsRetouchClass {
	VipsOperationClass parent_class;
} VipsRetouchClass;
*/
typedef VipsOperationClass VipsRetouchClass;

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

G_DEFINE_TYPE( VipsRetouch, vips_retouch, VIPS_TYPE_OPERATION );

#ifdef __cplusplus
}
#endif /*__cplusplus*/



template<class T>
static int
vips_retouch_gen_template( VipsRegion *oreg, void *seq, void *a, void *b, gboolean *stop )
{
  std::cout<<"vips_retouch_gen_template() begin"<<std::endl;
  
  VipsRegion *ir = (VipsRegion *) seq;
  VipsRetouch *retouch = (VipsRetouch *) b;
  

  /* Do the actual processing
   */

  /* Output area we are building.
   */
  VipsRect *r = &oreg->valid;
  VipsRect in_area, out_area, mask_area, in_area_offset;
/*  int i;
  VipsRect point_area;
  VipsRect point_clip;
  int point_clip_right, point_clip_bottom;
  int x, x0, y, y0, ch, row1, row2, col, mx, my1, my2;*/
  int line_size = r->width * oreg->im->Bands; //layer->in_all[0]->Bands; 

  VipsRect image_area = {0, 0, ir->im->Xsize, ir->im->Ysize};

  if( !retouch->processor ) return 1;
  if( !retouch->processor->get_par() ) return 1;

  PF::RetouchPar* par = dynamic_cast<PF::RetouchPar*>( retouch->processor->get_par() );
  if( !par ) return 1;

  T *p, *pin, *pout, *pbgd;
  
  if( !ir )
    return( -1 );
  if( vips_region_prepare( ir, r ) )
    return( -1 );

  for( int y = 0; y < r->height; y++ ) {
    p = (T*)VIPS_REGION_ADDR( ir, r->left, r->top + y );
    pout = (T*)VIPS_REGION_ADDR( oreg, r->left, r->top + y );
    memcpy( pout, p, sizeof(T)*line_size );
    /*
    for( x = 0; x < line_size; x += oreg->im->Bands ) {
      for( ch = 0; ch < oreg->im->Bands; ch++ ) {
        pout[x+ch] = p[x+ch];
      }
    }
    */
  }

  std::cout<<"vips_retouch_gen_template()"<<std::endl;

  /**/
#ifndef NDEBUG
  std::cout<<"vips_retouch_gen(): "<<std::endl;
  if( retouch->processor->get_par()->get_config_ui() )
    std::cout<<"  name: "<<retouch->processor->get_par()->get_config_ui()->get_layer()->get_name()<<std::endl;
  std::cout<<"  output region: top="<<oreg->valid.top
	   <<" left="<<oreg->valid.left
	   <<" width="<<oreg->valid.width
	   <<" height="<<oreg->valid.height<<std::endl;
#endif
  /**/

//  std::vector<PF::StrokesGroup>& groups = par->get_strokes();
//  std::list< std::pair<int, int> >::iterator pi;

#ifndef NDEBUG
  std::cout<<"vips_retouch_gen(): n. of groups: "<<groups.size()<<std::endl;
#endif
/*  if( retouch->group_num < 0 ) return 1;
  if( (int)groups.size() <= retouch->group_num )
      return 1;
  PF::StrokesGroup& group = groups[retouch->group_num];

  bool prepared = false;
  int delta_row = group.get_delta_row()/par->get_scale_factor();
  int delta_col = group.get_delta_col()/par->get_scale_factor();
#ifndef NDEBUG
  std::cout<<"vips_retouch_gen(): starting group #"<<retouch->group_num<<std::endl;
  std::cout<<"  n. of strokes: "<<group.get_strokes().size()<<std::endl;
  std::cout<<"  Drow: "<<delta_row<<"  Dcol: "<<delta_col<<std::endl;
#endif
*/
//  Shape* shape = NULL;
  Shape* shape = retouch->stroke_num;
  
/*  switch (retouch->group_num)
  {
  case PF::Shape::circle:
//    shape = &par->get_circles()[retouch->stroke_num];
    shape = par->get_circle_draw();
    break;
  case PF::Shape::rectangle:
//    shape = &par->get_rectangles1()[retouch->stroke_num];
    shape = par->get_rect_draw();
    break;
  }
  */
  if (shape == NULL)
  return( 0 );
  std::cout<<"vips_retouch_gen_template() 1"<<std::endl;

/*  int delta_row = shape->get_source_point().get_x()/par->get_scale_factor();
  int delta_col = shape->get_source_point().get_y()/par->get_scale_factor();

  // Input area = oreg area translated by (delta_row, delta_col)
  // and intersected with the image area
  in_area.width = r->width;
  in_area.height = r->height;
  in_area.top = r->top + delta_row;
  in_area.left = r->left + delta_col;
  std::cout<<"vips_retouch_gen_template() in_area.left: "<<in_area.left<<", in_area.top: "<<in_area.top<<std::endl;
  std::cout<<"vips_retouch_gen_template() in_area.width: "<<in_area.width<<", in_area.height: "<<in_area.height<<std::endl;
  vips_rect_intersectrect( &in_area, &image_area, &in_area );

  shape->get_rect(&mask_area);
  
  std::cout<<"vips_retouch_gen_template() par->get_scale_factor(): "<<par->get_scale_factor()<<std::endl;
  std::cout<<"vips_retouch_gen_template() in_area.left: "<<in_area.left<<", in_area.top: "<<in_area.top<<std::endl;
  std::cout<<"vips_retouch_gen_template() in_area.width: "<<in_area.width<<", in_area.height: "<<in_area.height<<std::endl;
  std::cout<<"vips_retouch_gen_template() mask_area.left: "<<mask_area.left<<", mask_area.top: "<<mask_area.top<<std::endl;
  std::cout<<"vips_retouch_gen_template() mask_area.width: "<<mask_area.width<<", mask_area.height: "<<mask_area.height<<std::endl;
  
  vips_rect_intersectrect( &in_area, &mask_area, &in_area );
  if (in_area.width < 1 || in_area.height < 1)
  return( 0 );
  std::cout<<"vips_retouch_gen_template() 2"<<std::endl;

  // Output area: input area translated by (delta_row, delta_col)
  out_area.left = in_area.left + delta_col;
  out_area.top = in_area.top + delta_row;
  out_area.width = in_area.width;
  out_area.height = in_area.height;
*/
  // out area is shape dest area intersect with output area
  shape->get_mask_rect(&mask_area);
  vips_rect_intersectrect( r, &mask_area, &out_area );
  if (out_area.width < 1 || out_area.height < 1)
  return( 0 );

  int delta_row = shape->get_source_point().get_y()/*/par->get_scale_factor()*/;
  int delta_col = shape->get_source_point().get_x()/*/par->get_scale_factor()*/;
  
  // in area is outarea offset by shape source and intersect with image area
  in_area_offset.width = out_area.width;
  in_area_offset.height = out_area.height;
  in_area_offset.top = out_area.top + delta_row;
  in_area_offset.left = out_area.left + delta_col;
  vips_rect_intersectrect( &in_area_offset, &image_area, &in_area );
  if (in_area.width < 1 || in_area.height < 1)
  return( 0 );

  float *mask = shape->get_mask();
  if (mask == NULL)
    return( 0 );
  
  if( vips_region_prepare( ir, &in_area ) )
    return( -1 );
  std::cout<<"vips_retouch_gen_template() 3"<<std::endl;

  const int width = std::min(in_area.width, out_area.width);
  const int height = std::min(in_area.height, out_area.height);
  const int left = std::min(in_area.left, out_area.left);
  const int top = std::min(in_area.top, out_area.top);
  
    std::cout<<"vips_retouch_gen_template() mask_area.left: "<<mask_area.left<<", mask_area.top: "<<mask_area.top<<std::endl;
//    std::cout<<"vips_retouch_gen_template() mask_area.width: "<<mask_area.width<<", mask_area.height: "<<mask_area.height<<std::endl;
  //  std::cout<<"vips_retouch_gen_template() in_area.left: "<<in_area.left<<", in_area.top: "<<in_area.top<<std::endl;
  //  std::cout<<"vips_retouch_gen_template() in_area.width: "<<in_area.width<<", in_area.height: "<<in_area.height<<std::endl;
//  std::cout<<"vips_retouch_gen_template() out_area.left: "<<out_area.left<<", out_area.top: "<<out_area.top<<std::endl;
//  std::cout<<"vips_retouch_gen_template() out_area.width: "<<out_area.width<<", out_area.height: "<<out_area.height<<std::endl;

  const int ch = oreg->im->Bands;
  const int offset_left = out_area.left + (in_area.left - in_area_offset.left);
  const int offset_top = out_area.top + (in_area.top - in_area_offset.top);
  
  for( int y = 0; y < height; y++ ) {
    pin = (T*)VIPS_REGION_ADDR( ir, in_area.left, in_area.top + y );
    pout = (T*)VIPS_REGION_ADDR( oreg, offset_left, offset_top + y );
    
    for( int x = 0; x < width; x++, pin+=ch, pout+=ch ) {
//      std::cout<<"vips_retouch_gen_template() mask[(y+offset_top)*mask_area.width+x+offset_left]: "<<mask[(y+offset_top)*mask_area.width+x+offset_left]<<std::endl;
//      if (mask[(y+offset_top)*mask_area.width+x+offset_left] > 0.) {
      int x_mask = offset_left+x;
      int y_mask = offset_top + y;
      const float f = mask[(y_mask-mask_area.top)*mask_area.width+(x_mask-mask_area.left)];
        if (f > 0.) {
        for (int c = 0; c < ch; c++)
          pout[c] = (pin[c] * f) + (pout[c] * (1.f - f));
      }
    }
  }

/*  PF::Array2D<float> opacity_max;
  opacity_max.Init( out_area.width, out_area.height, out_area.top, out_area.left );
  for( int ic = 0; ic < out_area.width; ic++ ) {
    for( int ir = 0; ir < out_area.height; ir++ ) {
      opacity_max.GetLocal(ir, ic) = 0;
    }
  }
*/
  // We keep a copy of the original pixels that needs to be blended with the cloned ones
/*  PF::Array2D<T> bgd;
  bgd.Init( out_area.width*oreg->im->Bands, out_area.height,
      out_area.top, out_area.left*oreg->im->Bands );
  for( y = 0; y < out_area.height; y++ ) {
    pout = (T*)VIPS_REGION_ADDR( oreg, out_area.left, out_area.top + y );
    for( x = 0; x < out_area.width*oreg->im->Bands; x++ ) {
      bgd.Get( out_area.top+y, out_area.left*oreg->im->Bands+x ) = pout[x];
    }
  }
*/
/*  PF::Stroke<PF::Stamp>& stroke = group.get_strokes()[retouch->stroke_num];

  std::list< std::pair<int, int> >& points = stroke.get_points();
#ifndef NDEBUG
  std::cout<<"vips_retouch_gen(): starting stroke"<<std::endl;
  std::cout<<"  n. of points: "<<points.size()<<std::endl;
#endif

  PF::Stamp& pen = stroke.get_pen();
  unsigned int pen_size = pen.get_size()/par->get_scale_factor();
  unsigned int pen_size2 = pen_size*pen_size;

  //std::cout<<"  pen size="<<pen_size<<"  opacity="<<pen.get_opacity()
  //         <<"  smoothness="<<pen.get_smoothness()<<std::endl;

  PF::StampMask* resized_mask = NULL;
  PF::StampMask* mask = &(pen.get_mask());
  if( pen_size != pen.get_size() ) {
    resized_mask = new PF::StampMask;
    resized_mask->init( pen_size*2+1, pen.get_opacity(), pen.get_smoothness() );
    mask = resized_mask;
  }

  point_area.width = point_area.height = pen_size*2 + 1;
  for( pi = points.begin(); pi != points.end(); ++pi ) {
    x0 = pi->first/par->get_scale_factor();
    y0 = pi->second/par->get_scale_factor();
#ifndef NDEBUG
    std::cout<<"  point @ x0="<<x0<<"  y0="<<y0<<std::endl;
#endif

    point_area.left = x0 - pen_size;
    point_area.top = y0 - pen_size;
    // The area covered by the current point is intersected with the output area.
    // If the point is completely outside of the output area, it is skipped.
    vips_rect_intersectrect( &out_area, &point_area, &point_clip );
    if( point_clip.width<1 || point_clip.height<1 ) continue;

    point_clip_right = point_clip.left + point_clip.width - 1;
    point_clip_bottom = point_clip.top + point_clip.height - 1;

    // We have at least one point to process, so we need to process the input pixels
    if( !prepared ) {
#ifndef NDEBUG
      std::cout<<"  preparing region ir:  top="<<in_area.top
          <<" left="<<in_area.left
          <<" width="<<in_area.width
          <<" height="<<in_area.height<<std::endl;
#endif
      if( vips_region_prepare( ir, &in_area ) )
        return( -1 );
      prepared = true;
    }

    for( y = 0; y <= (int)pen_size; y++ ) {
      row1 = y0 - y;
      row2 = y0 + y;
      my1 = pen_size - y;
      my2 = pen_size + y;
      //int L = pen.get_size() - y;
      int D = (int)sqrt( pen_size2 - y*y );
      int startcol = x0 - D;
      if( startcol < point_clip.left )
        startcol = point_clip.left;
      int endcol = x0 + D;
      if( endcol >= point_clip_right )
        endcol = point_clip_right;
      int colspan = (endcol + 1 - startcol)*oreg->im->Bands;

      //endcol = x0;

      //std::cout<<"row1="<<row1<<"  row2="<<row2<<"  startcol="<<startcol<<"  endcol="<<endcol<<"  colspan="<<colspan<<std::endl;
      //std::cout<<"point_clip.left="<<point_clip.left<<"  point_clip.top="<<point_clip.top
      //         <<"  point_clip.width="<<point_clip.width<<"  point_clip.height="<<point_clip.height<<std::endl;

      
      if( (row1 >= point_clip.top) && (row1 <= point_clip_bottom) ) {
        p =    (T*)VIPS_REGION_ADDR( ir, startcol-delta_col, row1-delta_row );
        pbgd = &( bgd.Get(row1, startcol*oreg->im->Bands) );
        pout = (T*)VIPS_REGION_ADDR( oreg, startcol, row1 );
        mx = startcol - x0 + pen_size;
        for( x = 0, col = startcol; x < colspan; x += oreg->im->Bands, col++, mx++ ) {
          float mval = mask->get( mx, my1 );
          //std::cout<<"    opacity_max.Get("<<row1<<", "<<col<<")="<<opacity_max.Get(row1, col)<<"    mval="<<mval<<std::endl;
          if( mval < opacity_max.Get(row1, col) )
            continue;
          for( ch = 0; ch < oreg->im->Bands; ch++ ) {
            float val = mval*p[x+ch] + (1.0f-mval)*pbgd[x+ch];
            pout[x+ch] = static_cast<T>(val);
          }
          opacity_max.Get(row1, col) = mval;
          //std::cout<<"x="<<x<<"+"<<point_clip.left<<"="<<x+point_clip.left<<std::endl;
        }
      }
      if( (row2 != row1) && (row2 >= point_clip.top) && (row2 <= point_clip_bottom) ) {
        p =    (T*)VIPS_REGION_ADDR( ir, startcol-delta_col, row2-delta_row );
        pbgd = &( bgd.Get(row2, startcol*oreg->im->Bands) );
        pout = (T*)VIPS_REGION_ADDR( oreg, startcol, row2 );
        mx = startcol - x0 + pen_size;
        for( x = 0, col = startcol; x < colspan; x += oreg->im->Bands, col++, mx++ ) {
          float mval = mask->get( mx, my2 );
          //std::cout<<"    opacity_max.Get("<<row2<<", "<<col<<")="<<opacity_max.Get(row2, col)<<"    mval="<<mval<<std::endl;
          if( mval < opacity_max.Get(row2, col) )
            continue;
          for( ch = 0; ch < oreg->im->Bands; ch++ ) {
            float val = mval*p[x+ch] + (1.0f-mval)*pbgd[x+ch];
            pout[x+ch] = static_cast<T>(val);
            //pout[x+ch] = p[x+ch];
          }
          opacity_max.Get(row2, col) = mval;
        }
      }
    }
  }

  if( resized_mask ) delete resized_mask;
*/
  return( 0 );
}


static int
vips_retouch_gen( VipsRegion *oreg, void *seq, void *a, void *b, gboolean *stop )
{
  VipsRegion *ir = (VipsRegion *) seq;
  //g_print("vips_retouch_gen() called, ir=%p\n", ir);
  if( !ir ) return 1;

  int result = 0;

  switch( ir->im->BandFmt ) {
  case VIPS_FORMAT_UCHAR:
    result = vips_retouch_gen_template<unsigned char>( oreg, seq, a, b, stop );
    break;
  case VIPS_FORMAT_USHORT:
    result = vips_retouch_gen_template<unsigned short int>( oreg, seq, a, b, stop );
    break;
  case VIPS_FORMAT_FLOAT:
    result = vips_retouch_gen_template<float>( oreg, seq, a, b, stop );
    break;
  case VIPS_FORMAT_DOUBLE:
    result = vips_retouch_gen_template<double>( oreg, seq, a, b, stop );
    break;
  default:
    break;
  }

  return result;
}

  
static int
vips_retouch_build( VipsObject *object )
{
  VipsObjectClass *klass = VIPS_OBJECT_GET_CLASS( object );
  VipsOperation *operation = VIPS_OPERATION( object );
  VipsRetouch *retouch = (VipsRetouch *) object;
  int i;

  if( VIPS_OBJECT_CLASS( vips_retouch_parent_class )->build( object ) )
    return( -1 );

  if( !retouch->in )
    return( -1 );

  //retouch->rand = 0;/*random();*/

  //if( retouch->processor->identity() == 1 ) 
  //  return( vips_image_write( retouch->in, conversion->out ) );

  if( vips_image_pio_input( retouch->in ) ||
	vips_check_coding_known( klass->nickname, retouch->in ) )
      return( -1 );

  /* Get ready to write to @out. @out must be set via g_object_set() so
   * that vips can see the assignment. It'll complain that @out hasn't
   * been set otherwise.
   */
  g_object_set( retouch, "out", vips_image_new(), NULL ); 

  /* Set demand hints. 
  */
  if( vips_image_pipelinev( retouch->out,
      VIPS_DEMAND_STYLE_ANY,
      retouch->in, NULL ) )
    return( -1 );

  PF::OpParBase* par = retouch->processor->get_par();

#ifndef NDEBUG
  std::cout<<"vips_retouch_build(): output format = "<<par->get_format()<<std::endl;
#endif
  vips_image_init_fields( retouch->out,
			  retouch->in->Xsize,  retouch->in->Ysize,
			  retouch->in->Bands, retouch->in->BandFmt,
			  retouch->in->Coding,
			  retouch->in->Type,
			  1.0, 1.0);
  if( vips_image_generate( retouch->out,
      vips_start_one, vips_retouch_gen, vips_stop_one,
      retouch->in, retouch ) )
    return( -1 );

  return( 0 );
}


static void
vips_retouch_class_init( VipsRetouchClass *klass )
{
  GObjectClass *gobject_class = G_OBJECT_CLASS( klass );
  VipsObjectClass *vobject_class = VIPS_OBJECT_CLASS( klass );
  VipsOperationClass *operation_class = VIPS_OPERATION_CLASS( klass );

  gobject_class->set_property = vips_object_set_property;
  gobject_class->get_property = vips_object_get_property;

  vobject_class->nickname = "retouch";
  vobject_class->description = _( "Photoflow retouch" );
  vobject_class->build = vips_retouch_build;

  operation_class->flags = VIPS_OPERATION_SEQUENTIAL_UNBUFFERED/*+VIPS_OPERATION_NOCACHE*/;

  int argid = 0;

  VIPS_ARG_IMAGE( klass, "in", argid,
      _( "Input" ),
      _( "Input image" ),
      VIPS_ARGUMENT_REQUIRED_INPUT,
      G_STRUCT_OFFSET( VipsRetouch, in ) );
  argid += 1;

  VIPS_ARG_IMAGE( klass, "out", argid, 
		  _( "Output" ), 
		  _( "Output image" ),
		  VIPS_ARGUMENT_REQUIRED_OUTPUT, 
		  G_STRUCT_OFFSET( VipsRetouch, out ) );
  argid += 1;

  VIPS_ARG_POINTER( klass, "processor", argid, 
		    _( "Processor" ),
		    _( "Image processing object" ),
		    VIPS_ARGUMENT_REQUIRED_INPUT,
		    G_STRUCT_OFFSET( VipsRetouch, processor ) );
  argid += 1;

  VIPS_ARG_INT( klass, "group_num", argid,
      _( "StrokeNum" ),
      _( "Stroke number" ),
      VIPS_ARGUMENT_REQUIRED_INPUT,
    G_STRUCT_OFFSET( VipsRetouch, group_num ),
    -1, 1000, -1);
  argid += 1;

/*  VIPS_ARG_INT( klass, "stroke_num", argid,
      _( "StrokeNum" ),
      _( "Stroke number" ),
      VIPS_ARGUMENT_REQUIRED_INPUT,
    G_STRUCT_OFFSET( VipsRetouch, stroke_num ),
    -1, 1000, -1);*/
  VIPS_ARG_POINTER( klass, "stroke_num", argid, 
        _( "StrokeNum" ),
        _( "Stroke number" ),
        VIPS_ARGUMENT_REQUIRED_INPUT,
        G_STRUCT_OFFSET( VipsRetouch, stroke_num ) );
  argid += 1;
}

static void
vips_retouch_init( VipsRetouch *retouch )
{
  //retouch->in = NULL;
  retouch->group_num = -1;
//  retouch->stroke_num = -1;
  retouch->stroke_num = NULL;
}

/**
 * vips_retouch:
 * @in: input image
 * @out: output image
 * @...: %NULL-terminated list of optional named arguments
 *
 * Returns: 0 on success, -1 on error.
 */
int
vips_retouch( VipsImage* in, VipsImage **out, PF::ProcessorBase* proc, int group_num, /*int*/PF::Shape* stroke_num, ...)
{
  va_list ap;
  int result;

  va_start( ap, stroke_num );
  result = vips_call_split( "retouch", ap, in, out, proc, group_num, stroke_num );
  va_end( ap );

  return( result );
}

/*
    VipsImage* in = image; 
    VipsImage* out; 
    VipsImage* imap; 
    VipsArea *area;
    VipsImage **array; 
    const int N = 0;

    if (vips_call("retouch", NULL, &imap, 0, gradient, NULL, NULL, NULL))
      verror ();
    //g_object_unref( imap );

    area = vips_area_new_array_object( 1 );
    array = (VipsImage **) area->data;
    array[0] = in;
    g_object_ref( array[0] );
    if (vips_call("retouch", area, &out, 0, bc, imap, NULL, NULL))
      verror ();
    vips_area_unref( area );
    g_object_unref( out );
    in = out;

    for(int i = 0; i < N; i++) {
      area = vips_area_new_array_object( 1 );
      array = (VipsImage **) area->data;
      array[0] = in;
      g_object_ref( array[0] );
      if (vips_call("retouch", area, &out, 0, invert, NULL, NULL, NULL))
        verror ();
      vips_area_unref( area );
*/
