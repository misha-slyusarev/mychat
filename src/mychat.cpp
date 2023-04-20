#include "mychat.hpp"

#include <future>
#include <thread>

using json = nlohmann::json;

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
  inputField->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnSubmitAsync, this);
  submitButton->Bind(wxEVT_BUTTON, &MainFrame::OnSubmitAsync, this);

  // Set input field's font size
  wxFont font = inputField->GetFont();
  font.SetPointSize(font.GetPointSize() + 5);
  inputField->SetFont(font);

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

void MainFrame::OnSubmitAsync(wxCommandEvent& event) {
  std::string request = inputField->GetValue().ToStdString();
  chatWindow->AppendText("You: " + request + "\n");

  // Clear input field
  inputField->SetValue(wxEmptyString);

  auto future = std::async(std::launch::async, [this, request]() {
    return chatApi->sendRequest(request);
  });

  // Execute API request on a separate thread
  std::thread([this, future = std::move(future)]() mutable {
    // Update chat window with response once it is received
    std::string response = future.get();
    chatWindow->AppendText("They: " + response + "\n");
  }).detach();
};
