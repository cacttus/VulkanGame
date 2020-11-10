#include "../math/Minimax.h"
#include "../math/Math.h"
#include "../math/Random.h"
namespace VG {

float MpFloat::next() {
  checkWasSet();
  return Math::random()->nextFloat(_min, _max);
}
vec3 MpVec3::next() {
  checkWasSet();
  return Math::random()->nextVec3(_min, _max);
}
ivec3 Mpivec3::next() {
  checkWasSet();
  return Math::random()->nextIVec3(_min, _max);
}
vec4 MpVec4::next() {
  checkWasSet();
  return Math::random()->nextVec4(_min, _max);
}
uint32_t MpUint::next() {
  checkWasSet();
  return Math::random()->nextUint32(_min, _max);
}
int32_t MpInt::next() {
  checkWasSet();
  return Math::random()->nextInt32(_min, _max);
}




}

