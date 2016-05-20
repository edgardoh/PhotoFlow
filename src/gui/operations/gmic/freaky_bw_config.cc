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

#include "freaky_bw_config.hh"

#include "../../../operations/gmic/freaky_bw.hh"


PF::GmicFreaky_BWConfigGUI::GmicFreaky_BWConfigGUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "Freaky B&W (G'MIC)"  ),
  updateButton( "Update" ),
  prop_strength_slider( this, "strength", "strength", 90, 0, 100, 1, 5, 1),
  prop_oddness_slider( this, "oddness", "oddness", 20, 0, 100, 1, 5, 1),
  prop_brightness_slider( this, "brightness", "brightness", 0, -1, 1, .01, .1, 1),
  prop_contrast_slider( this, "contrast", "contrast", 0.1, -1, 2, .1, .5, 1)
{
  //controlsBox.pack_start( iterations_slider );
  controlsBox.pack_start( updateButton );
  prop_strength_slider.set_passive( true );
  controlsBox.pack_start( prop_strength_slider );
  prop_oddness_slider.set_passive( true );
  controlsBox.pack_start( prop_oddness_slider );
  prop_brightness_slider.set_passive( true );
  controlsBox.pack_start( prop_brightness_slider );
  prop_contrast_slider.set_passive( true );
  controlsBox.pack_start( prop_contrast_slider );
  //controlsBox.pack_start( paddingSlider );

  updateButton.signal_clicked().connect( sigc::mem_fun(this, &GmicFreaky_BWConfigGUI::on_update) );
  
  add_widget( controlsBox );
}



void PF::GmicFreaky_BWConfigGUI::on_update()
{
  if( get_layer() && get_layer()->get_image() && 
      get_layer()->get_processor() &&
      get_layer()->get_processor()->get_par() ) {
    GmicFreaky_BWPar* par = dynamic_cast<GmicFreaky_BWPar*>( get_layer()->get_processor()->get_par() );
    if( !par ) return;
    par->refresh();
    get_layer()->get_image()->lock();
    std::cout<<"  updating image"<<std::endl;
    get_layer()->get_image()->update();
    get_layer()->get_image()->unlock();
  }
}


void PF::GmicFreaky_BWConfigGUI::open()
{
  OperationConfigGUI::open();
}
