#include "../base/Logger.h"
#include "../base/Img32.h"
#include "../base/GLContext.h"
#include "../base/Allocator.h"
#include "../base/FileSystem.h"
#include "../base/OperatingSystem.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderUniform.h"
#include "../gfx/OpenGLUtils.h"
#include "../model/VertexTypes.h"
#include "../model/VertexFormat.h"
#include "../model/UtilMeshGrid.h"
#include "../model/UtilMeshAxis.h"
#include "../model/UtilMeshSphere.h"
#include "../model/UtilMeshBox.h"
#include "../model/UtilMeshInline.h"
#include <iostream>

namespace BR2 {

RenderUtils::RenderUtils(std::shared_ptr<GLContext> ctx){
  _pContext=ctx;
}
#pragma region Debug Draw
//UNSAFE
void RenderUtils::renderTexturedQuadAttrib(std::shared_ptr<CameraNode> cam, float size) {
  int attr_v = 0;
  int attr_c = 1;
  int attr_n = 2;
  int attr_t = 3;
  //UNSAFE
  static const float s = 100;
  static const GLfloat _vertexes[] = {
      0, 0, 0,
      0, 0, s,
      s, 0, 0,
      s, 0, s};
  static const GLfloat _colors[] = {
      1, 1, 1, 1,
      1, 1, 1, 1,
      1, 1, 1, 1,
      1, 1, 1, 1};
  static const GLfloat _normals[] = {
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0};
  static const GLfloat _tcoords[] = {
      0, 1,
      0, 0,
      1, 1,
      1, 0};
  static const GLushort _indexes[] = {
      0,
      1,
      2,
      2,
      1,
      3,
  };

  static const int _nIndexes = 6;

  getContext()->glActiveTexture(GL_TEXTURE0);

  GLuint bdVerts,
      bdColors,
      bdTextures,
      bdNormals,
      bdIndexes,
      vaoIndexes;

  getContext()->glGenVertexArrays(1, (GLuint*)&vaoIndexes);
  getContext()->glBindVertexArray(vaoIndexes);

  getContext()->glGenBuffers(1, (GLuint*)&bdVerts);
  getContext()->glGenBuffers(1, (GLuint*)&bdColors);
  getContext()->glGenBuffers(1, (GLuint*)&bdTextures);
  getContext()->glGenBuffers(1, (GLuint*)&bdNormals);
  getContext()->glGenBuffers(1, (GLuint*)&bdIndexes);

  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdVerts);
  getContext()->glBufferData(GL_ARRAY_BUFFER,
                                     4 * sizeof(GLfloat) * 3,
                                     _vertexes,
                                     GL_STATIC_DRAW);

  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdColors);
  getContext()->glBufferData(GL_ARRAY_BUFFER,
                                     4 * sizeof(GLfloat) * 4,
                                     _colors,
                                     GL_STATIC_DRAW);

  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdNormals);
  getContext()->glBufferData(GL_ARRAY_BUFFER,
                                     4 * sizeof(GLfloat) * 3,
                                     _normals,
                                     GL_STATIC_DRAW);

  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdTextures);
  getContext()->glBufferData(GL_ARRAY_BUFFER,
                                     4 * sizeof(GLfloat) * 2,
                                     _tcoords,
                                     GL_STATIC_DRAW);

  //Hmm...
  getContext()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bdIndexes);
  getContext()->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                     6 * sizeof(GL_UNSIGNED_SHORT),
                                     _indexes,
                                     GL_STATIC_DRAW);

  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,NULL);
  //glBindBuffer(GL_ARRAY_BUFFER,NULL);

  // MUST BIND BUFFER DATA TO USE VERTEX ATTRIBS
  // MUST USE VBOS WITH VERTEX ATTRIBS

  getContext()->glEnableVertexAttribArray(attr_v);
  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdVerts);
  getContext()->glVertexAttribPointer(attr_v,
                                              3,
                                              GL_FLOAT,
                                              GL_FALSE,
                                              sizeof(GLfloat) * 3,
                                              NULL);
  getContext()->glEnableVertexAttribArray(attr_c);
  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdColors);
  getContext()->glVertexAttribPointer(attr_c,
                                              4,
                                              GL_FLOAT,
                                              GL_FALSE,
                                              sizeof(GLfloat) * 4,
                                              NULL);
  getContext()->glEnableVertexAttribArray(attr_n);
  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdNormals);
  getContext()->glVertexAttribPointer(attr_n,
                                              3,
                                              GL_FLOAT,
                                              GL_FALSE,
                                              sizeof(GLfloat) * 3,
                                              NULL);
  getContext()->glEnableVertexAttribArray(attr_t);
  getContext()->glBindBuffer(GL_ARRAY_BUFFER, bdTextures);
  getContext()->glVertexAttribPointer(attr_t,
                                              2,
                                              GL_FLOAT,
                                              GL_FALSE,
                                              sizeof(GLfloat) * 2,
                                              NULL);
  // CheckGpuErrorsDbg();

  //std::cout<<_vertexes<<std::endl;
  //std::cout<<_colors<<std::endl;
  //std::cout<<_normals<<std::endl;
  //std::cout<<_tcoords<<std::endl;
  //std::cout<<_indexes<<std::endl;

  getContext()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bdIndexes);

  getContext()->glBindVertexArray(vaoIndexes);
  glDrawElements(GL_TRIANGLES, _nIndexes, GL_UNSIGNED_SHORT, nullptr);
  getContext()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)0);
  getContext()->glBindBuffer(GL_ARRAY_BUFFER, (GLuint)0);

  getContext()->glDeleteBuffers(1, (GLuint*)&bdVerts);
  getContext()->glDeleteBuffers(1, (GLuint*)&bdColors);
  getContext()->glDeleteBuffers(1, (GLuint*)&bdTextures);
  getContext()->glDeleteBuffers(1, (GLuint*)&bdNormals);
  getContext()->glDeleteBuffers(1, (GLuint*)&bdIndexes);
  getContext()->glDeleteVertexArrays(1, (GLuint*)&vaoIndexes);
}
void RenderUtils::drawAxisShader(std::shared_ptr<CameraNode> cam, float scale, float lineWidth, const mat4& transform) {
  UtilMeshAxis* ax = new UtilMeshAxis(getContext(), scale, lineWidth, transform);
  ax->init();
  ax->draw(cam);
  delete ax;
}
//void RenderUtils::drawWireSphereShader(float fRadius, vec3& vOffset, vec4& vColor, int32_t nSlices, int32_t nStacks)
//{
//    UtilMeshSphere* ax = new UtilMeshSphere(Gu::getGraphicsContext(), fRadius, vOffset, vColor, nSlices, nStacks);
//    ax->init();
//    ax->draw();
//    delete ax;
//}
void RenderUtils::drawWireSphereShader(std::shared_ptr<CameraNode> cam, float fRadius, const vec4& vColor, int32_t nSlices, int32_t nStacks, mat4* pMatrix) {
  UtilMeshSphere* ax = new UtilMeshSphere(getContext(), fRadius, vec3(0, 0, 0), vColor, nSlices, nStacks);
  ax->init();
  if (pMatrix != nullptr) {
    ax->setModelMatrix(*pMatrix);
  }
  ax->draw(cam);
  delete ax;
}
void RenderUtils::drawWireBoxShader(std::shared_ptr<CameraNode> cam, const Box3f* box, const vec3& vOffset, const vec4& vColor) {
  UtilMeshBox* ax = new UtilMeshBox(getContext(), box, vOffset, vColor);
  ax->init();
  ax->draw(cam);
  delete ax;
}
void RenderUtils::drawSolidBoxShaded(std::shared_ptr<CameraNode> cam, const Box3f* box, const vec3& vOffset, const vec4& vColor) {
  UtilMeshBox* ax = new UtilMeshBox(getContext(), box, vOffset, vColor);
  ax->setWireFrame(false);
  ax->init();
  ax->draw(cam);
  delete ax;
}
//void RenderUtils::drawPickBox(Box3f* box, uint32_t uiColorId)
//{
//    UtilMeshPickBox* ax = new UtilMeshPickBox(Gu::getGraphicsContext(), box, uiColorId);
//    ax->init();
//    ax->draw();
//    delete ax;
//}
//void RenderUtils::inlineBegin(GLenum type, UtilMeshInline& __out_ mesh)
//{
//    //mesh = new UtilMeshInline();
//    mesh.begin(type);
//}
//void RenderUtils::inlineVec3(vec3& v, UtilMeshInline& __out_ mesh)
//{
//    mesh.v3f1(v);
//}
//void RenderUtils::inlineEnd(UtilMeshInline& __out_ mesh)
//{
//    mesh.end();
//}
void RenderUtils::drawGridShader(std::shared_ptr<CameraNode> cam, float r, float g, float b, int32_t nSlices, float fSliceWidth, const vec3& center, std::shared_ptr<ShaderBase> pShader) {
  UtilMeshGrid* pGrid = new UtilMeshGrid(getContext(), r, g, b, nSlices, fSliceWidth, center);
  pGrid->init();
  pGrid->draw(cam);
  delete pGrid;
}
void RenderUtils::drawFrustumShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<FrustumBase> pf, const vec4& avColor) {
  getContext()->setLineWidth(3.0f);

  UtilMeshInline mi(getContext());
  Color4f c4 = avColor;
  mi.begin(GL_LINES);
  {
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntl), c4), v_v3c4(pf->PointAt(fpt_ftl), c4));  // Edges
    mi.vt2(v_v3c4(pf->PointAt(fpt_nbr), c4), v_v3c4(pf->PointAt(fpt_fbr), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_nbl), c4), v_v3c4(pf->PointAt(fpt_fbl), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntr), c4), v_v3c4(pf->PointAt(fpt_ftr), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ftl), c4), v_v3c4(pf->PointAt(fpt_ftr), c4));  // Far Quad
    mi.vt2(v_v3c4(pf->PointAt(fpt_fbl), c4), v_v3c4(pf->PointAt(fpt_fbr), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ftl), c4), v_v3c4(pf->PointAt(fpt_fbl), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ftr), c4), v_v3c4(pf->PointAt(fpt_fbr), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntl), c4), v_v3c4(pf->PointAt(fpt_ntr), c4));  // Near Quad
    mi.vt2(v_v3c4(pf->PointAt(fpt_nbl), c4), v_v3c4(pf->PointAt(fpt_nbr), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntl), c4), v_v3c4(pf->PointAt(fpt_nbl), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntr), c4), v_v3c4(pf->PointAt(fpt_nbr), c4));
  }
  mi.endAndDraw(cam);
  getContext()->setLineWidth(1.0f);
}
#pragma endregion

