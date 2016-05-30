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


#include "operation.hh"
#include "layer.hh"
//#include "../vips/vips_layer.h"
#include "../operations/draw.hh"
#include "../gui/widgets/textbox.hh"
#include "../gui/widgets/checkbox.hh"
#include "../gui/widgets/slider.hh"
#include "../gui/widgets/selector.hh"
#include "../gui/widgets/exposure_slider.hh"
#include "../gui/widgets/imagebutton.hh"
#include "../gui/widgets/colorbtn.hh"

int
vips_layer( int n, VipsImage **out, 
            PF::ProcessorBase* proc,
            VipsImage* imap, VipsImage* omap, 
            VipsDemandStyle demand_hint,
	    int width, int height, int nbands, ... );





void PF::OperationConfigUI::open()
{
  get_layer()->get_processor()->get_par()->save_properties(initial_params);
}




PF::OpParBase::OpParBase():
	render_mode(PF_RENDER_PREVIEW), 
  map_flag( false ),
  editing_flag( false ),
  modified_flag(false),
  intensity("intensity",this,1),
  grey_target_channel("grey_target_channel",this,-1,"Grey","Grey"),
  rgb_target_channel("rgb_target_channel",this,-1,"RGB","RGB"),
  lab_target_channel("lab_target_channel",this,-1,"Lab","Lab"),
  cmyk_target_channel("cmyk_target_channel",this,-1,"CMYK","CMYK"),
  mask_enabled("mask_enabled",this,true)
{
  //blend_mode.set_internal(true);
  intensity.set_internal(true);
  //opacity.set_internal(true);
  grey_target_channel.set_internal(true);
  rgb_target_channel.set_internal(true);
  lab_target_channel.set_internal(true);
  cmyk_target_channel.set_internal(true);
  
  processor = NULL;
  //out = NULL;
  config_ui = NULL;
  //blend_mode = PF_BLEND_PASSTHROUGH;
  //blend_mode = PF_BLEND_NORMAL;
  //blend_mode.set_enum_value( PF_BLEND_PASSTHROUGH );
  demand_hint = VIPS_DEMAND_STYLE_ANY;
  bands = 1;
  xsize = 100; ysize = 100;

  rgb_target_channel.add_enum_value(0,"R","R");
  rgb_target_channel.add_enum_value(1,"G","G");
  rgb_target_channel.add_enum_value(2,"B","B");

  lab_target_channel.add_enum_value(0,"L","L");
  lab_target_channel.add_enum_value(1,"a","a");
  lab_target_channel.add_enum_value(2,"b","b");

  cmyk_target_channel.add_enum_value(0,"C","C");
  cmyk_target_channel.add_enum_value(1,"M","M");
  cmyk_target_channel.add_enum_value(2,"Y","Y");
  cmyk_target_channel.add_enum_value(3,"K","K");

  //PF::PropertyBase* prop;
  //prop = new PF::Property<float>("intensity",&intensity);
}


PF::PropertyBase* PF::OpParBase::get_property(std::string name)
{
  std::list<PropertyBase*>::iterator pi;

  // Look into mapped properties first
  for(pi = mapped_properties.begin(); pi != mapped_properties.end(); pi++) {
    //std::cout<<"(*pi)->get_name(): "<<(*pi)->get_name()<<"    name: "<<name<<std::endl;
    if( (*pi)->get_name() == name ) return( *pi );
  }
  
  // If nothing is found, look into our own properties
  for(pi = properties.begin(); pi != properties.end(); pi++) {
    //std::cout<<"(*pi)->get_name(): "<<(*pi)->get_name()<<"    name: "<<name<<std::endl;
    if( (*pi)->get_name() == name ) return( *pi );
  }
  return NULL;
}


void PF::OpParBase::save_properties(std::list<std::string>& plist)
{
  std::list<PropertyBase*>::iterator pi;
  for(pi = mapped_properties.begin(); pi != mapped_properties.end(); pi++) {
    std::string str = (*pi)->get_str();
    plist.push_back(str);
  }
  for(pi = properties.begin(); pi != properties.end(); pi++) {
    std::string str = (*pi)->get_str();
    plist.push_back(str);
  }
}


