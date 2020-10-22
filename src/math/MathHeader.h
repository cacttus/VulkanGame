/**
*
*    @file MathHeader.h
*    @date November 29, 2014
*    @author MetalMario971
*
*    � 2014
*
*
*/
#pragma once
#ifndef __MATHHEADER_24654626514582104228509_H__
#define __MATHHEADER_24654626514582104228509_H__

//No includes
#include "../base/BaseHeader.h"

#ifdef BR_MATH_USE_STL
#include <algorithm>

//TODO: we have to replace the defines everywhere.
#define brMax(a, b) std::max((a), (b))
#define brMin(a, b) std::min((a), (b))
#define brClamp(x, a, b) std::clamp((x), (a), (b));

#endif

namespace BR2 {
//Define this to use the STL for max, min, etc.

#ifndef _MATH_DEFINES_DEFINED
#define _MATH_DEFINES_DEFINED
#ifndef M_E
#define M_E 2.71828182845904523536
#endif
#ifndef M_LOG2E
#define M_LOG2E 1.44269504088896340736
#endif
#ifndef M_LOG10E
#define M_LOG10E 0.434294481903251827651
#endif
#ifndef M_LN2
#define M_LN2 0.693147180559945309417
#endif
#ifndef M_LN10
#define M_LN10 2.30258509299404568402
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_2PI
#define M_2PI 6.283185307179586476925286766559
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif
#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616
#endif
#ifndef M_1_PI
#define M_1_PI 0.318309886183790671538
#endif
#ifndef M_2_PI
#define M_2_PI 0.636619772367581343076
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524401
#endif

#define M_SIN45 0.70710678118654752440084436210485
#endif

#define PERCENT(x) ((x)*0.01f)

//*** Matrix Multiplication Order
#define MATRIX_ORDER(a, b) ((b) * (a))

#define FUZZY_EQUALS_EPSILON (0.0002f)

//See coordinate system defined in BuildConfig.h
#ifdef COORDINATE_SYSTEM_LHS
#define COORDINATE_SYSTEM COORDINATE_SYSTEM_LHS
#else
#define COORDINATE_SYSTEM COORDINATE_SYSTEM_RHS
#endif

#ifdef COORDINATE_SYSTEM_LHS
#ifdef COORDINATE_SYSTEM_RHS
//You cannot have two coordinate systems dude
1 / 0
#endif
#endif

#define VEC3_MIN (vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
#define VEC3_MAX (vec3(FLT_MAX, FLT_MAX, FLT_MAX))
    //////////////////////////////////////////////////////////////////////////

    enum class CollisionResult { Stuck,
                                 Avoided,
                                 Collided };
namespace box_point_index {
typedef enum { box_nbr,
               box_nbl,
               box_ntr,
               box_ntl,
               box_fbr,
               box_fbl,
               box_ftr,
               box_ftl } e;
}
namespace box_plane_index {
typedef enum { bp_near,
               bp_far,
               bp_left,
               bp_right,
               bp_top,
               bp_bottom } e;
}
//2018/1/22 changed thiss
//Face_Right, Face_Left, Face_Bottom, Face_Top, Face_Back, Face_Front
namespace BoxSide {
typedef enum { Right,
               Left,
               Bottom,
               Top,
               Back,
               Front } e;
}
//////////////////////////////////////////////////////////////////////////
//FWDDCL

//Vector
template <class Tx>
class Vec2x;
template <class Tx>
class Vec3x;
template <class Tx>
class Vec4x;
template <class Tx>
class Box2x;
template <class Tx>
class Box3x;
template <class Tx>
class Quad3x;
template <class Tx>
class MinimaxParam;
template <typename Tx>
class Mat4x;
template <typename Tx>
class Mat3x;
template <typename Tx>
class Quaternion;

typedef Vec2x<float> vec2;
typedef Vec2x<double> dvec2;
typedef Vec2x<int32_t> ivec2;
typedef Vec2x<uint32_t> uvec2;
typedef Vec2x<unsigned char> Vec2ub;

typedef Vec3x<float> vec3;
typedef Vec3x<double> dvec3;
typedef Vec3x<int32_t> ivec3;
typedef Vec3x<uint32_t> uvec3;
typedef Vec3x<unsigned char> Vec3ub;

typedef Vec4x<float> vec4;
typedef Vec4x<double> dvec4;
typedef Vec4x<int32_t> ivec4;
typedef Vec4x<uint32_t> uvec4;
typedef Vec4x<unsigned char> Vec4ub;

template <typename Tx>
class Size2d { 
public:
  Tx width;
  Tx height;
  Size2d(){}
  Size2d(Tx dw, Tx dh){width=dw;height=dh;}
};
typedef Size2d<uint32_t> usize2;
typedef Size2d<int32_t> isize2;

template <typename Tx>
class Extent2d {
public:
  Vec2x<Tx> pos;
  Size2d<Tx> size;

