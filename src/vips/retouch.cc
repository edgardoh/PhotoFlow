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
#include "../operations/retouch.hh"

#define PF_MAX_INPUT_IMAGES 10

static GObject* object_in;

using namespace PF;

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
//  std::cout<<"vips_retouch_gen_template() begin"<<std::endl;
  
  VipsRegion *ir = (VipsRegion *) seq;
  VipsRetouch *retouch = (VipsRetouch *) b;
  

  /* Do the actual processing
   */

  /* Output area we are building.
   */
  VipsRect *r = &oreg->valid;
  VipsRect in_area, out_area, mask_area, in_area_offset;

  int line_size = r->width * oreg->im->Bands;

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
  }

//  std::cout<<"vips_retouch_gen_template()"<<std::endl;

  Shape* shape = retouch->stroke_num;
  
  if (shape == NULL)
  return( 0 );
//  std::cout<<"vips_retouch_gen_template() 1"<<std::endl;


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
//  std::cout<<"vips_retouch_gen_template() 3"<<std::endl;

  const int width = std::min(in_area.width, out_area.width);
  const int height = std::min(in_area.height, out_area.height);
  const int left = std::min(in_area.left, out_area.left);
  const int top = std::min(in_area.top, out_area.top);
  
  const int ch = oreg->im->Bands;
  const int offset_left = out_area.left + (in_area.left - in_area_offset.left);
  const int offset_top = out_area.top + (in_area.top - in_area_offset.top);
  
  for( int y = 0; y < height; y++ ) {
    pin = (T*)VIPS_REGION_ADDR( ir, in_area.left, in_area.top + y );
    pout = (T*)VIPS_REGION_ADDR( oreg, offset_left, offset_top + y );
    
    for( int x = 0; x < width; x++, pin+=ch, pout+=ch ) {
      int x_mask = offset_left+x;
      int y_mask = offset_top + y;
      const float f = mask[(y_mask-mask_area.top)*mask_area.width+(x_mask-mask_area.left)];
        if (f > 0.) {
        for (int c = 0; c < ch; c++)
          pout[c] = (pin[c] * f) + (pout[c] * (1.f - f));
      }
    }
  }

  return( 0 );
}


static int
vips_retouch_gen( VipsRegion *oreg, void *seq, void *a, void *b, gboolean *stop )
{
  VipsRegion *ir = (VipsRegion *) seq;

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

