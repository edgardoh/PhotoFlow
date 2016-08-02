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

#ifndef GMIC_GENERIC_CONFIG_DIALOG_HH
#define GMIC_GENERIC_CONFIG_DIALOG_HH

#include <gtkmm.h>
#include "../../operation_config_gui.hh"


namespace PF {

  class GmicGenericConfigGUI: public OperationConfigGUI
  {
    Gtk::VBox controlsBox;

  protected:
    bool m_controls_created;
    
  public:
    GmicGenericConfigGUI( Layer* l );
    
    virtual PF::GmicFilter& get_gmic_filter();
    
    void create_controls_internal(GmicFilter& gmf);
    virtual void create_controls();

    bool value_changed_widg_internal(PF::PFWidget* widg, GmicFilter& gmic_filter);
    bool value_reset_widg_internal(PF::PFWidget* widg, GmicFilter& gmic_filter);

    virtual void value_changed_widg(PF::PFWidget* widg);
    virtual void value_reset_widg(PF::PFWidget* widg);

    void do_update();
    
    void parameters_reset();

  };

}

#endif
