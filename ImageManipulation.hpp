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

class ImageManipulation {
public:
  ImageManipulation();
  ~ImageManipulation();

  void SetJpegFromBuffer(std::vector<unsigned char> &image_buffer);
  void SaveToFile(std::string filename);
  void Ukrainify(float opacity = 0.5f);

private:
  cimg_library::CImg<unsigned char> image;
};

#endif
