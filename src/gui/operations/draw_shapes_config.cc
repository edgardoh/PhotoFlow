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

#include "draw_shapes_config.hh"


PF::DrawShapesConfigGUI::DrawShapesConfigGUI( PF::Layer* layer ):
ShapesConfigGUI( layer, "draw shapes" ),
fgd_color_label("foreground color: "),
bgd_color_label("background color: "),
#ifdef GTKMM_2
fgd_color_button( Gdk::Color("white") ),
bgd_color_button( Gdk::Color("black") ),
#endif
#ifdef GTKMM_3
fgd_color_button( Gdk::RGBA("white") ),
bgd_color_button( Gdk::RGBA("black") ),
#endif
bgd_transparent_checkbox( this, "bgd_transparent", _("transparent"), false )
{
    colorButtonsBox1.pack_start( bgd_color_label, Gtk::PACK_SHRINK );
    colorButtonsBox1.pack_start( bgd_color_button, Gtk::PACK_SHRINK );
    colorButtonsBox1.pack_start( bgd_transparent_checkbox, Gtk::PACK_SHRINK );
    colorButtonsBox2.pack_start( fgd_color_label, Gtk::PACK_SHRINK );
    colorButtonsBox2.pack_start( fgd_color_button, Gtk::PACK_SHRINK );
    controlsBox.pack_start( colorButtonsBox1 );
    controlsBox.pack_start( colorButtonsBox2 );

    fgd_color_button.signal_color_set().
      connect( sigc::mem_fun(this, &PF::DrawShapesConfigGUI::on_fgd_color_changed) );
    bgd_color_button.signal_color_set().
      connect( sigc::mem_fun(this, &PF::DrawShapesConfigGUI::on_bgd_color_changed) );

    add_widget( controlsBox );

}

#define CALLBACK_BEGIN() \
/* Retrieve the layer associated to the filter */ \
PF::Layer* layer = get_layer(); \
if( !layer ) return; \
\
/* Retrieve the image the layer belongs to */ \
PF::Image* image = layer->get_image(); \
if( !image ) return; \
\
/* Retrieve the pipeline #0 (full resolution preview) */ \
PF::Pipeline* pipeline = image->get_pipeline( 0 ); \
if( !pipeline ) return; \
\
/* Find the pipeline node associated to the current layer */ \
PF::PipelineNode* node = pipeline->get_node( layer->get_id() ); \
if( !node ) return; \
if( !node->image ) return; \
\
PF::DrawShapesPar* par = dynamic_cast<PF::DrawShapesPar*>(get_par()); \
if( !par ) return; \


void PF::DrawShapesConfigGUI::on_fgd_color_changed()
{
  CALLBACK_BEGIN();
  
  bool modified = false;

//  PF::DrawShapesPar* par = dynamic_cast<PF::DrawShapesPar*>( get_par() );
//  if( !par ) return;
  
#ifdef GTKMM_2
  float r = fgd_color_button.get_color().get_red()/65535;
  float g = fgd_color_button.get_color().get_green()/65535;
  float b = fgd_color_button.get_color().get_blue()/65535;
  par->set_fgd_color(r, g, b);
#endif

#ifdef GTKMM_3
  par->set_fgd_color(fgd_color_button.get_rgba().get_red(), fgd_color_button.get_rgba().get_green(), fgd_color_button.get_rgba().get_blue());
#endif
  
  ShapeColor& color = par->get_shapes_group().get_current_color();
  for (int i = 0; i < get_selection_count(); i++)
  {
//    color.set(par->get_fgd_color().get().r, par->get_fgd_color().get().g, par->get_fgd_color().get().b);
    par->get_shape(get_selected(i))->set_color(color);
    modified = true;
  }
  
  if (modified) {
    par->shapes_modified();
    image->update();
  }
}


void PF::DrawShapesConfigGUI::on_bgd_color_changed()
{
  // Pointer to the associated Layer object
  PF::Layer* layer = get_layer();
  if( !layer ) return;

  // First of all, the new stroke is recorded by the "master" operation
  PF::ProcessorBase* processor = layer->get_processor();
  if( !processor || !(processor->get_par()) ) return;
  
  PF::DrawShapesPar* par = dynamic_cast<PF::DrawShapesPar*>( processor->get_par() );
  if( !par ) return;
  
#ifdef GTKMM_2
  float r = bgd_color_button.get_color().get_red()/65535;
  float g = bgd_color_button.get_color().get_green()/65535;
  float n = bgd_color_button.get_color().get_blue()/65535;
  par->set_bgd_color(r, g, b);
#endif

#ifdef GTKMM_3
  par->set_bgd_color(bgd_color_button.get_rgba().get_red(), bgd_color_button.get_rgba().get_green(), bgd_color_button.get_rgba().get_blue());
#endif

  if( layer->get_image() )
    layer->get_image()->update();
}

