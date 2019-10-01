#pragma once

#include "pattern.h"

namespace thirty_seven {

class Zooom : public Pattern {
 public:
  inline void Update(const Env& env) final {
    for (int i = 0; i < kRingCount; ++i) {
      int h = 256 - (env.ms() / 8 + 16 * i) % 256;
      SetRingHSV(i, h, 255, 255);
    }
  };
  std::string name() const final { return "Zooom"; }
};

}  // namespace thirty_seven
