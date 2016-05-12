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

/*
#define UNBOUND_L 1
#define UNBOUND_A 2
#define UNBOUND_B 4
#define UNBOUND_SHADOWS_L UNBOUND_L
#define UNBOUND_SHADOWS_A UNBOUND_A
#define UNBOUND_SHADOWS_B UNBOUND_B
#define UNBOUND_HIGHLIGHTS_L (UNBOUND_L << 3)  
#define UNBOUND_HIGHLIGHTS_A (UNBOUND_A << 3) 
#define UNBOUND_HIGHLIGHTS_B (UNBOUND_B << 3)  
#define UNBOUND_GAUSSIAN 64
#define UNBOUND_BILATERAL 128  
#define UNBOUND_DEFAULT                                                                                      \
    (UNBOUND_SHADOWS_L | UNBOUND_SHADOWS_A | UNBOUND_SHADOWS_B | UNBOUND_HIGHLIGHTS_L | UNBOUND_HIGHLIGHTS_A   \
        | UNBOUND_HIGHLIGHTS_B | UNBOUND_GAUSSIAN)

#define CLAMPF(a, mn, mx) ((a) < (mn) ? (mn) : ((a) > (mx) ? (mx) : (a)))
#define CLAMP_RANGE(x, y, z) (CLAMP(x, y, z))
#define MMCLAMPPS(a, mn, mx) (_mm_min_ps((mx), _mm_max_ps((a), (mn))))
*/

namespace PF 
{
/*
enum shahi_method_t
{
  SHAHI_GAUSSIAN,
  SHAHI_BILATERAL,
};


inline float sign(float x)
{
  return (x < 0 ? -1.0f : 1.0f);
}
*/


class WaveletDecomposePar: public OpParBase
{
//  PropertyBase method;
//  Property<float> shadows, highlights, wp_adjustment, radius, compress, sh_color_adjustment, hi_color_adjustment;
  Property<float> numScales, currScale;
  
//  ProcessorBase* gauss;
  ProcessorBase* convert2lab;
  ProcessorBase* convert2input;

  cmsHPROFILE in_profile;

public:
  WaveletDecomposePar();

  bool has_intensity() { return false; }
  bool needs_caching();

