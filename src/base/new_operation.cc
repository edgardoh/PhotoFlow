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


#include "new_operation.hh"
#include "layer.hh"
#include "../operations/operations.hh"
#include "../operations/vips_operation.hh"

PF::ProcessorBase* PF::new_operation( std::string op_type, PF::Layer* current_layer )
{
  PF::ProcessorBase* processor = NULL;

  if( op_type == "imageread" ) { 

    //processor = new PF::Processor<PF::ImageReaderPar,PF::ImageReader>();
    processor = new_image_reader();

  } else if( op_type == "raw_loader" ) {

    //processor = new PF::Processor<PF::BlenderPar,PF::BlenderProc>();
    processor = new_raw_loader();

  } else if( op_type == "raw_developer" ) {

    //processor = new PF::Processor<PF::BlenderPar,PF::BlenderProc>();
    processor = new_raw_developer();

  } else if( op_type == "raw_output" ) {

    //processor = new PF::Processor<PF::BlenderPar,PF::BlenderProc>();
    processor = new_raw_output();

  } else if( op_type == "buffer" ) {

    //processor = new PF::Processor<PF::BlenderPar,PF::BlenderProc>();
    processor = new_buffer();

  } else if( op_type == "blender" ) {

    //processor = new PF::Processor<PF::BlenderPar,PF::BlenderProc>();
    processor = new_blender();

  } else if( op_type == "clone" ) {

    //processor = new PF::Processor<PF::ClonePar,PF::CloneProc>();
    processor = new_clone();

  } else if( op_type == "crop" ) {

    //processor = new PF::Processor<PF::ClonePar,PF::CloneProc>();
    processor = new_crop();

  } else if( op_type == "scale" ) {

    //processor = new PF::Processor<PF::ClonePar,PF::CloneProc>();
    processor = new_scale();

  } else if( op_type == "perspective" ) {

    //processor = new PF::Processor<PF::ClonePar,PF::CloneProc>();
    processor = new_perspective();

  } else if( op_type == "invert" ) {

    //processor = new PF::Processor<PF::InvertPar,PF::Invert>();
    processor = new_invert();

  } else if( op_type == "threshold" ) {

    //processor = new PF::Processor<PF::InvertPar,PF::Invert>();
    processor = new_threshold();

  } else if( op_type == "desaturate" ) {

    //processor = new PF::Processor<PF::InvertPar,PF::Invert>();
    processor = new_desaturate();

  } else if( op_type == "uniform" ) {

    //processor = new PF::Processor<PF::InvertPar,PF::Invert>();
    processor = new_uniform();

  } else if( op_type == "gradient" ) {

    //processor = new PF::Processor<PF::GradientPar,PF::Gradient>();
    processor = new_gradient();

  } else if( op_type == "path_mask" ) {

    //processor = new PF::Processor<PF::GradientPar,PF::Gradient>();
    processor = new_path_mask();

  } else if( op_type == "shapes" ) {

    //processor = new PF::Processor<PF::GradientPar,PF::Gradient>();
    processor = new_shapes();

  } else if( op_type == "brightness_contrast" ) {

    //processor = new PF::Processor<PF::BrightnessContrastPar,PF::BrightnessContrast>();
    processor = new_brightness_contrast();

  } else if( op_type == "hue_saturation" ) {

    processor = new_hue_saturation();

  } else if( op_type == "hsl_mask" ) {

    processor = new_hsl_mask();

  } else if( op_type == "curves" ) {
      
    //processor = new PF::Processor<PF::CurvesPar,PF::Curves>();
    processor = new_curves();

  } else if( op_type == "channel_mixer" ) {
      
    //processor = new PF::Processor<PF::CurvesPar,PF::Curves>();
    processor = new_channel_mixer();

  } else if( op_type == "gaussblur" ) {
      
    //processor = new PF::Processor<PF::CurvesPar,PF::Curves>();
    processor = new_gaussblur();

   } else if( op_type == "denoise" ) {
      
    //processor = new PF::Processor<PF::CurvesPar,PF::Curves>();
    processor = new_denoise();

 } else if( op_type == "sharpen" ) {
      
    //processor = new PF::Processor<PF::CurvesPar,PF::Curves>();
    processor = new_sharpen();

  } else if( op_type == "convert2lab" ) {

    //processor = new PF::Processor<PF::Convert2LabPar,PF::Convert2LabProc>();
    processor = new_convert2lab();

  } else if( op_type == "convert_colorspace" ) {

    //processor = new PF::Processor<PF::Convert2LabPar,PF::Convert2LabProc>();
    processor = new_convert_colorspace();

  } else if( op_type == "draw" ) {

    //processor = new PF::Processor<PF::Convert2LabPar,PF::Convert2LabProc>();
    processor = new_draw();

  } else if( op_type == "clone_stamp" ) {

    //processor = new PF::Processor<PF::Convert2LabPar,PF::Convert2LabProc>();
    processor = new_clone_stamp();

  } else if( op_type == "lensfun" ) {
    processor = new_lensfun();

  } else if( op_type == "retouch" ) {

    processor = new_retouch();

  } else if( op_type == "shadows_highlights" ) {
    processor = new_shadows_highlights();

  } else if( op_type == "defringe" ) {
    processor = new_defringe();

  } else if( op_type == "split_details" ) {
    processor = new_split_details();

  } else if( op_type == "volume" ) {
    processor = new_volume();
  }

  if( !processor ) {
    // Try with G'MIC
    std::cout<<"PF::new_gmic_operation( "<<op_type<<" );"<<std::endl;
    processor = PF::new_gmic_operation( op_type );
  }
  /*
  } else { // it must be a VIPS operation...

    int pos = op_type.find( "vips-" );
    if( pos != 0 ) return NULL;
    std::string vips_op_type;
    vips_op_type.append(op_type.begin()+5,op_type.end());

    PF::Processor<PF::VipsOperationPar,PF::VipsOperationProc>* vips_op = 
      new PF::Processor<PF::VipsOperationPar,PF::VipsOperationProc>();
    //ProcessorBase* vips_op = new_vips_operation();
    vips_op->get_par()->set_op( vips_op_type.c_str() );
    processor = vips_op;
  }
  */

  if( processor && current_layer ) {
    current_layer->set_processor( processor );
    //if( processor->get_par() && processor->get_par()->has_opacity() )
    current_layer->set_blender( new_blender() );
    if( processor->get_par() )
      current_layer->set_cached( processor->get_par()->needs_caching() );
  }

  return processor;
}
