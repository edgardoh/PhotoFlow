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

#ifndef GMIC_GENERIC_TILED_H
#define GMIC_GENERIC_TILED_H


#include "../base/processor.hh"
#include "../gmic_generic.hh"


namespace PF 
{

class GmicGenericTiledPar: public OpParBase
{
  Property<GmicFilter> gmic_filter_prop;
	
  ProcessorBase* convert_format;
  ProcessorBase* convert_format2;

  int m_padding;

public:
  GmicGenericTiledPar();

  bool has_intensity() { return false; }
  bool has_opacity() { return true; }
  bool needs_caching() { return false; }

  int get_padding( int level ) { return m_padding; }    
  void set_padding( int p ) { m_padding = p; }    
  int get_verbosity_mode() { return 0; }

  std::string build_command();
  void set_gmic_filter(GmicFilter* gmf);
  GmicFilter* get_gmic_filter( ) { return &get_gmic_filer_prop(); }
  
  GmicFilter& get_gmic_filer_prop() { return gmic_filter_prop.get(); }
  
  VipsImage* build(std::vector<VipsImage*>& in, int first, 
                   VipsImage* imap, VipsImage* omap, 
                   unsigned int& level);
};


template < OP_TEMPLATE_DEF > 
class GmicGenericTiledProc
{
public: 
  void render(VipsRegion** ireg, int n, int in_first,
              VipsRegion* imap, VipsRegion* omap, 
              VipsRegion* oreg, OpParBase* par)
  { 
  }
};


  ProcessorBase* new_gmic_generic_tiled();
}

#endif 


