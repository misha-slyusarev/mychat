#include "mychat.hpp"

class MainApplication : public wxApp {
 public:
  bool OnInit() override {
    MainFrame *frame = new MainFrame(wxT("My Chat"));

    wxLog::SetActiveTarget(new wxLogWindow(frame, "Log"));

    frame->Show(true);
    return true;
  }
};

wxIMPLEMENT_APP(MainApplication);
