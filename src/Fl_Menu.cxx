//
// Menu code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2026 by Bill Spitzak and others.
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

// This file contains code for implementing Fl_Menu_Item, and for
// methods for bringing up popup menu hierarchies without using the
// Fl_Menu_ widget.

// The menu code is in the process of refactoring.

#include <FL/Fl.H>
#include "Fl_Screen_Driver.H"
#include "Fl_Window_Driver.H"
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Menu_.H>
#include <FL/fl_draw.H>
#include "flstring.h"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// This file will declare:
class Menu_Window_Basetype;
class Menu_Title_Window;
class Menu_Window;
struct Menu_State;

using menu_index_t = int;
using item_index_t = int;

// Global variables:

bool is_special_labeltype(uchar t) {
  return t == _FL_MULTI_LABEL || t == _FL_ICON_LABEL || t == _FL_IMAGE_LABEL;
}

// appearance of current menus are pulled from this parent widget:
const Fl_Menu_* button = nullptr;

// values for Menu_State.state:
enum class State : std::uint8_t {
  INIT = 0,     // no mouse up or down since popup() called
  PUSHED,       // mouse has been pushed on a normal item
  MENU_PUSHED,  // mouse has been pushed on a menu title
  DONE,         // exit the popup, the current item was picked
};

/*
 This class handles the current cascade of menu windows for a pulldown call.
 */
struct Menu_State
{
  // menu item under the mouse pinter or selected by keyboard, or nullptr
  const Fl_Menu_Item* current_item = nullptr;

  // index of the menu window that contains the current_item
  menu_index_t current_menu_ix = 0;

  // index of current_item within the menu window indicated by menu_number, -1 if none
  item_index_t current_item_ix = -1;

  // pointers to open menu windows
  std::array<Menu_Window*, 20> menu_window{};

  // number of open menuwindows
  menu_index_t num_menus = 0;

  // if true, pulldown is initiated by a menubar, and menu_window[0] holds the
  // horizontally arranged level 0 menu item list
  bool in_menubar = false;

  // State::INIT, etc. See above
  State state = State::INIT;

  // simulate a button in the top level of a menubar
  Menu_Window* menubar_button_helper = nullptr;

  // check if mouse coordinates are inside any of the menu windows
  bool is_inside(int mx, int my) const;

  // set the current menu item
  void set_current_item(const Fl_Menu_Item* i, menu_index_t m, item_index_t n);

  // set the current menu item
  void set_current_item(menu_index_t m, item_index_t n);

  // previous item in menu menu if possible
  bool prev_item(menu_index_t menu);

  // go to next item in menu menu if possible
  bool next_item(menu_index_t menu);

  // handle FL_SHORTCUT in any of the menu windows
  int handle_shortcut();

  // move menu item selection left
  int handle_left();

  // move menu item selection right
  int handle_right();

  // handle activation of the selected menu item
  int handle_select();

  // handle menu cancellation
  int handle_cancel();

  // handle any keyboard event from the menu windows
  int handle_keyboard_event();

  // handle all mouse events from the menu windows
  int handle_mouse_events(int e);

  // create a submenu based on the selected menu item m
  bool create_submenu(const Fl_Rect &r, const Menu_Window& cw, const Fl_Menu_Item *m,
                      const Fl_Menu_Item *initial_item, bool menubar);

  // delete all menu windows beyond the selected one
  void delete_unused_menus(const Menu_Window& cw, const Fl_Menu_Item* m);
};

// Global state of menu windows and popup windows.
Menu_State* menu_state = nullptr;


//
// ---- Menu_Window_Basetype ---------------------------------------------------
//

/*
  Base type for Menu_Title_Window and Menu_Window, derived from Fl_Menu_Window.
*/
class Menu_Window_Basetype : public Fl_Menu_Window
{
protected:

  /* Create a window that can hold menu items.
   The implementation is in the derived class Menu_Title_Window and Menu_Window.
   \param X, Y, W, H position and size of the window
   \param m saved in member variable `menu` for derived classes
   */
  Menu_Window_Basetype(int X, int Y, int W, int H, const Fl_Menu_Item *m)
  : Fl_Menu_Window(X, Y, W, H, nullptr),
    menu(m)
  {
    set_menu_window();
    Fl_Window_Driver::driver(this)->set_popup_window();
    end();
    set_modal();
    clear_border();
    // Put it on same screen as that where Menu_Window::parent_ is.
    screen_num(Fl_Window_Driver::menu_parent(nullptr)->screen_num());
  }

public:

  // Store a pointer to the first item in the menu array.
  const Fl_Menu_Item* menu{nullptr};

  // Use this to check this is a Fl_Menu_Window or a derived window class.
  virtual Menu_Window* as_menuwindow() { return nullptr; }
};

//
// ---- Menu_Title_Window ------------------------------------------------------
//

/*
  Menu window showing the title above a popup menu.
 */
class Menu_Title_Window : public Menu_Window_Basetype
{
protected:
  /* Draw the contents of the menu title window. */
  void draw() override {
    menu->draw(0, 0, w(), h(), button, 2);
  }

public:

  /* Create a window that hold the title of another menu.
   \param X, Y, W, H position and size of the window
   \param[in] L pointer to menu item that holds the label text
   \param[in] inbar true if this is part of an Fl_Menu_Bar
   */
  Menu_Title_Window(int X, int Y, int W, int H, const Fl_Menu_Item* L, bool inbar = false)
  : Menu_Window_Basetype(X, Y, W, H, L),
    in_menubar(inbar) { }

  // If set, title is part of a menubar.
  bool in_menubar{false};
};

//
// ---- Menu_Window ------------------------------------------------------------
//

/* A window that renders the menu items from an array and handles all events.
 The event handler runs in its own loop inside `Fl_Menu_Item::pulldown`.
 All Menu Windows are managed in the struct `menu_state`.
 */
class Menu_Window : public Menu_Window_Basetype
{
  // For tricky direct access
  friend class Fl_Window_Driver;

  // Fl_Menu_Item::pulldown does some direct manipulations
  friend struct Fl_Menu_Item;

protected:
  // Draw this window, either entirely, or just the selected and deselect items.
  void draw() override;

private:
  // Draw a single menu item in this window
  void draw_entry(const Fl_Menu_Item* m, int n, int eraseit);

  // Draw the submenu arrow
  static void draw_submenu_arrow(const Fl_Rect& bbox);

  // Draw the menu item shortcut text.
  void draw_shortcut(const Fl_Rect& bbox, const Fl_Menu_Item* mi) const;

  // Draw the menu item divider.
  static void draw_divider(const Fl_Rect& bbox);

  // Main event handler
  int handle_part1(int e);

  // Kludge to avoid abandoned window on macOS
  int handle_part2(int e, int ret);

  // All open menu windows are positioned relative to this window
  static Fl_Window *parent_;

  // Helper to store the height of the screen that contains the menu windows
  static int display_height_;

public:

  // Create our menu window
  Menu_Window(const Fl_Menu_Item* m, int X, int Y, int Wp, int Hp,
              const Fl_Menu_Item* picked, const Fl_Menu_Item* t,
              bool in_menubar = false, bool mb_title = false, int right_edge = 0);

