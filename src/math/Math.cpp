#include "../base/BaseHeader.h"
#include "../math/Math.h"
#include "../math/Random.h"

namespace BR2 {
std::shared_ptr<Random> Math::_random = nullptr;

void Math::initGlobals() {
  Math::_random = std::make_shared <Random>();
}

}  // namespace BR2
