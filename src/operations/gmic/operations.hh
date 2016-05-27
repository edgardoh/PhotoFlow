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

#ifndef GMIC_OPERATIONS_H
#define GMIC_OPERATIONS_H

namespace PF
{

  ProcessorBase* new_gmic();
  ProcessorBase* new_gmic_blur_bilateral();
  ProcessorBase* new_gmic_denoise();
  ProcessorBase* new_gmic_smooth_nlmeans();
  ProcessorBase* new_gmic_smooth_anisotropic();
  ProcessorBase* new_gmic_smooth_diffusion();
  ProcessorBase* new_gmic_smooth_mean_curvature();
  ProcessorBase* new_gmic_smooth_wavelets_haar();
  ProcessorBase* new_gmic_smooth_median();
  ProcessorBase* new_gmic_smooth_selective_gaussian();
  ProcessorBase* new_gmic_smooth_total_variation();
  ProcessorBase* new_gmic_emulate_film_colorslide();
  ProcessorBase* new_gmic_emulate_film_bw();
  ProcessorBase* new_gmic_emulate_film_instant_consumer();
  ProcessorBase* new_gmic_emulate_film_instant_pro();
  ProcessorBase* new_gmic_emulate_film_negative_color();
  ProcessorBase* new_gmic_emulate_film_negative_new();
  ProcessorBase* new_gmic_emulate_film_negative_old();
  ProcessorBase* new_gmic_emulate_film_print_films();
  ProcessorBase* new_gmic_emulate_film_various();
  ProcessorBase* new_gmic_gcd_despeckle();
  ProcessorBase* new_gmic_smooth_guided();
  ProcessorBase* new_gmic_iain_denoise();
  ProcessorBase* new_gmic_dream_smooth();
  ProcessorBase* new_gmic_freaky_bw();
  ProcessorBase* new_gmic_extract_foreground();
  ProcessorBase* new_gmic_tone_mapping();
  ProcessorBase* new_gmic_inpaint();
  ProcessorBase* new_gmic_convolve();
  ProcessorBase* new_gmic_gradient_norm();
  ProcessorBase* new_gmic_sharpen_rl();
  ProcessorBase* new_gmic_split_details();
  ProcessorBase* new_gmic_transfer_colors();
  ProcessorBase* new_gmic_watermark_fourier();
  ProcessorBase* new_gmic_generic_tiled();
  ProcessorBase* new_gmic_generic_untiled();
  //insert new operations here

  ProcessorBase* new_gmic_operation( std::string op_type );
}

#endif