  // Destructor
  ~Menu_Window() override;

  Menu_Window(const Menu_Window&) = delete;
  Menu_Window& operator=(const Menu_Window&) = delete;
  Menu_Window(Menu_Window&&) = delete;
  Menu_Window& operator=(Menu_Window&&) = delete;

  // Override to fixup the current selection
  void hide() override;

  // Override to handle all incoming events
  int handle(int e) override;

  // Change the index of the selected item, -1 for none. Trigger chatty callbacks
  // and marks the window area of the newly selected item for redraw.
  void set_selected(item_index_t n);

  // Find the index to the item under the given mouse coordinates.
  item_index_t find_selected(int mx, int my) const;

  // Calculate the horizontal position of an item by index for horizontal
  // menus inside a menubar.
  int titlex(int n) const;

  // Scroll so item i is visible on screen. This may move the entire window..
  void autoscroll(item_index_t n) const;

  // Also reposition the title (relative to the parent_ window?)
  void position(int X, int Y);

  // return true, if the given root coordinates are inside the window
  bool is_inside(int mx, int my) const;

  // Fake runtime type information
  Menu_Window* as_menuwindow() override { return this; }

  // Optional title for menubar windows and floating menus
  Menu_Title_Window* title{nullptr};

  // Height of the tallest menu item in the array, zero == menubar.
  int item_height{0};

  // Number of menu items in the window.
  item_index_t num_items{0};

  // Index of selected item, or -1 if none is selected.
  item_index_t selected{-1};

  // Remember the last item we drew selected, so we can redraw it unselected
  // when the selection changes. -1 if none.
  int drawn_selected{-1};

  // Width of the longest shortcut key text minus modifier keys
  int shortcut_width{0};

  // If set, the title window is also the button in Fl_Menu_Bar
  bool menubar_title{false};

  // In a cascading window, this points to the menu window that opened this menu.
  Menu_Window *origin{nullptr};

  // Used by the window driver
  int offset_y{0};
};

//
// ==== Implementations ========================================================
//
//
// ---- Menu_State -------------------------------------------------------------
//

/* Find out if any menu window is under the mouse.
  \return 1 if the coordinates are inside any of the menuwindows
*/
bool Menu_State::is_inside(int mx, int my) const {
  for (menu_index_t i = num_menus - 1; i >= 0; i--) {
    if (menu_window[static_cast<std::size_t>(i)]->is_inside(mx, my)) {
      return true;
    }
  }
  return false;
}

/* Remember this item in the state machine.
  \param[in] i current menu item
  \param[in] m index into menu window array
  \param[in] n index into visible item in that menu window
*/
void Menu_State::set_current_item(const Fl_Menu_Item* i, menu_index_t m, item_index_t n) {
  current_item = i;
  current_menu_ix = m;
  current_item_ix = n;
}

/* Find and store a menu item in the state machine.
  \param[in] m index into menu window array
  \param[in] n index into visible item in that menu window
*/
void Menu_State::set_current_item(menu_index_t m, item_index_t n) {
  current_item = (n >= 0) ? menu_window[static_cast<std::size_t>(m)]->menu->next(n) : nullptr;
  current_menu_ix = m;
  current_item_ix = n;
}

/* Go down to the next selectable menu item.
  If the event button is FL_Down, increment once, else go to the bottom of the menu.
  \param[in] menu index into menu window list
  \return `true` if an item was found, `false` if the menu wrapped
*/
bool Menu_State::next_item(menu_index_t menu) { // go to next item in menu menu if possible
  // `menu` is -1 if no item is currently selected, so use the first menu
  if (menu < 0) {
    menu = 0;
  }
  const Menu_Window &m = *(menu_window[static_cast<std::size_t>(menu)]);
  item_index_t item = (menu == current_menu_ix) ? current_item_ix : m.selected;
  bool wrapped = false;
  do {
    while (++item < m.num_items) {
      const Fl_Menu_Item* m1 = m.menu->next(item);
      if (m1->selectable()) {
        set_current_item(m1, menu, item);
        return true;
      }
    }
    if (wrapped) {
      break;
    }
    item = -1;
    wrapped = true;
  }
  while (Fl::event_key() != FL_Down);
  return false;
}

/* Go up to the previous selectable menu item.
  If the event button is FL_Up, decrement once, else go to the top of the menu.
  \param[in] menu index into menu window list
  \return `true` if an item was found, `false` if the menu wrapped
*/
bool Menu_State::prev_item(menu_index_t menu) { // previous item in menu menu if possible
  // `menu` is -1 if no item is currently selected, so use the first menu
  if (menu < 0) {
    menu = 0;
  }
  const Menu_Window &m = *(menu_window[static_cast<std::size_t>(menu)]);
  item_index_t item = (menu == current_menu_ix) ? current_item_ix : m.selected;
  bool wrapped = false;
  do {
    while (--item >= 0) {
      const Fl_Menu_Item* m1 = m.menu->next(item);
      if (m1->selectable()) {
        set_current_item(m1, menu, item);
        return true;
      }
    }
    if (wrapped) {
      break;
    }
    item = m.num_items;
    wrapped = true;
  }
  while (Fl::event_key() != FL_Up);
  return false;
}

/* Handle the FL_SHORTCUT event.
  \return 1 if the shortcut was found in the menu and handled.
*/
int Menu_State::handle_shortcut() {
  for (menu_index_t mymenu = num_menus; mymenu--;) {
    const Menu_Window &mw = *(menu_window[static_cast<std::size_t>(mymenu)]);
    int item = 0;
    const Fl_Menu_Item* m = mw.menu->find_shortcut(&item);
    if (m) {
      set_current_item(m, mymenu, item);
      if (!m->submenu()) {
        state = State::DONE;
      }
      return 1;
    }
  }
  return 0;
}

/* Move menu item selection left.
  \return 1
*/
int Menu_State::handle_left() {
  if (in_menubar && current_menu_ix <= 1) {
    prev_item(0);
  } else if (current_menu_ix > 0) {
    set_current_item(current_menu_ix - 1, menu_window[static_cast<std::size_t>(current_menu_ix - 1)]->selected);
  }
  return 1;
}

/* Move menu item selection right.
  \return 1
*/
int Menu_State::handle_right() {
  if (in_menubar && (current_menu_ix <= 0 || (current_menu_ix == num_menus - 1))) {
    next_item(0);
  } else if (current_menu_ix < num_menus - 1) {
    next_item(current_menu_ix + 1);
  }
  return 1;
}

/* Handle activation of the selected menu item.
  \return 1
*/
int Menu_State::handle_select() {
  // if the current item is a submenu with no callback,
  // simulate FL_Right to enter the submenu
  if (current_item
      && (!in_menubar || current_menu_ix > 0)
      && current_item->selectable()
      && current_item->submenu()
      && !current_item->callback_)
  {
    return handle_right();
  }
  // Ignore keypresses over inactive items, mark KEYBOARD event as used.
  if (current_item && !current_item->selectable()) {
    return 1;
  }
  // Mark the menu 'done' which will trigger the callback
  state = State::DONE;
  return 1;
}

