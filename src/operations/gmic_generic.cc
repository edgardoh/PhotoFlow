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

#include "gmic_generic.hh"

#include "../vips/gmic/gmic/src/gmic.h"
#include "../vips/gmic/gmic/src/gmic_stdlib.h"
#include "../vips/gmic/gmic/src/CImg.h"

const char PF::GmicColor::gmic_menu_dquote = 28;
const char PF::GmicColor::gmic_menu_eol = 29;

using namespace cimg_library;

int get_gmic_verbosity_mode() { return 0; }

CImg<char> get_gmic_locale() {
  CImg<char> locale(16);
  *locale = 0;
  locale[0] = 'e'; locale[1] = 'n'; locale[2] = 0;
  return locale;
}


// -----------------------------------
// GmicArgument
// -----------------------------------

void PF::GmicArgument::Init()
{
  arg_id = 0;
  arg_type = arg_type_none;
  arg_name = "";
  arg_description;
  val_str = "";
  val_default = 0;
  val_min = 0;
  val_max = 0;
  val_step1 = 0;
  val_step2 = 0;
  
  arg_list.clear();
}


// -----------------------------------
// GmicProperty
// -----------------------------------

void PF::GmicProperty::get_str_value(std::string& str_value)
{
  std::ostringstream str;
  
  switch ( get_prop_type() )
  {
  case PF::GmicArgument::GmicArgumentType::arg_type_float:
    str << get_val_float();
    break;
  case PF::GmicArgument::GmicArgumentType::arg_type_int:
  case PF::GmicArgument::GmicArgumentType::arg_type_choise:
    str << get_val_int();
    break;
  case PF::GmicArgument::GmicArgumentType::arg_type_bool:
      str << get_val_int();
    break;
  case PF::GmicArgument::GmicArgumentType::arg_type_text_multi:
  case PF::GmicArgument::GmicArgumentType::arg_type_text_single:
  case PF::GmicArgument::GmicArgumentType::arg_type_const:
  case PF::GmicArgument::GmicArgumentType::arg_type_file:
  case PF::GmicArgument::GmicArgumentType::arg_type_folder:
    str << get_val_str();
    break;
  case PF::GmicArgument::GmicArgumentType::arg_type_color:
    str << (int)(get_val_color().get_r()*255)<<","
    << (int)(get_val_color().get_g()*255)<<","
    << (int)(get_val_color().get_b()*255);
    if ( get_use_alpha() ) {
      str << ","<<(int)(get_val_color().get_a()*255);
    }
    break;
  case PF::GmicArgument::GmicArgumentType::arg_type_note:
  case PF::GmicArgument::GmicArgumentType::arg_type_link:
  case PF::GmicArgument::GmicArgumentType::arg_type_separator:
  case PF::GmicArgument::GmicArgumentType::arg_type_button:
  default:
    str << "";
    break;
 }
  
  str_value = str.str();
}

// -----------------------------------
// GmicFilter
// -----------------------------------

int PF::GmicFilter::get_property(const std::string prop_name)
{
  for ( int i = 0; i < m_prop_list.size(); i++ ) {
    if ( m_prop_list[i].get_prop_name() == prop_name)
      return i;
  }
  return -1;
}

int PF::GmicFilter::get_phf_property(const std::string prop_name)
{
  for ( int i = 0; i < m_phf_prop_list.size(); i++ ) {
    if ( m_phf_prop_list[i].get_prop_name() == prop_name)
      return i;
  }
  return -1;
}

bool PF::GmicFilter::get_property(const std::string prop_name, GmicProperty& prop_value)
{
  int n = get_property(prop_name);
  if ( n >= 0 ) {
    prop_value = m_prop_list[n];
    return true;
  }
  return false;
}

bool PF::GmicFilter::get_phf_property(const std::string prop_name, GmicProperty& prop_value)
{
  int n = get_phf_property(prop_name);
  if ( n >= 0 ) {
    prop_value = m_phf_prop_list[n];
    return true;
  }
  return false;
}

void PF::GmicFilter::set_property(GmicProperty& prop_value)
{
  int n = get_property( prop_value.get_prop_name() );
  if ( n >= 0 ) {
    m_prop_list[n] = prop_value;
  }

}

