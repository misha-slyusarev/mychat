#include <curl/curl.h>
#include <wx/log.h>
#include <wx/wx.h>

#include <fstream>
#include <string>

class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title) {
    inputField = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxSize(200, 24));

    submitButton = new wxButton(this, wxID_ANY, "Submit", wxDefaultPosition,
                                wxSize(80, 24));
    submitButton->Bind(wxEVT_BUTTON, &MainFrame::OnSubmit, this);

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(inputField, wxSizerFlags().Expand().Border(wxALL, 20));
    sizer->Add(submitButton, wxSizerFlags().Border(wxALL, 20));

    SetSizer(sizer);
    Centre();
  }

 private:
  void OnSubmit(wxCommandEvent &event) {
    wxString inputValue = inputField->GetValue();
    wxLogDebug(inputValue);

    std::ifstream infile("token.txt");
    std::string content((std::istreambuf_iterator<char>(infile)),
                        (std::istreambuf_iterator<char>()));

    if (!infile.good()) {
      wxLogDebug("Error: could not open file \"myfile.txt\"");
      return;
    }

    infile.close();

    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    CURL *curl = curl_easy_init();

    if (curl) {
      curl_easy_setopt(
          curl, CURLOPT_URL,
          "https://api.openai.com/v1/engines/davinci-codex/completions");
      wxLogDebug("Success!");
    } else {
      wxLogDebug("Error: couldn't initialize cURL");
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }

  wxTextCtrl *inputField;
  wxButton *submitButton;
};

class MainApplication : public wxApp {
 public:
  bool OnInit() override {
    MainFrame *frame = new MainFrame("My Chat");

    wxLog::SetActiveTarget(new wxLogWindow(frame, "Log"));

    frame->Show(true);
    return true;
  }
};

wxIMPLEMENT_APP(MainApplication);
