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
#include "wavdec.hh"


PF::WavDecPar::WavDecPar():
OpParBase(),
numScales("numScales",this,0),
currScale("currScale",this,0),
blendFactor("blendFactor",this,0.5f)
{
  wavdec_algo = new PF::Processor<PF::WavDecAlgoPar,PF::WavDecAlgoProc>();

  set_type("wavdec" );
  set_default_name( _("Wav Dec") );
}

VipsImage* PF::WavDecPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;
  VipsImage* srcimg = in[0];

  std::vector<VipsImage*> in2;

  WavDecAlgoPar* wavdec_par = dynamic_cast<WavDecAlgoPar*>( wavdec_algo->get_par() );
  int max_scales = wavdec_par->get_maxScales(srcimg->Xsize, srcimg->Ysize);
  if (numScales.get() > max_scales) {
    std::cout<<"WavDecPar::build: max scale is "<<max_scales<<" for this image preview size"<<std::endl;
  }
  else
    max_scales = numScales.get();
  wavdec_par->set_numScales( max_scales );
  wavdec_par->set_currScale( currScale.get() );
  wavdec_par->set_blendFactor( blendFactor.get() );

  wavdec_par->set_image_hints( srcimg );
  wavdec_par->set_format( get_format() );
  in2.clear();
  in2.push_back( srcimg );
  VipsImage* wavdec = wavdec_par->build( in2, 0, NULL, NULL, level );
  PF_UNREF( srcimg, "WavDecPar::build(): srcimg unref" );
  
  set_image_hints( wavdec );

  return wavdec;

}


PF::ProcessorBase* PF::new_wavdec()
{
  return new PF::Processor<PF::WavDecPar,PF::WavDecProc>();
}