void PF::GmicFilter::parse_arguments1(std::string filter_arguments1, std::vector<GmicArgument>& arg_list)
{
  std::string filter_arguments = PF::GmicProperty::quote_gmic(filter_arguments1);
  GmicArgument arg;
  
//  std::cout<<"PF::GmicFilter::parse_arguments(): filter_arguments: "<<filter_arguments<<std::endl;
  
  std::string arg_name;
  char temp_buff[20];
    // Count number of filter arguments.
    CImg<char> argument_name(256), _argument_type(32), argument_arg(65536);
    *argument_name = *_argument_type = *argument_arg = 0;
    unsigned int nb_arguments = 0;
    for (const char *argument = filter_arguments.c_str(); *argument; ) {
      int err = cimg_sscanf(argument,"%255[^=]=%31[ a-zA-z](%65535[^)]",
                            argument_name.data(),_argument_type.data(),&(argument_arg[0]=0));
      if (err!=3) err = cimg_sscanf(argument,"%255[^=]=%31[ a-zA-z]{%65535[^}]",
                                    argument_name.data(),_argument_type.data(),argument_arg.data());
      if (err!=3) err = cimg_sscanf(argument,"%255[^=]=%31[ a-zA-z][%65535[^]]",
                                    argument_name.data(),_argument_type.data(),argument_arg.data());
      if (err>=2) {
        argument += std::strlen(argument_name) + std::strlen(_argument_type) + std::strlen(argument_arg) + 3;
        if (*argument) ++argument;
        ++nb_arguments;
      } else break;
    }

    unsigned int current_table_line = 0;
    if (!nb_arguments) { // Filter requires no parameters -> 1x1 table with default message.

    } else { // Filter requires parameters -> Create parameters table.

      // Create new table for putting parameters inside.

      // Parse arguments list and add recognized one to the table.
      unsigned int current_argument = 0;
      const bool is_fave = false; //filter>=indice_faves;
      for (const char *argument = filter_arguments.c_str(); *argument; ) {
        int err = cimg_sscanf(argument,"%255[^=]=%31[ a-zA-Z_](%65535[^)]",
                              argument_name.data(),_argument_type.data(),&(argument_arg[0]=0));
        if (err!=3) err = cimg_sscanf(argument,"%255[^=]=%31[ a-zA-Z_][%65535[^]]",
                                      argument_name.data(),_argument_type.data(),argument_arg.data());
        if (err!=3) err = cimg_sscanf(argument,"%255[^=]=%31[ a-zA-Z_]{%65535[^}]",
                                      argument_name.data(),_argument_type.data(),argument_arg.data());
        if (err>=2) {
          argument += std::strlen(argument_name) + std::strlen(_argument_type) + std::strlen(argument_arg) + 3;
          if (*argument) ++argument;
          cimg::strpare(argument_name,' ',false,true);
          cimg::strpare(argument_name,'\"',true);
          cimg::strunescape(argument_name);
          cimg::strpare(_argument_type,' ',false,true);
          cimg::strpare(argument_arg,' ',false,true);

          
          const bool is_silent_argument = (*_argument_type=='_');
          char
            *const argument_type = _argument_type.data() + (is_silent_argument?1:0);


#if defined(_WIN64)
          typedef unsigned long long pint;
#else
          typedef unsigned long pint;
#endif

          // Check for a float-valued argument.
          bool found_valid_argument = false;
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"float")) {
            float value = 0, min_value = 0, max_value = 100;
            setlocale(LC_NUMERIC,"C");
            cimg_sscanf(argument_arg,"%f%*c%f%*c%f",&value,&min_value,&max_value);

            sprintf(temp_buff, "arg%i", current_argument);

            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_float;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_default = (double)value;
            arg.val_min = (double)min_value;
            arg.val_max = (double)max_value;
            arg.val_step1 = (double)(max_value - min_value)/100;
            arg.val_step2 = (double)(max_value - min_value)/20;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for an int-valued argument.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"int")) {
            float value = 0, min_value = 0, max_value = 100;
            setlocale(LC_NUMERIC,"C");
            cimg_sscanf(argument_arg,"%f%*c%f%*c%f",&value,&min_value,&max_value);

            sprintf(temp_buff, "arg%i", current_argument);

            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_int;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_default = (double)(int)cimg::round(value,1.0f);
            arg.val_min = (double)(int)cimg::round(min_value,1.0f);
            arg.val_max = (double)(int)cimg::round(max_value,1.0f);
            arg.val_step1 = (double)1;
            arg.val_step2 = (double)cimg::max(1.0,cimg::round((max_value - min_value)/20,1,1));
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a bool-valued argument.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"bool")) {
            cimg::strpare(argument_arg,' ',false,true); cimg::strpare(argument_arg,'\"',true);
            bool
              value = !(!*argument_arg || !cimg::strcasecmp(argument_arg,"false") ||
                        (argument_arg[0]=='0' && argument_arg[1]==0));

            sprintf(temp_buff, "arg%i", current_argument);

            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_bool;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_default = (double)value;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a button argument.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"button")) {
            float alignment = 0;
            setlocale(LC_NUMERIC,"C");
            if (cimg_sscanf(argument_arg,"%f",&alignment)!=1) alignment = 0;

            sprintf(temp_buff, "arg%i", current_argument);
            
            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_button;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_default = 0;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a choice-valued argument.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"choice")) {
            sprintf(temp_buff, "arg%i", current_argument);

              std::pair<std::string, std::string> choise_entry;
            CImg<char> s_entry(1024); *s_entry = 0;
            char end = 0; int err = 0;
            unsigned int value = 0;
            int enum_val = 1;
            const char *entries = argument_arg;
            if (cimg_sscanf(entries,"%u",&value)==1)
              entries+=cimg_snprintf(s_entry,s_entry.width(),"%u",value) + 1;
          arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_choise;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_default = value;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            int num_entries = 0;
            while (*entries) {
              if ((err = cimg_sscanf(entries,"%1023[^,]%c",s_entry.data(),&end))>0) {
                entries += std::strlen(s_entry) + (err==2?1:0);
                cimg::strpare(s_entry,' ',false,true); cimg::strpare(s_entry,'\"',true);
                cimg::strunescape(s_entry);
                
                char e_tmp[20];
                sprintf(e_tmp, "entry%i", num_entries++);
                choise_entry.first = e_tmp;
                choise_entry.second = (s_entry.data() == NULL) ? "": s_entry.data();
                arg.arg_list.push_back(choise_entry);
                
              } else break;
            }
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a single or multi-line text-valued argument.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"text")) {
            int line_number = 0;
            char sep = 0;
            if (cimg_sscanf(argument_arg,"%d%c",&line_number,&sep)==2 && sep==',' && line_number==1) {
              // Multi-line entry

              CImg<char> s_label(256); *s_label = 0;
              cimg_snprintf(s_label,s_label.width(),"  %s       ",argument_name.data());
              char *value = std::strchr(argument_arg,',') + 1;
              cimg::strpare(value,' ',false,true);
              cimg::strpare(value,'\"',true);
              for (char *p = argument_arg; *p; ++p) if (*p==gmic_dquote) *p='\"';
              
              arg.Init();
              arg.arg_id = current_argument;
              arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_text_multi;
              arg.arg_name = temp_buff;
              arg.val_str = (argument_arg.data() == NULL) ? "": argument_arg.data();
              arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
              arg.val_default = 0;
              arg.val_min = 0;
              arg.val_max = 0;
              arg.val_step1 = 0;
              arg.val_step2 = 0;
              
              arg_list.push_back(arg);


            } else { // Single-line entry
              char *value = (line_number!=0 || sep!=',')?argument_arg:(std::strchr(argument_arg,',') + 1);
              if (!is_fave) cimg::strunescape(value);
              cimg::strpare(value,' ',false,true);
              cimg::strpare(value,'\"',true);

              for (char *p = value; *p; ++p) if (*p==gmic_dquote) *p='\"';

              sprintf(temp_buff, "arg%i", current_argument);

              arg.Init();
              arg.arg_id = current_argument;
              arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_text_single;
              arg.arg_name = temp_buff;
              arg.val_str = (argument_arg.data() == NULL) ? "": argument_arg.data();
              arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
              arg.val_default = 0;
              arg.val_min = 0;
              arg.val_max = 0;
              arg.val_step1 = 0;
              arg.val_step2 = 0;
              
              arg_list.push_back(arg);
            }

            found_valid_argument = true; ++current_argument;
          }

          // Check for a filename or folder name argument.
          if (!found_valid_argument && (!cimg::strcasecmp(argument_type,"file") ||
                                        !cimg::strcasecmp(argument_type,"folder"))) {

            char *value = argument_arg;
            cimg::strpare(value,' ',false,true);
            cimg::strpare(value,'\"',true);
            
            sprintf(temp_buff, "arg%i", current_argument);
            
            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = cimg::uncase(argument_type[1])=='i'? PF::GmicArgument::GmicArgumentType::arg_type_file: PF::GmicArgument::GmicArgumentType::arg_type_folder;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_str = (value == NULL) ? "": value;
            arg.val_default = 0;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a color argument.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"color")) {
            float red = 0, green = 0, blue = 0, alpha = 255;
            setlocale(LC_NUMERIC,"C");

            alpha = -1;
            
            const int err = cimg_sscanf(argument_arg,"%f%*c%f%*c%f%*c%f",&red,&green,&blue,&alpha);
            red = red<0?0:red>255?255:red;
            green = green<0?0:green>255?255:green;
            blue = blue<0?0:blue>255?255:blue;
            
            sprintf(temp_buff, "arg%i", current_argument);
            
            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_color;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_color.set(red/255.f, green/255.f, blue/255.f, alpha/255.f);
            arg.m_use_alpha = ( alpha != -1 );
            arg.val_str = "";
            arg.val_default = 0;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a constant value.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"const")) {
            const char *value = argument_arg;
            
            sprintf(temp_buff, "arg%i", current_argument);
            
            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_const;
            arg.arg_name = temp_buff;
            arg.arg_description = (argument_name.data() == NULL) ? "": argument_name.data();
            arg.val_str = (value == NULL) ? "": value;
            arg.val_default = 0;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a note.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"note")) {
            cimg::strpare(argument_arg,' ',false,true);
            cimg::strpare(argument_arg,'\"',true);
            cimg::strunescape(argument_arg);
            
            sprintf(temp_buff, "arg%i", current_argument);
            
            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_note;
            arg.arg_name = temp_buff;
            arg.arg_description = "";
            arg.val_str = (argument_arg.data() == NULL) ? "": argument_arg.data();
            arg.val_default = 0;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for a link.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"link")) {
            CImg<char> label(1024), url(1024); *label = *url = 0;
            float alignment = 0.5f;
            switch (cimg_sscanf(argument_arg,"%f,%1023[^,],%1023s",&alignment,label.data(),url.data())) {
            case 2 : std::strcpy(url,label); break;
            case 1 : cimg_snprintf(url,url.width(),"%g",alignment); break;
            case 0 : if (cimg_sscanf(argument_arg,"%1023[^,],%1023s",label.data(),url.data())==1)
                std::strcpy(url,label); break;
            }
            cimg::strpare(label,' ',false,true); cimg::strpare(label,'\"',true);
            cimg::strunescape(label);
            cimg::strpare(url,' ',false,true); cimg::strpare(url,'\"',true);
            
            sprintf(temp_buff, "arg%i", current_argument);
            
            arg.Init();
            arg.arg_id = current_argument;
            arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_link;
            arg.arg_name = temp_buff;
            arg.arg_description = (label.data() == NULL) ? "": label.data();
            arg.val_str = (url.data() == NULL) ? "": url.data();
            arg.val_default = 0;
            arg.val_min = 0;
            arg.val_max = 0;
            arg.val_step1 = 0;
            arg.val_step2 = 0;
            
            arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          // Check for an horizontal separator.
          if (!found_valid_argument && !cimg::strcasecmp(argument_type,"separator")) {
              sprintf(temp_buff, "arg%i", current_argument);
              
              arg.Init();
              arg.arg_id = current_argument;
              arg.arg_type = PF::GmicArgument::GmicArgumentType::arg_type_separator;
              arg.arg_name = temp_buff;
              arg.arg_description = "";
              arg.val_str = "";
              arg.val_default = 0;
              arg.val_min = 0;
              arg.val_max = 0;
              arg.val_step1 = 0;
              arg.val_step2 = 0;
              
              arg_list.push_back(arg);

            found_valid_argument = true; ++current_argument;
          }

          if (!found_valid_argument) {
              std::fprintf(cimg::output(),
                           "\n[gmic_gimp]./error/ Found invalid parameter type '%s' for argument '%s'.\n",
                           argument_type,argument_name.data());
              std::fflush(cimg::output());
          } else ++current_table_line;
        } else break;
      }
    }

}

