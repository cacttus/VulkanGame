/**
*  @file Vec2x.h
*  @date February 26, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __VEC2X_2283131541297662696812355_H__
#define __VEC2X_2283131541297662696812355_H__

#include "../math/Math.h"


namespace VG {
/** 
 * @class Vec2x
 * @note This class must not have any virtual functions (vtable).
 */
template <class Tx>
class Vec2x {
  constexpr static std::size_t CompSize = 2;

public:
  Tx x = 0, y = 0;

  FORCE_INLINE Vec2x<Tx>();
  FORCE_INLINE Vec2x<Tx>(const Tx& dx, const Tx& dy);
  FORCE_INLINE Vec2x<Tx>(const Tx (&init_list)[Vec2x<Tx>::CompSize]);

  FORCE_INLINE Tx& u() { return x; }
  FORCE_INLINE Tx& v() { return y; }

  FORCE_INLINE const Tx& u() const { return x; }
  FORCE_INLINE const Tx& v() const { return y; }

  //
  FORCE_INLINE Tx length() const;
  FORCE_INLINE Tx length2() const;
  FORCE_INLINE Tx squaredLength() const;
  FORCE_INLINE void len_and_norm(vec2& __out_ n, float& __out_ len) const;
  FORCE_INLINE Vec2x<Tx>& normalize();
  FORCE_INLINE Vec2x<Tx>& normalize(float);
  FORCE_INLINE Vec2x<Tx> normalized();

  FORCE_INLINE Tx dot(const Vec2x<Tx>& v1) const;
  FORCE_INLINE Tx distance(const Vec2x<Tx>& v1) const;
  FORCE_INLINE Tx distance2(const Vec2x<Tx>& v1) const;
  FORCE_INLINE void construct(const Tx& dx, const Tx& dy);
  FORCE_INLINE void construct(const Vec2x<Tx>& rhs);
  FORCE_INLINE Vec2x<Tx> zero();

  std::string toString(int precis = -1) const;

  FORCE_INLINE static Vec2x<Tx> Vec2x_MIN();
  FORCE_INLINE static Vec2x<Tx> Vec2x_MAX();

  FORCE_INLINE static Vec2x<Tx> minv(const Vec2x<Tx>& v_a, const Vec2x<Tx>& v_b);  // - Returns the minimum vector from the given two
  FORCE_INLINE static Vec2x<Tx> maxv(const Vec2x<Tx>& v_a, const Vec2x<Tx>& v_b);  // returns maximujm vector of given t2o

  FORCE_INLINE bool operator==(const Vec2x<Tx>& v) const;
  FORCE_INLINE bool operator!=(const Vec2x<Tx>& v) const;

  FORCE_INLINE bool operator==(const Tx& f) const;
  FORCE_INLINE bool operator!=(const Tx& f) const;

  //remove unary negation
  FORCE_INLINE Vec2x<Tx> operator-() const;

  FORCE_INLINE Vec2x<Tx>& operator=(const Vec2x<Tx>& v);
  FORCE_INLINE Vec2x<Tx>& operator+=(const Vec2x<Tx>& v);
  FORCE_INLINE Vec2x<Tx>& operator-=(const Vec2x<Tx>& v);
  FORCE_INLINE Vec2x<Tx>& operator*=(const Vec2x<Tx>& v);
  FORCE_INLINE Vec2x<Tx>& operator/=(const Vec2x<Tx>& v);

  FORCE_INLINE Vec2x<Tx>& operator=(const Tx& f);
  FORCE_INLINE Vec2x<Tx>& operator+=(const Tx& f);
  FORCE_INLINE Vec2x<Tx>& operator-=(const Tx& f);
  FORCE_INLINE Vec2x<Tx>& operator*=(const Tx& f);
  FORCE_INLINE Vec2x<Tx>& operator/=(const Tx& f);

  FORCE_INLINE bool operator>(const Vec2x<Tx>& v) const;
  FORCE_INLINE bool operator>=(const Vec2x<Tx>& v) const;
  FORCE_INLINE bool operator<(const Vec2x<Tx>& v) const;
  FORCE_INLINE bool operator<=(const Vec2x<Tx>& v) const;

  FORCE_INLINE bool operator>(const Tx& f) const;
  FORCE_INLINE bool operator>=(const Tx& f) const;
  FORCE_INLINE bool operator<(const Tx& f) const;
  FORCE_INLINE bool operator<=(const Tx& f) const;

