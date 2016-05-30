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

#ifndef PF_FILTER_HH
#define PF_FILTER_HH

#include <string>
//#include "../base/operation.hh"
//#include <list>
//#include <vector>
//#include <sigc++/sigc++.h>
#include <gtkmm.h>


namespace PF 
{


	class FilterField
	{
	public:
	  enum FieldType
	  {
      field_type_none = 0,
      field_type_float = 1,
	    field_type_int = 2,
	    field_type_bool = 3,
	    field_type_button = 4,
	    field_type_choise = 5,
	    field_type_text_multi = 6,
	    field_type_text_single = 7,
	    field_type_file = 8,
	    field_type_folder = 9,
	    field_type_color = 10,
	    field_type_const = 11,
	    field_type_note = 12,
	    field_type_link = 13,
	    field_type_separator = 14,
	  };
	  
		int field_id;
		FieldType field_type;
		std::string field_name;
		Glib::ustring field_description;
		std::string str_value;
		double val_default;
		double val_min;
		double val_max;
		double val_step1;
		double val_step2;
		std::list<std::pair<std::string, Glib::ustring>> arg_list; // entries for a combo box
		
	public:
		FilterField();
		virtual ~FilterField() {}
	
		void Init();
	};
	
  class PFFilter
  {
    Glib::ustring filter_name;
    std::string filter_type;
    std::list<FilterField> filter_fields;
    
  public:
    PFFilter();
    virtual ~PFFilter() {}
  
    Glib::ustring& get_name() { return filter_name; }
    void set_name(Glib::ustring& s) { filter_name = s; }
    
    std::string& get_type() { return filter_type; }
    void set_type(std::string s) { filter_type = s; }
    
    std::list<FilterField>& get_fields() { return filter_fields; }
    void set_fields(std::list<FilterField>& fields) { filter_fields = fields; }
    
  };

}

#endif 


