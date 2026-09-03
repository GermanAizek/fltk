//
// Fl_File_Browser routines.
//
// Copyright 1999-2010 by Michael Sweet.
// Copyright 2016-2026 by Bill Spitzak and others.
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
// Contents:
//
//   Fl_File_Browser::full_height()     - Return the height of the list.
//   Fl_File_Browser::item_height()     - Return the height of a list item.
//   Fl_File_Browser::item_width()      - Return the width of a list item.
//   Fl_File_Browser::item_draw()       - Draw a list item.
//   Fl_File_Browser::Fl_File_Browser() - Create a Fl_File_Browser widget.
//   Fl_File_Browser::load()            - Load a directory into the browser.
//   Fl_File_Browser::filter()          - Set the filename filter.
//

//
// Include necessary header files...
//

#include <FL/Fl_File_Browser.H>
#include <FL/Fl.H>
#include "Fl_System_Driver.H"
#include <FL/fl_draw.H>
#include <FL/filename.H>
#include <FL/fl_string_functions.h>
#include <FL/Fl_Image.H>        // icon
#include <cstdlib>
#include <cstring>
#include "flstring.h"

//
// 'Fl_File_Browser::full_height()' - Return the height of the list.
//

int                                     // O - Height in pixels
Fl_File_Browser::full_height() const
{
  int th = 0;                           // Total height of list.

  for (int i = 0; i < size(); i++)
    th += item_height(find_line(i)) + linespacing();

  return th;
}


//
// 'Fl_File_Browser::item_height()' - Return the height of a list item.
//

int                                     // O - Height in pixels
Fl_File_Browser::item_height(void *p) const     // I - List item data
{
  // Figure out the standard text height...
  fl_font(textfont(), textsize());
  const int textheight = fl_height();

  // We always have at least 1 line...
  int height = textheight;

  // Scan for newlines...
  auto *const line = static_cast<FL_BLINE*>(p);
  const char* const line_txt = bline_txt(line);

  if (line != nullptr) {
    for (const char *t = line_txt; *t != '\0'; t++) {
      if (*t == '\n')
        height += textheight;
    }
  }

  // If we have enabled icons then add space for them...
  if (Fl_File_Icon::first() != nullptr && height < iconsize_)
    height = iconsize_;

  // Add space for the selection border..
  height += 2;

  // Return the height
  return height;
}


//
// 'Fl_File_Browser::item_width()' - Return the width of a list item.
//

int                                     // O - Width in pixels
Fl_File_Browser::item_width(void *p) const      // I - List item data
{
  // Scan for newlines...
  auto *const line = static_cast<FL_BLINE*>(p);
  const char* const line_txt = bline_txt(line);
  const int *const columns = column_widths();

  // Set the font and size...
  const size_t txt_len = strlen(line_txt);
  if (txt_len > 0 && line_txt[txt_len - 1] == '/')
    fl_font(static_cast<Fl_Font>(static_cast<unsigned int>(textfont()) | static_cast<unsigned int>(FL_BOLD)), textsize());
  else
    fl_font(textfont(), textsize());

  int width = 0;

  if (strchr(line_txt, '\n') == nullptr &&
      strchr(line_txt, column_char()) == nullptr)
  {
    // Do a fast width calculation...
    width = static_cast<int>(fl_width(line_txt));
  }
  else
  {
    // More than 1 line or have columns; find the maximum width...
    int tempwidth = 0;
    int column = 0;
    const char *start = line_txt;

    for (const char *t = line_txt; *t != '\0'; t++) {
      if (*t == '\n')
      {
        int frag_len = static_cast<int>(t - start);
        tempwidth += static_cast<int>(fl_width(start, frag_len));

        // Update the max width as needed...
        if (tempwidth > width)
          width = tempwidth;

        // Point back to the start of the fragment...
        start     = t + 1;
        tempwidth = 0;
        column    = 0;
      }
      else if (*t == column_char())
      {
        // Advance to the next column...
        column++;
        if (columns != nullptr)
        {
          tempwidth = 0;
          for (int i = 0; i < column && columns[i] != 0; i++)
            tempwidth += columns[i];
        }
        else
        {
          tempwidth = column * static_cast<int>(fl_height() * 0.6 * 8.0);
        }

        if (tempwidth > width)
          width = tempwidth;

        start = t + 1;
      }
    }

    if (*start != '\0')
    {
      tempwidth += static_cast<int>(fl_width(start));

      // Update the max width as needed...
      if (tempwidth > width)
        width = tempwidth;
    }
  }

  // If we have enabled icons then add space for them...
  if (Fl_File_Icon::first() != nullptr)
    width += iconsize_ + 8;

  // Add space for the selection border..
  width += 2;

  // Return the width
  return width;
}


