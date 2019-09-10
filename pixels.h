#pragma once

#include <cstdint>

#include "third_party/fastled_hsv2rgb/hsv2rgb.h"

namespace thirty_seven {

// One RGB pixel.
class RGB {
 public:
  // default constructor
  RGB() {}
  RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b() {}

  void SetHSV(uint8_t h, uint8_t s, uint8_t v) { hsv2rgb(h, s, v, &r, &g, &b); }

  uint8_t r = 0, g = 0, b = 0;
};

}  // namespace thirty_seven