void RenderUtils::saveTexture(string_t&& strLoc, GLuint iGLTexId, GLenum eTexTarget, int iCubeMapSide) {
  std::shared_ptr<Img32> bi = std::make_shared<Img32>();
  if (getContext()->getTextureDataFromGpu(bi.get(), iGLTexId, eTexTarget, iCubeMapSide) == true) {
    //the GL tex image must be flipped to show upriht/
    bi->flipV();
    Gu::saveImage(strLoc, bi);
    getContext()->chkErrRt();
  }
}

void RenderUtils::makeVertexFormats() {
  if (v_v3c4x2::_pVertexFormat == nullptr) {
    v_v3c4x2::_pVertexFormat = std::make_shared<VertexFormat>("v_v3c4x2");
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::c4_01);
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::x2_01);
  }
  if (v_v2c4x2::_pVertexFormat == nullptr) {
    v_v2c4x2::_pVertexFormat = std::make_shared<VertexFormat>("v_v2c4x2");
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::v2_01);
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::c4_01);
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::x2_01);
  }
  if (v_v3n3x2::_pVertexFormat == nullptr) {
    v_v3n3x2::_pVertexFormat = std::make_shared<VertexFormat>("v_v3n3x2");
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::n3_01);
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::x2_01);
  }
  if (v_v3x2::_pVertexFormat == nullptr) {
    v_v3x2::_pVertexFormat = std::make_shared<VertexFormat>("v_v3x2");
    v_v3x2::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3x2::_pVertexFormat->addComponent(VertexUserType::x2_01);
  }
  if (v_v3n3::_pVertexFormat == nullptr) {
    v_v3n3::_pVertexFormat = std::make_shared<VertexFormat>("v_v3n3");
    v_v3n3::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3n3::_pVertexFormat->addComponent(VertexUserType::n3_01);
  }
  if (v_v3::_pVertexFormat == nullptr) {
    v_v3::_pVertexFormat = std::make_shared<VertexFormat>("v_v3");
    v_v3::_pVertexFormat->addComponent(VertexUserType::v3_01);
  }
  if (v_v2x2::_pVertexFormat == nullptr) {
    v_v2x2::_pVertexFormat = std::make_shared<VertexFormat>("v_v2x2");
    v_v2x2::_pVertexFormat->addComponent(VertexUserType::v2_01);
    v_v2x2::_pVertexFormat->addComponent(VertexUserType::x2_01);
  }
  if (v_v2c4::_pVertexFormat == nullptr) {
    v_v2c4::_pVertexFormat = std::make_shared<VertexFormat>("v_v2c4");
    v_v2c4::_pVertexFormat->addComponent(VertexUserType::v2_01);
    v_v2c4::_pVertexFormat->addComponent(VertexUserType::c4_01);
  }
  if (v_v3c3x2n3::_pVertexFormat == nullptr) {
    v_v3c3x2n3::_pVertexFormat = std::make_shared<VertexFormat>("v_v3c3x2n3");
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::c3_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::x2_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::n3_01);
  }
  if (v_v3i2n3::_pVertexFormat == nullptr) {
    v_v3i2n3::_pVertexFormat = std::make_shared<VertexFormat>("v_v3i2n3");
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::i2_01);
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::n3_01);
  }
  if (v_v3c4::_pVertexFormat == nullptr) {
    v_v3c4::_pVertexFormat = std::make_shared<VertexFormat>("v_v3c4");
    v_v3c4::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3c4::_pVertexFormat->addComponent(VertexUserType::c4_01);
  }
  if (v_v3c4x2n3::_pVertexFormat == nullptr) {
    v_v3c4x2n3::_pVertexFormat = std::make_shared<VertexFormat>("v_v3c4x2n3");
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::v3_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::c4_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::x2_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::n3_01);
  }
  if (v_GuiVert::_pVertexFormat == nullptr) {
    v_GuiVert::_pVertexFormat = std::make_shared<VertexFormat>("v_GuiVert");
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::v4_01);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::v4_02);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::v4_03);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::v2_01);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::u2_01);
  }
}

}  // namespace BR2