void PF::GmicFilter::parse_arguments()
{
  if ( m_arg_list.size() > 0 ) return;
  
  parse_arguments1(get_filter_arguments(), m_arg_list);
}

int PF::GmicFilter::get_argument(const std::string arg_name)
{
  for ( int i = 0; i < m_arg_list.size(); i++ ) {
    if ( m_arg_list[i].get_arg_name() == arg_name)
      return i;
  }
  return -1;
}

bool PF::GmicFilter::get_argument(const std::string arg_name, GmicArgument& arg_value)
{
  int n = get_argument(arg_name);
  if ( n >= 0 ) {
    arg_value = m_arg_list[n];
    return true;
  }
  return false;

}

void PF::GmicFilter::create_properties(bool include_const)
{
//  std::cout<<"PF::GmicFilter::create_properties()"<<std::endl;
  if ( m_prop_list.size() > 0 ) return;
  
//  std::string args = get_filter_arguments();
  std::vector<GmicArgument> arg_list;
  
  parse_arguments1(get_filter_arguments(), arg_list);
  
  for (std::vector<GmicArgument>::iterator it=arg_list.begin(); it != arg_list.end(); ++it) {
    GmicProperty prop;
    prop.set_prop_type( it->get_arg_type() );
    prop.set_prop_name( it->get_arg_name() );

    switch ( it->get_arg_type() )
    {
    case PF::GmicArgument::GmicArgumentType::arg_type_float:
      prop.set_val_float( it->get_val_default() );
      m_prop_list.push_back( prop );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_int:
    case PF::GmicArgument::GmicArgumentType::arg_type_bool:
    case PF::GmicArgument::GmicArgumentType::arg_type_choise:
      if ( it->get_arg_description() != "Preview type" ) {
        prop.set_val_int( it->get_val_default() );
        m_prop_list.push_back( prop );
      }
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_text_multi:
    case PF::GmicArgument::GmicArgumentType::arg_type_text_single:
    case PF::GmicArgument::GmicArgumentType::arg_type_file:
    case PF::GmicArgument::GmicArgumentType::arg_type_folder:
      prop.set_val_str( it->get_val_str() );
      m_prop_list.push_back( prop );
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_color:
    {
      GmicColor color(it->get_val_color());
      prop.set_val_color( color );
      prop.set_use_alpha( it->get_use_alpha() );
      
      m_prop_list.push_back( prop );
    }
      break;
    case PF::GmicArgument::GmicArgumentType::arg_type_const:
      if ( include_const ) {
        prop.set_val_str( it->get_val_str() );
        m_prop_list.push_back( prop );
      }
      break;
    }

  }

}

