/**
*  @file QuadPlane.h
*  @author MetalMario971
*  @date 2 / 1 / 2010
*/
#pragma once
#ifndef __QUAD_PLANE_H__
#define __QUAD_PLANE_H__

#include "../math/TriPlane.h"
#include "../math/PlaneEx3.h"

namespace BR2 {
/**
*  @class QuadPlane
*  @brief Derived plane for extra info and stuff.  DO NOT ADD VIRTUAL METHODS
*/
class QuadPlane : public PlaneEx3 {
public:
  BR2_FORCE_INLINE QuadPlane();
  BR2_FORCE_INLINE QuadPlane(vec3&, vec3&, vec3&, vec3&);
  BR2_FORCE_INLINE NOT_VIRTUAL ~QuadPlane() override;

  BR2_FORCE_INLINE PlanePoint containsPoint(vec3& point); // - True if the point falls within the region of the quad plane.
  BR2_FORCE_INLINE PlaneHit hitTest(vec3& p1, vec3& p2);
  BR2_FORCE_INLINE void construct(vec3& dp1, vec3& dp2, vec3& dp3, vec3& dp4);
  BR2_FORCE_INLINE void construct();

public:
  vec3 p1, p2, p3, p4;
};

BR2_FORCE_INLINE PlaneHit
QuadPlane::hitTest(vec3& p1, vec3& p2) {
  PlaneHit ret;
  ret._bContained = false;

  float t = intersectLine(p1, p2).t;        // - Plane based
  if (t >= 0.0 && t <= 1.0) {
    vec3 v = p1 + (p2 - p1) * t;
    //if(v.y!=0) DebugBreak();
    PlanePoint pp = containsPoint(v);
    ret._bContained = pp.contained;        // - Triangle based
    ret._fHitTime = pp.t;
    return ret;
  }
  return ret;
}
//
//
BR2_FORCE_INLINE PlanePoint
QuadPlane::containsPoint(vec3& point) {
  PlanePoint ret;

  // - Here the problem is solved by turning the plane into two triangles.
  // - For CW verts only i think.
  if ((ret.contained = TriPlane::containsPoint(p1, p2, p3, point)))
    return ret;
  else if ((ret.contained = TriPlane::containsPoint(p2, p3, p4, point)))
    return ret;
  return ret;
}

/**
*    @fn construct()
*    @brief Override of the plane construct.  Will calculate the data inside of the QuadPlane
*/
BR2_FORCE_INLINE void
QuadPlane::construct(vec3& dp1, vec3& dp2, vec3& dp3, vec3& dp4) {
  p1 = dp1;
  p2 = dp2;
  p3 = dp3;
  p4 = dp4;
  construct();
}
/**
*    @fn construct()
*    @brief Override of the plane construct.  Will calculate the data inside of the QuadPlane
*/
BR2_FORCE_INLINE void
QuadPlane::construct() {
  PlaneEx3::construct(p1, p2, p3);
}
//
//ctor
//
BR2_FORCE_INLINE QuadPlane::QuadPlane(vec3& dp1, vec3& dp2, vec3& dp3, vec3& dp4) {
  construct(dp1, dp2, dp3, dp4);
}


BR2_FORCE_INLINE QuadPlane::QuadPlane() {

}

BR2_FORCE_INLINE QuadPlane::~QuadPlane() {

}



}

#endif
