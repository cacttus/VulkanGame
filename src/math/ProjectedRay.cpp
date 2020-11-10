
#include "../math/MathAll.h"

namespace VG {
vec3 ProjectedRay::getHitPoint(PlaneHit& ph) const {
  vec3 ret = getBegin() + getDir() * ph.getHitTime();
  return ret;
}
}