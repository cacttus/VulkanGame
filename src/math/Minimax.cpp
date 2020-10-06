#include "../math/Minimax.h"
#include "../math/Random.h"
#include "../base/Gu.h"
namespace BR2 {

float MpFloat::next() {
  checkWasSet();
  return Gu::getRandom()->nextFloat(_min, _max);
}
vec3 MpVec3::next() {
  checkWasSet();
  return Gu::getRandom()->nextVec3(_min, _max);
}
ivec3 Mpivec3::next() {
  checkWasSet();
  return Gu::getRandom()->nextIVec3(_min, _max);
}
vec4 MpVec4::next() {
  checkWasSet();
  return Gu::getRandom()->nextVec4(_min, _max);
}
uint32_t MpUint::next() {
  checkWasSet();
  return Gu::getRandom()->nextUint32(_min, _max);
}
int32_t MpInt::next() {
  checkWasSet();
  return Gu::getRandom()->nextInt32(_min, _max);
}




}

