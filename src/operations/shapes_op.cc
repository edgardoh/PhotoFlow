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

#include "shapes_op.hh"
#include "../base/processor.hh"


PF::ShapesPar::ShapesPar():
                OpParBase(),
                falloff_curve( "falloff_curve", this ), 
                shapes("shapes_grp", this)
{
  set_type( "shapes" );
  set_default_name( _("shapes") );
}

VipsImage* PF::ShapesPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  
  build_masks(get_shapes_group(), get_shapes_falloff_curve(), level); 
  
  return PF::OpParBase::build(in, first, imap, omap, level);
}

void PF::ShapesPar::build_masks(ShapesGroup& sg, SplineCurve& scurve, unsigned int level)
{
  int scale_factor = 1;
  for(unsigned int l = 0; l < level; l++ ) {
    scale_factor *= 2;
  }
  
  shapes_scaled.clear();
  shapes_scaled = sg;
  shapes_scaled.scale(scale_factor);
  shapes_scaled.build_mask(scurve);

}

PF::ProcessorBase* PF::new_shapes()
{
  return( new PF::Processor<PF::ShapesPar,PF::ShapesProc>() );
}

