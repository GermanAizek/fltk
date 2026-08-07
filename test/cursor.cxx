//
// Cursor test program for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
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
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>
#include "../fluid/pixmaps/compressed.xpm"

Fl_Cursor cursor = FL_CURSOR_DEFAULT;

Fl_Hor_Value_Slider *cursor_slider;

void choice_cb(Fl_Widget *, void *v) {
  cursor = (Fl_Cursor)(fl_intptr_t)v;
  cursor_slider->value(cursor);
  fl_cursor(cursor);
}

void custom_cb(Fl_Widget *widget, void *) {
  Fl_Pixmap pxm(compressed_xpm);
  Fl_RGB_Image rgb(&pxm);
  rgb.scale(16,16);
  widget->top_window()->cursor(&rgb, rgb.w()/2, rgb.h()/2);
}

Fl_Menu_Item choices[] = {
  {.text = "FL_CURSOR_DEFAULT",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_DEFAULT},
  {.text = "FL_CURSOR_ARROW",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_ARROW},
  {.text = "FL_CURSOR_CROSS",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_CROSS},
  {.text = "FL_CURSOR_WAIT",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_WAIT},
  {.text = "FL_CURSOR_INSERT",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_INSERT},
  {.text = "FL_CURSOR_HAND",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_HAND},
  {.text = "FL_CURSOR_HELP",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_HELP},
  {.text = "FL_CURSOR_MOVE",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_MOVE},
  {.text = "FL_CURSOR_NS",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_NS},
  {.text = "FL_CURSOR_WE",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_WE},
  {.text = "FL_CURSOR_NWSE",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_NWSE},
  {.text = "FL_CURSOR_NESW",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_NESW},
  {.text = "FL_CURSOR_N",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_N},
  {.text = "FL_CURSOR_NE",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_NE},
  {.text = "FL_CURSOR_E",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_E},
  {.text = "FL_CURSOR_SE",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_SE},
  {.text = "FL_CURSOR_S",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_S},
  {.text = "FL_CURSOR_SW",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_SW},
  {.text = "FL_CURSOR_W",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_W},
  {.text = "FL_CURSOR_NW",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_NW},
  {.text = "FL_CURSOR_NONE",
                           .shortcut_ = 0,
                           .callback_ = choice_cb,
                           .user_data_ = (void*)FL_CURSOR_NONE},
  {.text = "custom cursor", .shortcut_ = 0, .callback_ = custom_cb, .user_data_ = NULL},
  {.text = 0}
};

void setcursor(Fl_Widget *o, void *) {
  Fl_Hor_Value_Slider *slider = (Fl_Hor_Value_Slider *)o;
  cursor = Fl_Cursor((int)slider->value());
  fl_cursor(cursor);
}

// draw the label without any ^C or \nnn conversions:
class CharBox : public Fl_Box {
  void draw() FL_OVERRIDE {
    fl_font(FL_FREE_FONT,14);
    fl_draw(label(), x()+w()/2, y()+h()/2);
  }
public:
  CharBox(int X, int Y, int W, int H, const char* L) : Fl_Box(X,Y,W,H,L) {}
};

int main(int argc, char **argv) {
  Fl_Double_Window window(400,300);

  Fl_Choice choice(80,100,200,25,"Cursor:");
  choice.menu(choices);
  choice.callback(choice_cb);
  choice.when(FL_WHEN_RELEASE|FL_WHEN_NOT_CHANGED);

  Fl_Hor_Value_Slider slider1(80,180,310,30,"Cursor:");
  cursor_slider = &slider1;
  slider1.align(FL_ALIGN_LEFT);
  slider1.step(1);
  slider1.precision(0);
  slider1.bounds(0,255);
  slider1.value(0);
  slider1.callback(setcursor);
  slider1.value(cursor);

#if 0
  // draw the manual's diagram of cursors...
  window.size(400,800);
  int y = 300;
  Fl::set_font(FL_FREE_FONT, "cursor");
  char buf[100]; char *p = buf;
  for (Fl_Menu* m = choices; m->label(); m++) {
    Fl_Box* b = new Fl_Box(35,y,150,25,m->label());
    b->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    int n = (int)(m->argument());
    if (n == FL_CURSOR_NONE) break;
    if (n == FL_CURSOR_DEFAULT) n = FL_CURSOR_ARROW;
    p[0] = (char)((n-1)*2);
    p[1] = 0;
    b = new CharBox(15,y,20,20,p); p+=2;
    y += 25;
  }
#endif

  window.resizable(window);
  window.end();
  window.show(argc,argv);
  return Fl::run();
}
