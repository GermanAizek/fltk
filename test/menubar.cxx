//
// Menubar test program for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2023 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <FL/Fl.H>
#if defined(__APPLE__)  && !(defined(FLTK_USE_X11) || defined(FLTK_USE_WAYLAND))
#  define HAS_MAC_APP_MENU 1
#endif
#ifdef HAS_MAC_APP_MENU
#  include <FL/platform.H> // for Fl_Mac_App_Menu
#endif
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scheme_Choice.H>
#include <FL/Fl_Value_Slider.H>
#include <stdio.h>
#include <stdlib.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Terminal.H>
#include <FL/fl_ask.H>
#include <FL/fl_string_functions.h>

#define TERMINAL_HEIGHT 120

// Set the macro below to 1 to test shortcuts usually used for screen scaling.
// This should normally be set to 0, enable only for testing!
// Note: screen scaling does not work with ctrl/+/-/0 if enabled (1)!
#define OVERRIDE_SCALING_SHORTCUTS 0

// Globals
Fl_Terminal *G_tty = 0;

void window_cb(Fl_Widget* w, void*) {
  puts("window callback called");       // end of program, so stdout instead of G_tty
  ((Fl_Double_Window *)w)->hide();
}

void test_cb(Fl_Widget* w, void*) {
  Fl_Menu_* mw = (Fl_Menu_*)w;
  const Fl_Menu_Item* m = mw->mvalue();
  if (!m)
    G_tty->printf("NULL\n");
  else if (m->shortcut())
    G_tty->printf("%s - %s\n", m->label(), fl_shortcut_label(m->shortcut()));
  else
    G_tty->printf("%s\n", m->label());
}

void quit_cb(Fl_Widget*, void*) {
  switch (Fl::callback_reason()) {
    case FL_REASON_SELECTED:
      exit(0);
    case FL_REASON_GOT_FOCUS:
      G_tty->printf("Selecting this menu item will quit this application!\n");
      break;
    case FL_REASON_LOST_FOCUS:
      G_tty->printf("Risk of quitting averted.\n");
      break;
    default: break;
  }
}

Fl_Menu_Item hugemenu[100];