  Extent2d() {}
  Extent2d(Tx dx, Tx dy, Tx dw, Tx dh) {
    pos.x = dx;
    pos.y = dy;
    size.width = dw;
    size.height = dh;
  }
};
typedef Extent2d<uint32_t> uext2;
typedef Extent2d<int32_t> iext2;

//Noise & Minimax
class Noise;
class SimplexNoise;
template <class Tx>
class MatrixStack3x;
class RaycastHitList;
class v_v3n3x2;
class MpFloat;
class MpVec3;
class Mpivec3;
class MpVec4;
class MpUint;
class MpInt;
class Spline;
class CubicBezierSpline;

//Geom
typedef Box3x<int32_t> Box3i;
typedef Box3x<float> Box3f;
typedef Box2x<float> Box2f;
typedef Box2x<int32_t> Box2i;
typedef Box2x<uint32_t> Box2ui;
typedef Quad3x<float> Quad3f;  //Basically this would be the same
class Line3f;
class Plane3f;
class Polar3;
class Poly3f;
class QuadraticAttenuator;
class QuadPlane;
class Plane3f;
class ProjectedRay;
class Sphere3f;
class Hash;
class Polar3;
class Sphere3f;
class Vec3Basis;

//Matrix
typedef Mat4x<float> mat4;
typedef Mat3x<float> mat3;
typedef Quaternion<float> quat;
typedef MatrixStack3x<mat4> Mat4Stack;

//Pixel
typedef vec2 Pixel3f;

typedef vec2 Color3f;
typedef Vec3ub Color3ub;
typedef Vec3ub Pixel3ub;

typedef vec4 Color4f;
typedef Vec4ub Color4ub;
typedef Vec4ub Pixel4ub;

//String conversion
std::string operator+(const std::string& str, const vec2& rhs);
std::string operator+(const std::string& str, const vec3& rhs);
std::string operator+(const std::string& str, const vec4& rhs);
std::string operator+(const std::string& str, const ivec2& rhs);
std::string operator+(const std::string& str, const ivec3& rhs);
std::string operator+(const std::string& str, const ivec4& rhs);
std::string operator+(const std::string& str, const uvec2& rhs);
std::string operator+(const std::string& str, const uvec3& rhs);
std::string operator+(const std::string& str, const uvec4& rhs);
std::string operator+(const std::string& str, const mat3& rhs);
std::string operator+(const std::string& str, const mat4& rhs);

//////////////////////////////////////////////////////////////////////////
// Classes

// Collision Results
class LineHit : public VirtualMemory {
public:
  float t;
  LineHit() {}
  virtual ~LineHit() override {}
};
class PlanePoint : public VirtualMemory {
public:
  float t;  // - Position along the vector that was hit
  bool contained;
  PlanePoint() {}
  virtual ~PlanePoint() override {}
};
class PlaneHit : public VirtualMemory {
public:
  float _fHitTime;
  bool _bContained;

  FORCE_INLINE float getHitTime() { return _fHitTime; }
  FORCE_INLINE bool getIsContained() { return _bContained; }

  void makeFar() {
    _bContained = false;
    _fHitTime = FLT_MAX;
  }
  PlaneHit() {}
  virtual ~PlaneHit() override {}
};
// for quads
class DualPlaneHit : public VirtualMemory {
public:
  bool hit;  // True if either of the planes were hit.
  PlaneHit planeHit0;
  PlaneHit planeHit1;
  DualPlaneHit() {}
  virtual ~DualPlaneHit() override {}
};

class MathUtils : public VirtualMemory {
public:
  //juvenile sqrt optimization - simply removes the prolog
  //approx 30% faster than sqrtf
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
};

}  // namespace BR2

#endif
