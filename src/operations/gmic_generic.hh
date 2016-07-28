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

#ifndef PF_GMIC_GENERIC1_HH
#define PF_GMIC_GENERIC1_HH

#include <math.h>

#include <assert.h>
#include <iostream>
#include <string>

#include <gtkmm.h>

#include "../base/format_info.hh"
//#include "../base/color.hh"
//#include "../base/processor.hh"
//#include "../base/splinecurve.hh"
//#include "../gui/doublebuffer.hh"

//#include "../base/gmic_filter.hh"
//#include "../../base/gmic_filter.hh"
//#include "../vips/gmic/gmic/src/gmic.h"

namespace PF 
{

// -----------------------------------
// GmicColor
// -----------------------------------

class GmicColor
{
  float r, g, b;

public:
  GmicColor(): r(0), g(0), b(0)
  {
  }
  GmicColor(float r1, float g1, float b1): r(r1), g(g1), b(b1)
  {
  }
  GmicColor(const GmicColor& other): r(other.r), g(other.g), b(other.b)
  {
  }
  virtual ~GmicColor() {}
  
  void swap(GmicColor& first)
  {
    std::swap(r, first.r);
    std::swap(g, first.g);
    std::swap(b, first.b);
  }
  
  GmicColor& operator=(GmicColor other)
  {
    this->swap(other);
    return *this;
  }
  
  float get_r() const { return r; }
  void set_r(float s) { r = s; }
  float get_g() const { return g; }
  void set_g(float s) { g = s; }
  float get_b() const { return b; }
  void set_b(float s) { b = s; }

  void set(float r1, float g1, float b1) { r = r1; g = g1; b = b1; }
  void set(GmicColor& c) { r = c.get_r(); g = c.get_g(); b = c.get_b(); }
    
};

inline
bool operator ==(const GmicColor& l, const GmicColor& r)
{
  if( l.get_r() != r.get_r() ) return false;
  if( l.get_g() != r.get_g() ) return false;
  if( l.get_b() != r.get_b() ) return false;
  return true;
}

inline
bool operator !=(const GmicColor& l, const GmicColor& r)
{
  return( !(l==r) );
}


inline std::istream& operator >>( std::istream& str, GmicColor& gmfilter )
{
  float r, g, b;

  str>>r>>g>>b;
  gmfilter.set_r( r );
  gmfilter.set_g( g );
  gmfilter.set_b( b );
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const GmicColor& gmfilter )
{
  str<<gmfilter.get_r()<<" "<<gmfilter.get_g()<<" "<<gmfilter.get_b()<<" ";
  return str;
}

template<> inline
void set_gobject_property< GmicColor >(gpointer object, const std::string name,
    const GmicColor& value)
{
}

template<> inline
void set_gobject_property< std::vector<GmicColor> >(gpointer object, const std::string name,
    const std::vector<GmicColor>& value)
{
}


// -----------------------------------
// GmicArgument
// -----------------------------------

class GmicArgument
{
public:
  enum GmicArgumentType
  {
    arg_type_none = 0,
    arg_type_float = 1,
    arg_type_int = 2,
    arg_type_bool = 3,
    arg_type_button = 4,
    arg_type_choise = 5,
    arg_type_text_multi = 6,
    arg_type_text_single = 7,
    arg_type_file = 8,
    arg_type_folder = 9,
    arg_type_color = 10,
    arg_type_const = 11,
    arg_type_note = 12,
    arg_type_link = 13,
    arg_type_separator = 14,
  };
  
  int arg_id;
  int arg_type;
  std::string arg_name;
  Glib::ustring arg_description;
  std::string val_str;
  double val_default;
  double val_min;
  double val_max;
  double val_step1;
  double val_step2;
  std::list< std::pair<std::string, Glib::ustring> > arg_list; // entries for a combo box
  
public:
  GmicArgument()
  {
  }
  GmicArgument(const GmicArgument& other):
    arg_id(other.arg_id), arg_type(other.arg_type), arg_name(other.arg_name), 
    arg_description(other.arg_description), val_str(other.val_str), val_default(other.val_default), 
    val_min(other.val_min), val_max(other.val_max),
    val_step1(other.val_step1), val_step2(other.val_step2), 
    arg_list(other.arg_list)
  {
  }
  virtual ~GmicArgument() {  }
  