/* Handle menu cancellation.
  \return 1
*/
int Menu_State::handle_cancel() {
  set_current_item(nullptr, -1, 0);
  state = State::DONE;
  return 1;
}

/* Handle any keyboard event from the menu windows.
  \return 1 if the keyboard event was handled, else 0
*/
int Menu_State::handle_keyboard_event() {
  switch (Fl::event_key()) {
    case FL_BackSpace:
      prev_item(current_menu_ix);
      return 1;
    case FL_Up:
      if (in_menubar && current_menu_ix == 0) {
        // Do nothing...
      } else if (prev_item(current_menu_ix)) {
        // Do nothing...
      } else if (in_menubar && current_menu_ix == 1) {
        set_current_item(0, menu_window[0]->selected);
      }
      return 1;
    case FL_Tab:
      if (Fl::event_shift()) {
        prev_item(current_menu_ix);
        return 1;
      }
      if (in_menubar && current_menu_ix == 0) {
        return handle_right();
      }
      /* FALLTHROUGH */
    case FL_Down:
      if (current_menu_ix != 0 || !in_menubar) {
        next_item(current_menu_ix);
      } else if (current_menu_ix < num_menus - 1) {
        next_item(current_menu_ix + 1);
      }
      return 1;
    case FL_Right:
      return handle_right();
    case FL_Left:
      return handle_left();
    case FL_Enter:
    case FL_KP_Enter:
    case ' ':
      return handle_select();
    case FL_Escape:
      return handle_cancel();
    default:
      break;
  }
  return 0;
}

/* Handle all mouse events from the menu windows.
  \return 1 if the event was handled, else 0
*/
int Menu_State::handle_mouse_events(int e) {
  switch (e) {
    case FL_MOVE: {
      const int use_part1_extra = Fl::screen_driver()->need_menu_handle_part1_extra();
      if (use_part1_extra && state == State::DONE) {
        return 1; // Fix for STR #2619
      }
      /* FALLTHROUGH */
    }
    case FL_ENTER:
    case FL_PUSH:
    case FL_DRAG:
    {
      const int mx = Fl::event_x_root();
      const int my = Fl::event_y_root();
      item_index_t item = 0;
      menu_index_t mymenu = num_menus - 1;
      // Clicking or dragging outside menu cancels it...
      if ((!in_menubar || mymenu != 0) && !is_inside(mx, my)) {
        set_current_item(nullptr, -1, 0);
        if (e == FL_PUSH) {
          state = State::DONE;
        }
        return 1;
      }
      for (mymenu = num_menus - 1; ; mymenu--) {
        item = menu_window[static_cast<std::size_t>(mymenu)]->find_selected(mx, my);
        if (item >= 0) {
          break;
        }
        if (mymenu <= 0) {
          // buttons in menubars must be deselected if we move outside of them!
          if (current_menu_ix == -1 && e == FL_PUSH) {
            state = State::DONE;
            return 1;
          }
          if (current_item && current_menu_ix == 0 && !current_item->submenu()) {
            if (e == FL_PUSH) {
              state = State::DONE;
              set_current_item(nullptr, -1, 0);
            }
            return 1;
          }
          // all others can stay selected
          return 0;
        }
      }
      set_current_item(mymenu, item);
      if (e == FL_PUSH) {
        if (current_item && current_item->submenu() // this is a menu title
            && item != menu_window[static_cast<std::size_t>(mymenu)]->selected // and it is not already on
            && !current_item->callback_) // and it does not have a callback
        {
          state = State::MENU_PUSHED;
        } else {
          state = State::PUSHED;
        }
      }
      return 1;
    }
    case FL_RELEASE:
      // Mouse must either be held down/dragged some, or this must be
      // the second click (not the one that popped up the menu):
      if (!Fl::event_is_click()
          || state == State::PUSHED
          || (in_menubar && current_item && !current_item->submenu()))
      {
        // do nothing if they try to pick an inactive item, or a submenu with no callback
        if (!current_item || (current_item->selectable() &&
                                 (!current_item->submenu() || current_item->callback_ || (in_menubar && current_menu_ix <= 0))))
        {
          state = State::DONE;
        }
      }
      return 1;
    default:
      break;
  }
  return 0;
}

/* Create a submenu based on the selected menu item m.
  \param[in] r suggested rectangle for new menu window
  \param[in] cw window of menu window with currently selected item
  \param[in] m currently selected menu item
  \param[in] initial_item if set, the new menu is aligned so that this item
      is close to m, or under the mouse
  \param[in] menubar if set, the menu list is part of a menubar, so the window
      at 0 is a horizontal menu item list
  \return true if the menu list was update to show the initial_item
*/
bool Menu_State::create_submenu(const Fl_Rect &r, const Menu_Window& cw, const Fl_Menu_Item *m,
                                const Fl_Menu_Item *initial_item, bool menubar) {
  const Fl_Menu_Item* title = m;
  const Fl_Menu_Item* menutable = nullptr;
  if ((static_cast<unsigned int>(m->flags) & static_cast<unsigned int>(FL_SUBMENU)) != 0U) {
    menutable = m + 1;
  } else {
    menutable = static_cast<const Fl_Menu_Item*>(m->user_data_);
  }
  // figure out where new menu goes:
  int nX = 0;
  int nY = 0;
  if (current_menu_ix == 0 && in_menubar) {      // menu off a menubar:
    nX = cw.x() + cw.titlex(current_item_ix);
    nY = cw.y() + cw.h();
    initial_item = nullptr;
  } else {
    nX = cw.x() + cw.w();
    nY = cw.y() + current_item_ix * cw.item_height;
    title = nullptr;
  }
  if (initial_item) { // bring up submenu containing initial item:
    auto* n = new Menu_Window(menutable, r.x(), r.y(), r.w(), r.h(), initial_item, title, false, false, cw.x());
    menu_window[static_cast<std::size_t>(num_menus++)] = n;
    if (num_menus >= 2) {
      menu_window[static_cast<std::size_t>(num_menus - 1)]->origin = menu_window[static_cast<std::size_t>(num_menus - 2)];
    }
    // move all earlier menus to line up with this new one:
    if (n->selected >= 0) {
      int dy = n->y() - nY;
      int dx = n->x() - nX;
      int waX = 0;
      int waY = 0;
      int waW = 0;
      int waH = 0;
      const int ns = Fl_Window_Driver::menu_parent()->screen_num();
      Fl_Window_Driver::driver(n)->menu_window_area(waX, waY, waW, waH, ns);
      for (menu_index_t menu = 0; menu <= current_menu_ix; menu++) {
        Menu_Window* tt = menu_window[static_cast<std::size_t>(menu)];
        int nx = tt->x() + dx;
        if (nx < waX) {
          nx = waX;
          dx = -tt->x() + waX;
        }
        int ny = tt->y() + dy;
        if (ny < waY) {
          ny = waY;
          dy = -tt->y() + waY;
        }
        tt->position(nx, ny);
      }
      menu_state->set_current_item(num_menus - 1, n->selected);
      return true;
    }
  } else if (num_menus > current_menu_ix + 1 &&
             menu_window[static_cast<std::size_t>(current_menu_ix + 1)]->menu == menutable) {
    // the menu is already up:
    while (num_menus > current_menu_ix + 2) {
      delete menu_window[static_cast<std::size_t>(--num_menus)];
    }
    menu_window[static_cast<std::size_t>(num_menus - 1)]->set_selected(-1);
  } else {
    // delete all the old menus and create new one:
    while (num_menus > current_menu_ix + 1) {
      delete menu_window[static_cast<std::size_t>(--num_menus)];
    }
    menu_window[static_cast<std::size_t>(num_menus++)] = new Menu_Window(menutable, nX, nY,
        title ? 1 : 0, 0, nullptr, title, false, menubar,
        (title ? 0 : cw.x()));
    if (num_menus >= 2 && menu_window[static_cast<std::size_t>(num_menus - 2)]->item_height != 0) {
      menu_window[static_cast<std::size_t>(num_menus - 1)]->origin = menu_window[static_cast<std::size_t>(num_menus - 2)];
    }
  }
  return false;
}

