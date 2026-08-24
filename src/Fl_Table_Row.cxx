//
// Fl_Table_Row -- A row oriented table widget
//
//    A class specializing in a table of rows.
//    Handles row-specific selection behavior.
//
// Copyright 2002 by Greg Ercolano.
// Copyright 2009-2025 by Bill Spitzak and others.
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

//
// TODO:
//    o Row headings (only column headings supported currently)
//

#include <FL/Fl_Table_Row.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>

// for debugging...
// #define DEBUG 1
#ifdef DEBUG
#include <FL/names.h>
#include <stdio.h>              // fprintf()
#define PRINTEVENT \
    fprintf(stderr,"TableRow %s: ** Event: %s --\n", (label()?label():"none"), fl_eventnames[event]);
#else
#define PRINTEVENT
#endif


/**
  Checks to see if 'row' is selected.

  Returns 1 if selected, 0 if not. You can change the selection of a row
  by clicking on it, or by using select_row(row, flag)

  \p row \b should be a valid row. If the row is out of range the return
  value is 0 (zero).

  \param[in] row  row to be checked

  \return whether given row is selected
  \retval  1  row is selected
  \retval  0  row is not selected or \p row is out of range
*/
int Fl_Table_Row::row_selected(int row) const {
  if (row < 0 || row >= rows()) {
    return 0;
  }
  const auto urow = static_cast<size_t>(row);
  if ((urow / 8U) >= _rowselect.size()) {
    return 0;
  }
  return (_rowselect[urow / 8U] & (1U << (urow % 8U))) ? 1 : 0;
}

// Change row selection type
void Fl_Table_Row::type(TableRowSelectMode val) {
  _selectmode = val;
  switch ( _selectmode ) {
    case SELECT_NONE: {
      std::vector<uint8_t>().swap(_rowselect);
      redraw();
      break;
    }
    case SELECT_SINGLE: {
      int count = 0;
      for (uint8_t &sel : _rowselect) {
        if (sel) {
          for (unsigned int bit = 0; bit < 8U; bit++) {
            if (sel & (1U << bit)) {
              if (++count > 1) {  // only one allowed
                sel &= static_cast<uint8_t>(~(1U << bit));
              }
            }
          }
        }
      }
      redraw();
      break;
    }
    case SELECT_MULTI:
      break;
  }
}

/**
  Changes the selection state for \p 'row', depending on the value of \p 'flag'.

  The optional \p flag can be:
    -  0: clear selection
    -  1: set selection (default)
    -  2: toggle selection

  \param[in]  row   row to be selected, deselected, or toggled
  \param[in]  flag  change mode, see description
  \return     result of modification, see description
  \retval   0: selection state did not change
  \retval   1: selection state changed
  \retval  -1: row out of range or incorrect selection mode (\p flag)
*/

int Fl_Table_Row::select_row(int row, int flag) {
  int ret = 0;
  if ( row < 0 || row >= rows() ) { return(-1); }

  auto get_val = [&](int r) -> int {
    const auto ur = static_cast<size_t>(r);
    if ((ur / 8U) >= _rowselect.size()) {
      return 0;
    }
    return (_rowselect[ur / 8U] & (1U << (ur % 8U))) ? 1 : 0;
  };
  auto set_val = [&](int r, int v) {
    const auto ur = static_cast<size_t>(r);
    if (v) {
      if ((ur / 8U) >= _rowselect.size()) {
        _rowselect.resize(ur / 8U + 1, 0);
      }
      _rowselect[ur / 8U] |= static_cast<uint8_t>(1U << (ur % 8U));
    } else {
      if ((ur / 8U) < _rowselect.size()) {
        _rowselect[ur / 8U] &= static_cast<uint8_t>(~(1U << (ur % 8U)));
      }
    }
  };

  switch ( _selectmode ) {
    case SELECT_NONE:
      return(-1);

    case SELECT_SINGLE: {
      const int oldval = get_val(row);
      const int newval = (flag == 2) ? (oldval ^ 1) : flag;
      if (newval != oldval) {
        set_val(row, newval);
        redraw_range(row, row, leftcol, rightcol);
        ret = 1;
      }
      if (newval) {
        for (size_t i = 0; i < _rowselect.size(); i++) {
          if (_rowselect[i]) {
            for (unsigned int bit = 0; bit < 8U; bit++) {
              const int t = static_cast<int>(i * 8U + bit);
              if (t != row && (_rowselect[i] & (1U << bit))) {
                _rowselect[i] &= static_cast<uint8_t>(~(1U << bit));
                redraw_range(t, t, leftcol, rightcol);
              }
            }
          }
        }
      }
      break;
    }

    case SELECT_MULTI: {
      const int oldval = get_val(row);
      const int newval = (flag == 2) ? (oldval ^ 1) : flag;
      if ( newval != oldval ) {
        set_val(row, newval);
        if ( row >= toprow && row <= botrow ) {         // row visible?
          // Extend partial redraw range
          redraw_range(row, row, leftcol, rightcol);
        }
        ret = 1;
      }
    }
  }
  return(ret);
}

