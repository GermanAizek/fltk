//
// Application Main Menu code for the Fast Light Tool Kit (FLTK).
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

#include "app/Menu.h"

#include "Fluid.h"

#include "proj/mergeback.h"
#include "proj/undo.h"
#include "app/templates.h"
#include "nodes/Node.h"
#include "nodes/Group_Node.h"
#include "nodes/Window_Node.h"
#include "nodes/factory.h"
#include "panels/codeview_panel.h"
#include "app/shell_command.h"

#include <FL/Fl_Menu_Bar.H>

// In Snap_Action.h
extern void layout_suite_marker(Fl_Widget *, void *user_data);
extern void select_layout_preset_cb(Fl_Widget *, void *user_data);
extern Fl_Menu_Item main_layout_submenu_[];

using namespace fluid;


void write_cb(Fl_Widget *, void *) {
   Fluid.write_code_files();
}
void openwidget_cb(Fl_Widget *, void *) { Fluid.edit_selected(); }
void copy_cb(Fl_Widget*, void*) { Fluid.copy_selected(); }
void cut_cb(Fl_Widget *, void *) { Fluid.cut_selected(); }
void delete_cb(Fl_Widget *, void *) { Fluid.delete_selected(); }
void paste_cb(Fl_Widget*, void*) { Fluid.paste_from_clipboard(); }
void duplicate_cb(Fl_Widget*, void*) { Fluid.duplicate_selected(); }
static void sort_cb(Fl_Widget *,void *) { Fluid.sort_selected(); }
void about_cb(Fl_Widget *, void *) { Fluid.about(); }
void help_cb(Fl_Widget *, void *) {
  Fluid.show_help("fluid.html");
}
static void save_template_cb(Fl_Widget *, void *) { fluid::app::save_template(); }
void mergeback_cb(Fl_Widget *, void *);

void manual_cb(Fl_Widget *, void *) {
  Fluid.show_help("index.html");
}

static void menu_file_new_cb(Fl_Widget *, void *) {
  if (Fluid.proj.confirm_clear()) {
    Fluid.new_project();
  }
}
static void menu_file_new_from_template_cb(Fl_Widget *, void *) { Fluid.new_project_from_template(); }
static void menu_file_open_cb(Fl_Widget *, void *) { Fluid.open_project_file(std::string{}); }
static void menu_file_insert_cb(Fl_Widget *, void *) { Fluid.proj.load_or_merge(std::string{}); }
void menu_file_save_cb(Fl_Widget *, void *arg) { Fluid.proj.save(); }
void menu_file_save_as_cb(Fl_Widget *, void *arg) { Fluid.proj.save(Project::SaveOption::ASK_FOR_FILENAME); }
void menu_file_save_copy_cb(Fl_Widget *, void *arg) { Fluid.proj.save(Project::SaveOption::SAVE_COPY); }
static void menu_file_print_cb(Fl_Widget *, void *arg) { Fluid.print_snapshots(); }
void menu_file_open_history_cb(Fl_Widget *, void *v) { Fluid.open_project_file(std::string((const char*)v)); }
static void menu_layout_sync_resize_cb(Fl_Menu_ *m, void*) {
 if (m->mvalue()->value()) Fluid.proj.tree.allow_layout = 1; else Fluid.proj.tree.allow_layout = 0;
}
static void menu_file_revert_cb(Fl_Widget *, void *) { Fluid.proj.revert(); }
void exit_cb(Fl_Widget *,void *) { Fluid.quit(); }
static void write_strings_cb(Fl_Widget *, void *) { Fluid.proj.write_strings(); }
void toggle_widgetbin_cb(Fl_Widget *, void *) { Fluid.toggle_widget_bin(); }
/**
 This is the main Fluid menu.

 Design history is manipulated right inside this menu structure.
 Some menu items change or deactivate correctly, but most items just trigger
 various callbacks.

 \c New_Menu creates new widgets and is explained in detail in another location.

 \see New_Menu
 \todo This menu needs some major modernization. Menus are too long and their
    sorting is not always obvious.
 \todo Shortcuts are all over the place (Alt, Ctrl, Command, Shift-Ctrl,
    function keys), and there should be a help page listing all shortcuts.
 */
