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

class MainFrame : public wxFrame
{
public:
  MainFrame(const wxString &title);

private:
  void OnSubmit(wxCommandEvent &event);

  wxTextCtrl *inputField;
  wxButton *submitButton;
};

void send_request(std::string request);
