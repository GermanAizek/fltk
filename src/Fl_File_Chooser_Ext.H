#ifndef Fl_File_Chooser_Ext_H
#define Fl_File_Chooser_Ext_H

#include <FL/Fl_File_Chooser.H>
#include <string>

struct Fl_File_Chooser::ChooserExt {
  std::string directory_str_;
  std::string pattern_str_;
  std::string preview_text_str_;
  void (*callback_)(Fl_File_Chooser*, void *) { nullptr };
  void *data_ { nullptr };
};

struct Fl_File_Chooser::FavExt {
  Fl_Double_Window* favWindow_ { nullptr };
  Fl_File_Browser* favList_ { nullptr };
  Fl_Button* favUpButton_ { nullptr };
  Fl_Button* favDeleteButton_ { nullptr };
  Fl_Button* favDownButton_ { nullptr };
  Fl_Button* favCancelButton_ { nullptr };
  Fl_Return_Button* favOkButton_ { nullptr };
};

#define directory_ (ext_->directory_str_)
#define pattern_ (ext_->pattern_str_)
#define preview_text_ (ext_->preview_text_str_)
#define callback_ (ext_->callback_)
#define data_ (ext_->data_)
#define favWindow (fav_ ? fav_->favWindow_ : nullptr)
#define favList (fav_->favList_)
#define favUpButton (fav_->favUpButton_)
#define favDeleteButton (fav_->favDeleteButton_)
#define favDownButton (fav_->favDownButton_)
#define favCancelButton (fav_->favCancelButton_)
#define favOkButton (fav_->favOkButton_)

#endif
