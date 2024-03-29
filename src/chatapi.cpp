#include "mychat.hpp"

using json = nlohmann::json;

const std::string ChatApi::ENDPOINT =
    "https://api.openai.com/v1/chat/completions";

ChatApi::ChatApi() {
  CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (res != CURLE_OK) {
    wxLogDebug("cURL global init failed: %s", curl_easy_strerror(res));
  }

  curl = curl_easy_init();

  std::ifstream infile("token.txt");
  if (!infile.good()) {
    wxLogDebug("Error: could not open file \"token.txt\"");
    return;
  }
  std::string token((std::istreambuf_iterator<char>(infile)),
                    (std::istreambuf_iterator<char>()));
  infile.close();

  std::string auth_header = "Authorization: Bearer ";
  auth_header.append(strip(token));
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, auth_header.c_str());
}

ChatApi::~ChatApi() {
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

std::string ChatApi::sendRequest(std::string request) {
  std::string response_data;
  std::string response;

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
  wxLogDebug("Filled out payload");

  std::string payload_string = payload.dump();
  wxLogDebug(payload_string.c_str());

  wxLogDebug("Ready to send request!");
  if (curl) {
    wxLogDebug("Configuring cURL");

    // Set the verbose option to 1 to enable debug messages
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(curl, CURLOPT_URL, ENDPOINT.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_string.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload_string.length());

    wxLogDebug("cURL configured. Sending request...");

    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
      wxLogDebug("cURL error: %s", curl_easy_strerror(result));
    } else {
      wxLogDebug("Response received!");
      wxLogDebug("Response: %s", response_data);
      json response_json = json::parse(response_data);
      response = response_json["choices"][0]["message"]["content"];
    }
  } else {
    wxLogDebug("Error: couldn't initialize cURL");
  }

  return response;
}

std::string ChatApi::strip(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r\v\f");
  size_t end = str.find_last_not_of(" \t\n\r\v\f");
  if (start != std::string::npos && end != std::string::npos) {
    return str.substr(start, end - start + 1);
  }
  return "";
}

size_t ChatApi::writeCb(void *cnts, size_t size, size_t nmemb, void *res) {
  ((std::string *)res)->append((char *)cnts, size * nmemb);
  return size * nmemb;
}
