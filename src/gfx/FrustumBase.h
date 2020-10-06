/*----------------------------------------
  View frustum class.
  Calculates the view frustum each
  frame.
  Programmer: MetalMario971
  Sept 15, 2007
  Planes:
  n,f,l,r,t,b
  For detail see documentation.
  1/31/2010 - modified
  2/1/2010 - Fixed major bug in width of frustum near calculation that was messing things up before.
          also added enumerations of planes / points.
  20141203 mods for shadow maps
----------------------------------------*/
#pragma once
#ifndef __FRUSTUM_CLASS_H__
#define __FRUSTUM_CLASS_H__

#include "../base/BaseHeader.h"
#include "../math/MathAll.h"
#include "../gfx/GfxHeader.h"
#include "../model/SceneNode.h"
namespace BR2 {
/**
*  @class Frustum
*  @details Frustum class.
*  The normals of the frustum planes point inward
*/
class FrustumBase : public VirtualMemory {
public:
  static const int NumPlanes = 6;
  static const int NumPoints = 8;

public:
  FrustumBase(std::shared_ptr<RenderViewport> pv, float fov);
  virtual ~FrustumBase() override;

  mat4 getProjectionMatrix();
  Box3f* getBoundBox() { return _minimax; }
  Line3f getCenterAxis();  // - Returns the view vector of this frustum.
  vec3 getViewVector();    // - Returns the view vector of this frustum.
  vec3 getNearPlaneCenterPoint();
  QuadPlane PlaneAt(int i) { return Planes[i]; }
  vec3 PointAt(FrustumPoint i) { return Points[i]; }
  const QuadPlane* getPlane(int32_t i) ;
  const vec3* getPoint(int32_t i) ;
  vec3* getPoints() { return Points; }

  float getZFar() { return z_far; }
  float getZNear() { return z_near; }
  void setZFar(float f) { z_far = f; }
  void setZNear(float f) { z_near = f; }
  bool hasPointXZ(vec2& dp);
  bool hasPoint(vec3& p);  // - Checks for a point inside the frustum
  bool hasQuad(vec3& p1, vec3& p2, vec3& p3, vec3& p4);
  bool hasTri(vec3& p1, vec3& p2, vec3& p3);
  bool hasBox(const Box3f* pCube);
  bool hasFrustum(std::shared_ptr<FrustumBase> pf);
  float getTanFov2() { return tan_fov_2; }
  void update(const vec3& NormalizedView, const vec3& CamPos, const vec3& upVec, ProjectionMode::e fpt);
  void projectFrom(std::shared_ptr<FrustumBase> pOther, vec3& viewNormal, vec3& viewPos, float nearDist, float farDist, bool bOrthographic = true);  // - Creates a projection encompassing another frustum.
  void setFov(float fov);
  void screenPointToWorldPointNear(const vec2& screenPt, vec3& __out_ worldPt, float fPushMultiplier = 0.0f, bool bTest = false);
  void screenPointToWorldPointFar(const vec2& screenPt, vec3& __out_ worldPt, float fPushMultiplier = 0.0f, bool bTest = false);
  void screenQuadToWorldQuad(const Box2f* __in_ pScreenQuad, Quad3f* __out_ pWorldQuad, float fPushMultiplier = 0.001f, bool bTest = false);

protected:
  QuadPlane Planes[NumPlanes];
  vec3 Points[NumPoints];  //nbl fbl fbr nbr ntl ftl ftr ntr
  float tan_fov_2;
  float z_near = 1.0f;
  float z_far = 1000.0f;
  ProjectionMode::e _eProjectionMode;
  std::shared_ptr<RenderViewport> _pViewportRef;  // - Reference to camera viewport.
  Box3f* _minimax;

  void constructPointsAndPlanes(vec3& farCenter, vec3& nearCenter, vec3& upVec, vec3& rightVec,
                                float w_near_2, float w_far_2,
                                float h_near_2, float h_far_2);
  void projectScreenPointToWorldPoint(const vec2& screenPt, vec3& __out_ worldPt,
                                      FrustumPoint tl, FrustumPoint bl, FrustumPoint tr, float fPushMultiplier, bool bTest = false);
  void setupOrthographic(FrustumProjectionParameters* params);
  void setupPerspective(FrustumProjectionParameters* params, bool bProjectBox = false);
  void setupBox(FrustumProjectionParameters* params);

};

}  // namespace BR2

#endif