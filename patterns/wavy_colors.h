#pragma once

#include "pattern.h"

namespace thirty_seven {

class WavyColors : public Pattern {
 public:
  inline void Update(const Env& env) final {
    for (int i = 0; i < kPixelCount; ++i) {
      int x = (i * 10 + env.ms() / 8) % 256;
      pixels_[i].SetHSV(x, 255, 255);
    }
  };
};

}  // namespace thirty_seven
