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

#ifndef GMIC_GENERIC_UNTILED2_H
#define GMIC_GENERIC_UNTILED2_H


#include "../../base/processor.hh"

#include "gmic_untiled_op.hh"


namespace PF 
{

  class GmicGenericUntiled2Par: public GmicUntiledOperationPar
  {
    Property<GmicFilter> gmic_filter_prop;

/*    Property<int> iterations;
    Property<int> prop_interations;
    Property<int> prop_equalize;
    PropertyBase prop_merging_option;
    Property<float> prop_opacity;
    Property<int> prop_reverse;
    Property<float> prop_smoothness;
    Property<int> padding;*/
    //ProcessorBase* gmic;

  public:
    GmicGenericUntiled2Par();
    ~GmicGenericUntiled2Par() { std::cout<<"~GmicGenericUntiled2Par() called."<<std::endl; }

//    bool has_intensity() { return false; }
    bool has_opacity() { return true; }
//    bool needs_caching() { return false; }
//    bool init_hidden() { return true; }

    void set_gmic_filter(GmicFilter* gmf);
    GmicFilter* get_gmic_filter( ) { return &get_gmic_filer_prop(); }
    
    int get_verbosity_mode() { return 0; }

    GmicFilter& get_gmic_filer_prop() { return gmic_filter_prop.get(); }
    std::string build_command();

/*    VipsImage* build(std::vector<VipsImage*>& in, int first, 
                     VipsImage* imap, VipsImage* omap, 
                     unsigned int& level);*/
    std::vector<VipsImage*> build_many(std::vector<VipsImage*>& in, int first,
        VipsImage* imap, VipsImage* omap,
        unsigned int& level);

  };

  

  template < OP_TEMPLATE_DEF > 
  class GmicGenericUntiled2Proc
  {
  public: 
    void render(VipsRegion** ireg, int n, int in_first,
                VipsRegion* imap, VipsRegion* omap, 
                VipsRegion* oreg, OpParBase* par)
    {	
    }
  };




  ProcessorBase* new_gmic_generic_untiled2();
}

#endif 


