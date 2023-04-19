#include "mychat.hpp"

using json = nlohmann::json;
using namespace std;

MainFrame::MainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 780)) {
  chatApi = new ChatApi();

  chatWindow = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

  inputField = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_PROCESS_ENTER);

  submitButton =
      new wxButton(this, wxID_ANY, "Send", wxDefaultPosition, wxDefaultSize);

  // Assign events to form items
  inputField->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnSubmit, this);
  submitButton->Bind(wxEVT_BUTTON, &MainFrame::OnSubmit, this);

  // Manage items layout
  wxBoxSizer* chatWindowSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* inputSizer = new wxBoxSizer(wxHORIZONTAL);

  chatWindowSizer->Add(chatWindow, wxSizerFlags(1).Expand().Border(wxALL, 20));
  chatWindowSizer->Add(inputSizer, wxSizerFlags().Border(wxALL, 20).Expand());
  inputSizer->Add(inputField, wxSizerFlags().Border(wxALL, 5).Proportion(1));
  inputSizer->Add(submitButton, wxSizerFlags().Border(wxALL, 5).Proportion(0));

  SetSizer(chatWindowSizer);
  Centre();
}

void MainFrame::OnSubmit(wxCommandEvent& event) {
  string request = inputField->GetValue().ToStdString();
  string response = chatApi->sendRequest(request);

  // Append the user's message to the chat text control
  chatWindow->AppendText("You: " + request + "\n");
  chatWindow->AppendText("They: " + response + "\n");

  inputField->SetValue(wxEmptyString);  // Clear the input field
};