void PF::OpParBase::restore_properties(const std::list<std::string>& plist)
{
  std::list<PropertyBase*>::iterator pi;
  std::list<std::string>::const_iterator si;
  for(pi = mapped_properties.begin(), si = plist.begin(); 
      (pi != mapped_properties.end()) && (si != plist.end()); 
      pi++, si++) {
    (*pi)->set_str(*si);
  }
  for(pi = properties.begin(); 
      (pi != properties.end()) && (si != plist.end()); 
      pi++, si++) {
    (*pi)->set_str(*si);
  }
}


void PF::OpParBase::clear_modified() 
{ 
  modified_flag = false; 
  std::list<PropertyBase*>::iterator pi;
  for(pi = mapped_properties.begin();
      pi != mapped_properties.end(); 
      pi++) {
    (*pi)->clear_modified();
  }
  for(pi = properties.begin(); 
      pi != properties.end(); 
      pi++) {
    (*pi)->clear_modified();
  }
}

void PF::OpParBase::set_image_hints(int w, int h, VipsInterpretation interpr)
{
  xsize = w;
  ysize = h;
  coding = VIPS_CODING_NONE;
  interpretation = interpr;
}



void PF::OpParBase::set_image_hints(int w, int h, colorspace_t cs)
{
  xsize = w;
  ysize = h;
  coding = VIPS_CODING_NONE;
  switch(cs) {
  case PF_COLORSPACE_GRAYSCALE:
    interpretation = VIPS_INTERPRETATION_B_W; break;
  case PF_COLORSPACE_RGB:
    interpretation = VIPS_INTERPRETATION_RGB; break;
  case PF_COLORSPACE_LAB:
    interpretation = VIPS_INTERPRETATION_LAB; break;
  case PF_COLORSPACE_CMYK:
    interpretation = VIPS_INTERPRETATION_CMYK; break;
  default:
    interpretation = VIPS_INTERPRETATION_MULTIBAND; break;
  }
}


bool PF::OpParBase::import_settings( OpParBase* pin )
{
  if( !pin ) {
    std::cout<<"OpParBase::import_settings(): pin = NULL"<<std::endl;
    return false;
  }

  std::list<PropertyBase*>& propin = pin->get_properties();
  std::list<PropertyBase*>::iterator pi=propin.begin(), pj=properties.begin();
  for( ; pi != propin.end(); pi++, pj++ ) {
    if( !(*pj)->import( *pi ) ) {
      std::cout<<"OpParBase::import_settings(): failed to import values for property \""<<(*pj)->get_name()<<"\""<<std::endl;
      return false;
    }
  }

  std::list<PropertyBase*>& mpropin = pin->get_mapped_properties();
  std::list<PropertyBase*>::iterator mpi=mpropin.begin(), mpj=mapped_properties.begin();
  for( ; mpi != mpropin.end(); mpi++, mpj++ ) {
    if( !(*mpj)->import( *mpi ) ) {
      std::cout<<"OpParBase::import_settings(): failed to import values for property \""<<(*mpj)->get_name()<<"\""<<std::endl;
      return false;
    }
  }

  set_map_flag( pin->is_map() );
  //std::cout<<"OpParBase::import_settings(): set_editing_flag("<<pin->is_editing()<<")"<<std::endl;
  set_editing_flag( pin->is_editing() );
  //set_demand_hint( pin->get_demand_hint() );
  //set_image_hints( pin->get_xsize(), pin->get_ysize(),
	//	   pin->get_interpretation() );
  //set_nbands( pin->get_nbands() );
  //set_coding( pin->get_coding() );
  //set_format( pin->get_format() );
  return true;
}