// Select all rows to a known state
void Fl_Table_Row::select_all_rows(int flag) {
  switch ( _selectmode ) {
    case SELECT_NONE:
      return;

    case SELECT_SINGLE:
      if ( flag != 0 ) {
        return;
      }
      //FALLTHROUGH

    case SELECT_MULTI: {
      char changed = 0;
      if ( flag == 2 ) {
        if (_rowselect.size() * 8U < static_cast<size_t>(rows())) {
          _rowselect.resize((static_cast<size_t>(rows()) + 7U) / 8U, 0);
        }
        for (uint8_t &sel : _rowselect) {
          sel ^= 0xFFU;
          changed = 1;
        }
        if (rows() % 8 != 0 && !_rowselect.empty()) {
          const auto urem = static_cast<unsigned int>(rows() % 8);
          _rowselect.back() &= static_cast<uint8_t>((1U << urem) - 1U);
        }
      } else if (flag == 1) {
        if (_rowselect.size() * 8U < static_cast<size_t>(rows())) {
          _rowselect.resize((static_cast<size_t>(rows()) + 7U) / 8U, 0);
        }
        for (uint8_t &sel : _rowselect) {
          if (sel != 0xFFU) {
            changed = 1;
          }
          sel = 0xFFU;
        }
        if (rows() % 8 != 0 && !_rowselect.empty()) {
          const auto urem = static_cast<unsigned int>(rows() % 8);
          _rowselect.back() &= static_cast<uint8_t>((1U << urem) - 1U);
        }
      } else {
        for (const uint8_t sel : _rowselect) {
          if (sel != 0) {
            changed = 1;
          }
        }
        std::vector<uint8_t>().swap(_rowselect);
      }
      if ( changed ) {
        redraw();
      }
    }
  }
}

// Set number of rows
void Fl_Table_Row::rows(int val) {
  Fl_Table::rows(val);
  const size_t needed = (static_cast<size_t>(val) + 7U) / 8U;
  if (needed < _rowselect.size()) {
    _rowselect.resize(needed);
  }
  if (val % 8 != 0 && !_rowselect.empty()) {
    const auto urem = static_cast<unsigned int>(val % 8);
    _rowselect.back() &= static_cast<uint8_t>((1U << urem) - 1U);
  }
}

