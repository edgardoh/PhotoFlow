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

#ifndef PF_WAVDEC_H
#define PF_WAVDEC_H


namespace PF 
{

class WaveletDecomposePar: public OpParBase
{
  Property<float> numScales, currScale, blendFactor;
  
  ProcessorBase* convert2lab;
  ProcessorBase* convert2input;
  ProcessorBase* wavelet_decompose_algo;

  cmsHPROFILE in_profile;

public:
  WaveletDecomposePar();

  bool has_intensity() { return false; }
  bool needs_caching() { return false; }

  float get_numScales() { return (float)numScales.get(); }
  float set_numScales(float a) { numScales.set(a); }
  float get_currScale() { return (float)currScale.get(); }
  float set_currScale(float a) { currScale.set(a); }
  float get_blendFactor() { return (float)blendFactor.get(); }
  float set_blendFactor(float a) { blendFactor.set(a); }

  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};

class WaveletDecomposeAlgoPar: public OpParBase
{
  float numScales, currScale, blendFactor;

public:
  WaveletDecomposeAlgoPar(): OpParBase()
{
    numScales, currScale = 0;
    blendFactor = .128f;
}
  
  int get_padding() { return pow(2, get_numScales()); }

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
  float get_currScale() { return (float)currScale; }
  float set_currScale(float a) { currScale=a; }
  float get_blendFactor() { return (float)blendFactor; }
  float set_blendFactor(float a) { blendFactor=a; }
};



template < OP_TEMPLATE_DEF >
class WaveletDecomposeProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
//    std::cout<<"WaveletDecomposeProc::render "<<std::endl;
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
//    std::cout<<"WaveletDecomposeProc::render 2"<<std::endl;
  }
};



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class WaveletDecomposeAlgoProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
{
private:
#define DWT_MAX_CHANNELS 4
//  typedef struct dwt_params_t
//  {
    float *wd_layers; // buffer for internal use
    float *wd_image;
    int wd_channels_img[DWT_MAX_CHANNELS]; // index == dwt channel; wd_channels_img[index] == wd_image channel
    int wd_ch_im;
    int wd_width;
    int wd_height;
    int wd_channels;
    int wd_scales;
    int wd_return_layer;
    int wd_clear_im; // clear the image before return
//  } dwt_params_t;
    float wd_blend_factor; // .128f
    int wd_max_scales;
#define INDEX_WT_IMAGE(ch, index) (((index)*ch)+c)

public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
    std::cout<<"WaveletDecomposeAlgoProc::render "<<std::endl;
    
    if( ireg[0] == NULL ) {
      std::cout<<"WaveletDecomposeAlgoProc::render ireg[0] == NULL"<<std::endl;
      return;
    }

    WaveletDecomposeAlgoPar* opar = dynamic_cast<WaveletDecomposeAlgoPar*>(par);
    if( !opar ) {
      std::cout<<"WaveletDecomposeAlgoProc::render opar == NULL"<<std::endl;
      return;
    }

    
    float *pwavdec = NULL;
    
    Rect *r = &oreg->valid;
    Rect *ir = &ireg[0]->valid;
    
    std::cout<<"WaveletDecomposeProc::render "<<std::endl;
    std::cout<<"ir->width="<<ir->width<<",  ir->height="<<ir->height<<", r->width="<<r->width<<",  r->height="<<r->height<<std::endl;
    
    wd_width = ir->width;
    wd_height = ir->height;
    const int ch = oreg->im->Bands;
    
//    dwt_params_t dwt_p;
    
//    memset(&dwt_p, 0, sizeof(dwt_p));
    
    pwavdec = (float*)malloc(wd_width * wd_height * ch * sizeof(float));
    memset(pwavdec, 0, wd_width * wd_height * ch * sizeof(float));

    for( int y = 0; y < wd_height; y++ ) {
      float *pin = (float*)VIPS_REGION_ADDR( ireg[0], ir->left, ir->top + y );
      float *pwd = pwavdec + y * wd_width * ch;

      for( int x = 0; x < wd_width*ch; x++ ) pwd[x] = pin[x];
    }
    
    // set no of channels to decompose and convertion table
    wd_channels = 0;
    if (ch >= 1) wd_channels_img[wd_channels++] = 0;
    if (ch >= 2) wd_channels_img[wd_channels++] = 1;
    if (ch >= 3) wd_channels_img[wd_channels++] = 2;
    if (wd_channels == 0) wd_channels = 1;
    
    wd_image = pwavdec;
    wd_ch_im = ch;
    wd_scales = (int)opar->get_numScales();
    wd_return_layer = (int)opar->get_currScale();
    wd_clear_im = (wd_return_layer > 0);
    wd_blend_factor = opar->get_blendFactor();
    
    wd_max_scales = opar->get_maxScales(wd_width, wd_height);
    if (wd_scales > wd_max_scales)
    {
      std::cout<<"WaveletDecomposeAlgoProc::render: max scale is "<<wd_max_scales<<" for this image preview size"<<std::endl;
    }

    dwt_decompose();
      
    for( int y = 0; y < r->height; y++ ) {
      float *pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top + y );
      float *pwd = pwavdec + (y+abs(ir->height-r->height)/2) * wd_width * ch + (abs(ir->width-r->width)/2) * ch;

      for( int x = 0; x < r->width*ch; x++ ) pout[x] = pwd[x];
    }

    if (pwavdec) free(pwavdec);
    
  }
  
