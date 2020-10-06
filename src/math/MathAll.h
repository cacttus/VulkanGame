
/**
 * Math lazy dependencies.
 */
#pragma once
#ifndef __MATH_ALL_567230957209347_H__
#define __MATH_ALL_567230957209347_H__

#include "../math/MathHeader.h"
#include "../math/Vec2x.h"
#include "../math/Vec3x.h"
#include "../math/Vec4x.h"
#include "../math/Mat3x.h"
#include "../math/Mat4x.h"
#include "../math/Box3x.h"
#include "../math/Sphere3.h"
#include "../math/Box2x.h"
#include "../math/Quad3x.h"
#include "../math/QuadPlane.h"
#include "../math/TriPlane.h"
#include "../math/Line3.h"
#include "../math/Minimax.h"
#include "../math/Random.h"
#include <limits>

//This is for the ostringstream conversions below.
#include <sstream>
//For std::setprecision
#include <iomanip>

namespace BR2 {

template <class Tx>
Vec3x<Tx> Vec3x<Tx>::operator*(const Mat3x<Tx>& m) {
  Vec3x<Tx> ret;
  ret.x = (Tx)(m._m11 * x + m._m21 * x + m._m31 * x);
  ret.y = (Tx)(m._m12 * y + m._m22 * y + m._m32 * y);
  ret.z = (Tx)(m._m13 * z + m._m23 * z + m._m33 * z);
  return ret;
}
template <class Tx>
FORCE_INLINE bool Box3x<Tx>::sphereIntersect(Sphere3f* s) {
  return sphereIntersect(&(s->o), s->r);
}
template <typename Tx>
FORCE_INLINE Mat3x<Tx> Mat4x<Tx>::minor(int r, int c) {
  //Returns the minor at the specified row and column.
  Mat3x<Tx> m;
  m.setIdentity();
  if (r < 0 || r > 3 || c < 0 || c > 3) {
    return m;
  }

  int ind, mind = 0;
  for (ind = 0; ind < size(); ++ind) {
    if (rowNum(ind) != r && colNum(ind) != c) {
      m[mind++] = operator[](ind);
    }
  }

  return m;
}
template <typename Tx>
FORCE_INLINE Tx Mat4x<Tx>::cofactor(int r, int c) {
  //Returns the cofactor of this matrix at the specified row and column.
  // I.E. The determinant of the minor.
  return (Tx)pow(-1.00f, (Tx)r + (Tx)c) * minor(r, c).det();  // ** May be incorrect
}

template < typename Tx >
FORCE_INLINE Quaternion<Tx> Quaternion<Tx>::operator*(Vec3x<Tx> v) {
  return Quaternion<Tx>(x * v.x, y * v.y, z * v.z, w);
}
template < typename Tx >
FORCE_INLINE Vec3x<Tx> Quaternion<Tx>::vectorPart() {
  return Vec3x<Tx>(x, y, z);
}
template < typename Tx >
FORCE_INLINE Vec3x<Tx> Quaternion<Tx>::rotatePoint(Vec3x<Tx>& in) {
  return ((inverse() * in) * (*this)).vectorPart();
}
template < typename Tx >
FORCE_INLINE void Quaternion<Tx>::getAxisAngle(Vec4x<Tx>& v) {
  //http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/

  if (w == (Tx)1.0) {
    //Avoid divide by 0,( 1 - (cos(0) = 1)) =0
    v.x = v.z = v.w = 0;
    v.y = 1;
    return;
  }

  v.w = 2.0f * (Tx)acos(w);

  Tx w2 = w * w;
  Tx srw2_1 = 1.0f / (Tx)sqrt(1.0f - w2);
  v.x = x * srw2_1;
  v.y = y * srw2_1;
  v.z = z * srw2_1;
}
template < typename Tx >
FORCE_INLINE Mat4x<Tx> Quaternion<Tx>::toMat4() {
  //Convert quaternion into to mat4
  Mat4x<Tx> m;
  m._m11 = 1 - 2 * y * y - 2 * z * z;
  m._m12 = 2 * x * y + 2 * w * z;
  m._m13 = 2 * x * z - 2 * w * y;
  m._m14 = 0;
  m._m21 = 2 * x * y - 2 * w * z;
  m._m22 = 1 - 2 * x * x - 2 * z * z;
  m._m23 = 2 * y * z + 2 * w * x;
  m._m24 = 0;
  m._m31 = 2 * x * z + 2 * w * y;
  m._m32 = 2 * y * z - 2 * w * x;
  m._m33 = 1 - 2 * x * x - 2 * y * y;
  m._m34 = 0;
  m._m41 = 0;
  m._m42 = 0;
  m._m43 = 0;
  m._m44 = 1;

  return m;
}
template < typename Tx >
FORCE_INLINE Mat3x<Tx> Quaternion<Tx>::toMat3() {
  //Convert Quaternion to mat3
  Mat3x<Tx> m;
  m._m11 = 1 - 2 * y * y - 2 * z * z;
  m._m12 = 2 * x * y + 2 * w * z;
  m._m13 = 2 * x * z - 2 * w * y;
  m._m21 = 2 * x * y - 2 * w * z;
  m._m22 = 1 - 2 * x * x - 2 * z * z;
  m._m23 = 2 * y * z + 2 * w * x;
  m._m31 = 2 * x * z + 2 * w * y;
  m._m32 = 2 * y * z - 2 * w * x;
  m._m33 = 1 - 2 * x * x - 2 * y * y;

  return m;
}
template < typename Tx >
FORCE_INLINE Mat3x<Tx> Mat3x<Tx>::getRotationRad(Tx a, Tx x, Tx y, Tx z) {
  // - Reference: The openGL reference.http://pyopengl.sourceforge.net/documentation/manual/reference-GL.html
  Mat3x<Tx> Temp;

  if (Vec3x<Tx>(x, y, z).squaredLength() != 1.0) {
    Vec3x<Tx> v(x, y, z);
    v.normalize();
    x = v.x;
    y = v.y;
    z = v.z;
  }

  Tx c = (Tx)cos(a);
  Tx s = (Tx)sin(a);
  Tx nc = 1 - c;

  Temp._m11 = (x * x) * nc + c;
  Temp._m21 = (x * y) * nc + (z * s);
  Temp._m31 = (x * z) * nc - (y * s);

  Temp._m12 = (y * x) * nc - (z * s);
  Temp._m22 = (y * y) * nc + c;
  Temp._m32 = (y * z) * nc + (x * s);

  Temp._m13 = (z * x) * nc + (y * s);
  Temp._m23 = (z * y) * nc - (x * s);
  Temp._m33 = (z * z) * nc + c;

  return Temp;
}
template < typename Tx >
FORCE_INLINE Mat3x<Tx> Mat3x<Tx>::getRotationRad(Tx a, const Vec3x<Tx>& v) {
  return getRotationRad(a, v.x, v.y, v.z);
}
template < typename Tx >
FORCE_INLINE void Mat3x<Tx>::rotateRad(Tx a, Tx x, Tx y, Tx z) {
  *this *= getRotationRad(a, x, y, z);
}
template < typename Tx >
FORCE_INLINE void Mat3x<Tx>::rotateRad(Tx a, Vec3x<Tx>& vec) {
  *this *= getRotationRad(a, vec.x, vec.y, vec.z);
}
template < typename Tx >
FORCE_INLINE Mat4x<Tx> Mat3x<Tx>::getMat4() {
  Mat4x<Tx> mat;
  mat._m11 = _m11;
  mat._m12 = _m12;
  mat._m13 = _m13;
  mat._m21 = _m21;
  mat._m22 = _m22;
  mat._m23 = _m23;
  mat._m31 = _m31;
  mat._m32 = _m32;
  mat._m33 = _m33;

  return mat;
}
template < typename Tx >
FORCE_INLINE Mat3x<Tx>& Mat3x<Tx>::operator=(const Mat4x<Tx>& rhs) {
  _m11 = rhs._m11;
  _m12 = rhs._m12;
  _m13 = rhs._m13;

  _m21 = rhs._m21;
  _m22 = rhs._m22;
  _m23 = rhs._m23;

  _m31 = rhs._m31;
  _m32 = rhs._m32;
  _m33 = rhs._m33;

  return *this;
}
template < typename Tx >
FORCE_INLINE Vec3x<Tx> Mat3x<Tx>::operator*(const Vec3x<Tx>& v) {
  Vec3x<Tx> ret = {
      v.x * _m11 + v.y * _m12 + v.z * _m13,
      v.x * _m21 + v.y * _m22 + v.z * _m23,
      v.x * _m31 + v.y * _m32 + v.z * _m33};

  return ret;
}
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::minv(const Vec2x<Tx>& v_a, const Vec2x<Tx>& v_b) {
  Vec2x<Tx> out;

  out.x = MathUtils::brMin(v_a.x, v_b.x);
  out.y = MathUtils::brMin(v_a.y, v_b.y);

  return out;
}
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::maxv(const Vec2x<Tx>& v_a, const Vec2x<Tx>& v_b) {
  Vec2x<Tx> out;

  out.x = MathUtils::brMax(v_a.x, v_b.x);
  out.y = MathUtils::brMax(v_a.y, v_b.y);

  return out;
}
template <class Tx>
std::string Vec2x<Tx>::toString(int precision) const {
  std::ostringstream out;
  if (precision == -1) {
    out.precision(std::numeric_limits<float>::max_digits10);
  }
  else {
    out.precision((std::streamsize)precision);
  }
  out << x << "," << y;
  return out.str();
}
template <class Tx>
std::string Vec3x<Tx>::toString(int precision) const {
  std::ostringstream out;
  if (precision == -1) {
    out.precision(std::numeric_limits<float>::max_digits10);
  }
  else {
    out.precision((std::streamsize)precision);
  }
  out << x << "," << y << "," << z;
  return out.str();
}
template <class Tx>
std::string Vec4x<Tx>::toString(int precision) const {
  std::ostringstream out;
  if (precision == -1) {
    out.precision(std::numeric_limits<float>::max_digits10);
  }
  else {
    out.precision((std::streamsize)precision);
  }
  out << x << "," << y << "," << z << "," << w;
  return out.str();
}
template <typename Tx>
FORCE_INLINE std::string Mat3x<Tx>::toString(int precision) const {
  std::ostringstream out;
  if (precision == -1) {
    out.precision(std::numeric_limits<float>::max_digits10);
  }
  else {
    out.precision((std::streamsize)precision);
  }
  out << _m11 << " " << _m12 << " " << _m13 << "\n"
      << _m21 << " " << _m22 << " " << _m23 << "\n"
      << _m31 << " " << _m32 << " " << _m33 << "\n";
  return out.str();
}
template <typename Tx>
FORCE_INLINE std::string Mat4x<Tx>::toString(int precision) const {
  std::ostringstream out;
  if (precision == -1) {
    out.precision(std::numeric_limits<float>::max_digits10);
  }
  else {
    out.precision((std::streamsize)precision);
  }
  out << _m11 << " " << _m12 << " " << _m13 << " " << _m14 << "\n"
      << _m21 << " " << _m22 << " " << _m23 << " " << _m24 << "\n"
      << _m31 << " " << _m32 << " " << _m33 << " " << _m34 << "\n"
      << _m41 << " " << _m42 << " " << _m43 << " " << _m44 << "\n";
  return out.str();
}
// ------ Mem & offset --------
// vector offset to scalar offset
inline size_t vofftos(size_t row, size_t col, size_t items_per_row) {
  return (col * items_per_row + row);
}
//    return true if the (x,y) coordinate is within the grid.
//    the left boundary being zero the right being (size-1) and all indexes positive.
inline bool in_range(size_t x, size_t y, size_t row_size, size_t col_size) {
  return (((x >= 0) && (x < row_size)) && ((y >= 0) && (y < col_size)));
}
//
// scalar offset to vector offset ( row and column)
inline size_t sofftov_row(size_t off, size_t items_per_row) {
  return (size_t)(off / items_per_row);
}
inline size_t sofftov_col(size_t off, size_t items_per_row) {
  return (size_t)(off % items_per_row);
}
inline bool fuzzyEquals(float a, float b, float e) {
  //Epsilon cusyhion equals
  return (((b) <= ((a) + (e))) && ((b) >= ((a) - (e))));
}

typedef Vec4ub Pixel4ub;

}  // namespace BR2

#endif