//
// 'Fl_File_Browser::item_draw()' - Draw a list item.
//

void
Fl_File_Browser::item_draw(void *p,     // I - List item data
                           int  X,      // I - Upper-lefthand X coordinate // NOLINT(bugprone-easily-swappable-parameters)
                           int  Y,      // I - Upper-lefthand Y coordinate // NOLINT(bugprone-easily-swappable-parameters)
                           int  W,      // I - Width of item               // NOLINT(bugprone-easily-swappable-parameters)
    const int  H) const// I - Height of item              // NOLINT(bugprone-easily-swappable-parameters)
{
  // Draw the list item text...
  auto *const line = static_cast<FL_BLINE*>(p);
  const char* const line_txt = bline_txt(line);
  const auto line_flags = static_cast<unsigned char>(bline_flags(line));
  const void* const line_data = bline_data(line);

  const size_t txt_len = strlen(line_txt);
  if (txt_len > 0 && line_txt[txt_len - 1] == '/')
    fl_font(static_cast<Fl_Font>(static_cast<unsigned int>(textfont()) | static_cast<unsigned int>(FL_BOLD)), textsize());
  else
    fl_font(textfont(), textsize());

  const Fl_Color c = ((line_flags & static_cast<unsigned int>(BLINE_SELECTED)) != 0U)
                       ? fl_contrast(textcolor(), selection_color())
                       : textcolor();

  if (Fl_File_Icon::first() == nullptr)
  {
    // No icons, just draw the text...
    X++;
    W -= 2;
  }
  else
  {
    // Draw the icon if it is set...
    if (line_data != nullptr)
      static_cast<Fl_File_Icon*>(const_cast<void*>(line_data))->draw(
          X, Y + (H - iconsize_) / 2,
          iconsize_, iconsize_,
          ((line_flags & static_cast<unsigned int>(BLINE_SELECTED)) != 0U) ? FL_YELLOW : FL_LIGHT2,
          active_r());

    // Draw the text offset to the right...
    X += iconsize_ + 9;
    W -= iconsize_ - 10;
  }
  // Center the text vertically...
  int height = fl_height();

  for (const char *t = line_txt; *t != '\0'; t++) {
    if (*t == '\n')
      height += fl_height();
  }
  Y += (H - height) / 2;

  // Draw the text...
  const int *const columns = column_widths();
  int width = 0;
  int column = 0;
  char *start = const_cast<char*>(line_txt);

  if (active_r() != 0)
    fl_color(c);
  else
    fl_color(fl_inactive(c));

  for (char *t = start; *t != '\0'; t++) {
    if (*t == '\n') {
      *t = '\0';
      fl_draw(start, X + width, Y, W - width, fl_height(),
              static_cast<Fl_Align>(FL_ALIGN_LEFT | FL_ALIGN_CLIP));
      *t = '\n';

      // Point back to the start of the fragment...
      start  = t + 1;
      width  = 0;
      Y      += fl_height();
      column = 0;
    } else if (*t == column_char()) {
      int cW = W - width; // Clip width...

      if (columns != nullptr) {
        // Try clipping inside this column...
        int i = 0;
        for (; i < column && columns[i] != 0; i++) { /* empty */ }

        if (columns[i] != 0)
          cW = columns[i];
      }

      *t = '\0';
      fl_draw(start, X + width, Y, cW, fl_height(),
              static_cast<Fl_Align>(FL_ALIGN_LEFT | FL_ALIGN_CLIP));
      *t = column_char();

      // Advance to the next column...
      column++;
      if (columns != nullptr) {
        width = 0;
        for (int i = 0; i < column && columns[i] != 0; i++)
          width += columns[i];
      }
      else {
        width = column * static_cast<int>(fl_height() * 0.6 * 8.0);
      }
      start = t + 1;
    }
  }
  if (*start != '\0') {
    fl_draw(start, X + width, Y, W - width, fl_height(),
            static_cast<Fl_Align>(FL_ALIGN_LEFT | FL_ALIGN_CLIP));
  }
}


