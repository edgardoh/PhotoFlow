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

#include <iostream>

#include "imagebutton.hh"


PF::ImageButton::ImageButton(Glib::ustring i, Glib::ustring pi)
{
  img.set( i );
  pressed_img.set( pi );

  img_align.set(0.5,0.5,0,0);
  pressed_img_align.set(0.5,0.5,0,0);

  img_align.add(img);
  pressed_img_align.add(pressed_img);

  button_box.pack_start( img_align, Gtk::PACK_EXPAND_WIDGET );
  //button_box.pack_start( pressed_img, Gtk::PACK_SHRINK );
  event_box.add( button_box );
  event_box.add_events( Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK );

  pack_start( event_box, Gtk::PACK_EXPAND_WIDGET );

  show_all_children();

  //pressed_img.hide();
}


void PF::ImageButton::on_realize()
{
  //img.show();
  //pressed_img.hide();
  Gtk::VBox::on_realize();
}


void PF::ImageButton::on_map()
{
  //img.show();
  //pressed_img.hide();
  Gtk::VBox::on_map();
}


bool PF::ImageButton::on_button_press_event( GdkEventButton* button )
{
  if( button->type != GDK_BUTTON_PRESS || button->button != 1 ) return false;
#ifndef NDEBUG
  std::cout<<"PF::ToggleImageButton::on_button_press_event(): button "<<button->button<<" pressed."<<std::endl;
#endif
  button_box.remove( img_align );
  button_box.pack_start( pressed_img_align, Gtk::PACK_SHRINK );
  show_all_children();
  return true;
}


bool PF::ImageButton::on_button_release_event( GdkEventButton* button )
{
#ifndef NDEBUG
  std::cout<<"PF::ToggleImageButton::on_button_release_event(): button "<<button->button<<" released."<<std::endl;
#endif
  if( button->button != 1 ) return false;

  signal_clicked.emit();

  button_box.remove( pressed_img_align );
  button_box.pack_start( img_align, Gtk::PACK_SHRINK );
  show_all_children();
  return true;
}



PF::ToggleImageButton::ToggleImageButton(Glib::ustring active, Glib::ustring inactive,
    bool t, bool a):
active(true),
do_toggle(t)
{
  active_img.set( active );
  inactive_img.set( inactive );
  button_box.pack_start( active_img, Gtk::PACK_SHRINK );
  event_box.add( button_box );
  event_box.add_events( Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK );

  pack_start( event_box, Gtk::PACK_SHRINK );

  //event_box.signal_button_release_event().connect(sigc::mem_fun(*this,
  //    &ToggleImageButton::on_button_release_event) );

  set_active(a);
  show_all_children();
}



void PF::ToggleImageButton::set_active( bool newval )
{
  //std::cout<<"ToggleImageButton::set_active("<<newval<<") called (active="<<active<<")"<<std::endl;
  if( active == newval ) return;
  if( inactive_img.get_parent() == &button_box )
    button_box.remove( inactive_img );
  if( active_img.get_parent() == &button_box )
    button_box.remove( active_img );
  if( newval ) {
    button_box.pack_start( active_img, Gtk::PACK_SHRINK );
    active_img.show();
  } else {
    button_box.pack_start( inactive_img, Gtk::PACK_SHRINK );
    inactive_img.show();
  }
  active = newval;
}



void PF::ToggleImageButton::toggle()
{
  bool new_state = !is_active();
  //std::cout<<"ToggleImageButton::toggle(): is_active()="<<is_active()<<"  new_state="<<new_state<<std::endl;
  set_active( new_state );
  if( new_state ) {
    signal_activated.emit();
  } else {
    signal_deactivated.emit();
  }
}



bool PF::ToggleImageButton::on_button_press_event( GdkEventButton* button )
{
#ifndef NDEBUG
  std::cout<<"PF::ToggleImageButton::on_button_press_event(): button "<<button->button<<" pressed."<<std::endl;
#endif
  return true;
}


bool PF::ToggleImageButton::on_button_release_event( GdkEventButton* button )
{
#ifndef NDEBUG
  std::cout<<"PF::ToggleImageButton::on_button_release_event(): button "<<button->button<<" released."<<std::endl;
#endif
  if( button->button != 1 ) return false;

  if( do_toggle ) {
    toggle();
  } else {
    if( active ) signal_clicked.emit();
  }
  return true;
}

PF::RadioImageButton::RadioImageButton()
{
  btn_active = -1;
}

int PF::RadioImageButton::add_button( Glib::ustring active, Glib::ustring inactive, bool is_active )
{
  Gtk::EventBox* evt_box= new Gtk::EventBox();
  Gtk::VBox* btn_box = new Gtk::VBox();
  Gtk::Image* btn_active_img = new Gtk::Image();
  Gtk::Image* btn_inactive_img = new Gtk::Image();

  btn_active_img->set( active );
  btn_inactive_img->set( inactive );

  btn_box->pack_start( *btn_inactive_img, Gtk::PACK_SHRINK );
  btn_inactive_img->show();

  evt_box->add_events( Gdk::BUTTON_PRESS_MASK );
  evt_box->add( *btn_box );
  pack_start( *evt_box, Gtk::PACK_SHRINK );

  event_box.push_back(evt_box);
  button_box.push_back(btn_box);
  active_img.push_back(btn_active_img);
  inactive_img.push_back(btn_inactive_img);

  if (is_active || button_box.size()==1)
    set_btn_active(button_box.size()-1);

  return (button_box.size()-1);
}

void PF::RadioImageButton::set_btn_active(int n)
{
  if (btn_active == n) return;
  
  if (btn_active >= 0) {
    if( active_img[btn_active]->get_parent() == button_box[btn_active] )
      button_box[btn_active]->remove( *active_img[btn_active] );
    
    button_box[btn_active]->pack_start( *inactive_img[btn_active], Gtk::PACK_SHRINK );
    inactive_img[btn_active]->show();
  }
  
  btn_active = n;
  
  if (btn_active >= 0) {
        if( inactive_img[btn_active]->get_parent() == button_box[btn_active] )
          button_box[btn_active]->remove( *inactive_img[btn_active] );
    button_box[btn_active]->pack_start( *active_img[btn_active], Gtk::PACK_SHRINK );
    active_img[btn_active]->show();
  }

}

bool PF::RadioImageButton::on_button_press_event( GdkEventButton* button )
{
  int pressed = -1;
  
  for (int i=0; i < button_box.size(); i++) {
    if ( button->window == gtk_widget_get_parent_window( ((GtkWidget*)(button_box[i]->gobj())) ) ) {
      pressed = i;
//      std::cout<<"PF::RadioImageButton::on_button_press_event(): button_box[i] "<<i<<std::endl;
    }
  }
      
  if (pressed >= 0 && pressed < button_box.size()) {
    set_btn_active(pressed);
    
    signal_clicked.emit();
  }
  
  return true;
}

