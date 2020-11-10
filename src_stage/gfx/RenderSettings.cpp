#include "../gfx/RenderSettings.h"


namespace VG {
void RenderSettings::init() {
  _pRenderDebug = std::make_shared<RenderDebug>();
}


}//ns Game