std::string PF::GmicFilter::get_gmic_command(int verbosity_mode)
{
  std::string gmic_command = get_filter_command();
  int nbparams = get_properties_count();
  std::string lres;
  switch (verbosity_mode) {
  case 0: case 1: case 2: case 3: lres = "-v -99 -"; break;  // Quiet or Verbose.
  case 4: case 5 : lres = "-v 0 -"; break;                   // Very verbose.
  default: lres = "-debug -";                                // Debug.
  }

  lres += gmic_command;
  if (nbparams) {
    lres += ' ';
    for (int i = 0; i < nbparams; i++) {
      GmicProperty& prop = get_property(i);
      if ( prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_float || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_int || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_bool || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_choise || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_text_multi || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_text_single || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_file || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_folder || 
          prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_color ) {

        std::string _ss;
        prop.get_str_value(_ss);
        
/*        if ( prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_text_multi || 
            prop.get_prop_type() == GmicArgument::GmicArgumentType::arg_type_text_single ) {
          _ss = "\"" + _ss + "\"";
            }
            */
        const int l = (int)_ss.length();
        for (int i = 1; i<l - 1; ++i) {
          const char c = _ss[i];
          _ss[i] = c=='\"'?gmic_dquote:c;
          
        }
        
        lres += _ss + ',';
      }
      else {
        std::cout<<"PF::GmicGenericTiledPar::build_command() prop.get_prop_type(): "<<prop.get_prop_type()<<std::endl;
      }
    }
    if (lres.back() == ',') lres.back() = 0;
  }

  gmic_command = lres;

//  std::cout<<"PF::GmicFilter::get_gmic_command() gmic_command: "<<gmic_command<<std::endl;
  
  return gmic_command;
}

