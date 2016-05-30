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


#include "../base/pf_mkstemp.hh"

#include "gmic.hh"
#include "gmic_untiled_op.hh"



PF::GmicUntiledOperationPar::GmicUntiledOperationPar(): 
  UntiledOperationPar(),
  gmic_instance( NULL )
{	
}


PF::GmicUntiledOperationPar::~GmicUntiledOperationPar()
{
	std::cout<<"GmicUntiledOperationPar::~GmicUntiledOperationPar()"<<std::endl;
	if( gmic_instance ) delete gmic_instance;
}


gmic* PF::GmicUntiledOperationPar::new_gmic()
{
  //if( custom_gmic_commands ) delete [] custom_gmic_commands;
  if( gmic_instance ) delete gmic_instance;

  std::cout<<"Loading G'MIC custom commands..."<<std::endl;
  char fname[500]; fname[0] = 0;
#if defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
  snprintf( fname, 499, "%s\\gmic_def.gmic", PF::PhotoFlow::Instance().get_base_dir().c_str() );
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


bool PF::GmicUntiledOperationPar::run_gmic( VipsImage* in, std::string command )
{
  if( command.empty() ) 
    return false;
  std::cout<<"g'mic command: "<<command<<std::endl;
  if( !new_gmic() ) 
    return false;
  
  try 
  {
    gmic_instance->run( command.c_str() );
  } 
  catch (gmic_exception &e) // In case something went wrong.
  { 
    std::cout<<"PF::GmicUntiledOperationPar::run_gmic error: "<<e.what()<<std::endl;
    return false;
  }
  
  
  if( gmic_instance ) {
    delete gmic_instance;
    gmic_instance = NULL;
  }
  
  raster_images_attach( in );
  
  return true;
}

bool PF::GmicUntiledOperationPar::run_gmic2( VipsImage* in, std::vector<VipsImage*> vecout, std::string command )
{
}

bool PF::GmicUntiledOperationPar::run_gmic2( VipsImage* in, VipsImage** out, std::string command )
{
  if( command.empty() ) 
    return false;
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): g'mic command: "<<command<<std::endl;
  if( !new_gmic() ) 
    return false;
  
  VipsImage *t0;
  VipsImage *t1;

  /* Convert input to a float membuffer.
   */
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): vips_image_new_memory()"<<std::endl;
  t1 = vips_image_new_memory();
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): vips_cast_float()"<<std::endl;
  if( vips_cast_float( in, &t0, NULL ) )
    return( false ); 
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): vips_image_write()"<<std::endl;
  if( vips_image_write( t0, t1 ) )
    return( false ); 

  float *in_img = (float*)t1->data;
  
  gmic_list<float> image_list;
  image_list.assign(1);
  gmic_list<char> image_names;
  
  int width = in->Xsize;
  int height = in->Ysize;
  int ch = in->Bands;
  

  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy image to gmic"<<std::endl;
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

  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): call gmic"<<std::endl;

 
  try 
  {
    gmic_instance->run( command.c_str(), image_list, image_names );
  } 
  catch (gmic_exception &e) // In case something went wrong.
  { 
    std::cout<<"PF::GmicUntiledOperationPar::run_gmic2 error: "<<e.what()<<std::endl;
    return false;
  }

  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): image_list._width="<<image_list._width<<std::endl;
  
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output"<<std::endl;
  
  float *buf = NULL;
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output 1"<<std::endl;
  
//  VipsImage* out = NULL;
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output 2"<<std::endl;
  buf = (float*)malloc((height*width*ch) * sizeof(float));
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output 3"<<std::endl;
  
  for (int image_num = 0; image_num < 1/*image_list._width*/; image_num++) {
    std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output 4"<<std::endl;
  *out = vips_image_new_memory();
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output 5"<<std::endl;
  set_metadata( in, *out );
  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output 6"<<std::endl;
 
  {
    gmic_image<float>& img = image_list[image_num];
    std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): copy gmic output 7"<<std::endl;
    float *ptr = image_list[image_num]._data;
    
    std::cout<<"PF::GmicUntiledOperationPar::run_gmic2() img._height="<<img._height<<", img._width="<<img._width<<", img._spectrum="<<img._spectrum<<std::endl;

/*  g_assert(img._spectrum <= (unsigned int)ch);
    g_assert(img._height <= (unsigned int)height);
    g_assert(img._width <= (unsigned int)width);
*/  
    
//    for (unsigned int c = 0; c<img._spectrum; ++c) // ch channels.
//    {
      for (unsigned int y = 0; y<img._height; ++y) // height rows.
      {
        float *g_out = buf + (y*width*ch);
        for (unsigned int x = 0; x<img._width; ++x) //  columns.
        {
          g_out[(x*ch)+0] = g_out[(x*ch)+1] = g_out[(x*ch)+2] = (*(ptr++))/255.f;
        }
      }
//    }
  }

  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): update out image"<<std::endl;
  
  for (unsigned int y = 0; y<height; ++y) // height rows.
  {
      if( vips_image_write_line( *out, y, (VipsPel *) (buf + (y*width*ch)) ) )
      {
        std::cout<<"vips_image_write_line() error line "<<y<<std::endl;
        return( false );
      }
  }
  PF::vips_copy_metadata( in, *out );
//  vecout.push_back(out);
  }
  
  free(buf);

  std::cout<<"PF::GmicUntiledOperationPar::run_gmic2(): return gmic images"<<std::endl;
  
  if( gmic_instance ) {
    delete gmic_instance;
    gmic_instance = NULL;
  }
  
  return true;
}