Fl_Menu_Item menutable[] = {
  {.text = "foo", .shortcut_ = 0, .flags = FL_MENU_INACTIVE, .callback_ = 0, .user_data_ = 0},
  {.text = "&File", .shortcut_ = 0, .flags = FL_SUBMENU, .callback_ = 0, .user_data_ = 0},
    {.text = "&Open",
     .shortcut_ = FL_ALT + 'o', .flags = FL_MENU_INACTIVE, .callback_ = 0, .user_data_ = 0},
    {.text = "&Close",  .shortcut_ = 0,      .callback_ = 0},
    {.text = "&Quit",
     .shortcut_ = FL_ALT + 'q', .flags = FL_MENU_DIVIDER | FL_MENU_CHATTY, .callback_ = quit_cb, .user_data_ = 0},

#if (OVERRIDE_SCALING_SHORTCUTS)
    {.text = "CTRL/0", .shortcut_ = FL_COMMAND + '0', .callback_ = 0},
    {.text = "CTRL/-", .shortcut_ = FL_COMMAND + '-', .callback_ = 0},
    {.text = "CTRL/+", .shortcut_ = FL_COMMAND + '+', .callback_ = 0},
    {.text = "CTRL/=", .shortcut_ = FL_COMMAND + '=', .callback_ = 0},
#endif

    {.text = "shortcut", .shortcut_ = 'a'},
    {.text = "shortcut", .shortcut_ = FL_SHIFT + 'a'},
    {.text = "shortcut", .shortcut_ = FL_CTRL + 'a'},
    {.text = "shortcut", .shortcut_ = FL_CTRL + FL_SHIFT + 'a'},
    {.text = "shortcut", .shortcut_ = FL_ALT + 'a'},
    {.text = "shortcut", .shortcut_ = FL_ALT + FL_SHIFT + 'a'},
    {.text = "shortcut", .shortcut_ = FL_ALT + FL_CTRL + 'a'},
    {.text = "shortcut",
     .shortcut_ = FL_ALT + FL_SHIFT + FL_CTRL + 'a', .flags = FL_MENU_DIVIDER,
     .callback_ = 0, .user_data_ = 0},
    {.text = "shortcut", .shortcut_ = '\r' /*FL_Enter*/},
    {.text = "shortcut",
     .shortcut_ = FL_CTRL + FL_Enter, .flags = FL_MENU_DIVIDER,
     .callback_ = 0, .user_data_ = 0},
    {.text = "shortcut", .shortcut_ = FL_F + 1},
    {.text = "shortcut", .shortcut_ = FL_SHIFT + FL_F + 1},
    {.text = "shortcut", .shortcut_ = FL_CTRL + FL_F + 1},
    {.text = "shortcut", .shortcut_ = FL_SHIFT + FL_CTRL + FL_F + 1},
    {.text = "shortcut", .shortcut_ = FL_ALT + FL_F + 1},
    {.text = "shortcut", .shortcut_ = FL_ALT + FL_SHIFT + FL_F + 1},
    {.text = "shortcut", .shortcut_ = FL_ALT + FL_CTRL + FL_F + 1},
    {.text = "shortcut",
     .shortcut_ = FL_ALT + FL_SHIFT + FL_CTRL + FL_F + 1, .flags = FL_MENU_DIVIDER,
     .callback_ = 0, .user_data_ = 0},
    {.text = "&Submenus",
     .shortcut_ = FL_ALT + 'S',   .flags = FL_SUBMENU, .callback_ = 0, .user_data_ = (void*)"Submenu1"},
      {.text = "A very long menu item"},
      {.text = "&submenu",
     .shortcut_ = FL_CTRL + 'S',  .flags = FL_SUBMENU, .callback_ = 0, .user_data_ = (void*)"submenu2"},
        {.text = "item 1"},
        {.text = "item 2"},
        {.text = "item 3"},
        {.text = "item 4"},
        {.text = 0},
      {.text = "after submenu"},
      {.text = 0},
    {.text = 0},
  {.text = "&Edit", .shortcut_ = FL_F + 2, .flags = FL_SUBMENU, .callback_ = 0, .user_data_ = 0},
    {.text = "Undo", .shortcut_ = FL_ALT + 'z',  .callback_ = 0},
    {.text = "Redo",
     .shortcut_ = FL_ALT + 'r',  .flags = FL_MENU_DIVIDER, .callback_ = 0, .user_data_ = 0},
    {.text = "Cut", .shortcut_ = FL_ALT + 'x',  .callback_ = 0},
    {.text = "Copy", .shortcut_ = FL_ALT + 'c',  .callback_ = 0},
    {.text = "Paste", .shortcut_ = FL_ALT + 'v',  .callback_ = 0},
    {.text = "Inactive",
     .shortcut_ = FL_ALT + 'd',  .flags = FL_MENU_INACTIVE, .callback_ = 0, .user_data_ = 0},
    {.text = "Clear",       .shortcut_ = 0,           .flags = FL_MENU_DIVIDER, .callback_ = 0, .user_data_ = 0},
    {.text = "Invisible",
     .shortcut_ = FL_ALT + 'e',  .flags = FL_MENU_INVISIBLE, .callback_ = 0, .user_data_ = 0},
    {.text = "Preferences", .shortcut_ = 0,           .callback_ = 0},
    {.text = "Size",        .shortcut_ = 0,           .callback_ = 0},
    {.text = 0},
  {.text = "&Checkbox",
     .shortcut_ = FL_F + 3,
     .flags = FL_SUBMENU,
     .callback_ = 0,
     .user_data_ = 0},
    {.text = "  Greek:  ", .shortcut_ = 0, .flags = FL_MENU_HEADLINE, .callback_ = 0, .user_data_ = nullptr, .labeltype_ = 0, .labelfont_ = FL_BOLD },
    {.text = "&Alpha",
     .shortcut_ = FL_F + 2, .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)1},
    {.text = "&Beta",   .shortcut_ = 0,      .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)2},
    {.text = "&Gamma",  .shortcut_ = 0,      .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)3},
    {.text = "&Delta",  .shortcut_ = 0,      .flags = FL_MENU_TOGGLE | FL_MENU_VALUE, .callback_ = 0, .user_data_ = (void*)4},
    {.text = "&Epsilon",
     .shortcut_ = 0,      .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)5},
    {.text = "&Pi",     .shortcut_ = 0,      .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)6},
    {.text = "&Mu",     .shortcut_ = 0,      .flags = FL_MENU_TOGGLE | FL_MENU_DIVIDER, .callback_ = 0, .user_data_ = (void*)7},
    {.text = "  Colors:  ", .shortcut_ = 0, .flags = FL_MENU_HEADLINE, .callback_ = 0, .user_data_ = nullptr, .labeltype_ = 0, .labelfont_ = FL_BOLD },
    {.text = "Red",     .shortcut_ = 0,      .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)1, .labeltype_ = 0, .labelfont_ = 0, .labelsize_ = 0, .labelcolor_ = 1},
    {.text = "Black",   .shortcut_ = 0,      .flags = FL_MENU_TOGGLE | FL_MENU_DIVIDER, .callback_ = 0, .user_data_ = (void*)1},
    {.text = "  Digits:  ", .shortcut_ = 0, .flags = FL_MENU_HEADLINE, .callback_ = 0, .user_data_ = nullptr, .labeltype_ = 0, .labelfont_ = FL_BOLD },
    {.text = "00",      .shortcut_ = 0,      .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)1},
    {.text = "000",     .shortcut_ = 0,      .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = (void*)1},
    {.text = 0},
  {.text = "&Radio", .shortcut_ = 0, .flags = FL_SUBMENU, .callback_ = 0, .user_data_ = 0},
    {.text = "&Alpha",  .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)1},
    {.text = "&Beta",   .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)2},
    {.text = "&Gamma",  .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)3},
    {.text = "&Delta",  .shortcut_ = 0,      .flags = FL_MENU_RADIO | FL_MENU_VALUE, .callback_ = 0, .user_data_ = (void*)4},
    {.text = "&Epsilon",
     .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)5},
    {.text = "&Pi",     .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)6},
    {.text = "&Mu",     .shortcut_ = 0,      .flags = FL_MENU_RADIO | FL_MENU_DIVIDER, .callback_ = 0, .user_data_ = (void*)7},
    {.text = "Red",     .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)1},
    {.text = "Black",   .shortcut_ = 0,      .flags = FL_MENU_RADIO | FL_MENU_DIVIDER, .callback_ = 0, .user_data_ = (void*)1},
    {.text = "00",      .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)1},
    {.text = "000",     .shortcut_ = 0,      .flags = FL_MENU_RADIO, .callback_ = 0, .user_data_ = (void*)1},
    {.text = 0},
  {.text = "&Font",
     .shortcut_ = 0,
     .flags = FL_SUBMENU /*, 0, FL_BOLD, 20*/,
     .callback_ = 0,
     .user_data_ = 0 /*, 0, FL_BOLD, 20*/},
    {.text = "Normal",  .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = 0, .labelfont_ = 0, .labelsize_ = 14},
    {.text = "Bold",    .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = 0, .labelfont_ = FL_BOLD, .labelsize_ = 14},
    {.text = "Italic",  .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = 0, .labelfont_ = FL_ITALIC, .labelsize_ = 14},
    {.text = "BoldItalic",
     .shortcut_ = 0,
     .flags = 0,
     .callback_ = 0, .user_data_ = 0, .labeltype_ = 0, .labelfont_ = FL_BOLD + FL_ITALIC, .labelsize_ = 14},
    {.text = "Small",   .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = 0, .labelfont_ = FL_BOLD + FL_ITALIC, .labelsize_ = 10},
    {.text = "Emboss",  .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = (uchar)FL_EMBOSSED_LABEL},
    {.text = "Engrave", .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = (uchar)FL_ENGRAVED_LABEL},
    {.text = "Shadow",  .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = (uchar)FL_SHADOW_LABEL},
    {.text = "@->",     .shortcut_ = 0, .flags = 0, .callback_ = 0, .user_data_ = 0, .labeltype_ = (uchar)FL_SYMBOL_LABEL},
    {.text = 0},
  {.text = "&International",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = 0,
     .user_data_ = 0},
    {.text = "Sharp Ess", .shortcut_ = 0x0000df},
    {.text = "A Umlaut", .shortcut_ = 0x0000c4},
    {.text = "a Umlaut", .shortcut_ = 0x0000e4},
    {.text = "Euro currency", .shortcut_ = FL_COMMAND + 0x0020ac},
    {.text = "the &\xc3\xbc Umlaut"},  // &uuml;
    {.text = "the capital &\xc3\x9c"}, // &Uuml;
    {.text = "convert \xc2\xa5 to &\xc2\xa3"}, // Yen to GBP
    {.text = "convert \xc2\xa5 to &\xe2\x82\xac"}, // Yen to Euro
    {.text = "Hangul character Sios &\xe3\x85\x85"},
    {.text = "Hangul character Cieuc", .shortcut_ = 0x003148},
    {.text = 0},
  {.text = "E&mpty", .shortcut_ = 0, .flags = FL_SUBMENU, .callback_ = 0, .user_data_ = 0},
    {.text = 0},
  {.text = "&Inactive", .shortcut_ = 0,      .flags = FL_MENU_INACTIVE | FL_SUBMENU, .callback_ = 0, .user_data_ = 0},
    {.text = "A very long menu item"},
    {.text = "A very long menu item"},
    {.text = 0},
  {.text = "Invisible",
     .shortcut_ = 0,       .flags = FL_MENU_INVISIBLE | FL_SUBMENU, .callback_ = 0, .user_data_ = 0},
    {.text = "A very long menu item"},
    {.text = "A very long menu item"},
    {.text = 0},
  {.text = "&Huge", .shortcut_ = 0, .flags = FL_SUBMENU_POINTER, .callback_ = 0, .user_data_ = (void*)hugemenu},
  {.text = "button",
     .shortcut_ = FL_F + 4, .flags = FL_MENU_TOGGLE, .callback_ = 0, .user_data_ = 0},
  {.text = 0}
};

