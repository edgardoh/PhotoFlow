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


#include "gmic_generic_untiled.hh"



PF::GmicGenericUntiledPar::GmicGenericUntiledPar(): 
PF::OpParBase(),
gmic_instance( NULL ),
gmic_filter_prop("gmic_filter_prop", this)
{ 
  set_type( "gmic_generic_untiled" );
  set_default_name( _("GMIC Generic Untiled") );
}

PF::GmicGenericUntiledPar::~GmicGenericUntiledPar()
{
  std::cout<<"GmicGenericUntiledPar::~GmicGenericUntiledPar()"<<std::endl;
  if( gmic_instance ) delete gmic_instance;
}


VipsImage* PF::GmicGenericUntiledPar::build(std::vector<VipsImage*>& in, int first, 
                 VipsImage* imap, VipsImage* omap, 
                 unsigned int& level)
{
  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];

  if( !srcimg ) return NULL;

  std::string command = build_command();

  VipsImage* out = NULL;

  run_gmic( srcimg, &out, command );

  PF::vips_copy_metadata( srcimg, out );

  return out;
}

std::string PF::GmicGenericUntiledPar::build_command()
{
  return gmic_filter_prop.get().get_gmic_command(get_verbosity_mode());
}


void PF::GmicGenericUntiledPar::set_gmic_filter(GmicFilter* gmf)
{
  if ( gmf != NULL ) {
    GmicFilter& gmf_p = gmic_filter_prop.get();
    
    if ( gmf_p.get_properties_count() == 0 ) {
      gmf_p = *gmf;
      gmf_p.create_properties();
      
      set_default_name( gmf_p.get_filter_name() );
      
    }
  }
}

bool PF::GmicGenericUntiledPar::run_gmic( VipsImage* in, VipsImage** out, std::string command )
{
  if( !new_gmic() ) 
    return false;

  VipsImage *t0;
  VipsImage *t1;

  // Convert input to a float membuffer.

  t1 = vips_image_new_memory();
  if( vips_cast_float( in, &t0, NULL ) )
    return( false ); 

  if( vips_image_write( t0, t1 ) )
    return( false ); 

  float *in_img = (float*)t1->data;

  gmic_list<float> image_list;
  image_list.assign(1);
  gmic_list<char> image_names;

  int width = in->Xsize;
  int height = in->Ysize;
  int ch = in->Bands;

  // Initialize each image of 'image_list'
  {
    gmic_image<float>& img = image_list[0];
    img.assign(width, height, 1, ch);

    // Fill image buffer
    float *ptr = img._data;
    for (unsigned int c = 0; c<img._spectrum; ++c) // ch channels.
    {
      for (unsigned int y = 0; y<img._height; ++y) // height rows.
      {
        const float *g_in = in_img + (y*width*ch);
        for (unsigned int x = 0; x<img._width; ++x) //  columns.
        {
          *(ptr++) = g_in[(x*ch)+c]*255.f;
        }
      }
    }
  }

  // run GMIC
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic() width: "<<width<<", height: "<<height<<std::endl;
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic() command: "<<command<<std::endl;
  try 
  {
    gmic_instance->run( command.c_str(), image_list, image_names );
  } 
  catch (gmic_exception &e) // In case something went wrong.
  { 
    std::cout<<"PF::GmicUntiledOperationPar::run_gmic() error: "<<e.what()<<std::endl;
    return false;
  }

  
  // copy GMIC image back
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic() copy image from GMIC: #images: "<<image_list._width<<std::endl;

  float *buf = (float*)malloc((height*width*ch) * sizeof(float));

  for (int image_num = 0; image_num < 1/*image_list._width*/; image_num++) {
    // for now deal only with same size images
    std::cout<<"PF::GmicUntiledOperationPar::run_gmic() image_list[image_num]._width: "<<image_list[image_num]._width<<", image_list[image_num]._height: "<<image_list[image_num]._height<<std::endl;
    if ( image_list[image_num]._height == height && image_list[image_num]._width == width ) {
      // create a new image for each returned one
      *out = vips_image_new_memory();
      if( vips_image_pipelinev( *out, VIPS_DEMAND_STYLE_ANY, in, NULL ) )
                    return false;
      (*out)->BandFmt = VIPS_FORMAT_FLOAT;
      (*out)->Type = VIPS_INTERPRETATION_RGB;

      if ( *out ) {
        set_metadata( in, *out );

        gmic_image<float>& img = image_list[image_num];
        float *ptr = image_list[image_num]._data;
        bool copied = false;
        
        std::cout<<"PF::GmicUntiledOperationPar::run_gmic() img._spectrum: "<<img._spectrum<<", ch: "<<ch<<std::endl;

        if ( img._spectrum == (unsigned int)ch ) {
          copied = true;
          
          for (unsigned int c = 0; c<img._spectrum; ++c) // ch channels.
          {
            for (unsigned int y = 0; y<img._height; ++y) // height rows.
            {
              float *g_out = buf + (y*width*ch);
              for (unsigned int x = 0; x<img._width; ++x) //  columns.
              {
                g_out[(x*ch)+c] = (*(ptr++))/255.f;
              }
            }
          }
        }
        else if ( img._spectrum > (unsigned int)ch ) {
          copied = true;
          
          for (unsigned int c = 0; c<img._spectrum; ++c) // ch channels.
          {
            for (unsigned int y = 0; y<img._height; ++y) // height rows.
            {
              float *g_out = buf + (y*width*ch);
              for (unsigned int x = 0; x<img._width; ++x) //  columns.
              {
                if ( c < ch ) g_out[(x*ch)+c] = (*(ptr++))/255.f;
              }
            }
          }
        }
        else if ( img._spectrum == 1 && ch == 3 ) {
          copied = true;
          
          for (unsigned int y = 0; y<img._height; ++y) // height rows.
          {
            float *g_out = buf + (y*width*ch);
            for (unsigned int x = 0; x<img._width; ++x) //  columns.
            {
              g_out[(x*ch)+0] = g_out[(x*ch)+1] = g_out[(x*ch)+2] = (*(ptr++))/255.f;
            }
          }
        }

        // copy each row to Vips
        if ( copied ) {
          for (int y = 0; y<height; ++y) // height rows.
          {
            if( vips_image_write_line( *out, y, (VipsPel *) (buf + (y*width*ch)) ) )
            {
              std::cout<<"vips_image_write_line() error line "<<y<<std::endl;
              return( false );
            }
          }
  
          PF::vips_copy_metadata( in, *out );
        }
      }
    }
  }

  free(buf);

  if( gmic_instance ) {
    delete gmic_instance;
    gmic_instance = NULL;
  }

  return true;
}