VipsImage* PF::OpParBase::build(std::vector<VipsImage*>& in, int first, 
				VipsImage* imap, VipsImage* omap, unsigned int& level)
{
  VipsImage* outnew = NULL;
  VipsImage* invec[100];
  unsigned int n = 0;
  for(unsigned int i = 0; i < in.size(); i++) {
    if( !in[i] ) continue;
    invec[n] = in[i];
    n++;
  }
  if(n > 100) n = 100;
  switch( n ) {
  case 0:
    vips_layer( n, &outnew, processor, imap, omap, 
		get_demand_hint(), get_xsize(), get_ysize(), get_nbands(),
		NULL );
    break;
  case 1:
    vips_layer( n, &outnew, processor, imap, omap, 
		get_demand_hint(), get_xsize(), get_ysize(), get_nbands(),
		"in0", invec[0], NULL );
    break;
  case 2:
    vips_layer( n, &outnew, processor, imap, omap, 
    get_demand_hint(), get_xsize(), get_ysize(), get_nbands(),
    "in0", invec[0], "in1", invec[1], NULL );
    break;
  case 3:
    vips_layer( n, &outnew, processor, imap, omap,
    get_demand_hint(), get_xsize(), get_ysize(), get_nbands(),
    "in0", invec[0], "in1", invec[1],
    "in2", invec[2], NULL );
    break;
  default:
    break;
  }

#ifndef NDEBUG
  std::cout<<"OpParBase::build(): type="<<type<<"  format="<<get_format()<<std::endl
	   <<"input images:"<<std::endl;
  for(int i = 0; i < n; i++) {
    std::cout<<"  "<<(void*)invec[i]<<"   ref_count="<<G_OBJECT( invec[i] )->ref_count<<std::endl;
  }
  std::cout<<"imap: "<<(void*)imap<<std::endl<<"omap: "<<(void*)omap<<std::endl;
  std::cout<<"out: "<<(void*)outnew<<std::endl<<std::endl;
#endif

  //set_image( outnew );
#ifndef NDEBUG    
  std::cout<<"OpParBase::build(): outnew refcount ("<<(void*)outnew<<") = "<<G_OBJECT(outnew)->ref_count<<std::endl;
#endif
  return outnew;
}



std::vector<VipsImage*> PF::OpParBase::build_many(std::vector<VipsImage*>& in, int first,
        VipsImage* imap, VipsImage* omap, unsigned int& level)
{
  std::vector<VipsImage*> result;
  VipsImage* out = build( in, first, imap, omap, level );

  VipsImage* cached = out;
  if( out && needs_caching() ) {
    int tw = 64, th = 64;
    // reserve two complete rows of tiles
    int nt = out->Xsize*2/tw;
    VipsAccess acc = VIPS_ACCESS_RANDOM;
    int threaded = 1, persistent = 0;

    if( vips_tilecache(out, &cached,
        "tile_width", tw, "tile_height", th, "max_tiles", nt,
        "access", acc, "threaded", threaded, "persistent", persistent, NULL) ) {
      std::cout<<"GaussBlurPar::build(): vips_tilecache() failed."<<std::endl;
      return result;
    }
    PF_UNREF( out, "OpParBase::build_many(): out unref" );
  }

  result.push_back( cached );
  return result;
}


bool PF::OpParBase::save( std::ostream& ostr, int level )
{
  for(int i = 0; i < level; i++) ostr<<"  ";
  ostr<<"<operation type=\""<<get_type()<<"\">"<<std::endl;

  for( std::list<PropertyBase*>::iterator pi = properties.begin();
       pi != properties.end(); pi++ ) {
    for(int i = 0; i < level+1; i++) ostr<<"  ";
    ostr<<"<property name=\""<<(*pi)->get_name()<<"\" value=\"";
    (*pi)->to_stream( ostr );
    ostr<<"\">"<<std::endl;
    for(int i = 0; i < level+1; i++) ostr<<"  ";
    ostr<<"</property>"<<std::endl;
  }
  
  for( std::list<PropertyBase*>::iterator pi = mapped_properties.begin();
       pi != mapped_properties.end(); pi++ ) {
    for(int i = 0; i < level+1; i++) ostr<<"  ";
    ostr<<"<property name=\""<<(*pi)->get_name()<<"\" value=\"";
    (*pi)->to_stream( ostr );
    ostr<<"\">"<<std::endl;
    for(int i = 0; i < level+1; i++) ostr<<"  ";
    ostr<<"</property>"<<std::endl;
  }
  
  for(int i = 0; i < level; i++) ostr<<"  ";
  ostr<<"</operation>"<<std::endl;

  return true;
}