void PF::GmicFilter::reset_values()
{
  m_prop_list.clear();
  create_properties();
}

// -----------------------------------
// GmicMenu
// -----------------------------------

void PF::GmicMenu::get_def_filenames(char const* def_filename, std::string& out_def_filename)
{
  char fname[500]; fname[0] = 0;
#if defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
  snprintf( fname, 499, "%s\\%s", PF::PhotoFlow::Instance().get_base_dir().c_str(), def_filename );
  std::cout<<"G'MIC custom commands file: "<<fname<<std::endl;
  struct stat buffer;   
  int stat_result = stat( fname, &buffer );
  if( stat_result != 0 ) {
    fname[0] = 0;
  }
#elif defined(__APPLE__) && defined (__MACH__)
  snprintf( fname, 499, "%s/%s", PF::PhotoFlow::Instance().get_data_dir().c_str(), def_filename );
  std::cout<<"G'MIC custom commands file: "<<fname<<std::endl;
  struct stat buffer;
  int stat_result = stat( fname, &buffer );
  if( stat_result != 0 ) {
    fname[0] = 0;
  }
#else
  if( getenv("HOME") ) {
    snprintf( fname, 499, "%s/share/photoflow/%s", INSTALL_PREFIX, def_filename );
    std::cout<<"G'MIC custom commands file: "<<fname<<std::endl;
    struct stat buffer;   
    int stat_result = stat( fname, &buffer );
    if( stat_result != 0 ) {
      fname[0] = 0;
    }
  }
#endif
  
  out_def_filename = fname;
}

std::string PF::GmicProperty::unquote_gmic(std::string& str_val)
{
  std::string str_ret = str_val;
  
  for ( int i = 0; i < str_ret.size(); i++ ) if ( str_ret[i] == '\"') str_ret[i] = PF::GmicColor::gmic_menu_dquote;
  
  return str_ret;
}

std::string PF::GmicProperty::quote_gmic(std::string& str_val)
{
  std::string str_ret = str_val;
  
  for ( int i = 0; i < str_ret.size(); i++ ) if ( str_ret[i] == PF::GmicColor::gmic_menu_dquote) str_ret[i] = '\"';
  
  return str_ret;
}

void PF::GmicMenu::add_menu_entry(std::vector<GmicFilter>& menu_entries, bool is_folder, int entry_level, 
                                    std::string& entry_type, std::string& entry_name, 
                                    std::string& entry_command, std::string& entry_arguments) { 
  GmicFilter filter;
  
  filter.set_is_folder(is_folder);
  filter.set_entry_level(entry_level);
  filter.set_filter_command(entry_command);
  filter.set_filter_name(PF::GmicProperty::unquote_gmic(entry_name));
  filter.set_filter_type(entry_type);
  filter.set_filter_arguments(PF::GmicProperty::unquote_gmic(entry_arguments));
  
  menu_entries.push_back(filter);
}