/* Delete all menu windows beyond the selected one.
  This deletes menus in the list that are beyond the selected menu window w.
  It also fakes a menubar button entry by only showing the title of an emty menu.
  \param[in] cw the selected menu window
  \param[in] m the selected menu item within the menu window
*/
void Menu_State::delete_unused_menus(const Menu_Window& cw, const Fl_Menu_Item* m) {
  while (num_menus > current_menu_ix + 1) {
    delete menu_window[static_cast<std::size_t>(--num_menus)];
  }
  if (in_menubar && (current_menu_ix == 0)) {
    // kludge so "menubar buttons" turn "on" by using menu title:
    menubar_button_helper = new Menu_Window(nullptr,
        cw.x() + cw.titlex(current_item_ix),
        cw.y() + cw.h(), 0, 0,
        nullptr, m, false, true);
    menubar_button_helper->title->show();
  }
}

//
// ---- Menu_Window ------------------------------------------------------------
//

// Static members:
Fl_Window *Menu_Window::parent_ = nullptr;
int Menu_Window::display_height_ = 0;


/*
 Construct a menu window that can render a list of menu items.
 \param[in] m pointer to the first menu item in the array
 \param[in] X, Y position relative to parent_
 \param[in] Wp, Hp initial minimum size; if Wp is 0, the window will open on the
    screen with X and Y, else it will open in the screen with the mouse pointer.
 \param[in] picked pointer to the currently picked menu item, can be nullptr
 \param[in] t pointer to the menutitle window
 \param[in] in_menubar set if part of an Fl_Menu_Bar menu
 \param[in] mb_title set if the title window is also the button in Fl_Menu_Bar
 \param[in] right_edge maximum right edge of menu on current screen(?), not used
 */
Menu_Window::Menu_Window(const Fl_Menu_Item* m, int X, int Y, int Wp, int Hp,
                         const Fl_Menu_Item* picked, const Fl_Menu_Item* t,
                         bool in_menubar, bool mb_title, int right_edge)
: Menu_Window_Basetype(X, Y, Wp, Hp, m)
{
  int scr_x = 0;
  int scr_y = 0;
  int scr_w = 0;
  int scr_h = 0;
  const int tx = X;
  const int ty = Y;
  menubar_title = mb_title;
  const int n = Fl_Window_Driver::menu_parent()->screen_num();
  Fl_Window_Driver::driver(this)->menu_window_area(scr_x, scr_y, scr_w, scr_h, n);
  if (right_edge == 0 || right_edge > scr_x + scr_w) {
    right_edge = scr_x + scr_w;
  }
  (void)right_edge;

  if (m) {
    m = m->first(); // find the first item that needs to be rendered
  }
  if (button) {
    Fl_Boxtype b = button->menu_box();
    if (b == FL_NO_BOX) {
      b = button->box();
    }
    if (b == FL_NO_BOX) {
      b = FL_FLAT_BOX;
    }
    box(b);
  } else {
    box(FL_UP_BOX);
  }
  color(button && !Fl::scheme() ? button->color() : FL_GRAY);
  {
    item_index_t j = 0;
    if (m) {
      for (const Fl_Menu_Item* m1 = m; ; m1 = m1->next(), j++) {
        if (picked) {
          if (m1 == picked) {
            selected = j;
            picked = nullptr;
          } else if (m1 > picked) {
            selected = j - 1;
            picked = nullptr;
            Wp = Hp = 0;
          }
        }
        if (!m1->text) {
          break;
        }
      }
    }
    num_items = j;
  }

  if (in_menubar) {
    item_height = 0;
    title = nullptr;
    return;
  }

  item_height = 1;

  int shortcuts_w = 0;  // maximum width in pixels of all shortcut texts w/o modifiers
  int modifiers_w = 0;  // maximum width of all shortcut modifiers texts
  int titile_w = 0;     // width of the title window
  int title_h = 0;      // height of the title window
  if (t) {
    titile_w = t->measure(&title_h, button) + 12;
  }
  int W = 0;
  if (m) {
    for (; m->text; m = m->next()) {
      int hh = 0;
      int w1 = m->measure(&hh, button);
      if (hh + Fl::menu_linespacing() > item_height) {
        item_height = hh + Fl::menu_linespacing();
      }
      if ((static_cast<unsigned int>(m->flags) & static_cast<unsigned int>(FL_SUBMENU | FL_SUBMENU_POINTER)) != 0U) {
        w1 += FL_NORMAL_SIZE;
      }
      if (w1 > W) {
        W = w1;
      }
      // calculate the maximum width of all shortcuts
      if (m->shortcut_) {
        // s is a pointer to the UTF-8 string for the entire shortcut
        // k points only to the key part (minus the modifier keys)
        const char *k = nullptr;
        const char *s = fl_shortcut_label(m->shortcut_, &k);
        if (fl_utf_nb_char(reinterpret_cast<const unsigned char*>(k), static_cast<int>(strlen(k))) <= 4) {
          // a regular shortcut has a right-justified modifier followed by a left-justified key
          w1 = static_cast<int>(fl_width(s, static_cast<int>(k - s)));
          if (w1 > modifiers_w) {
            modifiers_w = w1;
          }
          w1 = static_cast<int>(fl_width(k)) + 4;
          if (w1 > shortcuts_w) {
            shortcuts_w = w1;
          }
        } else {
          // a shortcut with a long modifier is right-justified to the menu
          w1 = static_cast<int>(fl_width(s)) + 4;
          if (w1 > (modifiers_w + shortcuts_w)) {
            modifiers_w = w1 - shortcuts_w;
          }
        }
      }
    }
  }
  shortcut_width = shortcuts_w;
  if (selected >= 0 && Wp == 0) {
    X -= W / 2;
  }
  const int BW = Fl::box_dx(box());
  W += shortcuts_w + modifiers_w + 2 * BW + 7;
  if (Wp > W) {
    W = Wp;
  }
  if (titile_w > W) {
    W = titile_w;
  }

  if (X < scr_x) {
    X = scr_x;
  }
  if (X > scr_x + scr_w - W) {
    X = scr_x + scr_w - W;
  }
  x(X); w(W);
  h((num_items != 0 ? item_height * num_items - 4 : 0) + 2 * BW + 3);
  if (selected >= 0) {
    Y = Y + (Hp - item_height) / 2 - selected * item_height - BW;
  } else {
    Y = Y + Hp;
    // if the menu hits the bottom of the screen, we try to draw
    // it above the menubar instead. We will not adjust any menu
    // that has a selected item.
    if (Y + h() > scr_y + scr_h && Y - h() >= scr_y) {
      if (Hp > 1) {
        // if we know the height of the Fl_Menu_, use it
        Y = Y - Hp - h();
      } else if (t) {
        // assume that the menubar item height relates to the first
        // menuitem as well
        Y = Y - item_height - h() - Fl::box_dh(box());
      } else {
        // draw the menu to the right
        Y = Y - h() + item_height + Fl::box_dy(box());
      }
      if (t) {
        if (menubar_title) {
          Y = Y + Fl::menu_linespacing() - Fl::box_dw(button->box());
        } else {
          Y += 2 * title_h + 2 * BW + 3;
        }
      }
    }
  }
  if (m) {
    y(Y);
  } else {
    y(Y - 2); w(1); h(1);
  }

  if (t) {
    if (menubar_title) {
      const int dy = Fl::box_dy(button->box()) + 1;
      const int ht = button->h() - dy * 2;
      title = new Menu_Title_Window(tx, ty - ht - dy, titile_w, ht, t, true);
    } else {
      const int dy = 2;
      const int ht = title_h + 2 * BW + 3;
      title = new Menu_Title_Window(X, Y - ht - dy, titile_w, ht, t);
    }
  } else {
    title = nullptr;
  }
}