/**
  The constructor creates the Fl_File_Browser widget at the specified position and size.
  The destructor destroys the widget and frees all memory that has been allocated.
*/
Fl_File_Browser::Fl_File_Browser(int        X,  // I - Upper-lefthand X coordinate
                                 int        Y,  // I - Upper-lefthand Y coordinate
                                 int        W,  // I - Width in pixels
                                 int        H,  // I - Height in pixels
                                 const char *l) // I - Label text
    : Fl_Browser(X, Y, W, H, l),
      directory_(""),
      pattern_("*"),
      errmsg_(nullptr),
      filetype_(FILES),
      iconsize_(static_cast<uchar>(3 * textsize() / 2))
{
}


// DTOR
Fl_File_Browser::~Fl_File_Browser() {
  errmsg(nullptr);       // free()s prev errmsg, if any
}


/**
  Sets OS error message to a string, which can be NULL.
  Frees previous if any.
  void errmsg(const char *emsg);
 */
void Fl_File_Browser::errmsg(const char* emsg) {
  if (errmsg_ != nullptr) {
    std::free(const_cast<char*>(errmsg_));
    errmsg_ = nullptr;
  }
  errmsg_ = (emsg != nullptr) ? fl_strdup(emsg) : nullptr;
}


/**
  Loads the specified directory into the browser. If icons have been
  loaded then the correct icon is associated with each file in the list.

  If directory is "", all mount points (unix) or drive letters (Windows)
  are listed.

  The sort argument specifies a sort function to be used with
  fl_filename_list().

  Return value is the number of filename entries, or 0 if none.
  On error, 0 is returned, and errmsg() has OS error string if non-NULL.
*/
int                                             // O - Number of files loaded
Fl_File_Browser::load(const char     *directory,// I - Directory to load
                      Fl_File_Sort_F *sort)     // I - Sort function to use
{
  char filename[4096];                          // Current file

  errmsg(nullptr); // clear errors first

//  printf("Fl_File_Browser::load(\"%s\")\n", directory);

  clear();

  directory_ = directory;

  if (directory == nullptr) {
    errmsg("NULL directory specified");
    return 0;
  }

  int num_files = 0;

  if (directory_[0] == '\0') {
    //
    // No directory specified; for UNIX list all mount points.  For DOS
    // list all valid drive letters...
    //
    Fl_File_Icon *icon = Fl_File_Icon::find("any", Fl_File_Icon::DEVICE);
    if (icon == nullptr)
      icon = Fl_File_Icon::find("any", Fl_File_Icon::DIRECTORY);
    num_files = Fl::system_driver()->file_browser_load_filesystem(this, filename, static_cast<int>(sizeof(filename)), icon);
  } else {
    dirent **files = nullptr;        // Files in directory
    char emsg[1024] = "";

    // Build the file list, check for errors
    num_files = Fl::system_driver()->file_browser_load_directory(directory_,
                                                                 filename, sizeof(filename),
                                                                 &files, sort,
                                                                 emsg, sizeof(emsg));
    // printf("Fl_File_Browser::load(dir='%s',filename='%s'): failed, emsg='%s'\n", directory_, filename, emsg);

    if (num_files <= 0) {
      errmsg(emsg);
      return 0;
    }

    int num_dirs = 0;
    for (int i = 0; i < num_files; i++) {
      if (strcmp(files[i]->d_name, "./") != 0) {
        fl_snprintf(filename, sizeof(filename), "%s/%s", directory_, files[i]->d_name);

        Fl_File_Icon *const icon = Fl_File_Icon::find(filename);
        if ((icon != nullptr && icon->type() == Fl_File_Icon::DIRECTORY) ||
            (Fl::system_driver()->filename_isdir_quick(filename) != 0)) {
          num_dirs++;
          insert(num_dirs, files[i]->d_name, icon);
        } else if (filetype_ == FILES &&
                   fl_filename_match(files[i]->d_name, pattern_) != 0) {
          add(files[i]->d_name, icon);
        }
      }

      std::free(static_cast<void*>(files[i]));
    }

    std::free(static_cast<void*>(files));
  }

  return num_files;
}


//
// 'Fl_File_Browser::filter()' - Set the filename filter.
//

void
Fl_File_Browser::filter(const char *pattern)    // I - Pattern string
{
  // If pattern is NULL set the pattern to "*"...
  if (pattern != nullptr)
    pattern_ = pattern;
  else
    pattern_ = "*";
}