#include "GithubAPI.hpp"
#include "ImageManipulation.hpp"

#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << (argc > 0 ? argv[0] : "github-avatar-ukrainify")
              << " [github-profile-username]" << std::endl;
    return 1;
  }

  GithubAPI gAPI(argv[1]);
  ImageManipulation iM;

  if (gAPI.DownloadAvatar() == false) {
    return 1;
  }

  {
    std::vector<unsigned char> avatar;

    if (gAPI.GetAvatarBuffer(avatar) == 0) {
      return 1;
    }

    iM.SetJpegFromBuffer(avatar);
    iM.SaveToFile(gAPI.GetUsername() + ".jpg");
  }

  iM.Ukrainify(0.3f);
  iM.SaveToFile(gAPI.GetUsername() + ".modified.jpg");

  return 0;
}