Fl_Menu_Item pulldown[] = {
  {.text = "Red", .shortcut_ = FL_ALT + 'r'},
  {.text = "Green", .shortcut_ = FL_ALT + 'g'},
  {.text = "Blue", .shortcut_ = FL_ALT + 'b'},
  {.text = "Strange",
                            .shortcut_ = FL_ALT + 's', .flags = FL_MENU_INACTIVE, .callback_ = 0, .user_data_ = 0},
  {.text = "&Charm", .shortcut_ = FL_ALT + 'c'},
  {.text = "Truth", .shortcut_ = FL_ALT + 't'},
  {.text = "Beauty", .shortcut_ = FL_ALT + 'b'},
  {.text = 0}
};

#ifdef HAS_MAC_APP_MENU
Fl_Menu_Item menu_location[] = {
  {.text = "Fl_Menu_Bar", .shortcut_ = 0, .flags = FL_MENU_VALUE, .callback_ = 0, .user_data_ = 0},
  {.text = "Fl_Sys_Menu_Bar"},
  {.text = 0}
};

Fl_Sys_Menu_Bar* smenubar;

void menu_location_cb(Fl_Widget* w, void* data)
{
  Fl_Menu_Bar *menubar = (Fl_Menu_Bar*)data;
  if (((Fl_Choice*)w)->value() == 1) { // switch to system menu bar
    menubar->hide();
    const Fl_Menu_Item *menu = menubar->menu();
    smenubar = new Fl_Sys_Menu_Bar(0,0,0,30);
    smenubar->menu(menu);
    smenubar->callback(test_cb);
  } else { // switch to window menu bar
    menubar->copy(smenubar->menu());
    delete smenubar;
    menubar->show();
  }
}
#endif // HAS_MAC_APP_MENU

