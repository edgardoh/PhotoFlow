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

#ifndef GMIC_FREAKY_BW_H
#define GMIC_FREAKY_BW_H


#include "../../base/processor.hh"

#include "gmic_untiled_op.hh"


namespace PF 
{

  class GmicFreaky_BWPar: public GmicUntiledOperationPar
  {
    Property<float> prop_strength;
    Property<float> prop_oddness;
    Property<float> prop_brightness;
    Property<float> prop_contrast;

  public:
    GmicFreaky_BWPar();
    ~GmicFreaky_BWPar() { std::cout<<"~GmicFreaky_BWPar() called."<<std::endl; }

/*    bool has_intensity() { return false; }
    bool has_opacity() { return true; }
    bool needs_caching() { return false; }
    bool init_hidden() { return true; }
*/
    VipsImage* build(std::vector<VipsImage*>& in, int first, 
                     VipsImage* imap, VipsImage* omap, 
                     unsigned int& level);
/*    std::vector<VipsImage*> build_many(std::vector<VipsImage*>& in, int first,
        VipsImage* imap, VipsImage* omap,
        unsigned int& level);*/
  };

  

  template < OP_TEMPLATE_DEF > 
  class GmicFreaky_BWProc
  {
  public: 
    void render(VipsRegion** ireg, int n, int in_first,
                VipsRegion* imap, VipsRegion* omap, 
                VipsRegion* oreg, OpParBase* par)
    {	
    }
  };




  ProcessorBase* new_gmic_freaky_bw();
}

#endif 


