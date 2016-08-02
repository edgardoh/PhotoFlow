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

//#include "gmic_untiled_op.hh"
#include "../../vips/gmic/gmic/src/gmic.h"


namespace PF 
{

  class GmicGenericUntiledPar: public OpParBase
  {
    Property<GmicFilter> gmic_filter_prop;
    
    char* custom_gmic_commands;
    gmic* gmic_instance;

  public:
    GmicGenericUntiledPar();
    ~GmicGenericUntiledPar();

    bool has_intensity() { return false; }
    bool has_opacity() { return true; }
    bool needs_caching() { return false; }

    void set_gmic_filter(GmicFilter* gmf);
    GmicFilter* get_gmic_filter( ) { return &get_gmic_filer_prop(); }
    
    int get_verbosity_mode() { return 0; }

    GmicFilter& get_gmic_filer_prop() { return gmic_filter_prop.get(); }
    std::string build_command();
    bool run_gmic( VipsImage* in, VipsImage** out, std::string command );
    gmic* new_gmic();
    
    virtual void set_metadata2( VipsImage* in, VipsImage* out )
    {
      VipsImage* invec[2] = {in, NULL};
      vips__image_copy_fields_array( out, invec );
    }
    
    virtual void set_metadata( VipsImage* in, VipsImage* out )
    {
      set_metadata2( in, out );
      vips_image_init_fields( out,
          in->Xsize, in->Ysize, in->Bands,
          IM_BANDFMT_FLOAT,
          in->Coding, in->Type, in->Xres, in->Yres
          );
    }


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