  friend void swap(GmicArgument& first, GmicArgument& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      swap(first.arg_id, second.arg_id);
      swap(first.arg_type, second.arg_type);
      swap(first.arg_name, second.arg_name);
      
      swap(first.arg_description, second.arg_description);
      swap(first.val_str, second.val_str);
      swap(first.val_default, second.val_default);
      
      swap(first.val_min, second.val_min);
      swap(first.val_max, second.val_max);
      swap(first.val_step1, second.val_step1);
      swap(first.val_step2, second.val_step2);
      
      first.arg_list.swap(second.arg_list);
  }
  
  GmicArgument& operator=(GmicArgument other)
  {
      swap(*this, other);
      return *this;
  }
  
  static const char gmic_menu_dquote;
  static const char gmic_menu_eol;
  
  int get_arg_id() const { return arg_id; }
  void set_arg_id(int s) { arg_id = s; }
  
  int get_arg_type() const { return arg_type; }
  void set_arg_type(int s) { arg_type = s; }
  
  std::string get_arg_name() const { return arg_name; }
  void set_arg_name(std::string s) { arg_name = s; }
  
  Glib::ustring get_arg_description() const { return arg_description; }
  void set_arg_description(Glib::ustring s) { arg_description = s; }
  
  std::string get_val_str() const { return val_str; }
  void set_val_str(std::string s) { val_str = s; }
  
  double get_val_default() const { return val_default; }
  void set_val_default(double s) { val_default = s; }
  
  double get_val_min() const { return val_min; }
  void set_val_min(double s) { val_min = s; }
  
  double get_val_max() const { return val_max; }
  void set_val_max(double s) { val_max = s; }
  
  double get_val_step1() const { return val_step1; }
  void set_val_step1(double s) { val_step1 = s; }
  
  double get_val_step2() const { return val_step2; }
  void set_val_step2(double s) { val_step2 = s; }
  
  std::list< std::pair<std::string, Glib::ustring> >& get_arg_list() { return arg_list; }
  const std::list< std::pair<std::string, Glib::ustring> >& get_arg_list() const { return arg_list; }

  void set_arg_list(std::list< std::pair<std::string, Glib::ustring> >& s) { arg_list = s; }

  void Init();
  
  void get_str_value(std::string& str_value);
  
};

inline
bool operator ==(const GmicArgument& l, const GmicArgument& r)
{
  if( l.get_arg_id() != r.get_arg_id() ) return false;
  if( l.get_arg_type() != r.get_arg_type() ) return false;
  if( l.get_arg_name() != r.get_arg_name() ) return false;
  if( l.get_arg_description() != r.get_arg_description() ) return false;
  if( l.get_val_str() != r.get_val_str() ) return false;
  if( l.get_val_default() != r.get_val_default() ) return false;
  if( l.get_val_min() != r.get_val_min() ) return false;
  if( l.get_val_max() != r.get_val_max() ) return false;
  if( l.get_val_step1() != r.get_val_step1() ) return false;
  if( l.get_val_step2() != r.get_val_step2() ) return false;
  if( l.get_arg_list() != r.get_arg_list() ) return false;
  return true;
      }

inline
bool operator !=(const GmicArgument& l, const GmicArgument& r)
      {
  return( !(l==r) );
      }


inline std::istream& operator >>( std::istream& str, GmicArgument& gmfilter )
{
  int arg_id;
  int arg_type;
  std::string arg_name;
  std::string arg_description;
  std::string val_str;
  double val_default;
  double val_min;
  double val_max;
  double val_step1;
  double val_step2;
  
  std::getline(str, arg_name, PF::GmicArgument::gmic_menu_eol);
  std::getline(str, arg_description, PF::GmicArgument::gmic_menu_eol);
  std::getline(str, val_str, PF::GmicArgument::gmic_menu_eol);

  str>> arg_id
  >> arg_type
//  >> arg_name
//  >> arg_description
//  >> val_str
  >> val_default
  >> val_min
  >> val_max
  >> val_step1
  >> val_step2
  >> gmfilter.get_arg_list();
  
  gmfilter.set_arg_id( arg_id );
  
  gmfilter.set_arg_type( arg_type );
  
  gmfilter.set_arg_name( arg_name );
  
  gmfilter.set_arg_description( arg_description );
  
  gmfilter.set_val_str( val_str );
  
  gmfilter.set_val_default( val_default );
  
  gmfilter.set_val_min( val_min );
  
  gmfilter.set_val_max( val_max );
  
  gmfilter.set_val_step1( val_step1 );
  
  gmfilter.set_val_step2( val_step2 );
  

  return str;
}

inline std::ostream& operator <<( std::ostream& str, const GmicArgument& gmfilter )
{
  
  str<<gmfilter.get_arg_name()<<PF::GmicArgument::gmic_menu_eol
  <<gmfilter.get_arg_description()<<PF::GmicArgument::gmic_menu_eol
  <<gmfilter.get_val_str()<<PF::GmicArgument::gmic_menu_eol
  <<gmfilter.get_arg_id()<<" "
      <<gmfilter.get_arg_type()<<" "
      <<gmfilter.get_val_default()<<" "
      <<gmfilter.get_val_min()<<" "
      <<gmfilter.get_val_max()<<" "
      <<gmfilter.get_val_step1()<<" "
      <<gmfilter.get_val_step2()<<" "
      <<gmfilter.get_arg_list()/*<<" "*/;

  return str;
}

template<> inline
void set_gobject_property< GmicArgument >(gpointer object, const std::string name,
    const GmicArgument& value)
{
}

template<> inline
void set_gobject_property< std::vector<GmicArgument> >(gpointer object, const std::string name,
    const std::vector<GmicArgument>& value)
{
}


// -----------------------------------
// GmicProperty
// -----------------------------------

class GmicProperty
{
public:
  
