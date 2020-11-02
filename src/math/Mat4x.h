/**
*  @file Mat4x.h
*  @date Nov 16, 2007
*  @brief Column-major matrix
*  @author MetalMario971
*/
#pragma once
#ifndef __MAT__4__X__4__H__
#define __MAT__4__X__4__H__

#include "../math/Vec4x.h"
#include "../math/Vec3x.h"
#include "../math/Mat3x.h"
#include "../base/TypeConv.h"

namespace BR2 {
/**
*  @stuct Mat4x
*  @brief Row major 4x4 matrix. (64 bytes)
*  @note This class must not have any virtual functions (vtable).
*/

template <typename Tx>
class CACHE_ALIGN_16_WIN Mat4x {
  constexpr static std::size_t CompSize = 16;

public:
  Tx _m11, _m12, _m13, _m14,   // indexes: 0,1,2,3
      _m21, _m22, _m23, _m24,  // 4,5,6,7...
      _m31, _m32, _m33, _m34,  //
      _m41, _m42, _m43, _m44;  //

  FORCE_INLINE Mat4x<Tx>();
  FORCE_INLINE Mat4x<Tx>(const Tx (&init_list)[Mat4x<Tx>::CompSize]);
  FORCE_INLINE Mat4x<Tx>(Tx t0, Tx t1, Tx t2, Tx t3, Tx t4, Tx t5, Tx t6, Tx t7, Tx t8, Tx t9, Tx t10, Tx t11, Tx t12, Tx t13, Tx t14, Tx t15);

  FORCE_INLINE int32_t nRows() { return 4; }
  FORCE_INLINE int32_t nCols() { return 4; }
  FORCE_INLINE int32_t size() { return 16; }  // - Returns the number of entries in this matrix.
  FORCE_INLINE Vec4x<Tx> row(int row);
  FORCE_INLINE Vec4x<Tx> col(int col);

  FORCE_INLINE void copyTo(Mat4x<Tx>& to);

  //These can be confusing due to the matrix Order operations. They should be removed in favor of matrix::
  FORCE_INLINE void lookAt(const Vec3x<Tx>& eye, const Vec3x<Tx>& center, const Vec3x<Tx>& up);
  FORCE_INLINE Mat4x<Tx>& translate(Tx x, Tx y, Tx z);
  FORCE_INLINE Mat4x<Tx>& translate(const Vec3x<Tx>& v);
  FORCE_INLINE void rotateRad(Tx radians, Tx x, Tx y, Tx z);
  FORCE_INLINE Mat4x<Tx>& transpose();
  FORCE_INLINE Mat4x<Tx> transposed();
  FORCE_INLINE void clear();
  FORCE_INLINE void setIdentity();
  FORCE_INLINE void decompose(Vec4x<Tx>& pos, Mat4x<Tx>& rot, Vec4x<Tx>& scale) const;
  FORCE_INLINE void decompose(Vec4x<Tx>& pos, Vec4x<Tx>& rot, Vec4x<Tx>& scale, bool bDegreeRotation = false) const;
  FORCE_INLINE Tx at(int row, int col) const;
  FORCE_INLINE int32_t colNum(int ind);
  FORCE_INLINE int32_t rowNum(int ind);
  FORCE_INLINE Tx det();
  FORCE_INLINE Mat3x<Tx> minor(int r, int c);
  FORCE_INLINE Tx cofactor(int r, int c);
  FORCE_INLINE Mat4x<Tx> adj();
  FORCE_INLINE Mat4x<Tx> invert();
  FORCE_INLINE Mat4x<Tx> inverseOf();

  FORCE_INLINE Quaternion<Tx> getQuaternion();
  FORCE_INLINE Vec3x<Tx> getTranslation() const;
  FORCE_INLINE Mat4x<Tx>& setTranslation(Tx x, Tx y, Tx z);
  FORCE_INLINE void setTranslation(const Vec3x<Tx>& vec);
  FORCE_INLINE void setTranslationX(Tx x);
  FORCE_INLINE void setTranslationY(Tx y);
  FORCE_INLINE void setTranslationZ(Tx z);
  FORCE_INLINE Tx getTranslationX() const;
  FORCE_INLINE Tx getTranslationY() const;
  FORCE_INLINE Tx getTranslationZ() const;
  FORCE_INLINE Vec4x<Tx> getTranslationVector() const;