/* Destroy this window. */
Menu_Window::~Menu_Window() {
  Menu_Window::hide();
  delete title;
}

/* Fixup the selection and hide this window */
void Menu_Window::hide() {
  set_selected(-1);
  Menu_Window_Basetype::hide();
}

/* Handle events sent to the window.
 \param[in] e event number
 \return 1 if the event was used
 */
int Menu_Window::handle(int e) {
  const int use_part2 = Fl::screen_driver()->need_menu_handle_part2();
  int ret = handle_part1(e);
  if (use_part2 != 0) {
    ret = handle_part2(e, ret);
  }
  return ret;
}

/* Window event handling implementation.
 \param[in] e event number
 \return 1 if the event was used
 */
int Menu_Window::handle_part1(int e) {
  Menu_State &pp = *menu_state;
  switch (e) {
    case FL_KEYBOARD:
      if (pp.handle_keyboard_event()) {
        return 1;
      }
      break;
    case FL_SHORTCUT:
      if (pp.handle_shortcut()) {
        return 1;
      }
      break;
    case FL_MOVE:
    case FL_ENTER:
    case FL_PUSH:
    case FL_DRAG:
    case FL_RELEASE:
      if (pp.handle_mouse_events(e)) {
        return 1;
      }
      break;
    default:
      break;
  }
  return Fl_Window::handle(e);
}

int Menu_Window::handle_part2(int /*e*/, int ret) {
  Menu_State &pp = *menu_state;
  if (pp.state == State::DONE) {
    hide();
    if (pp.menubar_button_helper) {
      pp.menubar_button_helper->hide();
      if (pp.menubar_button_helper->title) {
        pp.menubar_button_helper->title->hide();
      }
    }
    menu_index_t i = pp.num_menus;
    while (i > 0) {
      Menu_Window *mw = pp.menu_window[static_cast<std::size_t>(--i)];
      if (mw) {
        mw->hide();
        if (mw->title) {
          mw->title->hide();
        }
      }
    }
  }
  return ret;
}

/* Set a new selected item.
 \param[in] n index into visible item list
 */
void Menu_Window::set_selected(item_index_t n) {
  if (n != selected) {
    if (selected != -1 && menu) {
      const Fl_Menu_Item *mi = menu->next(selected);
      if (mi && mi->callback_ && ((static_cast<unsigned int>(mi->flags) & static_cast<unsigned int>(FL_MENU_CHATTY)) != 0U)) {
        mi->do_callback(this, FL_REASON_LOST_FOCUS);
      }
    }
    selected = n;
    if (selected != -1 && menu) {
      const Fl_Menu_Item *mi = menu->next(selected);
      if (mi && mi->callback_ && ((static_cast<unsigned int>(mi->flags) & static_cast<unsigned int>(FL_MENU_CHATTY)) != 0U)) {
        mi->do_callback(this, FL_REASON_GOT_FOCUS);
      }
    }
    damage(FL_DAMAGE_CHILD);
  }
}

/* Find the item at the give pixel position.
 \param[in] mx, my position in pixels
 \return index of item that is under the pixel, or -1 for none
 */
item_index_t Menu_Window::find_selected(int mx, int my) const {
  if (!menu || !menu->text) {
    return -1;
  }
  mx -= x();
  my -= y();
  if (my < 0 || my >= h()) {
    return -1;
  }
  if (item_height == 0) { // menubar
    int xx = 3;
    int n = 0;
    const Fl_Menu_Item* m = menu->first();
    for (; ; m = m->next(), n++) {
      if (!m->text) {
        return -1;
      }
      xx += m->measure(nullptr, button) + 16;
      if (xx > mx) {
        break;
      }
    }
    return n;
  }
  if (mx < Fl::box_dx(box()) || mx >= w()) {
    return -1;
  }
  const item_index_t n = (my - Fl::box_dx(box()) - 1) / item_height;
  if (n < 0 || n >= num_items) {
    return -1;
  }
  return n;
}

/* Return horizontal position for item n in a menubar.
 \return position in window in pixels.
 */
int Menu_Window::titlex(int n) const {
  const Fl_Menu_Item* m = nullptr;
  int xx = 3;
  for (m = menu->first(); n--; m = m->next()) {
    xx += m->measure(nullptr, button) + 16;
  }
  return xx;
}

/* Scroll so item i is visible on screen.
 May scroll or move the window.
 \param[in] n index into visible menu items
 */
void Menu_Window::autoscroll(item_index_t n) const {
  int scr_y = 0;
  int scr_h = 0;
  int Y = y() + Fl::box_dx(box()) + 2 + n * item_height;

  int xx = 0;
  int ww = 0;
  Fl_Window_Driver::driver(this)->menu_window_area(xx, scr_y, ww, scr_h, this->screen_num());
  if (n == 0 && Y <= scr_y + item_height) {
    Y = scr_y - Y + 10;
  } else if (Y <= scr_y + item_height) {
    Y = scr_y - Y + 10 + item_height;
  } else {
    Y = Y + item_height - scr_h - scr_y;
    if (Y < 0) {
      return;
    }
    Y = -Y - 10;
  }
  Fl_Window_Driver::driver(this)->reposition_menu_window(x(), y() + Y);
}

/* Set the position of this menu and its title window. */
void Menu_Window::position(int X, int Y) {
  if (title) {
    title->position(X, title->y() + Y - y());
  }
  Fl_Menu_Window::position(X, Y);
}

/* Check if mouse is positions over the window.
 \return 1, if the given root coordinates are inside the window
 */
