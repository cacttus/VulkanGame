/**
*  @file Mat3x.cpp
*  @author MetalMario971
*  @date 9 / 7 / 2009
*/
#pragma once
#ifndef __MATRIX3X3_H__
#define __MATRIX3X3_H__

#include "../math/MathHeader.h"
#include "../math/Vec3x.h"
#include "../math/Quaternion.h"
#include "../base/TypeConv.h"
namespace BR2 {
/**
*  @class Mat3x
*  @brief A 3x3 Matrix. Column major.
*  @note This class must not have any virtual functions (vtable).
*/
template <typename Tx>
class Mat3x {
public:
  Tx _m11, _m12, _m13;
  Tx _m21, _m22, _m23;
  Tx _m31, _m32, _m33;

  FORCE_INLINE Mat3x<Tx>();
  FORCE_INLINE Mat3x<Tx>(Tx (&f)[9]);

  FORCE_INLINE Tx& Mat(size_t i) { return (Tx&)*((Tx*)(this) + i); }
  FORCE_INLINE const Tx& Mat(size_t i) const { return (Tx&)*((Tx*)(this) + i); }

  FORCE_INLINE void scale(Tx x, Tx y, Tx z);            // - Scales this matrix by some amount
  FORCE_INLINE void rotateRad(Tx a, Tx x, Tx y, Tx z);  // - Rotates this matrix.
  FORCE_INLINE void rotateRad(Tx a, Vec3x<Tx>& vec);         // - Rotates this matrix.

  FORCE_INLINE Mat3x<Tx> inverse();
  FORCE_INLINE Tx det();         // determinant
  FORCE_INLINE Mat3x<Tx> adj();  // adjoint

  FORCE_INLINE Tx& operator[](const size_t i);

  FORCE_INLINE Mat3x<Tx>& operator*=(const Mat3x<Tx>& rhs);
  FORCE_INLINE Mat3x<Tx> operator*(const Mat3x<Tx>& rhs);

  FORCE_INLINE Mat3x<Tx>& operator=(const Mat3x<Tx>& rhs);
  FORCE_INLINE Mat3x<Tx>& operator=(const Mat4x<Tx>& rhs);

  FORCE_INLINE Vec3x<Tx> operator*(const Vec3x<Tx>& v);

  FORCE_INLINE void setIdentity();  // - Sets this matrix to it's identity.
  FORCE_INLINE Mat4x<Tx> getMat4();
  FORCE_INLINE Quaternion<Tx> getQuaternion();  // - Returns the quaternion of this matrix