private:
  void dwt_decompose()
  {
  #ifdef _TIME_FFT_
    struct timeval tm1,tm2;
    gettimeofday(&tm1,NULL);
  #endif
    
    /* this function prepares for decomposing, which is done in the function dwt_wavelet_decompose() */
//    int max_scale = get_maxScales(wd_width, wd_height);
    int i, c, x;
    float *img = NULL;
    float *line;
    
    if (wd_scales > wd_max_scales) wd_scales = wd_max_scales;
    if (wd_return_layer > wd_scales+1) wd_return_layer = wd_scales+1;
    
    wd_layers = (float *)malloc(wd_width * wd_height * wd_channels * sizeof(float));
    if (wd_layers == NULL)
    {
      std::cout<<"not enough memory for wavelet decomposition"<<std::endl;
      goto cleanup;
    }
    memset(wd_layers, 0, wd_width * wd_height * wd_channels * sizeof(float));

    img = (float *)malloc(wd_width * wd_height * wd_channels * sizeof(float));
    if (img == NULL)
    {
      std::cout<<"not enough memory for wavelet decomposition"<<std::endl;
      goto cleanup;
    }
    memset(img, 0, wd_width * wd_height * wd_channels * sizeof(float));

    /* read the image into memory */
    if (wd_channels == wd_ch_im)
    {
      memcpy(img, wd_image, wd_width * wd_height * wd_channels * sizeof(float));
    }
    else
    {
      for (i = 0; i < wd_height; i++) 
      {
        line = wd_image + (i * wd_width * wd_ch_im);
        
        for (x = 0; x < wd_width; x++) 
        {
          for (c = 0; c < wd_channels; c++) 
          {
            img[INDEX_WT_IMAGE(wd_channels, i * wd_width + x)] = line[x * wd_ch_im + wd_channels_img[c]];
          }
        }
      }
    }

    dwt_wavelet_decompose(img);

    
  #ifdef _TIME_FFT_
    gettimeofday(&tm2,NULL);
    float perf = (tm2.tv_sec-tm1.tv_sec)*1000.0f + (tm2.tv_usec-tm1.tv_usec)/1000.0f;
    printf("time spent dwt_decompose: %.4f\n", perf/100.0f);
  #endif
    
  cleanup:
      if (img) free(img);
      if (wd_layers) 
      {
        free(wd_layers);
        wd_layers = NULL;
      }

  }

  void dwt_wavelet_decompose(float *img)
  {
    float *temp = NULL;
    unsigned int lpass, hpass;
    float *buffer[2] = {0, 0};
    int bcontinue = 1;
    
    const float lpass_add = sqrtf(.25f);
    const float lpass_mult = (1.f / 16.f);
    const float lpass_sub = wd_blend_factor; //.128f;
    
    const int size = wd_width * wd_height * wd_channels;

    /* image buffers */
    buffer[0] = img;
    /* temporary storage */
    buffer[1] = (float *)malloc(size * sizeof(float));
    if (buffer[1] == NULL)
    {
      std::cout<<"not enough memory for wavelet decomposition"<<std::endl;
      goto cleanup;
    }
    memset(buffer[1], 0, size * sizeof(float));

    temp = (float *)malloc(MAX(wd_width, wd_height) * wd_channels * sizeof(float));
    if (temp == NULL)
    {
      std::cout<<"not enough memory for wavelet decomposition"<<std::endl;
      goto cleanup;
    }
    memset(temp, 0, MAX(wd_width, wd_height) * wd_channels * sizeof(float));

    /* iterate over wavelet scales */
    lpass = 1;
    hpass = 0;
    for (unsigned int lev = 0; lev < wd_scales && bcontinue; lev++) 
    {
      lpass = (1 - (lev & 1));

  #ifdef _FFT_MULTFR_x
  #ifdef _OPENMP
  #pragma omp parallel for default(none) shared(buffer, temp, lev, lpass, hpass) schedule(static)
  #endif
  #endif
      for (int row = 0; row < wd_height; row++) 
      {
        dwt_hat_transform(temp, buffer[hpass] + (row * wd_width * wd_channels), 1, wd_width, 1 << lev);
        memcpy(&(buffer[lpass][row * wd_width * wd_channels]), temp, wd_width * wd_channels * sizeof(float));
      }
      
  #ifdef _FFT_MULTFR_x
  #ifdef _OPENMP
  #pragma omp parallel for default(none) shared(buffer, temp, lev, lpass, hpass) schedule(static)
  #endif
  #endif
      for (int col = 0; col < wd_width; col++) 
      {
        dwt_hat_transform(temp, buffer[lpass] + col*wd_channels, wd_width, wd_height, 1 << lev);
        for (int row = 0; row < wd_height; row++) 
        {
          for (int c = 0; c < wd_channels; c++) 
          buffer[lpass][INDEX_WT_IMAGE(wd_channels, row * wd_width + col)] = temp[INDEX_WT_IMAGE(wd_channels, row)];
        }
      }
        
  #ifdef _FFT_MULTFR_x
  #ifdef _OPENMP
  #pragma omp parallel for default(none) shared(buffer, lev, lpass, hpass) schedule(static)
  #endif
  #endif
      for (int i = 0; i < size; i++) 
      {
        /* rounding errors introduced here (division by 16) */
        buffer[lpass][i] = ( buffer[lpass][i] + lpass_add ) * lpass_mult;
        buffer[hpass][i] -= buffer[lpass][i] - lpass_sub;
      }

      if (wd_return_layer == 0)
      {
        dwt_add_layer(buffer[hpass], lev + 1);
      }
      else if (wd_return_layer == lev + 1)
      {
        dwt_get_image_layer(buffer[hpass]);
    
        bcontinue = 0;
      }
      
      hpass = lpass;
    }

    //  Wavelet residual
    if (wd_return_layer == wd_scales+1)
    {
      dwt_get_image_layer(buffer[lpass]);
    }
    else if (wd_return_layer == 0)
    {
      dwt_add_layer(buffer[hpass], wd_scales+1);
      
      dwt_get_image_layer(wd_layers);
    }

  cleanup:
    if (temp) free(temp);
    if (buffer[1]) free(buffer[1]);

  }

  void dwt_add_layer(float *const img, const int n_scale)
  {
    const float lpass_sub = wd_blend_factor; //.128f;
    
    if (n_scale == wd_scales+1)
    {
  #ifdef _FFT_MULTFR_x
  #ifdef _OPENMP
  #pragma omp parallel for default(none) schedule(static)
  #endif
  #endif
      for (int i=0; i<wd_width*wd_height*wd_channels; i++)
        wd_layers[i] += img[i];
    }
    else
    {
  #ifdef _FFT_MULTFR_x
  #ifdef _OPENMP
  #pragma omp parallel for default(none) schedule(static)
  #endif
  #endif
      for (int i=0; i<wd_width*wd_height*wd_channels; i++)
        wd_layers[i] += img[i] - lpass_sub;
    }

  }

  void dwt_get_image_layer(float *const layer)
  {
    if (wd_clear_im)
    {
      memset(wd_image, 0, wd_width * wd_height * wd_ch_im * sizeof(float));
    }
    
  #ifdef _FFT_MULTFR_x
  #ifdef _OPENMP
  #pragma omp parallel for default(none) schedule(static)
  #endif
  #endif
    for (int i = 0; i < wd_height; i++) 
    {
      for (int x = 0; x < wd_width; x++) 
      {
        for (int c = 0; c < wd_channels; c++) 
        {
          wd_image[wd_ch_im*((i*wd_width) + x) + wd_channels_img[c]] = layer[INDEX_WT_IMAGE(wd_channels, i * wd_width + x)];
        }
      }
    }
  }

  void dwt_hat_transform(float *temp, float *const base, const int st, const int size, const int sc)
  {
    int i, c;
    const float hat_mult = 2.f;
    
    for (i = 0; i < sc; i++)
    {
      for (c = 0; c < wd_channels; c++, temp++)
      {
        *temp = hat_mult * base[INDEX_WT_IMAGE(wd_channels, st * i)] + base[INDEX_WT_IMAGE(wd_channels, st * (sc - i))] + base[INDEX_WT_IMAGE(wd_channels, st * (i + sc))];
      }
    }
    for (; i + sc < size; i++)
    {
      for (c = 0; c < wd_channels; c++, temp++)
      {
        *temp = hat_mult * base[INDEX_WT_IMAGE(wd_channels, st * i)] + base[INDEX_WT_IMAGE(wd_channels, st * (i - sc))] + base[INDEX_WT_IMAGE(wd_channels, st * (i + sc))];
      }
    }
    for (; i < size; i++)
    {
      for (c = 0; c < wd_channels; c++, temp++)
      {
        *temp = hat_mult * base[INDEX_WT_IMAGE(wd_channels, st * i)] + base[INDEX_WT_IMAGE(wd_channels, st * (i - sc))]
                                                               + base[INDEX_WT_IMAGE(wd_channels, st * (2 * size - 2 - (i + sc)))];
      }
    }
    
  }


};


ProcessorBase* new_wavelet_decompose();

}

#endif 

