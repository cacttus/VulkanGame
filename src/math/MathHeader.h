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
  //TODO: duplicate Vec2x to use Width and Height as a mnemonic
  // #define Vec2_Class_Mnemonic(classname, xtype, ytype) class classname .. Tx xtype, ytype; .. ugly. debuggable? no
  Tx width;
  Tx height;
  Size2d(){}
  Size2d(Tx dw, Tx dh){width=dw;height=dh;}
  bool operator==(const Size2d<Tx>& rhs) const {
    bool ret = ((width == rhs.width) && (height == rhs.height));
    return ret;
  }
  bool operator!=(const Size2d<Tx>& rhs) const {
    bool ret = ((width != rhs.width) || (height != rhs.height));
    return ret;
  }

};
typedef Size2d<uint32_t> usize2;
typedef Size2d<int32_t> isize2;

template <typename Tx>
class Rect2d {
public:
  Vec2x<Tx> pos;
  Size2d<Tx> size;

  Rect2d() {}
  Rect2d(Tx dx, Tx dy, Tx dw, Tx dh) {
    pos.x = dx;
    pos.y = dy;
    size.width = dw;
    size.height = dh;
  }
    Rect2d(const Vec2x<Tx>& p, const Size2d<Tx>& s) {
    pos.x = p.x;
    pos.y = p.y;
    size.width = s.width;
    size.height = s.height;
  }
};
typedef Rect2d<uint32_t> urect2;
typedef Rect2d<int32_t> irect2;

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
class Random;

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


}  // namespace BR2

#endif
