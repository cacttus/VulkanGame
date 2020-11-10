/**
*  @file Quaternion.h
*  @date 6 / 12 / 2010
*  @author MetalMario971
*/
#pragma once
#ifndef __QUATERNION_3490673049687_H__
#define __QUATERNION_3490673049687_H__

#include "../math/MathHeader.h"
#include "../math/Vec3x.h"
#include "../math/Vec4x.h"

namespace BR2 {
/**
*  @class Quaternion
*  not that quaternions only support rotations and matrix translations would be lost in the conversion.
*  Q = [ (x,y,z), w ]
*  Q = [ v, w ]
*  Q = [ v sin(a) , cos(a) ]
*/
template <typename Tx>
class Quaternion {
public:
  Tx x, y, z, w;
  FORCE_INLINE Quaternion();
  FORCE_INLINE Quaternion(Tx a, Tx b, Tx c, Tx d);
  FORCE_INLINE Mat4x<Tx> toMat4();
  FORCE_INLINE Mat3x<Tx> toMat3();
  FORCE_INLINE Tx mag();
  FORCE_INLINE Vec3x<Tx> vectorPart();
  FORCE_INLINE Tx Dot(const Quaternion<Tx>& rhs) const;
  FORCE_INLINE Vec3x<Tx> rotatePoint(Vec3x<Tx>& in);
  FORCE_INLINE Quaternion<Tx> inverse();
  FORCE_INLINE Quaternion<Tx> operator*(const Quaternion<Tx>& rhs) const;
  FORCE_INLINE Quaternion<Tx> operator*(Tx f) const;
  FORCE_INLINE Quaternion<Tx> operator+(const Quaternion<Tx>& rhs) const;
  FORCE_INLINE Quaternion<Tx> slerpTo(Quaternion<Tx>& rhs, Tx t) const;
  FORCE_INLINE Quaternion<Tx> operator*(Vec3x<Tx> v);
  FORCE_INLINE void getAxisAngle(Vec4x<Tx>& v);
  FORCE_INLINE void construct(Tx dx, Tx dy, Tx dz, Tx dw);
};
template <typename Tx>
FORCE_INLINE Quaternion<Tx>::Quaternion() {
  x = y = z = w = 0;
}
template <typename Tx>
FORCE_INLINE Quaternion<Tx>::Quaternion(Tx dx, Tx dy, Tx dz, Tx dw) {
  x = dx;
  y = dy;
  z = dz;
  w = dw;
}
template <typename Tx>
FORCE_INLINE void Quaternion<Tx>::construct(Tx dx, Tx dy, Tx dz, Tx dw) {
  x = dx;
  y = dy;
  z = dz;
  w = dw;
}
template <typename Tx>
FORCE_INLINE Tx Quaternion<Tx>::Dot(const Quaternion<Tx>& rhs) const {
  return (x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}
template <typename Tx>
FORCE_INLINE Tx Quaternion<Tx>::mag() {
  return w * w + (x * x + y * y + z * z);
}
template <typename Tx>
FORCE_INLINE Quaternion<Tx> Quaternion<Tx>::inverse() {
  Tx L = 1 / mag();
  Quaternion<Tx> out;
  out.w = L * w;
  out.x = -L * x;
  out.y = -L * y;
  out.z = -L * z;
  return out;
}
template <typename Tx>
FORCE_INLINE Quaternion<Tx> Quaternion<Tx>::operator*(const Quaternion<Tx>& rhs) const {
  Quaternion<Tx> ret;

  //Normal quaternion multiplication
  //Q(w,v) = w=w1w2 - v1 dot v2, v= w1v2 + w2v1 + v2 cross v1.

  // - This method here is that of backwards multiplication
  // used in the 3D Math Primer For Graphics and Game Development:
  //    w=w1w2 - v1 dot v2, v= w1v2 + w2v1 + v1 cross v2.
  // this allows the quaternion rotations to be concatenated in a left to right order.
  ret.x = w * rhs.x + rhs.w * x + (y * rhs.z) - (rhs.y * z);
  ret.y = w * rhs.y + rhs.w * y + (z * rhs.x) - (rhs.z * x);
  ret.z = w * rhs.z + rhs.w * z + (x * rhs.y) - (rhs.x * y);
  ret.w = w * rhs.w - (x * rhs.x + y * rhs.y + z * rhs.z);
  return ret;
}
template <typename Tx>
FORCE_INLINE Quaternion<Tx> Quaternion<Tx>::operator*(Tx f) const {
  Quaternion<Tx> out;
  out.x = x * f;
  out.y = y * f;
  out.z = z * f;
  out.w = w * f;
  return out;
}
template <typename Tx>
FORCE_INLINE Quaternion<Tx> Quaternion<Tx>::operator+(const Quaternion<Tx>& rhs) const {
  Quaternion<Tx> out;
  out.x = x + rhs.x;
  out.y = y + rhs.y;
  out.z = z + rhs.z;
  out.w = w + rhs.w;
  return out;
}
template <typename Tx>
FORCE_INLINE Quaternion<Tx> Quaternion<Tx>::slerpTo(Quaternion<Tx>& rhs, Tx t) const {
  //SLERP Spherical Linear interpolate this quaternion to rhs.
  // @param rhs The Quat to slerp
  // @param t Interpolation value [0 to 1]
  Quaternion<Tx> ret;
  Tx s0, s1, sinAng, ang, cosAng, absAng;
  Tx sinSqr;

  cosAng = (Tx)Dot(rhs);
  absAng = (Tx)fabs(cosAng);

  if ((1 - absAng) > 1e-6f) {
    sinSqr = 1.0 - absAng * absAng;
    sinAng = Math::brRsqrt(sinSqr);
    ang = atan2f(sinSqr * sinAng, absAng);
    s0 = sinf((1.0f - t) * ang) * sinAng;
    s1 = sinf(t * ang) * sinAng;
  }
  else {
    s0 = (1.0f - t);
    s1 = t;
  }
  s1 = (cosAng >= 0.0f) ? s1 : -s1;
  ret.x = s0 * x + s1 * rhs.x;
  ret.y = s0 * y + s1 * rhs.y;
  ret.z = s0 * z + s1 * rhs.z;
  ret.w = s0 * w + s1 * rhs.w;

  return ret;
}

}  // namespace BR2

#endif