  FORCE_INLINE std::string toString(int precision = -1) const;

#pragma region Operators
  FORCE_INLINE Mat4x<Tx> operator+(const Mat4x<Tx>& m) const;
  FORCE_INLINE Tx& operator[](size_t i);
  FORCE_INLINE const Tx& operator[](size_t i) const;
  FORCE_INLINE Mat4x<Tx>& operator*=(const Mat4x<Tx>& m);
  FORCE_INLINE const Vec4x<Tx> operator*(const Vec4x<Tx>& v) const;
  FORCE_INLINE Mat4x<Tx>& operator=(Tx f);
  FORCE_INLINE bool operator==(const Mat4x<Tx>& rhs) const;
  FORCE_INLINE bool operator!=(const Mat4x<Tx>& rhs) const;
  FORCE_INLINE Mat4x<Tx>& operator=(const Mat4x<Tx>& m);
  FORCE_INLINE const Mat4x<Tx> operator*(const Mat4x<Tx>& m) const;
#pragma endregion

#pragma region Static Matrix Creators
  FORCE_INLINE static Mat4x<Tx> identity();
  FORCE_INLINE static Mat4x<Tx> translation(const Vec3x<Tx>& vTrans);
  FORCE_INLINE static Mat4x<Tx> translation(Tx x, Tx y, Tx z);
  FORCE_INLINE static Mat4x<Tx> rotation(Tx radians, const Vec3x<Tx>& vAxis);
  FORCE_INLINE static Mat4x<Tx> rotation(Tx radians, Tx x, Tx y, Tx z);
  FORCE_INLINE static Mat4x<Tx> scaling(const Vec3x<Tx>& vScale);
  FORCE_INLINE static Mat4x<Tx> scaling(Tx x, Tx y, Tx z);
  FORCE_INLINE static Mat4x<Tx> getOrientToVector(Vec3x<Tx> v, Vec3x<Tx> up);
  FORCE_INLINE static Mat4x<Tx> getLookAt(const Vec3x<Tx>& eye, const Vec3x<Tx>& center, const Vec3x<Tx>& up);
  FORCE_INLINE static Mat4x<Tx> getOrtho(Tx left, Tx right, Tx top, Tx bottom, Tx neard, Tx fard);
  FORCE_INLINE static Mat4x<Tx> getRotationToVector(Vec3x<Tx> v, Vec3x<Tx> up);
  FORCE_INLINE static bool parse(std::string, Mat4x<Tx>& mOut);
  FORCE_INLINE static Mat4x<Tx> projection(Tx n, Tx f, Tx l, Tx r, Tx t, Tx b);                     // set up a projection matrix.
  FORCE_INLINE static Mat4x<Tx> projection(Tx fov, Tx viewport_w, Tx viewport_h, Tx near, Tx far);  // set up a projection matrix.
#pragma endregion

} CACHE_ALIGN_16_GCC;
//////////////////////////////////////////////////////////////////////////

template <typename Tx>
FORCE_INLINE Mat4x<Tx>::Mat4x() {
  *this = 0.0f;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx>::Mat4x(Tx t0, Tx t1, Tx t2, Tx t3, Tx t4, Tx t5, Tx t6, Tx t7, Tx t8, Tx t9, Tx t10, Tx t11, Tx t12, Tx t13, Tx t14, Tx t15) {
  _m11 = t0;
  _m12 = t1;
  _m13 = t2;
  _m14 = t3;
  _m21 = t4;
  _m22 = t5;
  _m23 = t6;
  _m24 = t7;
  _m31 = t8;
  _m32 = t9;
  _m33 = t10;
  _m34 = t11;
  _m41 = t12;
  _m42 = t13;
  _m43 = t14;
  _m44 = t15;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx>::Mat4x(const Tx (&init_list)[Mat4x<Tx>::CompSize]) {
  static_assert(Mat4x<Tx>::CompSize == 16, "Invalid number of initializer list elements.");
  _m11 = init_list[0];
  _m12 = init_list[1];
  _m13 = init_list[2];
  _m14 = init_list[3];

  _m21 = init_list[4];
  _m22 = init_list[5];
  _m23 = init_list[6];
  _m24 = init_list[7];

  _m31 = init_list[8];
  _m32 = init_list[9];
  _m33 = init_list[10];
  _m34 = init_list[11];

  _m41 = init_list[12];
  _m42 = init_list[13];
  _m43 = init_list[14];
  _m44 = init_list[15];
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::identity() {
  return Mat4x<Tx>{
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1};
}
///////////////////////////////////////////////////////////////////////////
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::copyTo(Mat4x<Tx>& __out_ to) {
  to._m11 = _m11;
  to._m12 = _m12;
  to._m13 = _m13;
  to._m14 = _m14;
  to._m21 = _m21;
  to._m22 = _m22;
  to._m23 = _m23;
  to._m24 = _m24;
  to._m31 = _m31;
  to._m32 = _m32;
  to._m33 = _m33;
  to._m34 = _m34;
  to._m41 = _m41;
  to._m42 = _m42;
  to._m43 = _m43;
  to._m44 = _m44;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx>& Mat4x<Tx>::setTranslation(Tx x, Tx y, Tx z) {
  _m41 = x;
  _m42 = y;
  _m43 = z;

  return *this;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::operator+(const Mat4x<Tx>& m) const {
  Mat4x<Tx> ret;
  for (int i = 0; i < 16; ++i) {
    ret[i] = this->Mat(i) + m[i];
  }
  return ret;
}
/**
*  @fn getQuaternion
*  @brief Intel's version of matrix to a quaternion.
*  (http://cache-www.intel.com/cd/00/00/29/37/293748_293748.pdf)
*/
template <typename Tx>
FORCE_INLINE Quaternion<Tx> Mat4x<Tx>::getQuaternion() {
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
  Tx t = (Tx)(s0 * _m11 + s1 * _m22 + s2 * _m33 + 1.0f);
  //assert(t>=0.0);
  //if( t==0.0 ) t=1e-10f;
  Tx s = (Tx)((1.0 / std::sqrt(t)) * 0.5f);

  q[k0] = s * t;

  q[k1] = (Tx)((_m12 - s2 * _m21) * s);
  q[k2] = (Tx)((_m31 - s1 * _m13) * s);
  q[k3] = (Tx)((_m23 - s0 * _m32) * s);

  Quaternion<Tx> ret{q[k0], q[k1], q[k2], q[k3]};
  return ret;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::translation(const Vec3x<Tx>& vTrans) {
  return translation(vTrans.x, vTrans.y, vTrans.z);
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::translation(Tx x, Tx y, Tx z) {
  Mat4x<Tx> m = Mat4x<Tx>::identity();

  m._m41 = x;
  m._m42 = y;
  m._m43 = z;

  return m;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::rotation(Tx radians, const Vec3x<Tx>& vAxis) {
  return rotation(radians, vAxis.x, vAxis.y, vAxis.z);
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::rotation(Tx radians, Tx x, Tx y, Tx z) {
  // - Reference: The openGL reference.http://pyopengl.sourceforge.net/documentation/manual/reference-GL.html
  Mat4x<Tx> Temp = Mat4x<Tx>::identity();

  Tx c = (Tx)cos(radians);
  Tx s = (Tx)sin(radians);
  Tx nc = 1 - c;
  // row matrix

  Temp._m11 = (x * x) * nc + c;
  Temp._m12 = (x * y) * nc + (z * s);
  Temp._m13 = (x * z) * nc - (y * s);
  Temp._m14 = (Tx)0.0;

  Temp._m21 = (y * x) * nc - (z * s);
  Temp._m22 = (y * y) * nc + c;
  Temp._m23 = (y * z) * nc + (x * s);
  Temp._m24 = (Tx)0.0;

  Temp._m31 = (z * x) * nc + (y * s);
  Temp._m32 = (z * y) * nc - (x * s);
  Temp._m33 = (z * z) * nc + c;
  Temp._m34 = (Tx)0.0;

  Temp._m41 = (Tx)0.0;
  Temp._m42 = (Tx)0.0;
  Temp._m43 = (Tx)0.0;
  Temp._m44 = (Tx)1.0;

  return Temp;
}
/**
*  @fn getRotationToVector()
*  @brief Returns a matrix that would, when multiplied by a vector, rotate that vector to align with this input vector v. 
*/
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::getRotationToVector(Vec3x<Tx> v, Vec3x<Tx> up) {
  if (v.x + v.y + v.z != 1.0) {
    v.normalize();
  }
  if (up.x + up.y + up.z != 1.0) {
    up.normalize();
  }

  Tx ang = (Tx)acos(v.dot(up));

  if (ang == 0.0f) {
    // no need for rotation
    return Mat4x<Tx>::identity();
  }

  Vec3x<Tx> perp = up.cross(v);

  if (perp.x + perp.y + perp.z == 0.0)  // vectors are direct opposites.
  {
    Vec3x<Tx> scuz(up.y, -up.z, up.x);
    scuz.normalize();
    perp = scuz.cross(v);
  }

  //TODO: possible error may rotate opposite. (would need to do opposite cross of up.cross(v) scuz cross v

  return Mat4x<Tx>::getRotationRad(ang, perp.x, perp.y, perp.z);
}
/**
*  @fn scale()
*  @details Returns a scale matrix, but does not alter this matrix.
*  @param x,y,z the scaling value.
*  @return A matrix witht the specified scaling.
*
*/
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::scaling(const Vec3x<Tx>& vScale) {
  return scaling(vScale.x, vScale.y, vScale.z);
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::scaling(Tx x, Tx y, Tx z) {
  Mat4x<Tx> m = Mat4x<Tx>::identity();
  m._m11 = x;
  m._m22 = y;
  m._m33 = z;

  return m;
}

template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::projection(Tx fov_radians, Tx viewport_w, Tx viewport_h, Tx z_near, Tx z_far) {
  //setup a 3D projection matrix.
  //fov = field of view (radians)
  //viewport_w - width of viewport (swapchain image)
  //viewport_h - height of viewport.
  //near, far = near and far clipping planes.
  Tx e = (Tx)0.000001;
  if (viewport_w == 0) {
    viewport_w = 1;
  }
  if (fov_radians > (Tx)M_PI_2 - e) {
    fov_radians = (Tx)M_PI_2 - e;
  }
  if (fov_radians < 1 + e) {
    fov_radians = 1 + e;
  }
  Tx vpWidth_2 = (Tx)tan(fov_radians * (Tx)0.5) * z_near;
  Tx arat_1 = viewport_h / viewport_w;  // 1 / (w/h)
  Tx vw = vpWidth_2;
  Tx vh = vpWidth_2 * arat_1;

  return mat4::projection(
      z_near, z_far,
      vw, -vw,
      vh, -vh);
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::projection(Tx n, Tx f, Tx l, Tx r, Tx t, Tx b) {
  //Alternative projection matrix speicfying the viewport bounds.
  //TODO: [-1,1] OpenGL, [0,1] Vulkan
  Mat4x<Tx> m;

  m._m11 = (Tx)(2 * n) / (r - l);
  m._m21 = (Tx)0;
  m._m31 = (Tx)0;
  m._m41 = (Tx)0;

  m._m12 = (Tx)0;
  m._m22 = (Tx)(2 * n) / (t - b);  // *-1.0f; // we added a neagtive here because IDK WHY this is not right
  m._m32 = (Tx)0;
  m._m42 = (Tx)0;

  m._m13 = (Tx)(r + l) / (r - l);
  m._m23 = (Tx)(t + b) / (t - b);
  m._m33 = (Tx) - (f + n) / (f - n);
  m._m43 = (Tx)-1;

  m._m14 = (Tx)0;
  m._m24 = (Tx)0;
  m._m34 = (Tx) - (2 * f * n) / (f - n);
  m._m44 = (Tx)0;

#ifdef COORDINATE_SYSTEM_RHS
  m.transpose();
#endif

  return m;
}

//untested
//http://www.opengl.org/sdk/docs/man/xhtml/gluLookAt.xml
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::getOrientToVector(Vec3x<Tx> v, Vec3x<Tx> up) {
  Mat4x<Tx> m = Mat4x<Tx>::identity();

  v = v.normalize();

  if (up.x + up.y + up.z != 1.0) {
    up = up.normalize();
  }

  Vec3x<Tx> s = v.cross(up);
  Vec3x<Tx> u = s.cross(v);

  m._m11 = s.x;
  m._m21 = u.x;
  m._m31 = -v.x;

  m._m12 = s.y;
  m._m22 = u.y;
  m._m32 = -v.y;

  m._m13 = s.z;
  m._m23 = u.z;
  m._m33 = -v.z;

  return m;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::getLookAt(const Vec3x<Tx>& eye, const Vec3x<Tx>& center, const Vec3x<Tx>& up) {
  Mat4x<Tx> m;
  m.lookAt(eye, center, up);
  return m;
}
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::lookAt(const Vec3x<Tx>& eye, const Vec3x<Tx>& center, const Vec3x<Tx>& up) {
  // also see
  //http://www-01.ibm.com/support/knowledgecenter/ssw_aix_53/com.ibm.aix.opengl/doc/openglrf/gluLookAt.htm%23b5c874e426rree
  /*
    Let E be the 3d column vector(eyeX, eyeY, eyeZ).
    Let C be the 3d column vector(centerX, centerY, centerZ).
    Let U be the 3d column vector(upX, upY, upZ).
    Compute L = C - E.
    Normalize L.
    Compute S = L x U.
    Normalize S.
    Compute U' = S x L.
    (S, 0), (U', 0), (-L, 0), (-E, 1)  (all column vectors)
  */
  Vec3x<Tx> L = center - eye;
  L.normalize();
  Vec3x<Tx> S = L.cross(up);
  S.normalize();
  Vec3x<Tx> D = S.cross(L);
  Vec3x<Tx> E = eye;

  L = L * -1;
  E = E;

  Mat4x<Tx> mm;
  // It seems to be an exact transpose.
  mm._m11 = S.x, mm._m12 = S.y, mm._m13 = S.z, mm._m14 = 0,
  mm._m21 = D.x, mm._m22 = D.y, mm._m23 = D.z, mm._m24 = 0,
  mm._m31 = L.x, mm._m32 = L.y, mm._m33 = L.z, mm._m34 = 0,
  mm._m41 = 0, mm._m42 = 0, mm._m43 = 0, mm._m44 = 1;

  // Not sure if this is right. seems to mimic the opengl matrix correctly
  //mm._m11 = S.x, mm._m12 = D.x, mm._m13 = L.x, mm._m14 =   0,
  //mm._m21 = S.y, mm._m22 = D.y, mm._m23 = L.y, mm._m24 =   0,
  //mm._m31 = S.z, mm._m32 = D.z, mm._m33 = L.z, mm._m34 =   0,
  //mm._m41 =   0, mm._m42 =   0, mm._m43 =   0, mm._m44 =   1;

  Mat4x<Tx> mmTrans;
  mmTrans.setIdentity();
  mmTrans._m14 = -E.x;
  mmTrans._m24 = -E.y;
  mmTrans._m34 = -E.z;

  Mat4x<Tx> ret = mm * mmTrans;
#ifdef COORDINATE_SYSTEM_RHS
  ret = ret.transposed();
#endif
  (*this) = ret;
}
/**
*  @fn transCat()
*  @details Translates the values of this matrix.
*  @param x,y,z the translation value.
*
*/
template <typename Tx>
FORCE_INLINE Mat4x<Tx>& Mat4x<Tx>::translate(Tx x, Tx y, Tx z) {
  return (*this *= translation(x, y, z));
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx>& Mat4x<Tx>::translate(const Vec3x<Tx>& v) {
  return (*this *= translation(v.x, v.y, v.z));
}
/**
*  @fn rotateCat()
*  @details Rotates the values of this matrix.
*  @param a, x,y,z the rotation value in axis-angle form.
*  a is in degrees
*/
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::rotateRad(Tx a, Tx x, Tx y, Tx z) {
  *this *= getRotationRad(a, x, y, z);
}
/**
*  @fn transpose()
*  @details Transpose the matrix.
*/
template <typename Tx>
FORCE_INLINE Mat4x<Tx>& Mat4x<Tx>::transpose() {
  *this = {
      _m11, _m21, _m31, _m41,
      _m12, _m22, _m32, _m42,
      _m13, _m23, _m33, _m43,
      _m14, _m24, _m34, _m44};

  return *this;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::transposed() {
  Mat4x<Tx> ret = *this;
  ret.transpose();
  return ret;
}
/**
*  @fn clear
*  @details Clear the matrix.
*/
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::clear() {
  for (uint32_t n = 0; n < 16; ++n) {
    operator[](n) = (Tx)0.0;
  }
}
/**
*  setIdentity()
*    Sets the identity for this matrix.
*    Works on row matrices as well.
*/
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::setIdentity() {
  _m11 = _m22 = _m33 = _m44 = (Tx)1.0;
  _m21 = _m31 = _m41 = _m12 = (Tx)0.0;
  _m32 = _m42 = _m13 = _m23 = (Tx)0.0;
  _m43 = _m14 = _m24 = _m34 = (Tx)0.0;
}
/**
*  @fn at()
*  @brief Returns the value at the given coordinate in the matrix.
*  @remarks Only use 0-3 for indexes.
*/
template <typename Tx>
FORCE_INLINE Tx Mat4x<Tx>::at(int row, int col) const {
  return operator[](row* col + row);
}
/**
*  @fn colNum
*  @brief Returns the zero-based index of the column in which the supplied index lies.
*/
template <typename Tx>
FORCE_INLINE int32_t Mat4x<Tx>::colNum(int ind) {
  return (int32_t)floor((double)((double)ind / 4.0));
}
/**
*  @fn rowNum
*  @brief Returns the zero-based index of the row in which the supplied index lies.
*/
template <typename Tx>
FORCE_INLINE int32_t Mat4x<Tx>::rowNum(int ind) {
  return ind % 4;
}
/**
*  @fn det() (determinant)
*  @brief Compute the determinant (|M|).
*  @remarks If the determinant of a matrix is 0, then the matrix is singular, and it has no inverse,
*  thus, the inverse() function will return an identity matrix.
*  The determinant can also be computed by multiplying all of the minors by all of the cofactors (Laplace expansion), or by
*  pivoting a single element. This method can be optimized better.
*/
template <typename Tx>
FORCE_INLINE Tx Mat4x<Tx>::det() {
  Tx a = _m11 * ((_m22 * _m33 * _m44) + (_m23 * _m34 * _m42) + (_m24 * _m32 * _m43) - (_m22 * _m34 * _m43) - (_m23 * _m32 * _m44) - (_m24 * _m33 * _m42));
  Tx b = -_m12 * ((_m21 * _m33 * _m44) + (_m23 * _m34 * _m41) + (_m24 * _m31 * _m43) - (_m21 * _m34 * _m43) - (_m23 * _m31 * _m44) - (_m24 * _m33 * _m41));
  Tx c = +_m13 * ((_m21 * _m32 * _m44) + (_m22 * _m34 * _m41) + (_m24 * _m31 * _m42) - (_m21 * _m34 * _m42) - (_m22 * _m31 * _m44) - (_m24 * _m32 * _m41));
  Tx d = -_m14 * ((_m21 * _m32 * _m43) + (_m22 * _m33 * _m41) + (_m23 * _m31 * _m42) - (_m21 * _m33 * _m42) - (_m22 * _m31 * _m43) - (_m23 * _m32 * _m41));
  return a + b + c + d;
}
/**
*  @fn adj()
*  @brief returns the Adjoint of this matrix.
*/
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::adj() {
  //TODO:Optimize (transpose)
  Tx f[] = {
      cofactor(0, 0), cofactor(0, 1), cofactor(0, 2), cofactor(0, 3),
      cofactor(1, 0), cofactor(1, 1), cofactor(1, 2), cofactor(1, 3),
      cofactor(2, 0), cofactor(2, 1), cofactor(2, 2), cofactor(2, 3),
      cofactor(3, 0), cofactor(3, 1), cofactor(3, 2), cofactor(3, 3)};
  return Mat4x(f);  //.transpose();
}
/**
*  @fn getInverse()
*  @brief Returns the inverse of this matrix.
*  @remarks The inverse is the classical adjoint divided by the determinant of the adjoint
*/
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::invert() {
  //**Note:
  //Transpose of an orthogonal matrix is it's inverse
  //If we're orthogonal return the transpose.
  //   return this->transposed();

  // - Convert the matrix to Reduced RE form
  Mat4x<Tx> m;
  // - If the determinant is zero, return m.
  if (!det()) {
    return m;
  }
  // - Classical adjoint is favored here over Gaussian and reduced row-echlon form.
  m = adj();
  Tx d = m.det();
  for (int i = 0; i < 16; ++i) {
    m[i] /= d;
  }

  *this = m;

  return *this;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::inverseOf() {
  Mat4x<Tx> ret = *this;
  ret.invert();
  return ret;
}
/**
*  @fn row()
*  @brief Returns a row in the matrix.
*  @remarks Only use 0-3 for indexes
TODO:ERROR - this may be a column and row interchange problem.
*/
template <typename Tx>
FORCE_INLINE Vec4x<Tx> Mat4x<Tx>::row(int row) {
  return Vec4x<Tx>(operator[](row), operator[](row + 4), operator[](row + 8), operator[](row + 12));
}
/**
*  @fn col()
*  @brief Returns a col in the matrix.
*  @remarks Only use 0-3 for indexes
*/
template <typename Tx>
FORCE_INLINE Vec4x<Tx> Mat4x<Tx>::col(int col) {
  return Vec4x<Tx>(operator[](col * 4 + 0), operator[](col * 4 + 1), operator[](col * 4 + 2), operator[](col * 4 + 3));
}
template <typename Tx>
FORCE_INLINE const Tx& Mat4x<Tx>::operator[](size_t i) const {
  //TODO: get AssertOrThrow2 here without dependnecies.
  if (i >= Mat4x<Tx>::CompSize) {
    throw 0;
  }
  if (i == 0) {
    return this->_m11;
  }
  else if (i == 1) {
    return this->_m12;
  }
  else if (i == 2) {
    return this->_m13;
  }
  else if (i == 3) {
    return this->_m14;
  }
  else if (i == 4) {
    return this->_m21;
  }
  else if (i == 5) {
    return this->_m22;
  }
  else if (i == 6) {
    return this->_m23;
  }
  else if (i == 7) {
    return this->_m24;
  }
  else if (i == 8) {
    return this->_m31;
  }
  else if (i == 9) {
    return this->_m32;
  }
  else if (i == 10) {
    return this->_m33;
  }
  else if (i == 11) {
    return this->_m34;
  }
  else if (i == 12) {
    return this->_m41;
  }
  else if (i == 13) {
    return this->_m42;
  }
  else if (i == 14) {
    return this->_m43;
  }
  else {
    return this->_m44;
  }
}
template <typename Tx>
FORCE_INLINE Tx& Mat4x<Tx>::operator[](size_t i) {
  //TODO: get AssertOrThrow2 here without dependnecies.
  if (i >= Mat4x<Tx>::CompSize) {
    throw 0;
  }
  if (i == 0) {
    return this->_m11;
  }
  else if (i == 1) {
    return this->_m12;
  }
  else if (i == 2) {
    return this->_m13;
  }
  else if (i == 3) {
    return this->_m14;
  }
  else if (i == 4) {
    return this->_m21;
  }
  else if (i == 5) {
    return this->_m22;
  }
  else if (i == 6) {
    return this->_m23;
  }
  else if (i == 7) {
    return this->_m24;
  }
  else if (i == 8) {
    return this->_m31;
  }
  else if (i == 9) {
    return this->_m32;
  }
  else if (i == 10) {
    return this->_m33;
  }
  else if (i == 11) {
    return this->_m34;
  }
  else if (i == 12) {
    return this->_m41;
  }
  else if (i == 13) {
    return this->_m42;
  }
  else if (i == 14) {
    return this->_m43;
  }
  else {
    return this->_m44;
  }
}

/**
*  @fn Multiplication Operator.
*  @details Pre-multiplies this matrix by another matrix.
*  @param m: the matrix to multiply by.
*/
template <typename Tx>
FORCE_INLINE Mat4x<Tx>& Mat4x<Tx>::operator*=(const Mat4x<Tx>& m) {
  if (this == &m) {
    return *this;
  }

  Mat4x<Tx> tMat;

  //64 mul
  //48 add
  tMat._m11 = (_m11 * m._m11) + (_m12 * m._m21) + (_m13 * m._m31) + (_m14 * m._m41);
  tMat._m21 = (_m21 * m._m11) + (_m22 * m._m21) + (_m23 * m._m31) + (_m24 * m._m41);
  tMat._m31 = (_m31 * m._m11) + (_m32 * m._m21) + (_m33 * m._m31) + (_m34 * m._m41);
  tMat._m41 = (_m41 * m._m11) + (_m42 * m._m21) + (_m43 * m._m31) + (_m44 * m._m41);

  tMat._m12 = (_m11 * m._m12) + (_m12 * m._m22) + (_m13 * m._m32) + (_m14 * m._m42);
  tMat._m22 = (_m21 * m._m12) + (_m22 * m._m22) + (_m23 * m._m32) + (_m24 * m._m42);
  tMat._m32 = (_m31 * m._m12) + (_m32 * m._m22) + (_m33 * m._m32) + (_m34 * m._m42);
  tMat._m42 = (_m41 * m._m12) + (_m42 * m._m22) + (_m43 * m._m32) + (_m44 * m._m42);

  tMat._m13 = (_m11 * m._m13) + (_m12 * m._m23) + (_m13 * m._m33) + (_m14 * m._m43);
  tMat._m23 = (_m21 * m._m13) + (_m22 * m._m23) + (_m23 * m._m33) + (_m24 * m._m43);
  tMat._m33 = (_m31 * m._m13) + (_m32 * m._m23) + (_m33 * m._m33) + (_m34 * m._m43);
  tMat._m43 = (_m41 * m._m13) + (_m42 * m._m23) + (_m43 * m._m33) + (_m44 * m._m43);

  tMat._m14 = (_m11 * m._m14) + (_m12 * m._m24) + (_m13 * m._m34) + (_m14 * m._m44);
  tMat._m24 = (_m21 * m._m14) + (_m22 * m._m24) + (_m23 * m._m34) + (_m24 * m._m44);
  tMat._m34 = (_m31 * m._m14) + (_m32 * m._m24) + (_m33 * m._m34) + (_m34 * m._m44);
  tMat._m44 = (_m41 * m._m14) + (_m42 * m._m24) + (_m43 * m._m34) + (_m44 * m._m44);

  *this = tMat;

  return *this;
}
/**
*  Multiply this matrix by a vector.
*  Vector*Matrix always yields a vector
*/
template <typename Tx>
FORCE_INLINE const Vec4x<Tx> Mat4x<Tx>::operator*(const Vec4x<Tx>& v) const {
  // #if using row vectors, alert user, quit.
  Mat4x<Tx> m = *this;
  Vec4x<Tx> vret;

  m._m11 *= v.x;
  m._m12 *= v.x;
  m._m13 *= v.x;
  m._m14 *= v.x;

  m._m21 *= v.y;
  m._m22 *= v.y;
  m._m23 *= v.y;
  m._m24 *= v.y;

  m._m31 *= v.z;
  m._m32 *= v.z;
  m._m33 *= v.z;
  m._m34 *= v.z;

  m._m41 *= v.w;
  m._m42 *= v.w;
  m._m43 *= v.w;
  m._m44 *= v.w;

  vret.x = m._m11 + m._m21 + m._m31 + m._m41;
  vret.y = m._m12 + m._m22 + m._m32 + m._m42;
  vret.z = m._m13 + m._m23 + m._m33 + m._m43;
  vret.w = m._m14 + m._m24 + m._m34 + m._m44;

  return vret;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx>& Mat4x<Tx>::operator=(const Tx f) {
  for (int i = 0; i < 16; ++i) {
    operator[](i) = f;
  }

  return *this;
}
template <typename Tx>
FORCE_INLINE bool Mat4x<Tx>::operator==(const Mat4x<Tx>& rhs) const {
  for (int i = 0; i < 16; ++i) {
    if (operator[](i) != rhs.operator[](i)) {
      return false;
    }
  }
  return true;
}
template <typename Tx>
FORCE_INLINE bool Mat4x<Tx>::operator!=(const Mat4x<Tx>& rhs) const {
  for (int i = 0; i < 16; ++i) {
    if (operator[](i) != rhs.operator[](i)) {
      return true;
    }
  }
  return false;
}
template <typename Tx>
FORCE_INLINE Mat4x<Tx>& Mat4x<Tx>::operator=(const Mat4x<Tx>& m) {
  for (int i = 0; i < 16; ++i) {
    operator[](i) = m[i];
  }

  return *this;
}
template <typename Tx>
FORCE_INLINE const Mat4x<Tx> Mat4x<Tx>::operator*(const Mat4x<Tx>& m) const {
  Mat4x<Tx> tmp = *this;
  tmp *= m;
  return tmp;
}
template <typename Tx>
FORCE_INLINE Vec3x<Tx> Mat4x<Tx>::getTranslation() const {
  Vec3x<Tx> ret;
  ret.x = _m41;
  ret.y = _m42;
  ret.z = _m43;
  return ret;
}
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::setTranslation(const Vec3x<Tx>& vec) {
  _m41 = vec.x;
  _m42 = vec.y;
  _m43 = vec.z;
}
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::setTranslationX(Tx x) {
  _m41 = x;
}
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::setTranslationY(Tx y) {
  _m42 = y;
}
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::setTranslationZ(Tx z) {
  _m43 = z;
}
template <typename Tx>
FORCE_INLINE Tx Mat4x<Tx>::getTranslationX() const {
  return _m41;
}
template <typename Tx>
FORCE_INLINE Tx Mat4x<Tx>::getTranslationY() const {
  return _m42;
}
template <typename Tx>
FORCE_INLINE Tx Mat4x<Tx>::getTranslationZ() const {
  return _m43;
}
template <typename Tx>
FORCE_INLINE Vec4x<Tx> Mat4x<Tx>::getTranslationVector() const {
  return Vec4x<Tx>{_m41, _m42, _m43, _m44};
}

template <typename Tx>
FORCE_INLINE Mat4x<Tx> Mat4x<Tx>::getOrtho(Tx left, Tx right, Tx top,
                                           Tx bottom, Tx neard, Tx fard) {
  Mat4x<Tx> mm;

  Tx a1 = (Tx)2.0 / (right - left);
  Tx a2 = (Tx)2.0 / (top - bottom);   //IDK WY
  Tx a3 = (Tx)-2.0 / (fard - neard);  //IDK WY
  Tx t1 = (right + left) / (right - left) * (Tx)-1.0;
  Tx t2 = (top + bottom) / (top - bottom) * (Tx)-1.0;
  Tx t3 = (fard + neard) / (fard - neard) * (Tx)-1.0;

  //Row major order version
  //mm._m11 =a1, mm._m12 = 0, mm._m13 = 0, mm._m14 =t1,
  //mm._m21 = 0, mm._m22 =a2, mm._m23 = 0, mm._m24 =t2,
  //mm._m31 = 0, mm._m32 = 0, mm._m33 =a3, mm._m34 =t3,
  //mm._m41 = 0, mm._m42 = 0, mm._m43 = 0, mm._m44 = 1;

  // ** OpenGL version - the transpose of the former.
  mm._m11 = a1, mm._m12 = 0, mm._m13 = 0, mm._m14 = 0,
  mm._m21 = 0, mm._m22 = a2, mm._m23 = 0, mm._m24 = 0,
  mm._m31 = 0, mm._m32 = 0, mm._m33 = a3, mm._m34 = 0,
  mm._m41 = t1, mm._m42 = t2, mm._m43 = t3, mm._m44 = 1;

  return mm;
}

template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::decompose(Vec4x<Tx>& pos, Mat4x<Tx>& rot, Vec4x<Tx>& scale) const {
  //http://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati
  //11  21  31  41 << Don't use gl order
  //12  22  32  42
  //13  23  33  43
  //14  24  34  44
  pos = getTranslationVector();

  scale.x = Vec3x<Tx>(_m11, _m21, _m31).length();
  scale.y = Vec3x<Tx>(_m12, _m22, _m32).length();
  scale.z = Vec3x<Tx>(_m13, _m23, _m33).length();

  rot.setIdentity();
  rot._m11 = _m11 / scale.x;
  rot._m21 = _m21 / scale.x;
  rot._m31 = _m31 / scale.x;

  rot._m12 = _m12 / scale.y;
  rot._m22 = _m22 / scale.y;
  rot._m32 = _m32 / scale.y;

  rot._m13 = _m13 / scale.z;
  rot._m23 = _m23 / scale.z;
  rot._m33 = _m33 / scale.z;
}
template <typename Tx>
FORCE_INLINE void Mat4x<Tx>::decompose(Vec4x<Tx>& pos, Vec4x<Tx>& rot, Vec4x<Tx>& scale, bool bDegreeRotation) const {
  Mat4x<Tx> mOut;
  decompose(pos, mOut, scale);

  Quaternion<Tx> q = mOut.getQuaternion();

  q.getAxisAngle(rot);
  if (bDegreeRotation) {
    rot.w = MathUtils::degrees(rot.w);
  }
}
template <typename Tx>
FORCE_INLINE bool Mat4x<Tx>::parse(std::string tok, Mat4x<Tx>& mOut) {
  // - Parse csv matrix string.

  size_t n = 0;
  char c;
  Tx mat[16];
  std::string val = "";
  int mat_ind = 0;

  while (n < tok.length()) {
    c = tok[n++];
    if (c == ',' || c == '\n' || n == tok.length()) {
      mat[mat_ind++] = TypeConv::strToFloat(val);
      val = "";
    }
    else if (isalnum(c) || c == '-' || c == '.' || c == '+' || c == 'e') {
      val += c;
    }
  }

  mOut = mat;

  return true;
}

}  // namespace BR2

#endif