bool Menu_Window::is_inside(int mx, int my) const {
  if (mx < x_root() || mx >= x_root() + w() ||
      my < y_root() || my >= y_root() + h()) {
    return false;
  }
  if (item_height == 0 && find_selected(mx, my) == -1) {
    // in the menubar but out from any menu header
    return false;
  }
  return true;
}

/* Draw one menu item.
 \param[in] m pointer to the item
 \param[in] n index into the visible item list
 \param[in] eraseit if set, redraw the unselected background
 */
void Menu_Window::draw_entry(const Fl_Menu_Item* m, int n, int eraseit) {
  if (!m) {
    return;
  }

  const Fl_Rect bbox{
    Fl::box_dx(box()),
    Fl::box_dy(box()) + 1 + n * item_height + Fl::menu_linespacing() / 2 - 2,
    w() - Fl::box_dw(box()) - 1,
    item_height - Fl::menu_linespacing()
  };

  // Clear the entire item rect including the spacing
  if (eraseit != 0 && n != selected) {
    fl_push_clip(bbox.x() + 1, bbox.y() - (Fl::menu_linespacing() - 2) / 2,
                 bbox.w() - 2, bbox.h() + (Fl::menu_linespacing() - 2));
    draw_box(box(), 0, 0, w(), h(), button ? button->color() : color());
    fl_pop_clip();
  }

  // Draw the checkbox, radio box, the menu icon, and the label
  m->draw(bbox.x(), bbox.y(), bbox.w(), bbox.h(), button, n == selected ? 1 : 0);

  // Draw additional decorations on the right side of the label
  if (m->submenu()) {
    draw_submenu_arrow(bbox);
  } else if (m->shortcut_) {
    draw_shortcut(bbox, m);
  }
  if ((static_cast<unsigned int>(m->flags) & static_cast<unsigned int>(FL_MENU_DIVIDER)) != 0U) {
    draw_divider(bbox);
  }
}

/* Draw the submenu arrow.
  \param[in] bbox menu item bounding box
 */
void Menu_Window::draw_submenu_arrow(const Fl_Rect& bbox) {
  const auto unsigned_h = static_cast<unsigned int>(bbox.h() - 2);
  int sz = static_cast<int>(unsigned_h & ~1U) + 1;
  if (sz > 13) {
    sz = 13;
  }
  const int x1 = bbox.x() + bbox.w() - sz - 2;
  const int y1 = bbox.y() + (bbox.h() - sz) / 2 + 1;

  fl_draw_arrow(Fl_Rect(x1, y1, sz, sz), FL_ARROW_SINGLE, FL_ORIENT_RIGHT, fl_color());
}

/* Draw the benu item shortcut text.
  \param[in] bbox menu item bounding box
  \param[in] m take the shortcut from this menu item
 */
void Menu_Window::draw_shortcut(const Fl_Rect& bbox, const Fl_Menu_Item* m) const {
  const Fl_Font f = (m->labelsize_ || m->labelfont_) ? static_cast<Fl_Font>(m->labelfont_) :
                    button ? button->textfont() : FL_HELVETICA;
  fl_font(f, m->labelsize_ ? static_cast<Fl_Fontsize>(m->labelsize_) :
          button ? button->textsize() : FL_NORMAL_SIZE);
  const char *k = nullptr;
  const char *s = fl_shortcut_label(m->shortcut_, &k);
  if (fl_utf_nb_char(reinterpret_cast<const unsigned char*>(k), static_cast<int>(strlen(k))) <= 4) {
    const auto len = static_cast<std::size_t>(k - s);
    std::string buf(s, len);
    fl_draw(buf.c_str(), bbox.x(), bbox.y(),
            bbox.w() - shortcut_width, bbox.h(), FL_ALIGN_RIGHT);
    fl_draw(k, bbox.x() + bbox.w() - shortcut_width, bbox.y(),
            shortcut_width, bbox.h(), FL_ALIGN_LEFT);
  } else {
    fl_draw(s, bbox.x(), bbox.y(), bbox.w() - 4, bbox.h(), FL_ALIGN_RIGHT);
  }
}

/* Draw the divider. It's part of the menu, but drawn in the spacing area.
  \param[in] bbox menu item bounding box
*/
void Menu_Window::draw_divider(const Fl_Rect& bbox) {
  const int y_offset = (Fl::menu_linespacing() - 2) / 2;
  fl_color(FL_DARK3);
  fl_xyline(bbox.x() - 1, bbox.b() + y_offset, bbox.r());
  fl_color(FL_LIGHT3);
  fl_xyline(bbox.x() - 1, bbox.b() + y_offset + 1, bbox.r());
}


/* Draw the menuwindow. If the damage flags are FL_DAMAGE_CHILD, only redraw
 the old selected and the newly selected items.
 */
void Menu_Window::draw() {
  if (damage() != FL_DAMAGE_CHILD) {    // complete redraw
    if ((box() != FL_FLAT_BOX)
         && (Fl::is_scheme("gtk+") || Fl::is_scheme("plastic") || Fl::is_scheme("gleam"))) {
      fl_draw_box(FL_FLAT_BOX, 0, 0, w(), h(),
                  button ? button->color() : color());
    }
    fl_draw_box(box(), 0, 0, w(), h(), button ? button->color() : color());
    if (menu) {
      const Fl_Menu_Item* m = nullptr;
      int j = 0;
      for (m = menu->first(), j = 0; m->text; j++, m = m->next()) {
        draw_entry(m, j, 0);
      }
    }
  } else {
    if ((static_cast<unsigned int>(damage()) & static_cast<unsigned int>(FL_DAMAGE_CHILD)) != 0U && selected != drawn_selected) {
      draw_entry(menu->next(drawn_selected), drawn_selected, 1);
      draw_entry(menu->next(selected), selected, 1);
    }
  }
  drawn_selected = selected;
}

//
// ---- Fl_Menu_Item -----------------------------------------------------------
//

int Fl_Menu_Item::size() const {
  const Fl_Menu_Item* m = this;
  int nest = 0;
  for (;;) {
    if (!m->text) {
      if (nest == 0) {
        return static_cast<int>(m - this + 1);
      }
      nest--;
    } else if ((static_cast<unsigned int>(m->flags) & static_cast<unsigned int>(FL_SUBMENU)) != 0U) {
      nest++;
    }
    m++;
  }
}

namespace {

const Fl_Menu_Item* next_visible_or_not(const Fl_Menu_Item* m) {
  int nest = 0;
  do {
    if (!m->text) {
      if (nest == 0) {
        return m;
      }
      nest--;
    } else if ((static_cast<unsigned int>(m->flags) & static_cast<unsigned int>(FL_SUBMENU)) != 0U) {
      nest++;
    }
    m++;
  }
  while (nest != 0);
  return m;
}

} // namespace

const Fl_Menu_Item* Fl_Menu_Item::next(int n) const {
  if (n < 0) {
    return nullptr;
  }
  const Fl_Menu_Item* m = this;
  if (!m->visible()) {
    n++;
  }
  while (n != 0) {
    m = next_visible_or_not(m);
    if (m->visible() || !m->text) {
      n--;
    }
  }
  return m;
}

