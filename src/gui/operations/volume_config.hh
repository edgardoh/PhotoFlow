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

#ifndef VOLUME_CONFIG_DIALOG_HH
#define VOLUME_CONFIG_DIALOG_HH

#include <gtkmm.h>

#include "../operation_config_gui.hh"
#include "../../operations/volume.hh"
#include "../widgets/vslider.hh"


namespace PF {

  class VolumeConfigGUI: public OperationConfigGUI
  {
    Gtk::VBox controlsBox;
    Gtk::VBox controlsBox2;
    Gtk::HBox equalizerBox;
    Gtk::HBox globalBox;
    Gtk::HBox equalizerCheckboxBox;
    Gtk::Alignment equalizerCheckboxPadding;
    Gtk::Alignment equalizerPadding;
    Gtk::HSeparator separator;
    
    Selector modeSelector;

    Slider amount_slider;
    Slider threshold_slider;
    CheckBox enable_equalizer_box;
    VSlider blacks_amount_slider;
    VSlider shadows_amount_slider;
    VSlider midtones_amount_slider;
    VSlider highlights_amount_slider;
    VSlider whites_amount_slider;

    Gtk::VBox usmControlsBox;
    Slider usmRadiusSlider;

    Gtk::VBox bilateralControlsBox;
    Slider bilateralIterationsSlider, bilateralSigmasSlider,
      bilateralSigmarSlider;

  public:
    VolumeConfigGUI( Layer* l );
    
    bool has_preview() { return true; }

    void do_update();
  };

}

#endif
