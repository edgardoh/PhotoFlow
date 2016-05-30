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

#ifndef PF_WAVELET_DECOMPOSE_H
#define PF_WAVELET_DECOMPOSE_H


namespace PF 
{

class WaveletDecomposePar: public OpParBase
{
  Property<float> numScales, blendFactor;
  
  ProcessorBase* wav_dec_algo;
  ProcessorBase* subtract_image_algo;

public:
  WaveletDecomposePar();

  bool has_intensity() { return false; }
  bool needs_caching() { return true; }

  float get_numScales() { return (float)numScales.get(); }
  float set_numScales(float a) { numScales.set(a); }
  float get_blendFactor() { return (float)blendFactor.get(); }
  float set_blendFactor(float a) { blendFactor.set(a); }

  int get_padding(int numScales) { return pow(2, numScales); }

  std::vector<VipsImage*> build_many(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};

class WaveletDecomposeAlgoPar: public OpParBase
{
  float numScales, blendFactor;
  int padding;
  
public:
  WaveletDecomposeAlgoPar(): OpParBase()
  {
      numScales = 0;
      blendFactor = .5f;
  }
  
  int get_maxScales(const int width, const int height)
  {
    int maxscale = 0;
    
    /* smallest edge must be higher than or equal to 2^scales */
    unsigned int size = MIN(width, height);
    while (size >>= 1) maxscale++;

    return maxscale;
  } 

  /* Function to derive the output area from the input area
   */
  virtual void transform(const Rect* rin, Rect* rout)
  {
    int pad = get_padding();
    rout->left = rin->left+pad;
    rout->top = rin->top+pad;
    rout->width = rin->width-pad*2;
    rout->height = rin->height-pad*2;
  }

  /* Function to derive the area to be read from input images,
     based on the requested output area
  */
  virtual void transform_inv(const Rect* rout, Rect* rin)
  {
    int pad = get_padding();
    rin->left = rout->left-pad;
    rin->top = rout->top-pad;
    rin->width = rout->width+pad*2;
    rin->height = rout->height+pad*2;
  }

  float get_numScales() { return (float)numScales; }
  float set_numScales(float a) { numScales=a; }
  float get_blendFactor() { return (float)blendFactor; }
  float set_blendFactor(float a) { blendFactor=a; }
  int get_padding() { return padding; }
  int set_padding(int a) { padding=a; }
};



template < OP_TEMPLATE_DEF >
class WaveletDecomposeProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
  }
};

template < OP_TEMPLATE_DEF >
class WaveletDecomposeAlgoProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
  }
};



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class WaveletDecomposeAlgoProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
    
public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
  }
  
};


ProcessorBase* new_wavelet_decompose();

}

#endif 

