#include "../math/Random.h"
#include "../base/TypeConv.h"

#include <time.h>

namespace VG {
Random::Random() {
  setSeed((int32_t)time(NULL));
}
Random::Random(int32_t seed) {
  setSeed(seed);
#ifdef _USE_C_RAND
  srand((unsigned int)_last);
#endif
}
Random::~Random() {
}
string_t Random::randomNumericString(int nDigits) {
  string_t ret;
  for (int i = 0; i < nDigits; ++i) {
    ret += TypeConv::intToStr(nextInt32(0, 9));
  }
  return ret;
}


}//ns game
