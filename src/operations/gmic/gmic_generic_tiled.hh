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


namespace PF 
{

class GmicGenericTiledPar: public OpParBase
{
	Property<std::string> prop_name;
	Property<std::string> prop_command;
	Property<std::string> prop_arguments;
	
  ProcessorBase* gmic;
//  ProcessorBase* gmic_generic_algo;

  int padding;

public:
  GmicGenericTiledPar();

  bool has_intensity() { return false; }
  bool has_opacity() { return true; }
  bool needs_caching() { return false; }

  void set_prop_name(const std::string s) { prop_name.set( s ); set_default_name( s ); }
  std::string get_prop_name() { return prop_name.get(); }
  void set_prop_command(const std::string s) { prop_command.set( s ); }
  std::string get_prop_command() { return prop_command.get(); }
  void set_prop_arguments(const std::string s) { prop_arguments.set(s); }
  std::string get_prop_arguments() { return prop_arguments.get(); }

  int get_padding( int level ) { return padding; }    
  void set_padding( int p ) { padding = p; }    
  int get_verbosity_mode() { return 0; }

  void post_init();
  void create_properties();
  std::string build_command();
//  void create_properties(std::vector<std::string>& filter_arguments);

  VipsImage* build(std::vector<VipsImage*>& in, int first, 
                   VipsImage* imap, VipsImage* omap, 
                   unsigned int& level);
};


class GmicGenericTiledAlgoPar: public OpParBase
{
	std::string prop_name;
	std::string prop_command;
	std::string prop_arguments;

public:
  GmicGenericTiledAlgoPar(): OpParBase() { };

  void set_filter_name(const std::string s) { prop_name = s; }
  std::string get_filter_name() { return prop_name; }
  void set_filter_command(const std::string s) { prop_command = s; }
  std::string get_filter_command() { return prop_command; }
  void set_filter_arguments(const std::string s) { prop_arguments = s; }
  std::string get_filter_arguments() { return prop_arguments; }

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

template < OP_TEMPLATE_DEF > 
class GmicGenericTiledAlgoProc
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