  float get_numScales() { return (float)numScales.get(); }
  float get_currScale() { return (float)currScale.get(); }
/*  float get_wp_adjustment() { return wp_adjustment.get(); }
  float get_compress() { return compress.get(); }
  float get_sh_color_adjustment() { return sh_color_adjustment.get(); }
  float get_hi_color_adjustment() { return hi_color_adjustment.get(); }
*/
  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
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



template < OP_TEMPLATE_DEF_TYPE_SPEC >
class WaveletDecomposeProc< OP_TEMPLATE_IMP_TYPE_SPEC(float) >
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
//    void *user_data;
//  } dwt_params_t;
#define INDEX_WT_IMAGE(ch, index) (((index)*ch)+c)

public:
  void render(VipsRegion** ireg, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* oreg, OpParBase* par)
  {
//    if( n != 2 ) return;
    if( ireg[0] == NULL ) return;
//    if( ireg[1] == NULL ) return;

    WaveletDecomposePar* opar = dynamic_cast<WaveletDecomposePar*>(par);
    if( !opar ) return;

    
    float *pwavdec = NULL;
//    float *in_color_sp = NULL;
    
    Rect *r = &oreg->valid;
    wd_width = r->width;
    wd_height = r->height;
    const int ch = oreg->im->Bands;
//    const int ch = 1;
    
//    _rt_rt_user_data_t usr_data = {0};
//    wt_preview_types_t preview_type = wt_preview_type;
    
//    int gui_active = 0;
//    if (self->dev) gui_active = (self == self->dev->gui_module);
//    if (!gui_active && preview_type == WT_PREVIEW_CURRENT_SCALE) preview_type = WT_PREVIEW_FINAL_IMAGE;
    
//    dwt_params_t dwt_p;
//    wt_decompose_channels_t channels_req = wt_get_requested_channels(p);
    
//    memset(&dwt_p, 0, sizeof(dwt_p));
    
    pwavdec = (float*)malloc(wd_width * wd_height * ch * sizeof(float));
    memset(pwavdec, 0, wd_width * wd_height * ch * sizeof(float));
//    memcpy(in_retouch, ivoid, roi_rt->width * roi_rt->height * ch * sizeof(float));
    for( int y = 0; y < wd_height; y++ ) {
      float *pin = (float*)VIPS_REGION_ADDR( ireg[0], r->left, r->top + y );
//      pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top + y );
      float *pwd = pwavdec + y * wd_width * ch;

      for( int x = 0; x < wd_width*ch; x++ ) pwd[x] = pin[x];
    }
    
    // set no of channels to decompose and convertion table
    wd_channels = 0;
    if (ch >= 1) wd_channels_img[wd_channels++] = 0;
    if (ch >= 2) wd_channels_img[wd_channels++] = 1;
    if (ch >= 3) wd_channels_img[wd_channels++] = 2;
    if (wd_channels == 0) wd_channels = 1;
    
/*    usr_data.self = self;
    usr_data.piece = piece;
    usr_data.roi = *roi_rt;
    usr_data.mask_display = 0;
*/
    
    wd_image = pwavdec;
    wd_ch_im = ch;
//    width = width;
//    height = height;
    wd_scales = (int)opar->get_numScales();
    wd_return_layer = (int)opar->get_currScale();
    wd_clear_im = (wd_return_layer > 0);
    
    std::cout<<"wd_ch_im: "<<wd_ch_im<<std::endl;
    std::cout<<"wd_channels: "<<wd_channels<<std::endl;
    std::cout<<"wd_scales: "<<wd_scales<<std::endl;
    std::cout<<"wd_return_layer: "<<wd_return_layer<<std::endl;
    
//    user_data = NULL;

/*    // check if _this_ module should expose mask. 
    if(self->request_mask_display && self->dev->gui_attached && (self == self->dev->gui_module)
       && (piece->pipe == self->dev->pipe) )
    {
      for(size_t j = 0; j < roi_rt->width*roi_rt->height*ch; j += ch) in_retouch[j + 3] = 0.f;
      
      piece->pipe->mask_display = 1;
      usr_data.mask_display = 1;
    }
    */
/*    const int max_scales = dwt_get_max_scale(piece->buf_in.width, piece->buf_in.height);
    if (dwt_p.scales > max_scales && gui_active)
    {
      dt_control_log(_("max scale is %i for this image size"), max_scales);
    }
    else
    {
      const int max_scales2 = dwt_get_max_scale(roi_rt->width, roi_rt->height);
      if (dwt_p.scales > max_scales2 && gui_active)
      {
        printf("max scale is %i for this image preview size\n", max_scales2);
      }
    }
    */
    // convert to rgb/lab if needed
//    wt_convert_from_to_colorspace(p, in_retouch, roi_rt, &in_color_sp,ch, 1);

/*    if(self->suppress_mask && self->dev->gui_attached && (self == self->dev->gui_module)
       && (piece->pipe == self->dev->pipe))
    {*/
    
      dwt_decompose();
      
/*    }
    else
    {
      dwt_decompose(&dwt_p, rt_process_forms);
    }
*/
    // convert back from rgb/lab
/*    if (preview_type == WT_PREVIEW_FINAL_IMAGE)
    {
      wt_convert_from_to_colorspace(p, in_retouch, roi_rt, &in_color_sp, ch, 0);
    }
*/
/*    if (dwt_p.wd_return_layer > 0)
    {
      wt_lighten_rgb(in_retouch, roi_rt->width, roi_rt->height, wt_exposure, wt_bclip_percent, wt_wclip_percent, ch, wt_normalize);
    }
    */
//    wt_copy_ft_to_out(in_retouch, roi_rt, ovoid, roi_out, ch);
/*      for( int y = 0; y < wd_height; y++ ) {
//        float *pin = (float*)VIPS_REGION_ADDR( ireg[0], r->left, r->top + y );
        float *pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top + y );

        for( int x = 0; x < wd_width*3; x++ ) pout[x] = 0.f;
      }
      */
      for( int y = 0; y < wd_height; y++ ) {
//        float *pin = (float*)VIPS_REGION_ADDR( ireg[0], r->left, r->top + y );
        float *pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top + y );
        float *pwd = pwavdec + y * wd_width * ch;

        for( int x = 0; x < wd_width*ch; x++ ) pout[x] = pwd[x];
      }

