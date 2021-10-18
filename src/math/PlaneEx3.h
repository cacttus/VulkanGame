/**
*  @file PlaneEx3.h
*  @author MetalMario971
*  @date July 20, 2008
*/
#pragma once
#ifndef __MG_MATH_PLANEEX_3_H__
#define __MG_MATH_PLANEEX_3_H__

#include "../math/Plane3.h"
#include "../math/Mat3x.h"

namespace BR2 {
/**
*  @class PlaneEx3
*  @brief Extension of the plane class to include tangent and binormal space.  DO NOT ADD VIRTUAL METHODS
*/
class PlaneEx3 : public Plane3f {
public:
  BR2_FORCE_INLINE PlaneEx3();
  BR2_FORCE_INLINE NOT_VIRTUAL ~PlaneEx3() override;

  float dist2d(vec2& e);  // - Return the X/Z distnace from a vector2 (where y is z)

  BR2_FORCE_INLINE void calcTBN();
  BR2_FORCE_INLINE void construct(vec3& Normal, vec3& Point);
  BR2_FORCE_INLINE void construct(vec3& tri_p1, vec3& tri_p2, vec3& tri_p3);

  vec3 t, b;    // - TBN
  float u;         // - The value for texture space, this is initialized to 1.0
  vec3 origin;  // - Point on the plane.
};

BR2_FORCE_INLINE void PlaneEx3::construct(vec3& tri_p1, vec3& tri_p2, vec3& tri_p3) {
  //@fn construct() 
  // @brief Construct the components of the plane out of a triangle.
  // The origin will be p1.
  // The tangent vector will be p2 and the binormal will be p3
  // The normal will point outward in the clockwise rotation of the tangent vector to the binormal vector.
  // Vertexes treated CCW.
  u = 1.0;
  origin = tri_p1;

  t = (tri_p2 - tri_p1);
  b = (tri_p3 - tri_p1);

  t /= u;
  t.normalize();
  n = b.cross(t);  //20161129 - NOTE: CHANGED THIS FOR THE RHS COORDINATES

  n.normalize();
  b = n.cross(t);

  d = -n.dot(origin);
}
BR2_FORCE_INLINE void PlaneEx3::construct(vec3& normal, vec3& point) {
  Plane3f::construct(normal, point);
  origin = point;
}
BR2_FORCE_INLINE void PlaneEx3::calcTBN() {
}
BR2_FORCE_INLINE PlaneEx3::PlaneEx3() {
}
BR2_FORCE_INLINE PlaneEx3::~PlaneEx3() {
}

}  // namespace BR2

#endif