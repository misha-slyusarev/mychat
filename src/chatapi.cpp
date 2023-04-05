#include "mychat.hpp"

using json = nlohmann::json;

ChatApi::ChatApi() {
  CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (res != CURLE_OK) {
    wxLogDebug("cURL global init failed: %s", curl_easy_strerror(res));
  }
}

ChatApi::~ChatApi() { curl_global_cleanup(); }

void ChatApi::send_request(std::string request) {
  const std::string endpoint = "https://api.openai.com/v1/chat/completions";

  std::string response_data;
  std::string auth_header;

  wxLogDebug(request.c_str());

  json payload = {
      {"model", "gpt-3.5-turbo"}, {"temperature", 1.0},
      {"max_tokens", 500},        {"top_p", 1.0},
      {"frequency_penalty", 0.0}, {"presence_penalty", 0.6},
      {"stream", false},
  };
  json messages = json::array();
  messages.push_back({{"role", "user"}, {"content", request}});
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
  // wxLogDebug(auth_header.c_str());

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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ChatApi::write_cb);
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
}

std::string ChatApi::strip(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r\v\f");
  size_t end = str.find_last_not_of(" \t\n\r\v\f");
  if (start != std::string::npos && end != std::string::npos) {
    return str.substr(start, end - start + 1);
  }
  return "";
}

size_t ChatApi::write_cb(void *cnts, size_t size, size_t nmemb, void *res) {
  ((std::string *)res)->append((char *)cnts, size * nmemb);
  return size * nmemb;
}