  //MSVC doesn't like the scope resolution operator in front of templated methods with the same return type
  FORCE_INLINE Vec2x<Tx> operator+(const Vec2x<Tx>& v) const;
  FORCE_INLINE Vec2x<Tx> operator-(const Vec2x<Tx>& v) const;
  FORCE_INLINE Vec2x<Tx> operator*(const Vec2x<Tx>& v) const;
  FORCE_INLINE Vec2x<Tx> operator/(const Vec2x<Tx>& v) const;

  FORCE_INLINE Vec2x<Tx> operator+(const Tx& f) const;
  FORCE_INLINE Vec2x<Tx> operator-(const Tx& f) const;
  FORCE_INLINE Vec2x<Tx> operator*(const Tx& f) const;
  FORCE_INLINE Vec2x<Tx> operator/(const Tx& f) const;

  FORCE_INLINE bool compareTo(const Vec2x<Tx>* rhs) const;

  struct Vec2xCompLess {
    FORCE_INLINE bool operator()(const Vec2x<Tx>* lhs, const Vec2x<Tx>* rhs) const {
      return lhs->compareTo(rhs);
    }
  };
};
//////////////////////////////////////////////////////////////////////////
// Constructors
template <class Tx>
Vec2x<Tx>::Vec2x() {
}
template <class Tx>
Vec2x<Tx>::Vec2x(const Tx& dx, const Tx& dy) {
  x = dx;
  y = dy;
}
template <class Tx>
Vec2x<Tx>::Vec2x(const Tx (&init_list)[Vec2x<Tx>::CompSize]) {
  static_assert(Vec2x<Tx>::CompSize == 2, "Invalid number of initializer list elements.");
  x = init_list[0];
  y = init_list[1];
}
//////////////////////////////////////////////////////////////////////////
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::zero() {
  return Vec2x<Tx>(0, 0);
}
template <class Tx>
void Vec2x<Tx>::construct(const Vec2x<Tx>& rhs) {
  x = rhs.x;
  y = rhs.y;
}
template <class Tx>
void Vec2x<Tx>::construct(const Tx& dx, const Tx& dy) {
  x = dx;
  y = dy;
}

