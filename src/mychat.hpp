#include <wx/frame.h>
#include <wx/button.h>
#include <wx/textctrl.h>

#include <curl/curl.h>
#include <wx/log.h>
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/button.h>

#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

class ChatApi 
{
public:
  ChatApi();
  ~ChatApi();

  void send_request(std::string request);
  std::string strip(const std::string &str);

  static size_t write_cb(void *cnts, size_t size, size_t nmemb, void *res);

private:
  static const std::string ENDPOINT;

  CURL *curl;
  struct curl_slist *headers = NULL;
};

class MainFrame : public wxFrame
{
public:
  MainFrame(const wxString &title);

private:
  void OnSubmit(wxCommandEvent &event);

  wxTextCtrl *inputField;
  wxButton *submitButton;
};


