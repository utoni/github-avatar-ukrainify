#ifndef GITHUBAPI_H
#define GITHUBAPI_H 1

#include <string>
#include <vector>

typedef void CURL;

class GithubAPI {
public:
  GithubAPI(std::string username);
  ~GithubAPI();

  bool DownloadAvatar();
  size_t GetAvatarBuffer(std::vector<unsigned char> &avatar_buffer);
  bool AvatarToFile(std::string filename);
  std::string GetUsername() { return username; }

private:
  bool ResetCURL();
  bool GetAvatarURL();
  bool GetAvatarImage();
  void CheckReturnedUsername(std::string const & github_username);

  std::string username;
  std::string avatar_url;
  std::vector<unsigned char> avatar_buffer;

  CURL *curl;
};

#endif
