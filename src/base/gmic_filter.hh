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

#ifndef GMIC_FILTER_HH
#define GMIC_FILTER_HH

#include <string>
#include <list>
#include <vector>
#include <sigc++/sigc++.h>
#include <gtkmm.h>
#include "pf_filter.hh"
//#include "operation.hh"
//#include "../gui/operationstree.hh"


namespace PF 
{

/*	typedef enum _gmic_arg_type_t
	{
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
	} _gmic_arg_type_t;

	class GMicArgument
	{
	public:
		int arg_id;
		int arg_type;
		std::string arg_name;
		Glib::ustring arg_description;
		std::string arg_value;
		double arg_default;
		double arg_min;
		double arg_max;
		double arg_step1;
		double arg_step2;
		std::list<std::pair<std::string, Glib::ustring>> arg_list;
		
	public:
		GMicArgument();
	
		void Init();
	};
	*/
/*	class GMicCommand
	{
	public:
		GMicCommand() { }
	
		std::string build_command(std::string& filter_command, std::list<FilterField>& field_list, OpParBase *par);

	};
*/
  class GmicFilter: public PFFilter
  {
	  std::string filter_command;
	  
    void parse_arguments(std::string& filter_arguments, std::list<FilterField>& arg_list);

  public:
	  GmicFilter();
//	  virtual ~GmicFilter() {}
	  
    void set_command(std::string s) { filter_command = s; }
    std::string get_command() { return filter_command; }
  
    void parse_arguments(std::string& filter_arguments);
//    std::string build_command(OpParBase *par);

  };
  
	class GMicDefFilter
	{
		bool is_folder;
		int entry_level;
		GmicFilter filter;
//		std::string entry_type;
//		Glib::ustring entry_name;
//		std::string entry_command;
//    std::string entry_arguments;
//    std::string entry_phf_arguments;
//    bool exclude;
    
	public:
		GMicDefFilter();
		virtual ~GMicDefFilter() {}
//		GMicDefFilter(bool is_folder, int entry_level, std::string& e_type, Glib::ustring& e_name, std::string& e_command, std::string& e_arguments);
	
		bool get_is_folder() { return is_folder; }
		void set_is_folder(bool s) { is_folder = s; }
    int get_entry_level() { return entry_level; }
    void set_entry_level(int s) { entry_level = s; }
    GmicFilter& get_filter() { return filter; }
    void set_filter(GmicFilter& s) { filter = s; }
    
//		std::string& get_entry_type() { return entry_type; }
//		void set_entry_type(std::string s) { entry_type = s; }
/*		Glib::ustring& get_entry_name() { return entry_name; }
		void set_entry_name(Glib::ustring& s) { entry_name = s; }
		std::string& get_entry_command() { return entry_command; }
		void set_entry_command(std::string& s) { entry_command = s; }
    std::string& get_entry_arguments() { return entry_arguments; }
    void set_entry_arguments(std::string& s) { entry_arguments = s; }
    std::string& get_entry_phf_arguments() { return entry_phf_arguments; }
    void set_entry_phf_arguments(std::string& s) { entry_phf_arguments = s; }
    
    bool get_exclude() { return exclude; }
    void set_exclude(bool s) { exclude = s; }
*/
	};

	class GMicFilters
	{
	  std::list<GMicDefFilter> def_filters;
/*	  std::string gmic_def_filename;
	  std::string phf_def_filename;
	  
		std::list<GMicDefFilter> gmic_entries;
		std::list<GMicDefFilter> phf_entries;
		*/
    void get_def_filenames(char const* def_filename, std::string& out_def_filename);
		void parse_def_file(std::string& file_name, std::list<GMicDefFilter>& def_filters);
		
    bool find_match_entry(std::list<GMicDefFilter>& def_filters, GMicDefFilter& in, GMicDefFilter& out);

		void add_menu_entry(std::list<GMicDefFilter>& menu_entries, bool is_folder, int entry_level, 
                          std::string& entry_type, Glib::ustring& entry_name, 
                          std::string& entry_command, std::string& entry_arguments);
                          
	public:
/*    enum ColEntries {
      col_name = 0,
      col_command = 1,
      col_arguments_gmic = 2, 
      col_arguments_phf = 3
    };
      */
	  GMicFilters();
	  virtual ~GMicFilters() {}
	
	  void load_filters();
	  std::list<GMicDefFilter>& get_def_filters() { return def_filters; }

	};

}

#endif 


