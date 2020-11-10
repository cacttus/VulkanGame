/**
*  @file MathUtils.h
*  @date 10/10/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __MATHUTILS_16050053987699800801201475696_H__
#define __MATHUTILS_16050053987699800801201475696_H__

#include "./MathHeader.h"

namespace BR2 {

class Math : public VirtualMemory {
public:
  static void initGlobals();
  static std::shared_ptr<Random> random() { return _random; }

  FORCE_INLINE static bool isEven(int32_t i) {
    return ((i % 2) == 0);
  }
  FORCE_INLINE static bool isOdd(int32_t i) {
    return ((i % 2) != 0);
  }
  FORCE_INLINE static double brSqrt(double x) {
    return std::sqrt(x);
  }
  FORCE_INLINE static float brSqrt(float x) {
    //sqrtf undefined in clang..
    return (float)std::sqrt((double)x);
  }
  FORCE_INLINE static float brRsqrt(float x) {
    return 1.0f / brSqrt(x);
  }
  FORCE_INLINE static double brRsqrt(double x) {
    return 1.0 / std::sqrt(x);
  }
  FORCE_INLINE static float sqrf(float x) {
    return x * x;
  }
  static constexpr double radians(double d) {
    return (double)((d * (double)(M_PI)) / (double)180.0);
  }
  FORCE_INLINE static float degrees(float d) {
    return (float)((d * (float)(180.0)) / (float)M_PI);
  }
  FORCE_INLINE static int32_t getNumberOfDigits(int32_t i) {
    return i > 0 ? (int)log10f((float)i) + 1 : 1;
  }
  FORCE_INLINE static uint32_t getNumberOfDigits(uint32_t i) {
    return i > 0 ? (int)log10((double)i) + 1 : 1;
  }
#ifndef BR_MATH_USE_STL
  //Round is defined in the new MSVC lib.
  FORCE_INLINE static float round(float in) {
    return (in >= 0.5000f) ? (ceilf(in)) : (floorf(in));
  }
  FORCE_INLINE static double round(double in) {
    return (in >= 0.5000) ? (ceil(in)) : (floor(in));
  }
  FORCE_INLINE static float round(float value, int precision) {
    //http://stackoverflow.com/questions/3237247/c-rounding-to-the-nths-place
    //TODO: std::round()
    const int adjustment = (int)pow(10, precision);
    return floorf(value * (adjustment) + 0.5f) / adjustment;
  }
  FORCE_INLINE static double round(double value, int precision) {
    //TODO: std::round()
    const int adjustment = (int)pow(10, precision);
    return floor(value * (adjustment) + 0.5f) / adjustment;
  }
  //btMin/max copied from bullet.
  template <class T>
  FORCE_INLINE static const T& brMin(const T& a, const T& b) {
    return a < b ? a : b;
  }
  template <class T>
  FORCE_INLINE static const T& brMax(const T& a, const T& b) {
    return a > b ? a : b;
  }
  template <class T>
  FORCE_INLINE static T& brMin(T& a, T& b) {
    return a < b ? a : b;
  }
  template <class T>
  FORCE_INLINE static T& brMax(T& a, T& b) {
    return a > b ? a : b;
  }
  template <class T>
  FORCE_INLINE static T& brClamp(T& x, T& a, T& b) {
    return brMin(b, brMax(x, a));
  }
  template <class T>
  FORCE_INLINE static const T& brClamp(const T& x, const T& a, const T& b) {
    return brMin(b, brMax(x, a));
  }
#endif
  FORCE_INLINE static float brSign(const float& val) {
    return (float)((float)(0.0f < val) - (float)(val < 0.0f));
  }
  //#define clamp(x,_min,_max) (broMin((_max),broMax((x),(_min))))
  FORCE_INLINE static bool fuzzyEquals(float fa, float fb, float epsilon = FUZZY_EQUALS_EPSILON) {
    float bm = fb - epsilon;
    float bp = fb + epsilon;
    return (fa > bm) && (fa < bp);
  }
  template <class T>
  FORCE_INLINE static const bool brInRangeInclusive(const T& x, const T& a, const T& b) {
    return ((x >= a) && (x <= b));
  }
  template <class T>
  FORCE_INLINE static const bool brInRangeInclusive(const T& x, const T& a) {
    return brInRangeInclusive(x, -a, a);
  }
  FORCE_INLINE static float modulateGridValue(float f_value, float f_grid_unit, float f_1_grid_unit) {
    // - Modulate the given value.
    // Returns the fractional part of f for the given
    // f_1_grid_unit - 1/f_grid_unit    - supply this as an optimization
    // f_grid_unit - size of grid cubes
    // f_value - value to modulate.
    f_value = fmodf(f_value, f_grid_unit) * f_1_grid_unit;
    f_value += 1.0;
    f_value = fmodf(f_value, 1.0f);  // this works for -.25 etc.  see complaint comment below
    return f_value;
  }
  FORCE_INLINE static int signi(float x) {
    //    Return -1, or 1 sign of x
    int a = (int)(x >= 0);
    return (a * (1)) + ((!a) * (-1));
  }
  FORCE_INLINE static float signf(float fx) {
    return (float)(signi(fx));
  }
  FORCE_INLINE static float ceil_signed(float x) {
    //  return ceil of a number in the given direction. 0.6 = 1.0, -0.6 = -1.0
    return ((float)ceil(fabs(x))) * (signf(x));
  }

private:
  static std::shared_ptr<Random> _random;
};

}  // namespace BR2

#endif
