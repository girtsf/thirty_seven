#pragma once

#include <cassert>

#include "pixels.h"

namespace thirty_seven {

class Env;

class Pattern {
 public:
  static const int kPixelCount = 37;
  static const int kRingCount = 4;
  static const int kPixelCountPerRing[kRingCount];

  // Updates pixels for a new frame.
  virtual void Update(const Env& env) = 0;

  // Given LED count [0..kPixelCount-1), return which ring it is in, and the
  // number in the ring.
  static void GetPos(int num, int* ring, int* in_ring) {
    for (*ring = 0; *ring < kRingCount; (*ring)++) {
      if (num < kPixelCountPerRing[*ring]) {
        *in_ring = num;
        return;
      }
      num -= kPixelCountPerRing[*ring];
    }
    assert(false);
  }

  const thirty_seven::RGB& pixel(int i) const { return pixels_[i]; }

 protected:
  thirty_seven::RGB pixels_[kPixelCount];
};

}  // namespace thirty_seven