int Fl_Menu_Item::measure(int* hp, const Fl_Menu_* m) const {
  Fl_Label l;
  l.value   = text;
  l.image   = nullptr;
  l.deimage = nullptr;
  l.type    = static_cast<Fl_Labeltype>(labeltype_);
  l.font    = (labelsize_ || labelfont_) ? static_cast<Fl_Font>(labelfont_) : (m ? m->textfont() : FL_HELVETICA);
  l.size    = labelsize_ ? static_cast<Fl_Fontsize>(labelsize_) : (m ? m->textsize() : FL_NORMAL_SIZE);
  l.color   = FL_FOREGROUND_COLOR;
  l.h_margin_ = 0;
  l.v_margin_ = 0;
  l.spacing = 0;
  fl_draw_shortcut = 1;
  int w = 0;
  int h = 0;
  l.measure(w, hp ? *hp : h);
  fl_draw_shortcut = 0;
  if ((static_cast<unsigned int>(flags) & static_cast<unsigned int>(FL_MENU_TOGGLE | FL_MENU_RADIO)) != 0U) {
    w += FL_NORMAL_SIZE + 4;
  }
  return w;
}

void Fl_Menu_Item::draw(int x, int y, int w, int h, const Fl_Menu_* m,
                        int draw_mode) const {
  Fl_Label l;
  l.value   = text;
  l.image   = nullptr;
  l.deimage = nullptr;
  l.type    = static_cast<Fl_Labeltype>(labeltype_);
  l.font    = (labelsize_ || labelfont_) ? static_cast<Fl_Font>(labelfont_) : (m ? m->textfont() : FL_HELVETICA);
  l.size    = labelsize_ ? static_cast<Fl_Fontsize>(labelsize_) : (m ? m->textsize() : FL_NORMAL_SIZE);
  l.color   = labelcolor_ ? labelcolor_ : (m ? m->textcolor() : FL_FOREGROUND_COLOR);
  l.h_margin_ = 0;
  l.v_margin_ = 0;
  l.spacing = 0;
  if (!active()) {
    l.color = fl_inactive(l.color);
  }
  if (draw_mode != 0) {
    const Fl_Color r = m ? m->selection_color() : FL_SELECTION_COLOR;
    const Fl_Boxtype b = m && m->down_box() ? m->down_box() : FL_FLAT_BOX;
    l.color = fl_contrast(labelcolor_, r);
    if (draw_mode == 2) { // menu title
      fl_draw_box(b, x, y, w, h, r);
      x += 3;
      w -= 8;
    } else {
      fl_draw_box(b, x + 1, y - (Fl::menu_linespacing() - 2) / 2, w - 2, h + (Fl::menu_linespacing() - 2), r);
    }
  }

  if ((static_cast<unsigned int>(flags) & static_cast<unsigned int>(FL_MENU_TOGGLE | FL_MENU_RADIO)) != 0U) {
    const int d = (h - FL_NORMAL_SIZE + 1) / 2;
    const int W = h - 2 * d;

    Fl_Color check_color = labelcolor_;
    if (Fl::is_scheme("gtk+")) {
      check_color = FL_SELECTION_COLOR;
    }
    check_color = fl_contrast(check_color, FL_BACKGROUND2_COLOR);

    if ((static_cast<unsigned int>(flags) & static_cast<unsigned int>(FL_MENU_RADIO)) != 0U) {
      fl_draw_box(FL_ROUND_DOWN_BOX, x + 2, y + d, W, W, FL_BACKGROUND2_COLOR);
      if (value()) {
        int tW = (W - Fl::box_dw(FL_ROUND_DOWN_BOX)) / 2 + 1;
        if ((static_cast<unsigned int>(W - tW) & 1U) != 0U) {
          tW++;
        }
        const int td = (W - tW) / 2;
        fl_draw_radio(x + td + 1, y + d + td - 1, tW + 2, check_color);
      }
    } else {
      fl_draw_box(FL_DOWN_BOX, x + 2, y + d, W, W, FL_BACKGROUND2_COLOR);
      if (value()) {
        fl_draw_check(Fl_Rect(x + 3, y + d + 1, W - 2, W - 2), check_color);
      }
    }
    x += W + 3;
    w -= W + 3;
  }

  if (fl_draw_shortcut == 0) {
    fl_draw_shortcut = 1;
  }
  l.draw(x + 3, y, (w > 6) ? (w - 6) : 0, h, FL_ALIGN_LEFT);
  fl_draw_shortcut = 0;
}

const Fl_Menu_Item* Fl_Menu_Item::pulldown(
    int X, int Y, int W, int H,
    const Fl_Menu_Item* initial_item,
    const Fl_Menu_* pbutton,
    const Fl_Menu_Item* title,
    int menubar) const
{
  Fl_Group::current(nullptr);

  auto* const btn_widget = const_cast<Fl_Widget*>(static_cast<const Fl_Widget*>(static_cast<const void*>(pbutton)));
  Fl_Widget_Tracker wp(btn_widget);

  button = pbutton;
  if (pbutton && pbutton->window()) {
    Menu_Window::parent_ = const_cast<Fl_Window*>(pbutton->top_window());
    for (const Fl_Window* w = pbutton->window(); w; w = w->window()) {
      X += w->x();
      Y += w->y();
    }
  } else {
    X += Fl::event_x_root() - Fl::event_x();
    Y += Fl::event_y_root() - Fl::event_y();
    Menu_Window::parent_ = Fl::first_window();
  }

  int XX = 0;
  int YY = 0;
  int WW = 0;
  Fl::screen_xywh(XX, YY, WW, Menu_Window::display_height_, Menu_Window::parent_->screen_num());
  Menu_Window mw(this, X, Y, W, H, initial_item, title, menubar != 0);
  Fl::grab(mw);
  if (Menu_Window::parent_) {
    Menu_Window::parent_->cursor(FL_CURSOR_DEFAULT);
  }
  Menu_State pp;
  menu_state = &pp;
  pp.menu_window[0] = &mw;
  pp.num_menus = 1;
  pp.in_menubar = (menubar != 0);

  auto run_startup = [&]() {
    while (pp.current_menu_ix >= 0 && pp.current_menu_ix < pp.num_menus) {
      Menu_Window& cw = *pp.menu_window[static_cast<std::size_t>(pp.current_menu_ix)];
      const Fl_Menu_Item* m_item = pp.current_item;
      if (!m_item || !m_item->selectable()) {
        cw.set_selected(-1);
        initial_item = nullptr;
        break;
      }
      cw.set_selected(pp.current_item_ix);

      if (m_item == initial_item) {
        initial_item = nullptr;
      }
      if (m_item->submenu()) {
        if (pp.create_submenu(Fl_Rect{X, Y, W, H}, cw, m_item, initial_item, menubar != 0)) {
          continue;
        }
      } else {
        pp.delete_unused_menus(cw, m_item);
      }
      break;
    }
  };

  if (initial_item && mw.selected >= 0) {
    menu_state->set_current_item(0, mw.selected);
    run_startup();
  } else {
    pp.current_item = nullptr;
    pp.current_menu_ix = 0;
    pp.current_item_ix = -1;
    if (menubar != 0) {
      if (!mw.handle(FL_DRAG)) {
        Fl::grab(nullptr);
        return nullptr;
      }
    }
    initial_item = pp.current_item;
    if (initial_item) {
      if (menubar != 0 && !initial_item->selectable()) {
        Fl::grab(nullptr);
        return nullptr;
      }
      run_startup();
    }
  }

  for (;;) {
    for (menu_index_t k = (menubar != 0) ? 1 : 0; k < pp.num_menus; k++) {
      if (!pp.menu_window[static_cast<std::size_t>(k)]->shown()) {
        if (pp.menu_window[static_cast<std::size_t>(k)]->title) {
          pp.menu_window[static_cast<std::size_t>(k)]->title->show();
        }
        pp.menu_window[static_cast<std::size_t>(k)]->show();
      }
    }

    {
      const Fl_Menu_Item* oldi = pp.current_item;
      Fl::wait();
      if (pbutton && wp.deleted()) {
        break;
      }
      if (pp.state == State::DONE) {
        break;
      }
      if (pp.current_item == oldi) {
        continue;
      }
    }

    if (pp.menubar_button_helper) {
      delete pp.menubar_button_helper;
      pp.menubar_button_helper = nullptr;
    }

    if (!pp.current_item) {
      pp.menu_window[static_cast<std::size_t>(pp.num_menus - 1)]->set_selected(-1);
      continue;
    }

    initial_item = nullptr;
    if (pp.current_menu_ix < 0 || pp.current_menu_ix >= pp.num_menus) {
      continue;
    }
    pp.menu_window[static_cast<std::size_t>(pp.current_menu_ix)]->autoscroll(pp.current_item_ix);
    run_startup();
  }

  const Fl_Menu_Item* m = (pbutton && wp.deleted()) ? nullptr : pp.current_item;
  delete pp.menubar_button_helper;
  pp.menubar_button_helper = nullptr;
  while (pp.num_menus > 1) {
    delete pp.menu_window[static_cast<std::size_t>(--pp.num_menus)];
  }
  mw.hide();
  Fl::grab(nullptr);
  Menu_Window::parent_ = nullptr;
  return m;
}

