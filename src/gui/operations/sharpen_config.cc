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

#include "sharpen_config.hh"


PF::SharpenConfigGUI::SharpenConfigGUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "Sharpen" ),
  modeSelector( this, "method", "Sharpen method: ", 0 ),
  usmRadiusSlider( this, "usm_radius", "Radius", 1, 0, 100, 0.05, 0.1, 1),
rlSigmaSlider( this, "rl_sigma", "Sigma", 1, 0, 100, 0.05, 0.1, 1),
rlIterationsSlider( this, "rl_iterations", "Iterations", 10, 1, 100, 1, 5, 1)
{
  controlsBox.pack_start( modeSelector, Gtk::PACK_SHRINK );

  usmControlsBox.pack_start( usmRadiusSlider, Gtk::PACK_SHRINK );

  rlControlsBox.pack_start( rlSigmaSlider, Gtk::PACK_SHRINK );
  rlControlsBox.pack_start( rlIterationsSlider, Gtk::PACK_SHRINK );

  add_widget( controlsBox );

  get_main_box().show_all_children();
}




void PF::SharpenConfigGUI::do_update()
{
  if( get_layer() && get_layer()->get_image() && 
      get_layer()->get_processor() &&
      get_layer()->get_processor()->get_par() ) {

    OpParBase* par = get_layer()->get_processor()->get_par();
    PropertyBase* prop = par->get_property( "method" );
    if( !prop )  return;

    //std::cout<<"PF::SharpenConfigGUI::do_update() called."<<std::endl;

    if( usmControlsBox.get_parent() == &controlsBox )
      controlsBox.remove( usmControlsBox );
    if( rlControlsBox.get_parent() == &controlsBox )
      controlsBox.remove( rlControlsBox );

    switch( prop->get_enum_value().first ) {
    case PF::SHARPEN_USM:
      controlsBox.pack_start( usmControlsBox, Gtk::PACK_SHRINK );
      usmControlsBox.show();
			break;
    case PF::SHARPEN_DECONV:
      controlsBox.pack_start( rlControlsBox, Gtk::PACK_SHRINK );
      rlControlsBox.show();
			break;
    case PF::SHARPEN_MICRO:
			break;
		}
  }
  controlsBox.show_all_children();

  OperationConfigGUI::do_update();
}