void PF::GmicMenu::parse_def_file(std::string& file_name, std::vector<GmicFilter>& def_filters)
{
  CImg<char> gmic_additional_commands;           // The buffer of additional G'MIC command implementations.
  CImgList<char> gmic_entries;                   // The list of recognized G'MIC menu entries.
  CImgList<char> gmic_1stlevel_entries;          // The treepath positions of 1st-level G'MIC menu entries.
  CImgList<char> gmic_commands;                  // The list of corresponding G'MIC commands to process the image.
  CImgList<char> gmic_preview_commands;          // The list of corresponding G'MIC commands to preview the image.
  CImgList<char> gmic_arguments;                 // The list of corresponding needed filter arguments.
  CImgList<char> gmic_faves;                     // The list of favorites filters and their default parameters.
  CImgList<double> gmic_preview_factors;         // The list of default preview factors for each filter.
  unsigned int nb_available_filters = 0;         // The number of available filters (non-testing).
  GtkWidget *markup2ascii = 0;                   // Used to convert markup to ascii strings.
  unsigned int indice_faves = 0;                 // The starting index of favorite filters.
  std::string entry_type;
  std::string entry_name;
  std::string entry_command;
  std::string entry_arguments;

  // Build list of filter sources.

  CImg<char> command(1024);
  CImgList<char> sources;
  CImg<char>::string(file_name.data()).move_to(sources);

  // Free existing definitions.
  gmic_additional_commands.assign();
  gmic_1stlevel_entries.assign();
  gmic_faves.assign();
  gmic_entries.assign(1);
  gmic_commands.assign(1);
  gmic_preview_commands.assign(1);
  gmic_preview_factors.assign(1);
  gmic_arguments.assign(1);

  // Get filter definition files from external web servers.
  CImg<char> filename(1024);
  CImg<char> filename_tmp(1024);
  char sep = 0;


  // Read local source files.
  
  CImgList<char> _gmic_additional_commands;
  bool is_default_update = false;
  cimglist_for(sources,l) {
    const char *s_basename = gmic::basename(sources[l]);
    
    if (s_basename) std::cout<<"PF::GMicFilters::load_gmic_filters() Read local source files: "<<s_basename<<std::endl;
    
    CImg<char> _s_basename = CImg<char>::string(s_basename);
    cimg::strwindows_reserved(_s_basename);
    if (!cimg::strncasecmp(sources[l],"http://",7) ||
        !cimg::strncasecmp(sources[l],"https://",8)) // Network file should have been copied in resources folder.
      cimg_snprintf(filename,filename.width(),"%s%s",
                    gmic::path_rc(),_s_basename.data());
    else // Local file, try to locate it at its hard-coded path.
    {
      cimg_snprintf(filename,filename.width(),"%s",
                    sources[l].data());
      
      std::cout<<"PF::GMicFilters::load_gmic_filters() Read local source file: "<<filename<<std::endl;
    }
    
    std::cout<<"PF::GMicFilters::load_gmic_filters() try to open file "<<filename<<std::endl;
    
    const unsigned int omode = cimg::exception_mode();
    try {
      CImg<char> com;
      bool add_code_separator = false;
      cimg::exception_mode(0);
      if (sources[l].back()==1) { // Overload default, add more checking.
        com.load_raw(filename);
        const char *_com = com.data(), *const ecom = com.end();
        while (_com<ecom && *_com<=32) ++_com;
        if (_com + 6<ecom && !std::strncmp(_com,"#@gmic",6)) {
          is_default_update = true;
          com.move_to(_gmic_additional_commands);
          add_code_separator = true;
        } else if (com.data() + 15<ecom && !std::strncmp(com,"1 unsigned_char",15)) {
          CImgList<char>::get_unserialize(com)[0].move_to(_gmic_additional_commands);
          is_default_update = true;
          add_code_separator = true;
        }
      } else {
        com.load_raw(filename);
        const char *const ecom = com.end();
        if (com.data() + 15<ecom && !std::strncmp(com,"1 unsigned_char",15))
          CImgList<char>::get_unserialize(com)[0].move_to(_gmic_additional_commands);
        else com.move_to(_gmic_additional_commands);
        add_code_separator = true;
      }
      if (add_code_separator)
        CImg<char>::string("\n#@gimp ________\n",false).unroll('y').move_to(_gmic_additional_commands);
    } catch(...) {
      std::cout<<"PF::GMicFilters::load_gmic_filters() Filter file not found "<<std::endl;
      
      if (get_gmic_verbosity_mode()>1)
        std::fprintf(cimg::output(),
                     "\n[gmic_gimp]./update/ Filter file '%s' not found.\n",
                     filename.data());
      std::fflush(cimg::output());
    }
    cimg::exception_mode(omode);
  }

  cimglist_for(_gmic_additional_commands,l) { // Remove unusual characters.
    char *_p = _gmic_additional_commands[l];
    cimg_for(_gmic_additional_commands[l],p,char) if (*p!=13) *(_p++) = (unsigned char)*p<' ' && *p!=10?' ':*p;
    if (_p<_gmic_additional_commands[l].end())
      CImg<char>(_gmic_additional_commands[l].data(),1,_p - _gmic_additional_commands[l].data()).
        move_to(_gmic_additional_commands[l]);
  }

  CImg<char>::vector(0).move_to(_gmic_additional_commands);
  (_gmic_additional_commands>'y').move_to(gmic_additional_commands);

  // Add fave folder if necessary (make it before actually adding faves to make tree paths valids).
  CImgList<char> gmic_1stlevel_names;
  gmic_1stlevel_names.assign(1);
  int prev_menu_level = 0;

  // Parse filters descriptions for GIMP, and create corresponding sorted treeview_store.
  CImg<char> line(256*1024), preview_command(256), arguments(65536), entry(256), locale = get_gmic_locale();
  *line = *preview_command = *arguments = *entry = 0;
  int level = 0, err = 0;
  bool is_testing = false;
  nb_available_filters = 0;
  cimg_snprintf(line,line.width(),"#@gimp_%s ",locale.data());

#define ADD_COMMAND_ENTRY(is_folder) \
    if ( (!is_folder && gmic_entries.size() > 1) || (is_folder && gmic_1stlevel_names.size() > 1) ) \
    { \
      \
      \
      if (is_folder) { \
        entry_type = ""; \
        entry_name = (gmic_1stlevel_names[1].data() == NULL) ? "": gmic_1stlevel_names[1].data(); \
        entry_command = (gmic_commands[1].data() == NULL) ? "": gmic_commands[1].data(); \
        entry_arguments = (gmic_arguments[1].data() == NULL) ? "": gmic_arguments[1].data(); \
      } \
      else { \
        entry_type = "gmic_generic_tiled"; \
        entry_name = (gmic_entries[1].data() == NULL) ? "": gmic_entries[1].data(); \
        entry_command = (gmic_commands[1].data() == NULL) ? "": gmic_commands[1].data(); \
        entry_arguments = (gmic_arguments[1].data() == NULL) ? "": gmic_arguments[1].data(); \
        if (entry_command == "_none_") entry_command = ""; \
        if (entry_command == "") entry_type = ""; \
      } \
      \
      \
      add_menu_entry(def_filters, is_folder, prev_menu_level, entry_type, entry_name, entry_command, entry_arguments); \
      gmic_entries.assign(1); \
      gmic_commands.assign(1); \
      gmic_arguments.assign(1); \
      gmic_1stlevel_names.assign(1); \
      prev_menu_level = level; \
      \
    } \

  // Use English for default language if no translated filters found.
  bool is_folder = true;
  if (!std::strstr(gmic_additional_commands,line)) { locale[0] = 'e'; locale[1] = 'n'; locale[2] = 0; }
  for (const char *data = gmic_additional_commands; *data; ) {
    char *_line = line;
    
    // Read new line.
    while (*data!='\n' && *data && _line<line.data() + line.width()) *(_line++) = *(data++); *_line = 0;
    while (*data=='\n') ++data; // Skip next '\n'.
    for (_line = line; *_line; ++_line) if (*_line<' ') *_line = ' '; // Replace non-usual characters by spaces.
    if (line[0]!='#' || line[1]!='@' || line[2]!='g' || // Check for a '#@gimp' line.
        line[3]!='i' || line[4]!='m' || line[5]!='p') continue;
    if (line[6]=='_') { // Check for a localized filter.
      // Whether the entry match current locale or not.
      if (line[7]==locale[0] && line[8]==locale[1] && line[9]==' ') _line = line.data() + 10;
      else continue;
    } else if (line[6]==' ') _line = line.data() + 7; else continue; // Check for a non-localized filter.

    
    if (*_line!=':') { // Check for a description of a possible filter or menu folder.
      *entry = *command = *preview_command = *arguments = 0;
      err = cimg_sscanf(_line," %255[^:]: %1023[^,]%*c %255[^,]%*c %65533[^\n]",
                        entry.data(),command.data(),preview_command.data(),arguments.data());

      if (err==1) { // If entry defines a menu folder.
        cimg::strpare(entry,' ',false,true);
        char *nentry = entry; while (*nentry=='_') { ++nentry; --level; }
        if (level<0) level = 0; else if (level>7) level = 7;
        cimg::strpare(nentry,' ',false,true); cimg::strpare(nentry,'\"',true);
        if (*nentry) {
          ADD_COMMAND_ENTRY(is_folder);
          is_folder = true;
          if (level) {
            
            CImg<char>::string(nentry).move_to(gmic_1stlevel_names);
          } else { // 1st-level folder.
            bool is_duplicate = false;

            if (!is_duplicate) {
              CImg<char>::string(nentry).move_to(gmic_1stlevel_names);
              unsigned int order = 0;
              for (unsigned int i = 0; i<4; ++i) {
                order<<=8;
              }
            }
          }
          ++level;
        }
      } else if (err>=2) { // If entry defines a regular filter.
        ADD_COMMAND_ENTRY(is_folder);
        is_folder = false;
        cimg::strpare(entry,' ',false,true);
        char *nentry = entry; while (*nentry=='_') { ++nentry; --level; }
        if (level<0) level = 0; else if (level>7) level = 7;
        cimg::strpare(nentry,' ',false,true); cimg::strpare(nentry,'\"',true);
        cimg::strpare(command,' ',false,true);
        cimg::strpare(arguments,' ',false,true);
        if (*nentry) {
          CImg<char>::string(nentry).move_to(gmic_entries);
          CImg<char>::string(command).move_to(gmic_commands);
          CImg<char>::string(arguments).move_to(gmic_arguments);
                    
          if (err>=3) { // Filter has a specified preview command.
            cimg::strpare(preview_command,' ',false,true);
            char *const preview_mode = std::strchr(preview_command,'(');
            bool is_accurate_when_zoomed = false;
            double factor = 1;
            char sep = 0;
            if (preview_mode &&
                ((cimg_sscanf(preview_mode + 1,"%lf)%c",&factor,&sep)==2 && sep=='+') ||
                 (cimg_sscanf(preview_mode + 1,"%lf%c",&factor,&sep)==2 && sep==')')) &&
                factor>=0) {
              *preview_mode = 0;
              is_accurate_when_zoomed = sep=='+';
            } else factor = -1;
            CImg<char>::string(preview_command).move_to(gmic_preview_commands);
            CImg<double>::vector(factor,(double)is_accurate_when_zoomed).move_to(gmic_preview_factors);
          } else {
            CImg<char>::string("_none_").move_to(gmic_preview_commands);
            CImg<double>::vector(-1,1).move_to(gmic_preview_factors);
          }
          if (!level) {
          }
          if (!is_testing) ++nb_available_filters;  // Count only non-testing filters.
        }
      }
    } else { // Line is the continuation of an entry.
      if (gmic_arguments) {
        if (gmic_arguments.back()) gmic_arguments.back().back() = ' ';
        cimg::strpare(++_line,' ',false,true);
        gmic_arguments.back().append(CImg<char>(_line,std::strlen(_line) + 1,1,1,1,true),'x');
      }
    }
  }

  ADD_COMMAND_ENTRY(is_folder);
  

}

