#ifndef GITHUBAPI_H
#define GITHUBAPI_H 1

#include <curl/curl.h>
#include <string>
#include <vector>

class GithubAPI {
public:
  GithubAPI(std::string username);
  ~GithubAPI();

  bool DownloadAvatar();
  size_t GetAvatarBuffer(std::vector<unsigned char> &avatar_buffer);
  std::string GetUsername() { return username; }

private:
  bool ResetCURL();
  bool GetAvatarURL();
  bool GetAvatarImage();

  std::string username;
  std::string avatar_url;
  std::vector<unsigned char> avatar_buffer;

  CURL *curl;
};

#endif
