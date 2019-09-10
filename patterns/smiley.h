#pragma once

#include "pattern.h"

namespace thirty_seven {

class Smiley : public Pattern {
 public:
  inline void Update(const Env& env) final {
    // Make everything black.
    for (int i = 0; i < kPixelCount; ++i) {
      pixels_[i].SetRGB(0, 0, 0);
    }
    // Except for these guys.
    for (int i : {9, 17, 25, 26, 27, 28, 29, 30, 31}) {
      pixels_[i].SetHSV((env.ms() / 8) % 256, 255, 255);
    }
  };
  std::string name() const final { return "Smiley"; }
};

}  // namespace thirty_seven