void menu_linespacing_cb(Fl_Widget* w, void*) {
  Fl_Value_Slider *fvs = (Fl_Value_Slider*)w;
  int val = (int)fvs->value();
  Fl::menu_linespacing(val);    // takes effect when someone opens a new menu..
}

#define WIDTH 700

Fl_Menu_* menus[4];

void about_cb(Fl_Widget*, void*) {
  fl_message("The menubar test app.");
}

class Dynamic_Choice: public Fl_Choice {
public:
  Dynamic_Choice(int x, int y, int w, int h, const char *label=nullptr)
  : Fl_Choice(x, y, w, h, label) { }
  int handle(int event) override {
    static int flip_flop = 0;
    if (event == FL_BEFORE_MENU) {
      // The following line is legal because we used `copy()` to create a
      // writable copy of the menu array when creating this Choice.
      Fl_Menu_Item *mi = const_cast<Fl_Menu_Item*>(menu());
      if (flip_flop == 1) {
        mi[7].flags |= FL_MENU_INACTIVE;
        mi[8].flags &= ~FL_MENU_INACTIVE;
        flip_flop = 0;
      } else {
        mi[7].flags &= ~FL_MENU_INACTIVE;
        mi[8].flags |= FL_MENU_INACTIVE;
        flip_flop = 1;
      }
    }
    return Fl_Choice::handle(event);
  }
};

