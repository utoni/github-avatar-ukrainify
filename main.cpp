#ifndef LOCAL_FILE
#include "GithubAPI.hpp"
#endif

#ifndef DOWNLOAD_ONLY
#include "ImageManipulation.hpp"
#endif

#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << (argc > 0 ? argv[0] : "github-avatar-ukrainify")
              << " [github-profile-username]" << std::endl;
    return 1;
  }

#ifndef LOCAL_FILE
  GithubAPI gAPI(argv[1]);
  if (gAPI.DownloadAvatar() == false) {
    return 1;
  }
#endif
#ifndef DOWNLOAD_ONLY
  ImageManipulation iM;
#endif

#if defined(DOWNLOAD_ONLY) && !defined(LOCAL_FILE)
  std::string outfile = "./" + gAPI.GetUsername() + ".jpg";
  std::cout << "Saving Avatar from '" << gAPI.GetUsername() << "' to '" << outfile << "'" << std::endl;
  if (gAPI.AvatarToFile(outfile) == false)
  {
    return 1;
  }
#else
  std::string file_basename;
  {
    std::vector<unsigned char> avatar;
#ifdef LOCAL_FILE
    file_basename = "out";
    iM.LoadImage(std::string(argv[1]));
#else
    file_basename = gAPI.GetUsername();
    if (gAPI.GetAvatarBuffer(avatar) == 0) {
      return 1;
    }
    if (iM.SetImageFromBuffer(avatar) == false) {
      return 1;
    }
    iM.SaveToFile(file_basename + ".jpg");
#endif
  }

  iM.Ukrainify(0.3f);
  iM.Israelify(0.3f);
  iM.SaveToFile(file_basename + ".modified.jpg");
#endif

  return 0;
}
