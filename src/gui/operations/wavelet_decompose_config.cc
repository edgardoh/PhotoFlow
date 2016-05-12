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

#include "wavelet_decompose_config.hh"


PF::WaveletDecomposeConfigGUI::WaveletDecomposeConfigGUI( PF::Layer* layer ):
OperationConfigGUI( layer, "Wavelet Decompose" ),
//modeSelector( this, "method", _("soften with: "), 0 ),
numScales_slider( this, "numScales", _("Scales"), 50, 0, 15, 1, 1, 1),
currScale_slider( this, "currScale", _("Current Scale"), 50, 0, 16, 1, 1, 1)
/*wp_adj_slider( this, "wp_adjustment", _("whithe point adjustment"), 0, -10, 10, 0.2, 1, 1),
radius_slider( this, "radius", _("radius"), 100, 0, 200, 2, 10, 1),
compress_slider( this, "compress", _("compress"), 50, 0, 100, 2, 10, 1),
sh_color_adj_slider( this, "sh_color_adjustment", _("shadows color adjustment"), 100, 0, 100, 2, 10, 1),
hi_color_adj_slider( this, "hi_color_adjustment", _("highlights color adjustment"), 50, 0, 100, 2, 10, 1)*/
{
  controlsBox.pack_start( numScales_slider, Gtk::PACK_SHRINK );
  controlsBox.pack_start( currScale_slider, Gtk::PACK_SHRINK );
/*  controlsBox.pack_start( wp_adj_slider, Gtk::PACK_SHRINK );
  controlsBox.pack_start( modeSelector, Gtk::PACK_SHRINK );
  controlsBox.pack_start( radius_slider, Gtk::PACK_SHRINK );
  controlsBox.pack_start( compress_slider, Gtk::PACK_SHRINK );
  controlsBox.pack_start( sh_color_adj_slider, Gtk::PACK_SHRINK );
  controlsBox.pack_start( hi_color_adj_slider, Gtk::PACK_SHRINK );
*/
  add_widget( controlsBox );

  get_main_box().show_all_children();
}



