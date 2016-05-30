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

#include "pf_filter.hh"

PF::FilterField::FilterField()
{
	Init();
}

void PF::FilterField::Init()
{
  field_id = 0;
  field_type = field_type_none;
  field_name = "";
  field_description;
  str_value = "";
  val_default = 0;
  val_min = 0;
  val_max = 0;
  val_step1 = 0;
  val_step2 = 0;
  
  arg_list.clear();
}

PF::PFFilter::PFFilter()
{
  
}

