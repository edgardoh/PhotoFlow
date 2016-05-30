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

#ifndef PF_SUBTRACT_IMAGE_HH
#define PF_SUBTRACT_IMAGE_HH


namespace PF 
{

class SubtractImagePar: public OpParBase
{
  Property<float> blendFactor;
  
  ProcessorBase* subtract_image_algo;

public:
  SubtractImagePar();

  bool has_intensity() { return false; }
  bool needs_caching() { return false; }

  float get_blendFactor() { return (float)blendFactor.get(); }
  float set_blendFactor(float a) { blendFactor.set(a); }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};

class SubtractImageAlgoPar: public OpParBase
{
  float blendFactor;

public:
  SubtractImageAlgoPar(): OpParBase()
{
    blendFactor = .0;
}
  
  float get_blendFactor() { return (float)blendFactor; }
  float set_blendFactor(float a) { blendFactor=a; }
};



template < OP_TEMPLATE_DEF >
class SubtractImageProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
  }
};

template < OP_TEMPLATE_DEF >
class SubtractImageAlgoProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
  }
};



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class SubtractImageAlgoProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
 
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    if( ireg[0] == NULL ) {
      std::cout<<"SubtractImageAlgoProc::render ireg[0] == NULL"<<std::endl;
      return;
    }

    SubtractImageAlgoPar* opar = dynamic_cast<SubtractImageAlgoPar*>(par);
    if( !opar ) {
      std::cout<<"SubtractImageAlgoProc::render opar == NULL"<<std::endl;
      return;
    }

    Rect *ir = &ireg[0]->valid;
    const int wd_width = ir->width;
    const int wd_height = ir->height;
    const int ch = oreg->im->Bands;
    
    const float blend_factor = opar->get_blendFactor();
    
    for( int y = 0; y < wd_height; y++ ) {
      float *pin = (float*)VIPS_REGION_ADDR( ireg[0], ir->left, ir->top + y );
      float *pout = (float*)VIPS_REGION_ADDR( oreg, ir->left, ir->top + y );

      for( int x = 0; x < wd_width*ch; x++ ) pout[x] -= pin[x] - blend_factor;
    }
    
  }
  

};


ProcessorBase* new_subtract_image();

}

#endif 

