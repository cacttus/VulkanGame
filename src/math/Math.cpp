#include "../base/BaseHeader.h"
#include "../math/Math.h"
#include "../math/Random.h"

namespace VG {
std::shared_ptr<Random> Math::_random = nullptr;

void Math::initGlobals() {
  Math::_random = std::make_shared <Random>();
}

}  // namespace VG
