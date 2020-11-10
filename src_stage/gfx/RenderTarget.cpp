#include "../base/BaseHeader.h"
#include "./RenderTarget.h"


namespace VG {
RenderTarget::RenderTarget(std::shared_ptr<GLContext> ct) : GLFramework(ct) {
}
RenderTarget::~RenderTarget() {
}

}//ns Game
