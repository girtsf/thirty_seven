// Environment.
//
// Probably just time, but maybe we'll have something else in here too?
class Env {
 public:
  inline Env(uint32_t us) : us_(us) {}

  inline uint32_t us() const { return us_; }
  // Note that these time functions are approximate.
  inline uint32_t ms() const { return us_ / 1024; }

 private:
  uint32_t us_;
};
