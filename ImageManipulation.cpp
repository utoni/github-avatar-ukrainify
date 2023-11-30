#include "ImageManipulation.hpp"

#include <iostream>

ImageManipulation::ImageManipulation() {}

ImageManipulation::~ImageManipulation() {}

struct png_memory_io {
  std::vector<unsigned char> &image_buffer;
  size_t offset;
};

static void read_png_buffer_from_memory(png_structp png_ptr, png_bytep outBytes,
                                        png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    std::cerr << "Read PNG buffer from memory failed: IO Pointer NULL"
              << std::endl;
    return;
  }

  struct png_memory_io *pmi = (struct png_memory_io *)io_ptr;
  size_t maxBytesToRead =
      (pmi->image_buffer.size() - pmi->offset > byteCountToRead
           ? byteCountToRead
           : pmi->image_buffer.size() - pmi->offset);
  memcpy(outBytes, pmi->image_buffer.data() + pmi->offset, maxBytesToRead);
  pmi->offset += maxBytesToRead;
}

// More or less copy-pasta from CImg.h's _load_png(...)
static bool load_png_buffer(std::vector<unsigned char> &image_buffer,
                            cimg_library::CImg<unsigned char> &image,
                            unsigned int *const bits_per_value) {
  png_structp png_ptr;
  png_infop info_ptr, end_info;
  struct png_memory_io pmi = {.image_buffer = image_buffer, .offset = 0};

  png_uint_32 W, H;
  int bit_depth, color_type, interlace_type;
  bool is_gray = false;

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return false;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return false;
  }

  end_info = png_create_info_struct(png_ptr);
  if (end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return false;
  }

  png_set_read_fn(png_ptr, &pmi, read_png_buffer_from_memory);
  png_set_sig_bytes(png_ptr, 0);
  png_read_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, &W, &H, &bit_depth, &color_type,
               &interlace_type, NULL, NULL);
  png_set_interlace_handling(png_ptr);
  if (bits_per_value != NULL) {
    *bits_per_value = (unsigned int)bit_depth;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
    color_type = PNG_COLOR_TYPE_RGB;
    bit_depth = 8;
  }
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png_ptr);
    is_gray = true;
    bit_depth = 8;
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0) {
    png_set_tRNS_to_alpha(png_ptr);
    color_type |= PNG_COLOR_MASK_ALPHA;
  }
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png_ptr);
    color_type |= PNG_COLOR_MASK_COLOR;
    is_gray = true;
  }
  if (color_type == PNG_COLOR_TYPE_RGB) {
    png_set_filler(png_ptr, 0xffffU, PNG_FILLER_AFTER);
  }

  png_read_update_info(png_ptr, info_ptr);
  if (bit_depth != 8 && bit_depth != 16) {
    png_destroy_read_struct(&png_ptr, &end_info, NULL);
    return false;
  }
  const int byte_depth = bit_depth >> 3;

  png_bytep *const imgData = new png_bytep[H];
  for (unsigned int row = 0; row < H; ++row) {
    imgData[row] = new png_byte[(size_t)byte_depth * 4 * W];
  }
  png_read_image(png_ptr, imgData);
  png_read_end(png_ptr, end_info);

  if (color_type != PNG_COLOR_TYPE_RGB &&
      color_type != PNG_COLOR_TYPE_RGB_ALPHA) {
    png_destroy_read_struct(&png_ptr, &end_info, (png_infopp)0);
    return false;
  }
  const bool is_alpha = (color_type == PNG_COLOR_TYPE_RGBA);
  try {
    image.assign(W, H, 1,
                 (is_gray == true ? 1 : 3) + (is_alpha == true ? 1 : 0));
  } catch (...) {
    throw;
  }

  unsigned char *ptr_r = image.data(0, 0, 0, 0);
  unsigned char *ptr_g = (is_gray == true ? 0 : image.data(0, 0, 0, 1));
  unsigned char *ptr_b = (is_gray == true ? 0 : image.data(0, 0, 0, 2));
  unsigned char *ptr_a =
      (is_alpha == false ? 0 : image.data(0, 0, 0, is_gray == true ? 1 : 3));

  switch (bit_depth) {
  case 8: {
    cimg_forY(image, y) {
      const unsigned char *ptrs = imgData[y];
      cimg_forX(image, x) {
        *(ptr_r++) = *(ptrs++);
        if (ptr_g)
          *(ptr_g++) = *(ptrs++);
        else
          ++ptrs;
        if (ptr_b)
          *(ptr_b++) = *(ptrs++);
        else
          ++ptrs;
        if (ptr_a)
          *(ptr_a++) = *(ptrs++);
        else
          ++ptrs;
      }
    }
  } break;
  case 16: {
    cimg_forY(image, y) {
      unsigned short const *ptrs = (unsigned short *)imgData[y];
      if (cimg_library::cimg::endianness() == false) {
        cimg_library::cimg::invert_endianness(ptrs, 4 * image._width);
      }
      cimg_forX(image, x) {
        *(ptr_r++) = *(ptrs++);
        if (ptr_g)
          *(ptr_g++) = *(ptrs++);
        else
          ++ptrs;
        if (ptr_b)
          *(ptr_b++) = *(ptrs++);
        else
          ++ptrs;
        if (ptr_a)
          *(ptr_a++) = *(ptrs++);
        else
          ++ptrs;
      }
    }
  } break;
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

  cimg_forY(image, n) { delete[] imgData[n]; }
  delete[] imgData;

  return true;
}

bool ImageManipulation::SetImageFromBuffer(
    std::vector<unsigned char> &image_buffer) {
  switch (GetImageFormat(image_buffer)) {
  case IF_UNSUPPORTED:
    goto failure;
  case IF_JPEG:
    image.load_jpeg_buffer(image_buffer.data(), image_buffer.size());
    return true;
  case IF_PNG:
    return load_png_buffer(image_buffer, image, NULL);
  }

failure:
  std::cerr << "Unsupported Image Format.." << std::endl;
  return false;
}

void ImageManipulation::SaveToFile(std::string filename) {
  image.save(filename.c_str());
}

void ImageManipulation::Ukrainify(float opacity) {
  size_t const channels = 3;
  unsigned char upper_color[channels] = {0, 87, 184};
  unsigned char lower_color[channels] = {255, 215, 0};

  image.draw_rectangle(0, 0, image.width(), image.height() / 2, upper_color,
                       opacity);
  image.draw_rectangle(0, image.height() / 2, image.width(), image.height(),
                       lower_color, opacity);
}

enum ImageFormat
ImageManipulation::GetImageFormat(std::vector<unsigned char> &image_buffer) {
  if (image_buffer.size() < 8) {
    return IF_UNSUPPORTED;
  }

  if (image_buffer[0] == 0xFF && image_buffer[1] == 0xD8 &&
      image_buffer[2] == 0xFF) {
    return IF_JPEG;
  }

  if (png_sig_cmp(image_buffer.data(), 0, 8) == 0) {
    return IF_PNG;
  }

  return IF_UNSUPPORTED;
}
