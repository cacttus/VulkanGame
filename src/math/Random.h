/**
*  @file Random.h
*  @date July 15, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __RANDOM_311671063623511130142920_H__
#define __RANDOM_311671063623511130142920_H__

#include "../base/BaseHeader.h"
#include "../base/MachineTypes.h"
#include "../base/StringUtil.h"
#include "../math/Minimax.h"
#include "../math/Algorithm.h"

namespace VG {
/**
*  @class Random.h
*  @brief Random numbers and noise.
*/
class Random : public VirtualMemoryShared<Random> {
public:
  Random();
  Random(int32_t seed);
  virtual ~Random() override;
  FORCE_INLINE void setSeed(int32_t seed) { _last = seed; }

  // RNG. NOTE: Update to STL's mersenne (if faster).
  FORCE_INLINE int32_t static_mersenne(int32_t seed);

  // Noise
  FORCE_INLINE float noise_1d(int32_t x);

  // Scalar
  FORCE_INLINE int32_t nextInt32();
  FORCE_INLINE int32_t nextInt32(int32_t min, int32_t max);
  FORCE_INLINE uint32_t nextUint32(uint32_t minXyz, uint32_t maxXyz);
  FORCE_INLINE float nextFloat(float min, float max);
  FORCE_INLINE bool nextBool();

  // Vector
  FORCE_INLINE ivec3 nextIVec3(int32_t minXyz, int32_t maxXyz);
  FORCE_INLINE vec3 nextVec3(float min, float max);
  FORCE_INLINE ivec3 nextIVec3(const ivec3& minXyz, const ivec3& maxXyz);
  FORCE_INLINE vec3 nextVec3(const vec3& min, const vec3& max);
  FORCE_INLINE vec4 nextVec4(const vec4& min, const vec4& max);

  // Shortcuts
  FORCE_INLINE float frand01();  //Same as nextFloat(0,1)
  FORCE_INLINE float frand11();  //Same as nextFloat(-1,1)
  FORCE_INLINE vec3 rv301();     //same as nextVec3(0,1)
  FORCE_INLINE vec3 rv311();     //same as nextVec3(-1,1)

  // Algorithms
  FORCE_INLINE void shuffle(int32_t** xarr, int32_t count, int32_t nIterations = 1);
  string_t randomNumericString(int32_t nDigits);  // - Returns a random hash code (not unique)

private:
  int32_t _last = 1283094874;
};
#pragma region Noise
/**
*  @fn static_mersenne
*  @brief This mersenne function is based on a mersenne function with an index of 1.
*/
FORCE_INLINE int32_t Random::static_mersenne(int32_t seed) {
  int32_t i = 1, y;

  y = (0x6c078965 * (seed ^ (seed >> 30)) + i) & 0xffffffff;
  y = y ^ (y >> 11);
  y = y ^ ((y << 7) & 0x9d2c5680);
  y = y ^ ((y << 15) & 0xefc60000);
  y = y ^ (y >> 18);

  return y;
}
FORCE_INLINE float Random::noise_1d(int32_t x) {
  //This is a neato way to get the fractional part of a float.
  int32_t a = (static_mersenne(x) & 0x007fffff) | 0x40000000;
  return (*((float*)&a) - 3.0f);
}
#pragma endregion
#pragma region Scalar
FORCE_INLINE bool Random::nextBool() {
  return (nextInt32() % 2 == 0);  //even/odd=bool
}
FORCE_INLINE int32_t Random::nextInt32() {
#ifdef _USE_C_RAND
  return rand();
#else
  return (_last = Alg::mint(_last));
#endif
}
FORCE_INLINE int32_t Random::nextInt32(int32_t min, int32_t max) {
  AssertOrThrow2(max >= min);
  return min + (nextInt32() % (max - min + 1));
}
FORCE_INLINE float Random::nextFloat(float min, float max) {
#ifdef _USE_C_RAND
  float r = (float)nextInt() / RAND_MAX;  //[0,1]
  return (r * (max - min) + min);
#else
  float ret = Alg::meft01(nextInt32());
  ret *= (max - min);
  ret += min;
  return ret;
#endif
}
FORCE_INLINE uint32_t Random::nextUint32(uint32_t min, uint32_t max) {
  AssertOrThrow2(max >= min);
  return min + (((uint32_t)nextInt32()) % (max - min + 1));
}
#pragma endregion
#pragma region Vector
FORCE_INLINE ivec3 Random::nextIVec3(int32_t minXyz, int32_t maxXyz) {
  return ivec3(
      nextInt32(minXyz, maxXyz),
      nextInt32(minXyz, maxXyz),
      nextInt32(minXyz, maxXyz));
}
FORCE_INLINE vec3 Random::nextVec3(float minXyz, float maxXyz) {
  return vec3(
      nextFloat(minXyz, maxXyz),
      nextFloat(minXyz, maxXyz),
      nextFloat(minXyz, maxXyz));
}
FORCE_INLINE ivec3 Random::nextIVec3(const ivec3& minXyz, const ivec3& maxXyz) {
  return ivec3(
      nextInt32(minXyz.x, maxXyz.x),
      nextInt32(minXyz.y, maxXyz.y),
      nextInt32(minXyz.z, maxXyz.z));
}
FORCE_INLINE vec3 Random::nextVec3(const vec3& minXyz, const vec3& maxXyz) {
  return vec3(
      nextFloat(minXyz.x, maxXyz.x),
      nextFloat(minXyz.y, maxXyz.y),
      nextFloat(minXyz.z, maxXyz.z));
}
FORCE_INLINE vec4 Random::nextVec4(const vec4& minXyz, const vec4& maxXyz) {
  return vec4(
      nextFloat(minXyz.x, maxXyz.x),
      nextFloat(minXyz.y, maxXyz.y),
      nextFloat(minXyz.z, maxXyz.z),
      nextFloat(minXyz.w, maxXyz.w));
}
#pragma endregion
#pragma region Algorithms
FORCE_INLINE void Random::shuffle(int32_t** xarr, int32_t count, int32_t nIterations) {
  // @fn shuffle
  // @brief Randomly shuffle pointers
  // @details This allows us to simply iterate over the exhastive list of possible movements until we find one that works.
  int32_t shval;
  for (int niter = 0; niter < nIterations; ++niter) {
    for (int ni = 0; ni < count; ++ni) {
      shval = nextInt32(0, count - 1);
      int32_t* tmp = xarr[shval];
      xarr[shval] = xarr[ni];
      xarr[ni] = tmp;
    }
  }
}
#pragma endregion
#pragma region Shortcuts
FORCE_INLINE float Random::frand01() {
  return nextFloat(0.0f, 1.0f);
}
FORCE_INLINE float Random::frand11() {
  return nextFloat(-1.0f, 1.0f);
}
FORCE_INLINE vec3 Random::rv301() {
  return vec3(
      nextFloat(0.0f, 1.0f),
      nextFloat(0.0f, 1.0f),
      nextFloat(0.0f, 1.0f));
}
FORCE_INLINE vec3 Random::rv311() {
  return vec3(
      nextFloat(-1.0f, 1.0f),
      nextFloat(-1.0f, 1.0f),
      nextFloat(-1.0f, 1.0f));
}
#pragma endregion

}  // namespace VG

#endif
