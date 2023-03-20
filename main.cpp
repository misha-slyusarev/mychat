#include <curl/curl.h>
#include <wx/log.h>
#include <wx/wx.h>

#include <fstream>
#include <string>

std::string format_string(std::string &t, const std::string &s) {
  size_t index = t.find("$");
  if (index != std::string::npos) {
    t.replace(index, 1, s);
  }
  return t;
}

static size_t write_cb(void *cnts, size_t size, size_t nmemb, void *res) {
  ((std::string *)res)->append((char *)cnts, size * nmemb);
  return size * nmemb;
}

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
    std::string response_data;
    std::string auth_header;

    CURL *curl;

    const std::string endpoint = "https://api.openai.com/v1/chat/completions";
    std::string payload_template = R"(
      {
        "model": "gpt-3.5-turbo",
        "messages": [{"role": "user", "content": "$"}],
        "temperature": 0.7
      }
    )";

    wxString request = inputField->GetValue();
    wxLogDebug(request);

    std::string payload =
        format_string(payload_template, request.ToStdString());

    wxLogDebug(payload.c_str());

    std::ifstream infile("token.txt");

    if (!infile.good()) {
      wxLogDebug("Error: could not open file \"token.txt\"");
      return;
    }
    std::string token((std::istreambuf_iterator<char>(infile)),
                      (std::istreambuf_iterator<char>()));

    infile.close();
    auth_header = "Authorization: Bearer " + token;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());

    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
      curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

      CURLcode result = curl_easy_perform(curl);

      if (result != CURLE_OK) {
        wxLogDebug("cURL error: %s", curl_easy_strerror(result));
      } else {
        wxLogDebug("Response: %s", response_data);
      }

      wxLogDebug("Success!");
    } else {
      wxLogDebug("Error: couldn't initialize cURL");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }

  wxTextCtrl *inputField;
  wxButton *submitButton;
};

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