  int prop_id;
  int prop_type;
  std::string prop_name;
  std::string val_str;
  int val_int;
  float val_float;
  GmicColor val_color;
  
public:
  GmicProperty()
  {
  }
  GmicProperty(const GmicProperty& other):
    prop_id(other.prop_id), prop_type(other.prop_type), prop_name(other.prop_name), 
    val_str(other.val_str), val_int(other.val_int), val_float(other.val_float), 
    val_color(other.val_color)
  {
  }
  virtual ~GmicProperty() {  }
  
  friend void swap(GmicProperty& first, GmicProperty& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      swap(first.prop_id, second.prop_id);
      swap(first.prop_type, second.prop_type);
      swap(first.prop_name, second.prop_name);
      
      swap(first.val_str, second.val_str);
      swap(first.val_int, second.val_int);
      swap(first.val_float, second.val_float);
      first.val_color.swap(second.val_color);
  }
  
  GmicProperty& operator=(GmicProperty other)
  {
      swap(*this, other);
      return *this;
  }
  
  int get_prop_id() const { return prop_id; }
  void set_prop_id(int s) { prop_id = s; }
  
  int get_prop_type() const { return prop_type; }
  void set_prop_type(int s) { prop_type = s; }
  
  std::string get_prop_name() const { return prop_name; }
  void set_prop_name(std::string s) { prop_name = s; }
  
  std::string get_val_str() const { return val_str; }
  void set_val_str(std::string s) { val_str = s; }
  
  int get_val_int() const { return val_int; }
  void set_val_int(int s) { val_int = s; }
  
  float get_val_float() const { return val_float; }
  void set_val_float(float s) { val_float = s; }
  
  GmicColor get_val_color() const { return val_color; }
  void set_val_color(GmicColor& s) { val_color = s; }
  
 
  void get_str_value(std::string& str_value);
  
};

inline
bool operator ==(const GmicProperty& l, const GmicProperty& r)
{
  if( l.get_prop_id() != r.get_prop_id() ) return false;
  if( l.get_prop_type() != r.get_prop_type() ) return false;
  if( l.get_prop_name() != r.get_prop_name() ) return false;
  if( l.get_val_str() != r.get_val_str() ) return false;
  if( l.get_val_int() != r.get_val_int() ) return false;
  if( l.get_val_float() != r.get_val_float() ) return false;
  if( l.get_val_color() != r.get_val_color() ) return false;
  return true;
      }

inline
bool operator !=(const GmicProperty& l, const GmicProperty& r)
      {
  return( !(l==r) );
      }


inline std::istream& operator >>( std::istream& str, GmicProperty& gmfilter )
{
  int prop_id;
  int prop_type;
  std::string /*Glib::ustring*/ prop_name;
  std::string /*Glib::ustring*/ val_str;
  int val_int;
  float val_float;
  GmicColor val_color;
  
  std::getline(str, prop_name, PF::GmicArgument::gmic_menu_eol);
  std::getline(str, val_str, PF::GmicArgument::gmic_menu_eol);

  str>> prop_id
  >> prop_type
//  >> prop_name
//  >> val_str
  >> val_int
  >> val_float
  >> val_color;
  
  gmfilter.set_prop_id( prop_id );
  
  gmfilter.set_prop_type( prop_type );
  
  gmfilter.set_prop_name( prop_name );
  
  gmfilter.set_val_str( val_str );
  
  gmfilter.set_val_int( val_int );
  
  gmfilter.set_val_float( val_float );
  
  gmfilter.set_val_color( val_color );
  
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const GmicProperty& gmfilter )
{
  std::string /*Glib::ustring*/ prop_name = gmfilter.get_prop_name();
  std::string /*Glib::ustring*/ val_str = gmfilter.get_val_str();

  str
//      <<gmfilter.get_prop_name()/*<<" "*/
//      <<gmfilter.get_val_str()/*<<" "*/
      <<prop_name<<PF::GmicArgument::gmic_menu_eol
      <<val_str<<PF::GmicArgument::gmic_menu_eol
      
      <<gmfilter.get_prop_id()<<" "
            <<gmfilter.get_prop_type()<<" "
      <<gmfilter.get_val_int()<<" "
      <<gmfilter.get_val_float()<<" "
      <<gmfilter.get_val_color()/*<<" "*/;

  return str;
}

template<> inline
void set_gobject_property< GmicProperty >(gpointer object, const std::string name,
    const GmicProperty& value)
{
}

template<> inline
void set_gobject_property< std::vector<GmicProperty> >(gpointer object, const std::string name,
    const std::vector<GmicProperty>& value)
{
}



// -----------------------------------
// GmicFilter1
// -----------------------------------

class GmicFilter1
{
  bool is_folder;
  int entry_level;
  