const Fl_Menu_Item* Fl_Menu_Item::popup(
  int X, int Y,
  const char* title,
  const Fl_Menu_Item* picked,
  const Fl_Menu_* menu_button
) const {
  static Fl_Menu_Item dummy;
  dummy.text = title;
  return pulldown(X, Y, 0, 0, picked, menu_button, title ? &dummy : nullptr);
}

const Fl_Menu_Item* Fl_Menu_Item::find_shortcut(int* ip, const bool require_alt) const {
  const Fl_Menu_Item* m = this;
  if (m) {
    for (int ii = 0; m->text; m = next_visible_or_not(m), ii++) {
      if (m->active()) {
        if (Fl::test_shortcut(m->shortcut_)
            || (!is_special_labeltype(static_cast<uchar>(m->labeltype_)) && Fl_Widget::test_shortcut(m->text, require_alt))
            || (m->labeltype_ == _FL_MULTI_LABEL
                && !is_special_labeltype(reinterpret_cast<const Fl_Multi_Label*>(m->text)->typea)
                && Fl_Widget::test_shortcut(reinterpret_cast<const Fl_Multi_Label*>(m->text)->labela, require_alt))
            || (m->labeltype_ == _FL_MULTI_LABEL
                && !is_special_labeltype(reinterpret_cast<const Fl_Multi_Label*>(m->text)->typeb)
                && Fl_Widget::test_shortcut(reinterpret_cast<const Fl_Multi_Label*>(m->text)->labelb, require_alt))) {
          if (ip) {
            *ip = ii;
          }
          return m;
        }
      }
    }
  }
  return nullptr;
}

const Fl_Menu_Item* Fl_Menu_Item::test_shortcut() const {
  const Fl_Menu_Item* m = this;
  const Fl_Menu_Item* ret = nullptr;
  if (m) {
    for (; m->text; m = next_visible_or_not(m)) {
      if (m->active()) {
        if (Fl::test_shortcut(m->shortcut_)) {
          return m;
        }
        if (!ret && m->submenu()) {
          const Fl_Menu_Item* s = ((static_cast<unsigned int>(m->flags) & static_cast<unsigned int>(FL_SUBMENU)) != 0U)
                                      ? m + 1
                                      : static_cast<const Fl_Menu_Item*>(m->user_data_);
          ret = s->test_shortcut();
        }
      }
    }
  }
  return ret;
}

//
// ---- Fl_Window_Driver -------------------------------------------------------
//

namespace {

Menu_Window *to_menuwindow(Fl_Window *win) {
  if (!win || !Fl_Window_Driver::driver(win)->popup_window() || !win->menu_window()) {
    return nullptr;
  }
  return static_cast<Menu_Window_Basetype*>(win)->as_menuwindow();
}

} // namespace

Fl_Window *Fl_Window_Driver::menu_parent(int *display_height) {
  if (display_height) {
    *display_height = Menu_Window::display_height_;
  }
  return Menu_Window::parent_;
}

bool Fl_Window_Driver::to_menutitle(Fl_Window *win) {
  if (!win || !Fl_Window_Driver::driver(win)->popup_window() || !win->menu_window()) {
    return false;
  }
  return static_cast<Menu_Window_Basetype*>(win)->as_menuwindow() == nullptr;
}

Fl_Window *Fl_Window_Driver::menu_leftorigin(Fl_Window *win) {
  const Menu_Window *mwin = to_menuwindow(win);
  return mwin ? mwin->origin : nullptr;
}

Fl_Window *Fl_Window_Driver::menu_title(Fl_Window *win) {
  const Menu_Window *mwin = to_menuwindow(win);
  return mwin ? mwin->title : nullptr;
}

int Fl_Window_Driver::menu_itemheight(Fl_Window *win) {
  const Menu_Window *mwin = to_menuwindow(win);
  return mwin ? mwin->item_height : 0;
}

int Fl_Window_Driver::menu_bartitle(Fl_Window *win) {
  const Menu_Window *mwin = to_menuwindow(win);
  return mwin ? static_cast<int>(mwin->menubar_title) : 0;
}

int Fl_Window_Driver::menu_selected(Fl_Window *win) {
  const Menu_Window *mwin = to_menuwindow(win);
  return mwin ? mwin->selected : -1;
}

int *Fl_Window_Driver::menu_offset_y(Fl_Window *win) {
  Menu_Window *mwin = to_menuwindow(win);
  return mwin ? &(mwin->offset_y) : nullptr;
}

bool Fl_Window_Driver::is_floating_title(Fl_Window *win) {
  if (!win || !Fl_Window_Driver::driver(win)->popup_window() || !win->menu_window()) {
    return false;
  }
  const auto *mwin = static_cast<Menu_Window_Basetype*>(win)->as_menuwindow();
  return !mwin && !static_cast<Menu_Title_Window*>(win)->in_menubar;
}

void Fl_Window_Driver::scroll_to_selected_item(Fl_Window *win) {
  const Menu_Window *mwin = to_menuwindow(win);
  if (mwin && mwin->selected > 0) {
    mwin->autoscroll(mwin->selected);
  }
}