int PF::vips_copy_metadata( VipsImage* in, VipsImage* out )
{
  if( !out ) return 0;
  int Xsize = out->Xsize;
  int Ysize = out->Ysize;
  int bands = out->Bands;
  VipsBandFormat fmt = out->BandFmt;
  VipsCoding coding = out->Coding;
  VipsInterpretation type = out->Type;
  gdouble xres = out->Xres;
  gdouble yres = out->Yres;
  VipsImage* invec[2] = {in, NULL};
  vips__image_copy_fields_array( out, invec );
  vips_image_init_fields( out,
      Xsize, Ysize, bands, fmt,
      coding, type, xres, yres
      );
return 0;
}



float PF::vivid_light_f(float nbottom, float ntop)
{
  //nbottom = 50.0f/255.0f;
  //ntop = 200.0f/255.0f;
  float nvivid;
  if( ntop <= 0.5 )
    nvivid = PF::color_burn( nbottom, ntop*2.0f );
  else
    nvivid = PF::color_dodge( nbottom, ntop*2.0f-1.0f );

  //std::cout<<"vivid_light=("<<nbottom*255<<","<<ntop*255<<")="<<nvivid*255.0f<<std::endl;
  return nvivid;
}
#if 0
void PF::OpParBase::create_properties() 
{
  
//  GMicArgumentList arg_list;
//  GMicArgumentList phf_arg_list;
  PropertyBase* prop;
  std::list<FilterField>field_list = get_pf_filter().get_fields();
  
//  std::cout<<"PF::OpParBase::create_properties() "<<get_pf_filter().get_name()<<", "<<get_pf_filter().get_type()<<std::endl;
  
/*  arg_list.parse_arguments(filter_arguments[2]);
  std::list<GMicArgument>arg_l = arg_list.get_arg_list();
  phf_arg_list.parse_arguments(filter_arguments[3]);
  std::list<GMicArgument>phf_arg_l = phf_arg_list.get_arg_list();
*/
/*  for (std::list<GMicArgument>::iterator it=phf_arg_l.begin(); it != phf_arg_l.end(); ++it) {
    if (it->field_description == "phf_padding") padding = std::stoi(it->str_value);
  }
*/
  for (std::list<FilterField>::iterator it=field_list.begin(); it != field_list.end(); ++it) {
    
//    std::cout<<"PF::OpParBase::create_properties()"<<", "<<it->field_type<<", "<<it->field_description<<std::endl;
    
    prop = NULL;
    switch (it->field_type)
    {
    case FilterField::field_type_float:
      prop = new Property<float>(it->field_name, this, it->val_default);
      break;
    case FilterField::field_type_int:
      prop = new Property<int>(it->field_name, this, it->val_default);
      break;
    case FilterField::field_type_bool:
      prop = new Property<bool>(it->field_name, this, it->val_default);
      break;
    case FilterField::field_type_button:
      break;
    case FilterField::field_type_choise:
    {
      int enum_val = 0;
      std::list<std::pair<std::string, Glib::ustring>>l_arg_l = it->arg_list;
      prop = new PropertyBase(it->field_name, this );
      for (std::list<std::pair<std::string, Glib::ustring>>::iterator itl=l_arg_l.begin(); itl != l_arg_l.end(); ++itl) {
          prop->add_enum_value( enum_val++, (*itl).first, (*itl).second );
      }
      prop->set_enum_value((int) it->val_default);
    }
      break;
    case FilterField::field_type_text_multi:
      prop = new Property<std::string>(it->field_name, this, it->field_description);
      break;
    case FilterField::field_type_text_single:
      prop = new Property<std::string>(it->field_name, this, it->field_description);
      break;
    case FilterField::field_type_file:
      break;
    case FilterField::field_type_folder:
      break;
    case FilterField::field_type_color:
      prop = new Property<RGBColor>(it->field_name, this, RGBColor(it->val_default, it->val_min, it->val_max));
      break;
    case FilterField::field_type_const:
      prop = new Property<std::string>(it->field_name, this, it->str_value);
      break;
    case FilterField::field_type_note:
      break;
    case FilterField::field_type_link:
      break;
    case FilterField::field_type_separator:
      break;
    }
  }

}