  std::string filter_name;
  std::string filter_type;
  std::string filter_command;
  std::string filter_arguments;
  std::string filter_phf_arguments;
  
  std::list<GmicProperty> m_prop_list;

protected:

public:
  GmicFilter1()
  {
  }
  GmicFilter1(const GmicFilter1& other):
    is_folder(other.is_folder), entry_level(other.entry_level), 
    filter_name(other.filter_name), filter_type(other.filter_type), filter_command(other.filter_command), 
    filter_arguments(other.filter_arguments),filter_phf_arguments(other.filter_phf_arguments), 
    m_prop_list(other.m_prop_list)
  {
  }
  virtual ~GmicFilter1() {  }
  
  friend void swap(GmicFilter1& first, GmicFilter1& second) // nothrow
  {
      // enable ADL (not necessary in our case, but good practice)
      using std::swap;

      // by swapping the members of two classes,
      // the two classes are effectively swapped
      swap(first.is_folder, second.is_folder);
      swap(first.entry_level, second.entry_level);
      swap(first.filter_name, second.filter_name);
      swap(first.filter_type, second.filter_type);
      swap(first.filter_command, second.filter_command);
      swap(first.filter_arguments, second.filter_arguments);
      swap(first.filter_phf_arguments, second.filter_phf_arguments);
      first.m_prop_list.swap(second.m_prop_list);
  }
  
  GmicFilter1& operator=(GmicFilter1 other)
  {
      swap(*this, other);
      return *this;
  }
  
  bool get_is_folder() const { return is_folder; }
  void set_is_folder(bool s) { is_folder = s; }
  
  int get_entry_level() const { return entry_level; }
  void set_entry_level(int s) { entry_level = s; }
  
  std::string get_filter_name() const { return filter_name; }
  void set_filter_name(std::string s) { filter_name = s; }

  std::string get_filter_type() const { return filter_type; }
  void set_filter_type(std::string s) { filter_type = s; }

  std::string get_filter_command() const { return filter_command; }
  void set_filter_command(std::string s) { filter_command = s; }

  std::string get_filter_arguments() const { return filter_arguments; }
  void set_filter_arguments(std::string s) { filter_arguments = s; }

  std::string get_filter_phf_arguments() const { return filter_phf_arguments; }
  void set_filter_phf_arguments(std::string s) { filter_phf_arguments = s; }

  std::list<GmicProperty>& get_prop_list() { return m_prop_list; }
  const std::list<GmicProperty>& get_prop_list() const { return m_prop_list; }

  int get_poperties_count() { return m_prop_list.size(); }
 
  static void parse_arguments(std::string& filter_arguments, std::list<GmicArgument>& arg_list);
  