Fl_Menu_Item Application::main_menu[] = {
  {.text = "&File",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
  {.text = "&New", .shortcut_ = FL_COMMAND + 'n', .callback_ = menu_file_new_cb},
  {.text = "&Open...", .shortcut_ = FL_COMMAND + 'o', .callback_ = menu_file_open_cb},
  {.text = "&Insert...",
     .shortcut_ = FL_COMMAND + 'i', .flags = FL_MENU_DIVIDER, .callback_ = menu_file_insert_cb, .user_data_ = nullptr},
  {.text = "&Save",
     .shortcut_ = FL_COMMAND + 's', .callback_ = menu_file_save_cb, .user_data_ = nullptr},
  {.text = "Save &As...",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 's', .callback_ = menu_file_save_as_cb, .user_data_ = (void*)1},
  {.text = "Sa&ve A Copy...", .shortcut_ = 0, .callback_ = menu_file_save_copy_cb, .user_data_ = (void*)2},
  {.text = "&Revert...", .shortcut_ = 0, .flags = FL_MENU_DIVIDER, .callback_ = menu_file_revert_cb, .user_data_ = nullptr},
  {.text = "New &From Template...",
     .shortcut_ = FL_COMMAND + 'N', .callback_ = menu_file_new_from_template_cb, .user_data_ = nullptr},
  {.text = "Save As &Template...", .shortcut_ = 0, .flags = FL_MENU_DIVIDER, .callback_ = save_template_cb, .user_data_ = nullptr},
  {.text = "&Print...", .shortcut_ = FL_COMMAND + 'p', .callback_ = menu_file_print_cb},
  {.text = "Write &Code",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 'c', .callback_ = write_cb, .user_data_ = nullptr},
  {.text = "MergeBack Code",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 'm', .callback_ = mergeback_cb, .user_data_ = 0},
  {.text = "&Write Strings",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 'w', .flags = FL_MENU_DIVIDER, .callback_ = write_strings_cb, .user_data_ = nullptr},
  {.text = Fluid.history.relpath[0],
     .shortcut_ = FL_COMMAND + '1', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[0]},
  {.text = Fluid.history.relpath[1],
     .shortcut_ = FL_COMMAND + '2', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[1]},
  {.text = Fluid.history.relpath[2],
     .shortcut_ = FL_COMMAND + '3', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[2]},
  {.text = Fluid.history.relpath[3],
     .shortcut_ = FL_COMMAND + '4', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[3]},
  {.text = Fluid.history.relpath[4],
     .shortcut_ = FL_COMMAND + '5', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[4]},
  {.text = Fluid.history.relpath[5],
     .shortcut_ = FL_COMMAND + '6', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[5]},
  {.text = Fluid.history.relpath[6],
     .shortcut_ = FL_COMMAND + '7', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[6]},
  {.text = Fluid.history.relpath[7],
     .shortcut_ = FL_COMMAND + '8', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[7]},
  {.text = Fluid.history.relpath[8],
     .shortcut_ = FL_COMMAND + '9', .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[8]},
  {.text = Fluid.history.relpath[9], .shortcut_ = 0, .flags = FL_MENU_DIVIDER, .callback_ = menu_file_open_history_cb, .user_data_ = Fluid.history.abspath[9]},
  {.text = "&Quit", .shortcut_ = FL_COMMAND + 'q', .callback_ = exit_cb},
  {.text = nullptr},
  {.text = "&Edit",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
  {.text = "&Undo", .shortcut_ = FL_COMMAND + 'z', .callback_ = fluid::proj::Undo::undo_cb},
  {.text = "&Redo",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 'z', .flags = FL_MENU_DIVIDER, .callback_ = fluid::proj::Undo::redo_cb, .user_data_ = nullptr},
  {.text = "C&ut", .shortcut_ = FL_COMMAND + 'x', .callback_ = cut_cb},
  {.text = "&Copy", .shortcut_ = FL_COMMAND + 'c', .callback_ = copy_cb},
  {.text = "&Paste", .shortcut_ = FL_COMMAND + 'v', .callback_ = paste_cb},
  {.text = "Dup&licate", .shortcut_ = FL_COMMAND + 'u', .callback_ = duplicate_cb},
  {.text = "&Delete",
     .shortcut_ = FL_Delete, .flags = FL_MENU_DIVIDER, .callback_ = delete_cb, .user_data_ = nullptr},
  {.text = "Select &All", .shortcut_ = FL_COMMAND + 'a', .callback_ = select_all_cb},
  {.text = "Select &None",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 'a', .flags = FL_MENU_DIVIDER, .callback_ = select_none_cb, .user_data_ = nullptr},
  {.text = "Pr&operties...", .shortcut_ = FL_F + 1, .callback_ = openwidget_cb},
  {.text = "&Sort", .shortcut_ = 0, .callback_ = sort_cb},
  {.text = "&Earlier", .shortcut_ = FL_F + 2, .callback_ = earlier_cb},
  {.text = "&Later", .shortcut_ = FL_F + 3, .callback_ = later_cb},
  {.text = "&Group", .shortcut_ = FL_F + 7, .callback_ = group_cb},
  {.text = "Ung&roup",
     .shortcut_ = FL_F + 8, .flags = FL_MENU_DIVIDER,
     .callback_ = ungroup_cb, .user_data_ = nullptr},
  {.text = "Hide O&verlays",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 'o',
     .callback_ = toggle_overlays},
  {.text = "Hide Guides", .shortcut_ = FL_COMMAND + FL_SHIFT + 'g', .callback_ = toggle_guides},
  {.text = "Hide Restricted",
     .shortcut_ = FL_COMMAND + FL_SHIFT + 'r',
     .callback_ = toggle_restricted},
  {.text = "Show Widget &Bin...", .shortcut_ = FL_ALT + 'b', .callback_ = toggle_widgetbin_cb},
  {.text = "Show Code View",
     .shortcut_ = FL_ALT + 'c', .flags = FL_MENU_DIVIDER, .callback_ = (Fl_Callback*)toggle_codeview_cb, .user_data_ = nullptr},
  {.text = "Settings...", .shortcut_ = FL_ALT + 'p', .callback_ = show_settings_cb},
  {.text = nullptr},
  {.text = "&New", .shortcut_ = 0, .flags = FL_SUBMENU_POINTER, .callback_ = nullptr, .user_data_ = (void*)New_Menu},
  {.text = "&Layout",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
  {.text = "&Align",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
    {.text = "&Left",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)10},
    {.text = "&Center",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)11},
    {.text = "&Right",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)12},
    {.text = "&Top",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)13},
    {.text = "&Middle",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)14},
    {.text = "&Bottom",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)15},
  {.text = nullptr},
  {.text = "&Space Evenly",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
    {.text = "&Across",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)20},
    {.text = "&Down",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)21},
  {.text = nullptr},
  {.text = "&Make Same Size",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
    {.text = "&Width",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)30},
    {.text = "&Height",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)31},
    {.text = "&Both",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)32},
  {.text = nullptr},
  {.text = "&Center In Group",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
    {.text = "&Horizontal",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)40},
    {.text = "&Vertical",
     .shortcut_ = 0,
     .callback_ = (Fl_Callback*)align_widget_cb,
     .user_data_ = (void*)41},
  {.text = nullptr},
  {.text = "Synchronized Resize", .shortcut_ = 0, .flags = FL_MENU_TOGGLE | FL_MENU_DIVIDER, .callback_ = (Fl_Callback*)menu_layout_sync_resize_cb, .user_data_ = nullptr },
  {.text = "&Grid and Size Settings...",
     .shortcut_ = FL_COMMAND + 'g',
     .flags = FL_MENU_DIVIDER, .callback_ = show_grid_cb, .user_data_ = nullptr},
  {.text = "Presets", .shortcut_ = 0, .flags = FL_SUBMENU_POINTER, .callback_ = layout_suite_marker, .user_data_ = (void*)main_layout_submenu_ },
  {.text = "Application", .shortcut_ = 0, .flags = FL_MENU_RADIO | FL_MENU_VALUE, .callback_ = select_layout_preset_cb, .user_data_ = (void*)nullptr },
  {.text = "Dialog",      .shortcut_ = 0, .flags = FL_MENU_RADIO, .callback_ = select_layout_preset_cb, .user_data_ = (void*)1 },
  {.text = "Toolbox",     .shortcut_ = 0, .flags = FL_MENU_RADIO, .callback_ = select_layout_preset_cb, .user_data_ = (void*)2 },
  {.text = nullptr},
{.text = "&Shell", .shortcut_ = 0, .flags = FL_SUBMENU_POINTER, .callback_ = Fd_Shell_Command_List::menu_marker, .user_data_ = (void*)Fd_Shell_Command_List::default_menu},
  {.text = "&Help",
     .shortcut_ = 0,
     .flags = FL_SUBMENU,
     .callback_ = nullptr,
     .user_data_ = nullptr},
  {.text = "&Rapid development with FLUID...", .shortcut_ = 0, .callback_ = help_cb},
  {.text = "&FLTK Programmers Manual...",
     .shortcut_ = 0,
     .flags = FL_MENU_DIVIDER, .callback_ = manual_cb, .user_data_ = nullptr},
  {.text = "&About FLUID...", .shortcut_ = 0, .callback_ = about_cb},
  {.text = nullptr},
{.text = nullptr}};

/**
Show or hide the code preview window.
*/
void toggle_codeview_cb(Fl_Double_Window *, void *) {
 codeview_toggle_visibility();
}

/**
Show or hide the code preview window, button callback.
*/
void toggle_codeview_b_cb(Fl_Button*, void *) {
 codeview_toggle_visibility();
}