void PF::OperationConfigUI::create_controls(OperationConfigUI *op, std::list<FilterField>& field_list)
{
//  GMicArgumentList arg_list;
//  std::list<FilterField>field_list = get_pf_filter().get_fields();

//  arg_list.parse_arguments(columns[GMicLoad::col_arguments_gmic]);
//  std::list<GMicArgument>arg_l = arg_list.get_arg_list();
  Slider *sl;
  CheckBox *chb;
  Gtk::Button *btn;
  Selector *sel;
  TextBox *txt;
  Gtk::Label *lbl;
  Gtk::HSeparator *sep;
  Gtk::LinkButton *linkb;
  ColorBtn *clrbtn;
  std::string description;
  std::string name;
  OperationConfigUI *op1;
  
  for (std::list<FilterField>::iterator it=field_list.begin(); it != field_list.end(); ++it) {
    switch (it->field_type)
    {
    case FilterField::field_type_float:
      description = it->field_description;
      name = it->field_name+"";
      op1 = op;
            sl = new Slider( op1, name, description, 
                (double)it->val_default, (double)it->val_min, (double)it->val_max, 
                (double)it->val_step1, (double)it->val_step2, (double)1);
            controlsBox.pack_start( *sl );
      break;
    case FilterField::field_type_int:
/*            sl = new Slider( op, it->field_name, it->field_description, 
                (double)it->val_default, (double)it->val_min, (double)it->val_max, 
                (double)it->val_step1, (double)it->val_step2, 1);*/
            controlsBox.pack_start( *sl );
            break;
    case FilterField::field_type_bool:
      chb = new CheckBox( op, it->field_name, it->field_description, it->val_default );
      controlsBox.pack_start( *chb );
      break;
    case FilterField::field_type_button:
      btn = new Gtk::Button(it->field_description);
      controlsBox.pack_start( *btn );
      break;
    case FilterField::field_type_choise:
      sel = new Selector( op, it->field_name, it->field_description, (int)it->val_default );
      controlsBox.pack_start( *sel );
      break;
    case FilterField::field_type_text_multi:
      txt = new TextBox( op, it->field_name, it->field_description, it->field_description );
      controlsBox.pack_start( *txt );
      break;
    case FilterField::field_type_text_single:
      txt = new TextBox( op, it->field_name, it->field_description, it->field_description );
      controlsBox.pack_start( *txt );
      break;
    case FilterField::field_type_file:
      break;
    case FilterField::field_type_folder:
      break;
    case FilterField::field_type_color:
    {
#ifdef GTKMM_2
      Gdk::Color clr;
      clr.set_rgb(it->val_default, it->val_min, it->val_max);
//      clrbtn = new Gtk::ColorButton(clr);
#endif
#ifdef GTKMM_3
      Gdk::RGBA clr;
      clr.set_rgba(it->val_default, it->val_min, it->val_max);
//      clrbtn = new Gtk::ColorButton(clr);
#endif
      clrbtn = new ColorBtn(op, it->field_name, it->field_description, clr);
      controlsBox.pack_start( *clrbtn );
      
//      clrbtn->signal_color_set().
//          connect( sigc::mem_fun(op, &PF::GmicGenericConfigGUI::on_color_btn_changed) );
    }
      break;
    case FilterField::field_type_const:
      break;
    case FilterField::field_type_note:
      lbl = new Gtk::Label();
      lbl->set_use_markup(true);
      lbl->set_label( it->str_value );
      lbl->set_line_wrap(true);
      lbl->set_halign(Gtk::Align::ALIGN_START);
      controlsBox.pack_start( *lbl, Gtk::PACK_SHRINK );
      break;
    case FilterField::field_type_link:
      linkb = new Gtk::LinkButton();
      linkb->set_label( it->field_description );
      linkb->set_uri( it->str_value );
      linkb->set_halign(Gtk::Align::ALIGN_START);
      controlsBox.pack_start( *linkb );
      break;
    case FilterField::field_type_separator:
      sep = new Gtk::HSeparator();
      controlsBox.pack_start( *sep );
      break;
    }
  }

}
#endif