  void create_properties();
  
};

inline
bool operator ==(const GmicFilter1& l, const GmicFilter1& r)
{
  if( l.get_is_folder() != r.get_is_folder() ) return false;
  if( l.get_entry_level() != r.get_entry_level() ) return false;
  if( l.get_filter_name() != r.get_filter_name() ) return false;
  if( l.get_filter_type() != r.get_filter_type() ) return false;
  if( l.get_filter_command() != r.get_filter_command() ) return false;
  if( l.get_filter_arguments() != r.get_filter_arguments() ) return false;
  if( l.get_filter_phf_arguments() != r.get_filter_phf_arguments() ) return false;
  if( l.get_prop_list() != r.get_prop_list() ) return false;
  return true;
      }

inline
bool operator !=(const GmicFilter1& l, const GmicFilter1& r)
      {
  return( !(l==r) );
      }


inline std::istream& operator >>( std::istream& str, GmicFilter1& gmfilter )
{
  std::cout<<"inline std::istream& operator >>( std::istream& str, GmicFilter1& gmfilter ) "<<std::endl;
  
  std::string /*Glib::ustring*/ name, command, arguments, phf_arguments, ftype;
  bool is_folder;
  int entry_level;

  std::getline(str, name, PF::GmicArgument::gmic_menu_eol);
  std::getline(str, ftype, PF::GmicArgument::gmic_menu_eol);
  std::getline(str, command, PF::GmicArgument::gmic_menu_eol);
  std::getline(str, arguments, PF::GmicArgument::gmic_menu_eol);
  std::getline(str, phf_arguments, PF::GmicArgument::gmic_menu_eol);
  
  str>>is_folder
  >>entry_level
/*  >>name
  >>ftype
  >>command
  >>arguments
  >>phf_arguments*/
  >>gmfilter.get_prop_list();

  gmfilter.set_is_folder(is_folder);
  gmfilter.set_entry_level(entry_level);
  
  gmfilter.set_filter_name(name);
  gmfilter.set_filter_type(ftype);
  gmfilter.set_filter_command(command);
  gmfilter.set_filter_arguments(arguments);
  gmfilter.set_filter_phf_arguments(phf_arguments);
  
  return str;
}

inline std::ostream& operator <<( std::ostream& str, const GmicFilter1& gmfilter )
{
  std::cout<<"inline std::ostream& operator >>( std::ostream& str, GmicFilter1& gmfilter ) "<<std::endl;

  
  std::string /*Glib::ustring*/ name, command, arguments, phf_arguments, ftype;
  
  name = gmfilter.get_filter_name();
  command = gmfilter.get_filter_command();
  arguments = gmfilter.get_filter_arguments();
  phf_arguments = gmfilter.get_filter_phf_arguments();
  ftype = gmfilter.get_filter_type();
  
//  std::quoted(in)
  str
  //      <<gmfilter.get_filter_name()/*<<" "*/
//      <<gmfilter.get_filter_type()/*<<" "*/
//      <<gmfilter.get_filter_command()/*<<" "*/
//      <<gmfilter.get_filter_arguments()/*<<" "*/
//      <<gmfilter.get_filter_phf_arguments()/*<<" "*/
      <<name<<PF::GmicArgument::gmic_menu_eol
      <<ftype<<PF::GmicArgument::gmic_menu_eol
      <<command<<PF::GmicArgument::gmic_menu_eol
      <<arguments<<PF::GmicArgument::gmic_menu_eol
      <<phf_arguments<<PF::GmicArgument::gmic_menu_eol
      <<gmfilter.get_is_folder()<<" "
            <<gmfilter.get_entry_level()<<" "
      <<gmfilter.get_prop_list()/*<<" "*/;
  
  return str;
}

template<> inline
void set_gobject_property< GmicFilter1 >(gpointer object, const std::string name,
    const GmicFilter1& value)
{
}

template<> inline
void set_gobject_property< std::vector<GmicFilter1> >(gpointer object, const std::string name,
    const std::vector<GmicFilter1>& value)
{
}



// -----------------------------------
// GmicMenu
// -----------------------------------

class GmicMenu
{
  std::list<GmicFilter1> m_menu_entries;

  static void get_def_filenames(char const* def_filename, std::string& out_def_filename);
  static void parse_def_file(std::string& file_name, std::list<GmicFilter1>& def_filters);
  
  static bool find_match_entry(std::list<GmicFilter1>& def_filters, GmicFilter1& in, GmicFilter1& out);

  static void add_menu_entry(std::list<GmicFilter1>& menu_entries, bool is_folder, int entry_level, 
                        std::string& entry_type, Glib::ustring& entry_name, 
                        std::string& entry_command, std::string& entry_arguments);
                        
public:

  GmicMenu() { }
  virtual ~GmicMenu() {}

  void load_filters();
  std::list<GmicFilter1>& get_menu_entries() { return m_menu_entries; }

  static std::string unquote_gmic(std::string& str_val);
  static std::string unquote_gmic(Glib::ustring& str_val);
  static std::string quote_gmic(std::string& str_val);


};



}


#endif 


