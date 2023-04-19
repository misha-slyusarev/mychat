#include "mychat.hpp"

using json = nlohmann::json;
using namespace std;

MainFrame::MainFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title) {
  chatApi = new ChatApi();

  chatTextCtrl =
      new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                     wxSize(300, 200), wxTE_MULTILINE | wxTE_READONLY);

  inputField = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxSize(300, 24));

  submitButton =
      new wxButton(this, wxID_ANY, "Send", wxDefaultPosition, wxSize(80, 24));
  submitButton->Bind(wxEVT_BUTTON, &MainFrame::OnSubmit, this);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(chatTextCtrl, wxSizerFlags().Expand().Border(wxALL, 20));
  sizer->Add(inputField, wxSizerFlags().Expand().Border(wxALL, 20));
  sizer->Add(submitButton, wxSizerFlags().Border(wxALL, 20));

  SetSizer(sizer);
  Centre();
}

void MainFrame::OnSubmit(wxCommandEvent &event) {
  string request = inputField->GetValue().ToStdString();
  string response = chatApi->sendRequest(request);

  // Append the user's message to the chat text control
  chatTextCtrl->AppendText("You: " + request + "\n");
  chatTextCtrl->AppendText("They: " + response + "\n");

  inputField->SetValue(wxEmptyString);  // Clear the input field
};
