#include "GithubAPI.hpp"
#include "json.hpp"

#include <curl/curl.h>
#include <fstream>
#include <iostream>

GithubAPI::GithubAPI(std::string username)
    : username(username), avatar_url(), avatar_buffer() {
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
}

GithubAPI::~GithubAPI() {
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

static size_t WriteToStringCallback(void *contents, size_t size, size_t nmemb,
                                    void *userp) {
  std::string *const out = (std::string *)userp;

  out->append((char *)contents, size * nmemb);
  return size * nmemb;
}

static size_t WriteToBufferCallback(void *contents, size_t size, size_t nmemb,
                                    void *userp) {
  std::vector<unsigned char> *const out = (std::vector<unsigned char> *)userp;

  out->insert(out->end(), (unsigned char *)contents,
              (unsigned char *)contents + (size * nmemb));
  return size * nmemb;
}

bool GithubAPI::DownloadAvatar() {
  if (GetAvatarURL() == false) {
    return false;
  }

  return GetAvatarImage();
}

size_t GithubAPI::GetAvatarBuffer(std::vector<unsigned char> &avatar_buffer) {
  avatar_buffer = this->avatar_buffer;
  return avatar_buffer.size();
}

bool GithubAPI::AvatarToFile(std::string filename) {
  std::vector<unsigned char> avatar_buffer;

  if (GetAvatarBuffer(avatar_buffer) == 0) {
    return false;
  }

  {
    std::ofstream avout(filename, std::ios::out | std::ios::binary);
    avout.write((char const *)avatar_buffer.data(), avatar_buffer.size());
    avout.close();
  }

  return true;
}

bool GithubAPI::ResetCURL() {
  if (curl == nullptr) {
    return false;
  }

  curl_easy_reset(curl);
  return true;
}

bool GithubAPI::GetAvatarURL() {
  bool ret = true;
  CURLcode res;
  struct curl_slist *headers = NULL;
  std::string json_str;
  nlohmann::json json;

  std::string gURL = "https://api.github.com/users/" + username;

  if (ResetCURL() == false) {
    ret = false;
    goto failure;
  }

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  curl_easy_setopt(curl, CURLOPT_URL, gURL.c_str());
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "github-avatar-ukrainify/0.1");

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToStringCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json_str);

  res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "libcurl returned: " << curl_easy_strerror(res) << std::endl;
    ret = false;
    goto failure;
  }

  json = nlohmann::json::parse(json_str);
  avatar_url = json["avatar_url"];
  CheckReturnedUsername(json["login"]);
failure:
  curl_slist_free_all(headers);

  return ret;
}

bool GithubAPI::GetAvatarImage() {
  bool ret = true;
  CURLcode res;
  struct curl_slist *headers = NULL;

  if (ResetCURL() == false) {
    ret = false;
    goto failure;
  }

  avatar_buffer.clear();
  headers = curl_slist_append(headers, "Accept: image/*");

  curl_easy_setopt(curl, CURLOPT_URL, avatar_url.c_str());
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "github-avatar-ukrainify/0.1");

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToBufferCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &avatar_buffer);

  res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "libcurl returned: " << curl_easy_strerror(res) << std::endl;
    ret = false;
    goto failure;
  }

failure:
  curl_slist_free_all(headers);

  return ret;
}

void GithubAPI::CheckReturnedUsername(std::string const & github_username) {
  std::string returned_username = github_username;
  std::string desired_username = username;

  std::transform(returned_username.begin(), returned_username.end(), returned_username.begin(),
    [](unsigned char c){ return std::tolower(c); });
  std::transform(desired_username.begin(), desired_username.end(), desired_username.begin(),
    [](unsigned char c){ return std::tolower(c); });

  if (returned_username != desired_username) {
    std::cerr << "Username mismatch (Github != CmdArg): " << github_username << " != " << desired_username
              << std::endl;
  }
}
