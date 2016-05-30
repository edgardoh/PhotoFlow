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

#ifndef GMIC_GENERIC_UNTILED_H
#define GMIC_GENERIC_UNTILED_H


#include "../../base/processor.hh"

#include "gmic_untiled_op.hh"


namespace PF 
{

  class GmicGenericUntiledPar: public GmicUntiledOperationPar
  {
    Property<std::string> prop_name;
    Property<std::string> prop_command;
    Property<std::string> prop_arguments;

  public:
    GmicGenericUntiledPar();
    ~GmicGenericUntiledPar() { std::cout<<"~GmicGenericUntiledPar() called."<<std::endl; }

    void set_prop_name(const std::string s) { prop_name.set( s ); set_default_name( s ); }
    std::string get_prop_name() { return prop_name.get(); }
    void set_prop_command(const std::string s) { prop_command.set( s ); }
    std::string get_prop_command() { return prop_command.get(); }
    void set_prop_arguments(const std::string s) { prop_arguments.set(s); }
    std::string get_prop_arguments() { return prop_arguments.get(); }

    void post_init();
//    void create_properties(std::vector<std::string>& filter_arguments);

/*    std::vector<VipsImage*> build_many(std::vector<VipsImage*>& in, int first,
        VipsImage* imap, VipsImage* omap,
        unsigned int& level);*/
    VipsImage* build(std::vector<VipsImage*>& in, int first, 
                     VipsImage* imap, VipsImage* omap, 
                     unsigned int& level);
  };

  

  template < OP_TEMPLATE_DEF > 
  class GmicGenericUntiledProc
  {
  public: 
    void render(VipsRegion** ireg, int n, int in_first,
                VipsRegion* imap, VipsRegion* omap, 
                VipsRegion* oreg, OpParBase* par)
    {	
    }
  };




  ProcessorBase* new_gmic_generic_untiled();
}

#endif 


