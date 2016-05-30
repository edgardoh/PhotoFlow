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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../base/new_operation.hh"
#include "subtr_image.hh"


PF::SubtractImagePar::SubtractImagePar():
OpParBase(),
blendFactor("blendFactor",this,0.5f)
{
  subtract_image_algo = new PF::Processor<PF::SubtractImageAlgoPar,PF::SubtractImageAlgoProc>();

  set_type("subtract_image" );
  set_default_name( _("Subtract Image") );
}

VipsImage* PF::SubtractImagePar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;
  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  SubtractImageAlgoPar* subtract_image_par = dynamic_cast<SubtractImageAlgoPar*>( subtract_image_algo->get_par() );
  subtract_image_par->set_blendFactor( blendFactor.get() );

  subtract_image_par->set_image_hints( srcimg );
  subtract_image_par->set_format( get_format() );
  in2.clear();
  in2.push_back( srcimg );
  VipsImage* subtract_image = subtract_image_par->build( in2, 0, NULL, NULL, level );
  PF_UNREF( srcimg, "SubtractImagePar::build(): cached unref" );
  
  set_image_hints( subtract_image );

  return subtract_image;

}


PF::ProcessorBase* PF::new_subtract_image()
{
  return new PF::Processor<PF::SubtractImagePar,PF::SubtractImageProc>();
}

