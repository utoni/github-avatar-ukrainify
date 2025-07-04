#ifndef IMAGEMANIPULATION_H
#define IMAGEMANIPULATION_H 1

/* *************** */
// required for CImg.h
#include <cstdio>
#include <jerror.h>
#include <jpeglib.h>
/* *************** */

#include "CImg.h"

#include <string>
#include <vector>

enum ImageFormat { IF_UNSUPPORTED, IF_JPEG, IF_PNG };

class ImageManipulation {
public:
  ImageManipulation();
  ~ImageManipulation();

  void LoadImage(const std::string & filename);
  bool SetImageFromBuffer(std::vector<unsigned char> &image_buffer);
  void SaveToFile(std::string filename);
  void Ukrainify(float opacity = 0.5f);
  void Israelify(float opacity = 0.75f);

private:
  cimg_library::CImg<unsigned char> image;

  enum ImageFormat GetImageFormat(std::vector<unsigned char> &image_buffer);
};

#endif
