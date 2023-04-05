#include "mychat.hpp"

using json = nlohmann::json;
using namespace std;

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
  ChatApi chatApi = ChatApi();
  chatApi.send_request(request);
  inputField->SetValue(wxEmptyString);  // Clear the input field
};