gmic* PF::GmicGenericUntiledPar::new_gmic()
{
  //if( custom_gmic_commands ) delete [] custom_gmic_commands;
  if( gmic_instance ) delete gmic_instance;

  std::cout<<"Loading G'MIC custom commands..."<<std::endl;
  char fname[500]; fname[0] = 0;
#if defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
  snprintf( fname, 499, "%s\\gmic_def.gmic", PF::PhotoFlow::Instance().get_data_dir().c_str() );
  std::cout<<"G'MIC custom commands file: "<<fname<<std::endl;
  struct stat buffer;   
  int stat_result = stat( fname, &buffer );
  if( stat_result != 0 ) {
    fname[0] = 0;
  }
#elif defined(__APPLE__) && defined (__MACH__)
  //snprintf( fname, 499, "%s/../share/photoflow/gmic_def.gmic", PF::PhotoFlow::Instance().get_base_dir().c_str() );
  snprintf( fname, 499, "%s/gmic_def.gmic", PF::PhotoFlow::Instance().get_data_dir().c_str() );
  std::cout<<"G'MIC custom commands file: "<<fname<<std::endl;
  struct stat buffer;
  int stat_result = stat( fname, &buffer );
  if( stat_result != 0 ) {
    fname[0] = 0;
  }
#else
  if( getenv("HOME") ) {
    //snprintf( fname, 499, "%s/.photoflow/gmic_update.gmic", getenv("HOME") );
    snprintf( fname, 499, "%s/share/photoflow/gmic_def.gmic", INSTALL_PREFIX );
    std::cout<<"G'MIC custom commands file: "<<fname<<std::endl;
    struct stat buffer;   
    int stat_result = stat( fname, &buffer );
    if( stat_result != 0 ) {
      //snprintf( fname, 499, "%s/gmic_def.gmic", PF::PhotoFlow::Instance().get_base_dir().c_str() );
      //stat_result = stat( fname, &buffer );
      //if( stat_result != 0 ) {
      fname[0] = 0;
      //}
    }
  }
#endif
  if( strlen( fname ) > 0 ) {
    std::ifstream t;
    int length;
    t.open(fname);      // open input file
    t.seekg(0, std::ios::end);    // go to the end
    length = t.tellg();           // report location (this is the length)
    t.seekg(0, std::ios::beg);    // go back to the beginning
    custom_gmic_commands = new char[length];    // allocate memory for a buffer of appropriate dimension
    t.read(custom_gmic_commands, length);       // read the whole file into the buffer
    t.close();                    // close file handle
    std::cout<<"G'MIC custom commands loaded"<<std::endl;
  }

  /* Make a gmic for this thread.
   */
  gmic_instance = new gmic( 0, custom_gmic_commands, false, 0, 0 ); 
  return gmic_instance;
}



PF::ProcessorBase* PF::new_gmic_generic_untiled()
{
  return( new PF::Processor<PF::GmicGenericUntiledPar,PF::GmicGenericUntiledProc>() );
}