// search for a matching filter into def_filters
bool PF::GmicMenu::find_match_entry(std::vector<GmicFilter>& def_filters, GmicFilter& in, GmicFilter& out)
{
  for ( int i = 0; i < def_filters.size(); i++ ) {
    if( (def_filters[i].get_filter_name() == in.get_filter_name() && /*def_filters[i].get_is_folder() == in.get_is_folder() ) && */
       /* (*/ def_filters[i].get_filter_command() == in.get_filter_command() /*|| !in.get_is_folder() */) ) {
      out = def_filters[i];
      return true;
    }
  }
  
  return false;
}

void PF::GmicMenu::load_filters()
{
  int exclude_lvl = -1;
  GmicProperty prop_value;
  GmicFilter phf_out;
  std::vector<GmicArgument> phf_arg_list;

  std::string gmic_def_filename;
  std::string phf_def_filename;
  std::string phf_args;

  std::vector<GmicFilter> gmic_def_filters;
  std::vector<GmicFilter> phf_def_filters;

  m_menu_entries.clear();

  // get full path for definition files
  get_def_filenames("gmic_def.gmic", gmic_def_filename);
  get_def_filenames("phf_def.gmic", phf_def_filename);

  // load GMIC filters
  parse_def_file(gmic_def_filename, gmic_def_filters);

  // load custom PhF commands for GMIC filters
  parse_def_file(phf_def_filename, phf_def_filters);
  
  // merge G'MIC filter def with custom PhF
  for ( int i = 0; i < gmic_def_filters.size(); i++ ) {
    GmicFilter& gmic_filter = gmic_def_filters[i];
    
    if ( exclude_lvl == -1 || exclude_lvl >= gmic_filter.get_entry_level() ) {
      exclude_lvl = -1;

      bool found = find_match_entry(phf_def_filters, gmic_filter, phf_out);
      if (found) { 
//        phf_args = phf_out.get_filter_arguments();
        phf_arg_list.clear();
        GmicFilter::parse_arguments1(phf_out.get_filter_arguments(), phf_arg_list);
        
        // check all the custom arguments, some are processed here
        for ( int h = 0; h < phf_arg_list.size(); h++ ) {
          GmicArgument& ar = phf_arg_list[h];
          
          if ( ar.get_arg_description() == "phf_process" ) {
            if ( ar.get_val_str() == "untiled" )
              gmic_filter.set_filter_type("gmic_generic_untiled");
            else if ( ar.get_val_str() == "untiled2" )
              gmic_filter.set_filter_type("gmic_generic_untiled2");
            else if ( ar.get_val_str() == "tiled" )
              gmic_filter.set_filter_type("gmic_generic_tiled");
          }
          else if ( ar.get_arg_description() == "phf_exclude" ) {
            if ( ar.get_val_str() == "1" ) {
              exclude_lvl = gmic_filter.get_entry_level();
            }
          }
          else if (ar.get_arg_description().compare(0, 4,"phf_") == 0) {
            // add only custom PhF properties
            prop_value.set_prop_id( gmic_filter.get_phf_properties_count() );
            prop_value.set_prop_type( ar.get_arg_type() );
            prop_value.set_prop_name( ar.get_arg_description() );
            prop_value.set_val_str( ar.get_val_str() );
            prop_value.set_val_int( (int)ar.get_val_default() );
            prop_value.set_val_float( (float)ar.get_val_default() );
            prop_value.set_val_color( ar.get_val_color() );
            prop_value.set_use_alpha( ar.get_use_alpha() );
            gmic_filter.add_phf_property(prop_value);
          }
        }

        // add custom arguments to the current filter
        gmic_filter.set_filter_phf_arguments(phf_out.get_filter_arguments());
      }
      else {
        // check if menu should be excluded
        if ( gmic_filter.get_is_folder() ) {
          for ( int j = 0; j < phf_def_filters.size(); j++ ) {
            if( (phf_def_filters[j].get_filter_name() == gmic_filter.get_filter_name() && phf_def_filters[j].get_filter_command() == "phf_exclude(1)" ) ) {
              exclude_lvl = gmic_filter.get_entry_level();
              break;
            }
          }
        }
      }
      
      // if not excluded add filter to the menu
      if (exclude_lvl == -1) {
        m_menu_entries.push_back(gmic_filter);
      }
    }
  }

}

