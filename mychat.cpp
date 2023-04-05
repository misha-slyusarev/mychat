#include "mychat.hpp"

using json = nlohmann::json;
using namespace std;

std::string format_string(std::string &t, const std::string &s) {
  size_t index = t.find("$");
  if (index != std::string::npos) {
    t.replace(index, 1, s);
  }
  return t;
}

// remove whitespaces and technical symbols from a string
std::string strip(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r\v\f");
  size_t end = str.find_last_not_of(" \t\n\r\v\f");
  if (start != std::string::npos && end != std::string::npos) {
    return str.substr(start, end - start + 1);
  }
  return "";
}

// write callback for cURL
static size_t write_cb(void *cnts, size_t size, size_t nmemb, void *res) {
  ((std::string *)res)->append((char *)cnts, size * nmemb);
  return size * nmemb;
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
  const std::string endpoint = "https://api.openai.com/v1/chat/completions";

  std::string response_data;
  std::string auth_header;

  CURL *curl;

  wxString request = inputField->GetValue();
  wxLogDebug(request);

  json payload = {
      {"model", "gpt-3.5-turbo"}, {"temperature", 1.0},
      {"max_tokens", 500},        {"top_p", 1.0},
      {"frequency_penalty", 0.0}, {"presence_penalty", 0.6},
      {"stream", false},
  };
  json messages = json::array();
  messages.push_back({{"role", "user"}, {"content", request.ToStdString()}});
  payload["messages"] = messages;

  std::string payload_string = payload.dump();
  wxLogDebug(payload_string.c_str());

  std::ifstream infile("token.txt");
  if (!infile.good()) {
    wxLogDebug("Error: could not open file \"token.txt\"");
    return;
  }
  std::string token((std::istreambuf_iterator<char>(infile)),
                    (std::istreambuf_iterator<char>()));
  infile.close();

  auth_header.append("Authorization: Bearer ").append(strip(token));
  wxLogDebug(auth_header.c_str());

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, auth_header.c_str());

  if (curl) {
    wxLogDebug("Sending request..");
    // Set the verbose option to 1 to enable debug messages
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_string.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload_string.length());

    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
      wxLogDebug("cURL error: %s", curl_easy_strerror(result));
    } else {
      wxLogDebug("Response received!");
      wxLogDebug("Response: %s", response_data);
    }
  } else {
    wxLogDebug("Error: couldn't initialize cURL");
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
};