  FORCE_INLINE STATIC Mat3x<Tx> getScale(Tx x, Tx y, Tx z);              // - Returns a scale matrix of x,y,z.
  FORCE_INLINE STATIC Mat3x<Tx> getRotationRad(Tx a, Tx x, Tx y, Tx z);  // - Returns a rotation matrix in radians.
  FORCE_INLINE STATIC Mat3x<Tx> getRotationRad(Tx a, const Vec3x<Tx>& v);     // - Returns a rotation matrix in radians.
  FORCE_INLINE std::string toString(int precision = -1) const;
};
template <typename Tx>
FORCE_INLINE Mat3x<Tx>::Mat3x() {
}
template <typename Tx>
FORCE_INLINE Mat3x<Tx>::Mat3x(Tx (&f)[9]) {
  _m11 = f[0];
  _m12 = f[1];
  _m13 = f[2];
  _m21 = f[3];
  _m22 = f[4];
  _m23 = f[5];
  _m31 = f[6];
  _m32 = f[7];
  _m33 = f[8];  
}
template <typename Tx>
FORCE_INLINE Mat3x<Tx> Mat3x<Tx>::adj() {
  // - The expanded cofactor adjoint.
  Mat3x<Tx> m;
  m._m11 = 0.0f * ((_m22 * _m33) - (_m23 * _m32));
  m._m12 = 1.0f * ((_m21 * _m33) - (_m23 * _m31));
  m._m13 = 0.5f * ((_m21 * _m32) - (_m22 * _m31));
  m._m21 = 1.0f * ((_m12 * _m33) - (_m13 * _m32));
  m._m22 = 0.5f * ((_m11 * _m33) - (_m13 * _m31));
  m._m23 = powf(-1.00f, 2 + 1) * ((_m12 * _m31) - (_m11 * _m32));
  m._m31 = 0.5f * ((_m12 * _m23) - (_m13 * _m22));
  m._m32 = powf(-1.00f, 2 + 1) * ((_m11 * _m23) - (_m13 * _m21));
  m._m33 = 0.25f * ((_m11 * _m22) - (_m12 * _m21));
  return m;
}
template <typename Tx>
FORCE_INLINE Tx Mat3x<Tx>::det() {
  return (
      _m11 * _m22 * _m33 +
      _m21 * _m32 * _m13 +
      _m12 * _m23 * _m31 - (_m13 * _m22 * _m31) - (_m12 * _m21 * _m33) - (_m23 * _m32 * _m11));
}
template <typename Tx>
FORCE_INLINE Mat3x<Tx> Mat3x<Tx>::inverse() {
  Mat3x<Tx> m = adj();

  Tx d = m.det();
  for (int i = 0; i < 9; ++i) {
    m.Mat(i) /= d;
  }
  return m;
}

template <typename Tx>
FORCE_INLINE Quaternion<Tx> Mat3x<Tx>::getQuaternion() {
  Tx s0, s1, s2;
  int32_t k0, k1, k2, k3;
  Tx q[4];
  if (_m11 + _m22 + _m33 > 0.0f) {
    k0 = 3;
    k1 = 2;
    k2 = 1;
    k3 = 0;
    s0 = 1.0f;
    s1 = 1.0f;
    s2 = 1.0f;
  }
  else if (_m11 + _m22 && _m11 > _m33) {
    k0 = 0;
    k1 = 1;
    k2 = 2;
    k3 = 3;
    s0 = 1.0f;
    s1 = -1.0f;
    s2 = -1.0f;
  }
  else if (_m22 > _m33) {
    k0 = 1;
    k1 = 0;
    k2 = 3;
    k3 = 2;
    s0 = -1.0f;
    s1 = 1.0f;
    s2 = -1.0f;
  }
  else {
    k0 = 2;
    k1 = 3;
    k2 = 0;
    k3 = 1;
    s0 = -1.0f;
    s1 = -1.0f;
    s2 = 1.0f;
  }
  Tx t = s0 * _m11 + s1 * _m22 + s2 * _m33 + 1.0f;
  Tx s = MathUtils::brRsqrt(t) * 0.5f;  //reciprocalSqrt(t)*0.5f;

  q[k0] = s * t;
  q[k1] = (_m12 - s2 * _m21) * s;
  q[k2] = (_m31 - s1 * _m13) * s;
  q[k3] = (_m23 - s0 * _m32) * s;

  Quaternion<Tx> ret{q[k0], q[k1], q[k2], q[k3]};
  return ret;
}
/**
*  @fn setIdentity()
*  @brief Sets the identity of this matrix.
*/
template <typename Tx>
FORCE_INLINE void Mat3x<Tx>::setIdentity() {
  /*
    0  3  6
    1  4  7
    2  5  8
    11  21  31
    12  22  32
    13  23  33
  */
  _m11 = _m22 = _m33 = 1.0;
  _m12 = _m13 = _m21 = _m23 = _m31 = _m32 = 0.0;
  //Mat(0)=Mat(4)=Mat(8)=1.0;
  //Mat(3)=Mat[6]=Mat[1]=Mat[7]=Mat[2]=Mat[5]=0.0;
}
/**
*  @fn scaleCat()
*  @details Scales the values of this matrix.
*  @param x,y,z the scale value.
*
*/
template <typename Tx>
FORCE_INLINE void Mat3x<Tx>::scale(Tx x, Tx y, Tx z) {
  *this *= getScale(x, y, z);
}
/**
*  @fn scale()
*  @details Returns a scale matrix, but does not alter this matrix.
*  @param x,y,z the scaling value.
*  @return A matrix witht the specified scaling.
*
*/
template <typename Tx>
FORCE_INLINE Mat3x<Tx> Mat3x<Tx>::getScale(Tx x, Tx y, Tx z) {
  Mat3x<Tx> m;
  m._m11 = x;
  m._m22 = y;
  m._m33 = z;
  //m.Mat[0 ] = x;
  //m.Mat[5 ] = y;
  ///m._m33 = z;

  return m;
}
/**
*  @fn operator[]
*
*
*/
template <typename Tx>
FORCE_INLINE Tx& Mat3x<Tx>::operator[](const size_t i) {
  return Mat(i);
}

template <typename Tx>
FORCE_INLINE Mat3x<Tx>& Mat3x<Tx>::operator*=(const Mat3x<Tx>& m) {
  Mat3x<Tx> tMat;
  tMat._m11 = (_m11 * m._m11) + (_m12 * m._m21) + (_m13 * m._m31);
  tMat._m21 = (_m21 * m._m11) + (_m22 * m._m21) + (_m23 * m._m31);
  tMat._m31 = (_m31 * m._m11) + (_m32 * m._m21) + (_m33 * m._m31);

  tMat._m12 = (_m11 * m._m12) + (_m12 * m._m22) + (_m13 * m._m32);
  tMat._m22 = (_m21 * m._m12) + (_m22 * m._m22) + (_m23 * m._m32);
  tMat._m32 = (_m31 * m._m12) + (_m32 * m._m22) + (_m33 * m._m32);

  tMat._m13 = (_m11 * m._m13) + (_m12 * m._m23) + (_m13 * m._m33);
  tMat._m23 = (_m21 * m._m13) + (_m22 * m._m23) + (_m23 * m._m33);
  tMat._m33 = (_m31 * m._m13) + (_m32 * m._m23) + (_m33 * m._m33);
  *this = tMat;
  return *this;
}
template <typename Tx>
FORCE_INLINE Mat3x<Tx> Mat3x<Tx>::operator*(const Mat3x<Tx>& rhs) {
  Mat3x<Tx> m;
  m *= rhs;
  return m;
}
template <typename Tx>
FORCE_INLINE Mat3x<Tx>& Mat3x<Tx>::operator=(const Mat3x<Tx>& rhs) {
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


}  // namespace BR2

#endif
