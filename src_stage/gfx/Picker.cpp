#include "../core/opengl/GLContext.h"
#include "../base/InputManager.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/FpsMeter.h"
#include "../base/GraphicsWindow.h"
#include "../gfx/Picker.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/DeferredFramebuffer.h"
#include "../gfx/RenderPipe.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/CameraNode.h"
#include "../core/opengl/OpenGLUtils.h"

namespace VG {
Picker::Picker(std::shared_ptr<GLContext> pc, std::shared_ptr<RenderPipe> rp) : GLFramework(pc) {
  _pRenderPipe = rp;
}
Picker::~Picker() {
}
uint32_t Picker::genPickId() {
  //DEBUG pick ID that shows the color of the picked object.
  _iid++;
  if (_iid == Picker::c_iInvalidPickId) {
    _iid = 0;
  }

  return _iid;
}
void Picker::update(std::shared_ptr<InputManager> pFingers) {
  updatePickedPixel((int32_t)pFingers->getMousePos_Relative().x, (int32_t)pFingers->getMousePos_Relative().y);
}
void Picker::updatePickedPixel(int32_t x, int32_t y) {
  //vec2 mp = Gu::GetMousePosInWindow();
  //if (!Gu::GetRenderManager()->getViewport()->containsPointRelativeToWindow(mp)){
  //    return;
  //}

  //getContext()->debugGetRenderState();

  getContext()->glBindFramebuffer(GL_READ_FRAMEBUFFER, _pRenderPipe->getBlittedDeferred()->getFramebufferId());
  getContext()->chkErrDbg();

  glReadBuffer(GL_COLOR_ATTACHMENT4);
  getContext()->chkErrDbg();

  //getContext()->debugGetRenderState();

  samplePixelId(x, y, _uiLastSelectedPixelId);
  getContext()->chkErrDbg();

#ifdef _DEBUG

  if (_uiLastSelectedPixelId > 0) {
    if (_pRenderPipe->getWindow()->getFpsMeter()->frameMod(20)) {
      BRLogDebug("(" + x + "," + y + "), picked " + _uiLastSelectedPixelId);
    }
  }

  getContext()->chkErrDbg();

#endif

  glReadBuffer(GL_NONE);
  getContext()->chkErrDbg();
}
void Picker::samplePixelId(int32_t x, int32_t y, uint32_t& __out_ selectedId) {
  GLuint pixel = 0;

  //https://www.khronos.org/opengles/sdk/docs/man/xhtml/glReadPixels.xml
  //If the currently bound framebuffer is not the default framebuffer object, color components
  // are read from the color image attached to the GL_COLOR_ATTACHMENT0 attachment point.
  getContext()->debugGetRenderState();

BRLogTODO("Serious hack right here- fix it");
  int32_t iHeight = 600; //getContext()->getActiveCamera()->getViewport()->getHeight();

  glReadPixels(x - 1,
               iHeight - y + 1,
               1, 1,
               GL_RED_INTEGER,
               GL_UNSIGNED_INT,
               (GLvoid*)&pixel);

  getContext()->chkErrDbg();

  selectedId = pixel;
}

}  // namespace VG