    if (pwavdec) free(pwavdec);
//    if (in_color_sp) free(in_color_sp);


    
  }
  
private:
  void dwt_decompose()
  {
  #ifdef _TIME_FFT_
    struct timeval tm1,tm2;
    gettimeofday(&tm1,NULL);
  #endif
    
    /* this function prepares for decomposing, which is done in the function dwt_wavelet_decompose() */
    int max_scale = dwt_get_max_scale(wd_width, wd_height);
    int i, c, x;
    float *img = NULL;
    float *line;
    
    if (wd_scales > max_scale) wd_scales = max_scale;
    if (wd_return_layer > wd_scales+1) wd_return_layer = wd_scales+1;
    
    wd_layers = (float *)malloc(wd_width * wd_height * wd_channels * sizeof(float));
    if (wd_layers == NULL)
    {
      //dt_control_log(_("not enough memory for wavelet decomposition"));
      std::cout<<"not enough memory for wavelet decomposition"<<std::endl;
      goto cleanup;
    }
    memset(wd_layers, 0, wd_width * wd_height * wd_channels * sizeof(float));

    img = (float *)malloc(wd_width * wd_height * wd_channels * sizeof(float));
    if (img == NULL)
    {
      //dt_control_log(_("not enough memory for wavelet decomposition"));
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

//    if (layer_func) layer_func(img, p, 0);
    
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
    const float lpass_sub = .128f;
    
    const int size = wd_width * wd_height * wd_channels;

    /* image buffers */
    buffer[0] = img;
    /* temporary storage */
    buffer[1] = (float *)malloc(size * sizeof(float));
    if (buffer[1] == NULL)
    {
      //dt_control_log(_("not enough memory for wavelet decomposition"));
      std::cout<<"not enough memory for wavelet decomposition"<<std::endl;
      goto cleanup;
    }
    memset(buffer[1], 0, size * sizeof(float));

    temp = (float *)malloc(MAX(wd_width, wd_height) * wd_channels * sizeof(float));
    if (temp == NULL)
    {
      //dt_control_log(_("not enough memory for wavelet decomposition"));
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

//      if (layer_func) layer_func(buffer[hpass], p, lev + 1);
      
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
//      if (layer_func) layer_func(buffer[lpass], p, wd_scales+1);
      
      dwt_get_image_layer(buffer[lpass]);
    }
    else if (wd_return_layer == 0)
    {
//      if (layer_func) layer_func(buffer[hpass], p, wd_scales+1);
      
      dwt_add_layer(buffer[hpass], wd_scales+1);
      
//      if (layer_func) layer_func(wd_layers, p, wd_scales+2);
      
      dwt_get_image_layer(wd_layers);
    }

  cleanup:
    if (temp) free(temp);
    if (buffer[1]) free(buffer[1]);

  }

  void dwt_add_layer(float *const img, const int n_scale)
  {
    const float lpass_sub = .128f;
    
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
//    const int channels = (wd_channels==4) ? channels-1: channels;
    
    if (wd_clear_im)
    {
      if (wd_ch_im == 4)
      {
        // don't want to erase the mask in the alpha channel
        for (int y=0; y < wd_height; y++)
        {
          float *l = wd_image + (y * wd_width * wd_ch_im);
          
          for (int x=0; x < wd_width; x++, l+=4)
          {
            l[0] = l[1] = l[2] = 0;
          }
        }
      }
      else
      {
        memset(wd_image, 0, wd_width * wd_height * wd_ch_im * sizeof(float));
      }
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

  int dwt_get_max_scale(const int width, const int height)
  {
    int maxscale = 0;
    
    /* smallest edge must be higher than or equal to 2^scales */
    unsigned int size = MIN(width, height);
    while (size >>= 1) maxscale++;

    return maxscale;
  } 


};


ProcessorBase* new_wavelet_decompose();

}

#endif 


