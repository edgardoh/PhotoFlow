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

#include "gmic_generic_config_unt2.hh"
#include "../../../operations/gmic/gmic_generic_untiled2.hh"
#include "../../widgets/colorbtn.hh"


PF::GmicGenericConfigUntiled2GUI::GmicGenericConfigUntiled2GUI( PF::Layer* layer ):
GmicGenericConfigGUI( layer )
{

}

PF::GmicFilter& PF::GmicGenericConfigUntiled2GUI::get_gmic_filter()
{
  PF::GmicGenericUntiled2Par* par = dynamic_cast<PF::GmicGenericUntiled2Par*>(get_par());
  return par->get_gmic_filer_prop();
}


#define GMIC_EVENT_BEGIN() \
/* Retrieve the layer associated to the filter */ \
PF::Layer* layer = get_layer(); \
if( !layer ) return; \
\
/* Retrieve the image the layer belongs to */ \
PF::Image* image = layer->get_image(); \
if( !image ) return; \
\
PF::GmicGenericUntiledPar* par = dynamic_cast<PF::GmicGenericUntiledPar*>(get_par()); \
if( !par ) return; \

/*
void PF::GmicGenericConfigUntiled2GUI::value_changed_widg(PF::PFWidget* widg)
{
  if (widg) {

    GMIC_EVENT_BEGIN()

    bool modified = false;
    GmicFilter& gmic_filter = par->get_gmic_filer_prop();

    modified = value_changed_widg_internal(widg, gmic_filter);
    
    if ( modified ) {
      par->modified();
      image->update();
    }
  }
}

void PF::GmicGenericConfigUntiled2GUI::create_controls()
{
  if (m_controls_created) return;
  
  PF::GmicGenericUntiledPar* par = dynamic_cast<PF::GmicGenericUntiledPar*>(get_par());
  if( !par ) return;
  
  GmicFilter& gmf = par->get_gmic_filer_prop();
  
  create_controls_internal(gmf);
}
*/