template <class Tx>
Vec2x<Tx> Vec2x<Tx>::Vec2x_MIN() {
  return Vec2x<Tx>(-COMP_MAX<Tx>::m(), -COMP_MAX<Tx>::m());
}
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::Vec2x_MAX() {
  return Vec2x<Tx>(COMP_MAX<Tx>::m(), COMP_MAX<Tx>::m());
}
template <class Tx>
Tx Vec2x<Tx>::length() const {
  return Math::brSqrt(x * x + y * y);
}
template <class Tx>
Tx Vec2x<Tx>::length2() const {
  return x * x + y * y;
}
template <class Tx>
Tx Vec2x<Tx>::squaredLength() const { return length(); }
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::normalize() {
  Tx a = length();
  return normalize(a);

  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::normalize(float len) {
  if (len != 0) {
    Tx a1 = 1.0f / len;
    x *= a1;
    y *= a1;
  }
  else {
    x = y = 0;
  }
  return *this;
}
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::normalized() {
  Vec2x<Tx> ret = *this;
  Tx a = ret.length();
  if (a != 0) {
    ret.x /= a;
    ret.y /= a;
  }
  else {
    ret.x = ret.y = 0;
  }
  return ret;
}
template <class Tx>
void Vec2x<Tx>::len_and_norm(vec2& __out_ n, float& __out_ len) const {
  //Computes length and normal to avoid having do do len() then norm()
  len = length();
  n = *this;
  n.normalize(len);
}
template <class Tx>
Tx Vec2x<Tx>::dot(const Vec2x<Tx>& v) const {
  return (x * v.x + y * v.y);
}
template <class Tx>
Tx Vec2x<Tx>::distance(const Vec2x<Tx>& v1) const {
  return ((*this) - v1).length();
}
template <class Tx>
Tx Vec2x<Tx>::distance2(const Vec2x<Tx>& v1) const {
  return ((*this) - v1).length2();
}
//Operators
template <class Tx>
bool Vec2x<Tx>::operator==(const Vec2x<Tx>& v) const {
  return (x == v.x ? (y == v.y ? 1 : 0) : 0);
}
template <class Tx>
bool Vec2x<Tx>::operator!=(const Vec2x<Tx>& v) const {
  return (x != v.x ? 1 : (y != v.y ? 1 : 0));
}
template <class Tx>
bool Vec2x<Tx>::operator==(const Tx& f) const {
  return (x == f ? (y == f ? 1 : 0) : 0);
}
template <class Tx>
bool Vec2x<Tx>::operator!=(const Tx& f) const {
  return (x != f ? 1 : (y != f ? 1 : 0));
}

template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator-() const {
  return Vec2x<Tx>(-x, -y);
}
//

//
// row vector times matrix
//
//template < class Tx >
//Vec2x<Tx> & Vec2x<Tx>::operator*=( const Mat3<Tx>& m )
//{
//  x=m._m11*x + m._m12*y + m._m13*z;
//  y=m._m21*x + m._m22*y + m._m23*z;
//  z=m._m31*x + m._m32*y + m._m33*z;
//  return *this;
//}

//
//
//
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator=(const Vec2x<Tx>& v) {
  //Check for self-reassignment
  if (this == &v) return *this;

  x = v.x;
  y = v.y;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator+=(const Vec2x<Tx>& v) {
  if (this == &v)
    return *this;
  x += v.x;
  y += v.y;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator-=(const Vec2x<Tx>& v) {
  if (this == &v)
    return *this;
  x -= v.x;
  y -= v.y;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator*=(const Vec2x<Tx>& v) {
  if (this == &v)
    return *this;
  x *= v.x;
  y *= v.y;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator/=(const Vec2x<Tx>& v) {
  if (this == &v)
    return *this;
  x /= v.x;
  y /= v.y;
  return *this;
}

//
//
//
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator=(const Tx& f) {
  x = f;
  y = f;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator+=(const Tx& f) {
  x += f;
  y += f;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator-=(const Tx& f) {
  x -= f;
  y -= f;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator*=(const Tx& f) {
  x *= f;
  y *= f;
  return *this;
}
template <class Tx>
Vec2x<Tx>& Vec2x<Tx>::operator/=(const Tx& f) {
  x /= f;
  y /= f;
  return *this;
}

template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator+(const Vec2x<Tx>& v) const {
  Vec2x<Tx> tmp = *this;
  tmp += v;
  return tmp;
}

template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator-(const Vec2x<Tx>& v) const {
  Vec2x<Tx> tmp = *this;
  tmp -= v;
  return tmp;
}

template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator*(const Vec2x<Tx>& v) const {
  Vec2x<Tx> tmp = *this;
  tmp *= v;
  return tmp;
}

template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator/(const Vec2x<Tx>& v) const {
  Vec2x<Tx> tmp = *this;
  tmp /= v;
  return tmp;
}

//
//
//
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator+(const Tx& f) const {
  Vec2x<Tx> tmp = *this;
  tmp += f;
  return tmp;
}
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator-(const Tx& f) const {
  Vec2x<Tx> tmp = *this;
  tmp -= f;
  return tmp;
}
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator*(const Tx& f) const {
  Vec2x<Tx> tmp = *this;
  tmp *= f;
  return tmp;
}
template <class Tx>
Vec2x<Tx> Vec2x<Tx>::operator/(const Tx& f) const {
  Vec2x<Tx> tmp = *this;
  tmp /= f;
  return tmp;
}

//
//
//
template <class Tx>
bool Vec2x<Tx>::operator>(const Vec2x<Tx>& v) const {
  return (x > v.x && y > v.y);
}
template <class Tx>
bool Vec2x<Tx>::operator>=(const Vec2x<Tx>& v) const {
  return (x >= v.x && y >= v.y);
}
template <class Tx>
bool Vec2x<Tx>::operator<(const Vec2x<Tx>& v) const {
  return (x < v.x && y < v.y);
}
template <class Tx>
bool Vec2x<Tx>::operator<=(const Vec2x<Tx>& v) const {
  return (x <= v.x && y <= v.y);
}

//
//
//
template <class Tx>
bool Vec2x<Tx>::operator>(const Tx& f) const {
  return (x > f && y > f);
}
template <class Tx>
bool Vec2x<Tx>::operator>=(const Tx& f) const {
  return (x >= f && y >= f);
}
template <class Tx>
bool Vec2x<Tx>::operator<(const Tx& f) const {
  return (x < f && y < f);
}
template <class Tx>
bool Vec2x<Tx>::operator<=(const Tx& f) const {
  return (x <= f && y <= f);
}

template <class Tx>
bool Vec2x<Tx>::compareTo(const Vec2x<Tx>* rhs) const {
  const Vec2x<Tx>* lhs = this;

  if (lhs->x < rhs->x)
    return true;
  else if (lhs->x > rhs->x)
    return false;
  else {
    if (lhs->y < rhs->y)
      return true;
    else  //if(lhs->y > rhs->y)
      return false;
  }
}

//////////////////////////////////////////////////////////////////////////
// - Vector shorthands
STATIC FORCE_INLINE vec2& normalize(vec2& v1) { return v1.normalize(); }
STATIC FORCE_INLINE float dot(vec2& v1, vec2& v2) { return v1.dot(v2); }

}  // namespace VG

#endif
