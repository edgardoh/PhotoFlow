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

#include "shapes_mask.hh"
#include "../base/processor.hh"


PF::ShapesMaskPar::ShapesMaskPar():
ShapesPar(),
invert("invert",this,false)
{
  set_type( "shapes_mask" );
  set_default_name( _("shapes mask") );
}



VipsImage* PF::ShapesMaskPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  VipsImage* out = PF::ShapesPar::build( in, first, imap, omap, level );

  int tw = 64, th = 64, nt = out->Xsize*2/tw;  
  VipsImage* cached = out;
  
  VipsAccess acc = VIPS_ACCESS_RANDOM;
  bool threaded = true, persistent = false;
  if( vips_tilecache(out, &cached,
      "tile_width", tw, "tile_height", th, "max_tiles", nt,
      "access", acc, "threaded", threaded, "persistent", persistent, NULL) ) {
    std::cout<<"ShapesMaskPar::build(): vips_tilecache() failed."<<std::endl;
    return NULL;
  }
  PF_UNREF( out, "GaussBlurPar::build(): iter_in unref" );
  
  return cached;
}


PF::ProcessorBase* PF::new_shapes_mask()
{
  return( new PF::Processor<PF::ShapesMaskPar,PF::ShapesMaskProc>() );
}