// Handle events
int Fl_Table_Row::handle(int event) {
  PRINTEVENT;

  // Make snapshots of realtime event states *before* we service user's cb,
  // which may do things like post popup menus that return with unexpected button states.
  const int _event_button = Fl::event_button();
  //int _event_clicks = Fl::event_clicks();     // uncomment if needed
  const int _event_x      = Fl::event_x();
  const int _event_y      = Fl::event_y();
  //int _event_key    = Fl::event_key();        // uncomment if needed
  const int _event_state  = Fl::event_state();
  //Fl_Widget *_focus = Fl::focus();            // uncomment if needed

  // Let base class handle event
  //     Note: base class may invoke user callbacks that post menus,
  //     so from here on use event state snapshots (above).
  //
  int ret = Fl_Table::handle(event);

  // The following code disables cell selection.. why was it added? -erco 05/18/03
  // if ( ret ) { _last_y = Fl::event_y(); return(1); } // base class 'handled' it (eg. column resize)

  const int shiftstate = (static_cast<unsigned int>(_event_state) & static_cast<unsigned int>(FL_CTRL)) ? FL_CTRL :
  (static_cast<unsigned int>(_event_state) & static_cast<unsigned int>(FL_SHIFT)) ? FL_SHIFT : 0;

  // Which row/column are we over?
  int R, C;                             // row/column being worked on
  ResizeFlag resizeflag;                // which resizing area are we over? (0=none)
  TableContext context = cursor2rowcol(R, C, resizeflag);
  switch ( event ) {
    case FL_PUSH:
      if ( _event_button == 1 ) {
        _last_push_x = _event_x;        // save regardless of context
        _last_push_y = _event_y;        // " "

        // Handle selection in table.
        //     Select cell under cursor, and enable drag selection mode.
        //
        if ( context == CONTEXT_CELL ) {
          // Ctrl key? Toggle selection state
          switch ( shiftstate ) {
            case FL_CTRL:
              select_row(R, 2);         // toggle
              break;

            case FL_SHIFT: {
              select_row(R, 1);
              if ( _last_row > -1 ) {
                int srow = R, erow = _last_row;
                if ( srow > erow ) {
                  srow = _last_row;
                  erow = R;
                }
                for ( int row = srow; row <= erow; row++ ) {
                  select_row(row, 1);
                }
              }
              break;
            }

            default:
              select_all_rows(0);       // clear all previous selections
              select_row(R, 1);
              break;
          }

          _last_row = R;
          _dragging_select = 1;
          ret = 1;      // FL_PUSH handled (ensures FL_DRAG will be sent)
          // redraw();  // redraw() handled by select_row()
        }
      }
      break;

    case FL_DRAG: {
      if ( _dragging_select ) {
        // Dragged off table edges? Handle scrolling
        const int offtop = toy - _last_y;                     // >0 if off top of table
        const int offbot = _last_y - (toy + toh);             // >0 if off bottom of table

        if ( offtop > 0 && row_position() > 0 ) {
          // Only scroll in upward direction
          const int diff = _last_y - _event_y;
          if ( diff < 1 ) {
            ret = 1;
            break;
          }
          row_position(row_position() - diff);
          context = CONTEXT_CELL; R = row_position();  // HACK: fake it
          if ( R < 0 || R > rows() ) { ret = 1; break; }      // HACK: ugly
        }
        else if ( offbot > 0 && botrow < rows() ) {
          // Only scroll in downward direction
          const int diff = _event_y - _last_y;
          if ( diff < 1 ) {
            ret = 1;
            break;
          }
          row_position(row_position() + diff);
          context = CONTEXT_CELL; R = botrow;            // HACK: fake it
          if ( R < 0 || R > rows() ) { ret = 1; break; }        // HACK: ugly
        }
        if ( context == CONTEXT_CELL ) {
          switch ( shiftstate ) {
            case FL_CTRL:
              if ( R != _last_row ) {           // toggle if dragged to new row
                select_row(R, 2);               // 2=toggle
              }
              break;

            case FL_SHIFT:
            default:
              select_row(R, 1);
              if ( _last_row > -1 ) {
                int srow = R, erow = _last_row;
                if ( srow > erow ) {
                  srow = _last_row;
                  erow = R;
                }
                for ( int row = srow; row <= erow; row++ ) {
                  select_row(row, 1);
                }
              }
              break;
          }
          ret = 1;                              // drag handled
          _last_row = R;
        }
      }
      break;
    }

    case FL_RELEASE:
      if ( _event_button == 1 ) {
        _dragging_select = 0;
        ret = 1;                        // release handled
        // Clicked off edges of data table?
        //    A way for user to clear the current selection.
        //
        const int databot = tiy + table_h;
        const int dataright = tix + table_w;
        if (
            ( _last_push_x > dataright && _event_x > dataright ) ||
            ( _last_push_y > databot && _event_y > databot )
            ) {
          select_all_rows(0);                   // clear previous selections
        }
      }
      break;

    default:
      break;
  }
  _last_y = _event_y;
  return(ret);
}