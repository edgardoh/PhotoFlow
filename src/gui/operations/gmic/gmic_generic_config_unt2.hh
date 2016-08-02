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

#ifndef GMIC_GENERIC_CONFIG_UNTILED2_DIALOG_HH
#define GMIC_GENERIC_CONFIG_UNTILED2_DIALOG_HH

#include <gtkmm.h>
#include "gmic_generic_config.hh"


namespace PF {

  class GmicGenericConfigUntiled2GUI: public GmicGenericConfigGUI
  {
    
  public:
    GmicGenericConfigUntiled2GUI( Layer* l );
    
    virtual PF::GmicFilter& get_gmic_filter();

//    void create_controls();

//    void value_changed_widg(PF::PFWidget* widg);

  };

}

#endif