int main(int argc, char **argv) {
  for (int i=0; i<99; i++) {
    char buf[100];
    snprintf(buf, 100,"item %d",i);
    hugemenu[i].text = fl_strdup(buf);
  }
  Fl_Double_Window window(WIDTH,400+TERMINAL_HEIGHT);

  Fl_Scheme_Choice scheme_choice(300, 50, 100, 25, "&scheme");

  G_tty = new Fl_Terminal(0,400,WIDTH,TERMINAL_HEIGHT);

  window.callback(window_cb);
  Fl_Menu_Bar menubar(0,0,WIDTH,30); menubar.menu(menutable);
  menubar.callback(test_cb);
  menus[0] = &menubar;
  Fl_Menu_Button mb1(100,100,120,25,"&menubutton"); mb1.menu(pulldown);
  mb1.tooltip("this is a menu button");
  mb1.callback(test_cb);
  menus[1] = &mb1;
  Dynamic_Choice ch(300,100,80,25,"&choice:");
  ch.copy(pulldown);
  ch.add("Flip");
  ch.add("Flop");
  ch.tooltip("this is a choice menu");
  ch.callback(test_cb);
  menus[2] = &ch;
  Fl_Menu_Button mb(0,0,WIDTH,400,"&popup");
  mb.type(Fl_Menu_Button::POPUP3);
  mb.menu(menutable);
  mb.remove(1); // delete the "File" submenu
  mb.callback(test_cb);
  menus[3] = &mb;
  Fl_Box b(200,200,200,100,"Press right button\nfor a pop-up menu");
  window.resizable(&mb);
  window.size_range(300,400,0,400+TERMINAL_HEIGHT);
#ifdef HAS_MAC_APP_MENU
  Fl_Choice ch2(500,100,150,25,"Use:");
  ch2.menu(menu_location);
  ch2.callback(menu_location_cb, &menubar);
  ch2.value(1);
  menu_location_cb(&ch2, &menubar);
#endif

  Fl_Value_Slider menu_linespacing_slider(500,150,150,20,"Fl::menu_linespacing()");
  menu_linespacing_slider.tooltip("Changes the line spacing between all menu items");
  menu_linespacing_slider.type(1);
  //menu_linespacing_slider.labelsize(14);
  menu_linespacing_slider.value(Fl::menu_linespacing());
  menu_linespacing_slider.color((Fl_Color)46);
  menu_linespacing_slider.selection_color((Fl_Color)1);
  //menu_linespacing_slider.textsize(10);
  menu_linespacing_slider.align(Fl_Align(FL_ALIGN_LEFT));
  menu_linespacing_slider.range(0.1, 50.0);
  menu_linespacing_slider.step(1.0);
  menu_linespacing_slider.callback(menu_linespacing_cb);

  window.end();

  Fl_Sys_Menu_Bar::about(about_cb, NULL);
#ifdef HAS_MAC_APP_MENU
  Fl_Menu_Item custom[] = {
    {.text = "Preferences…",  .shortcut_ = 0, .flags = FL_MENU_DIVIDER, .callback_ = test_cb, .user_data_ = NULL},
    {.text = "Radio1",        .shortcut_ = 0, .flags = FL_MENU_RADIO | FL_MENU_VALUE, .callback_ = test_cb, .user_data_ = NULL},
    {.text = "Radio2",        .shortcut_ = 0, .flags = FL_MENU_RADIO | FL_MENU_DIVIDER, .callback_ = test_cb, .user_data_ = NULL},
    {.text = 0}
  };
  Fl_Mac_App_Menu::custom_application_menu_items(custom);
  //Fl_Sys_Menu_Bar::window_menu_style(Fl_Sys_Menu_Bar::no_window_menu);
#endif
  window.show(argc, argv);
  return Fl::run();
}
