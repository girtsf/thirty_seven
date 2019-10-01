#pragma once

#include "pattern.h"

namespace thirty_seven {

class MovingLights : public Pattern {
 public:
  inline void Update(const Env& env) final {
    // Make everything black.
    for (int i = 0; i < kPixelCount; ++i) {
      pixels_[i].SetRGB(0, 0, 0);
    }
    // TODO: implement.
  };
  std::string name() const final { return "MovingLights"; }
};

}  // namespace thirty_seven
