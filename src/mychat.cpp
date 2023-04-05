#include "mychat.hpp"

using json = nlohmann::json;
using namespace std;

string format_string(string &t, const string &s) {
  size_t index = t.find("$");
  if (index != string::npos) {
    t.replace(index, 1, s);
  }
  return t;
}

MainFrame::MainFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title) {
  inputField = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxSize(200, 24));

  submitButton =
      new wxButton(this, wxID_ANY, "Submit", wxDefaultPosition, wxSize(80, 24));
  submitButton->Bind(wxEVT_BUTTON, &MainFrame::OnSubmit, this);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(inputField, wxSizerFlags().Expand().Border(wxALL, 20));
  sizer->Add(submitButton, wxSizerFlags().Border(wxALL, 20));

  SetSizer(sizer);
  Centre();
}

void MainFrame::OnSubmit(wxCommandEvent &event) {
  string request = inputField->GetValue().ToStdString();
  return send_request(request);
};
