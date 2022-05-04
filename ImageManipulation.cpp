#include "ImageManipulation.hpp"

#include <iostream>

ImageManipulation::ImageManipulation() {}

ImageManipulation::~ImageManipulation() {}

void ImageManipulation::SetJpegFromBuffer(
    std::vector<unsigned char> &image_buffer) {
  image.load_jpeg_buffer(image_buffer.data(), image_buffer.size());
}

void ImageManipulation::SaveToFile(std::string filename) {
  image.save(filename.c_str());
}

void ImageManipulation::Ukrainify(float opacity) {
  size_t const channels = 3;
  unsigned char upper_color[channels] = {0, 87, 184};
  unsigned char lower_color[channels] = {255, 215, 0};

  if (image.spectrum() != channels) {
    std::cerr << "Expected an image with " << channels << " Channels, but got "
              << image.spectrum() << std::endl;
    return;
  }

  image.draw_rectangle(0, 0, image.width(), image.height() / 2, upper_color,
                       opacity);
  image.draw_rectangle(0, image.height() / 2, image.width(), image.height(),
                       lower_color, opacity);
}
