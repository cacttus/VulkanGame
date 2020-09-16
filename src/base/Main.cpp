#include "../base/DebugHelper.h"
#include "../base/AppRunner.h"
#include "../base/Gu.h"
#include "../base/Logger.h"

//unit test includes
#include "../world/CSharpScript.h"
#include <iostream>

#define GLM_CONFIG_XYZW_ONLY
#include <glm/vec3.hpp>    // glm::vec3
#include <glm/vec4.hpp>    // glm::vec4
#include <glm/mat4x4.hpp>  // glm::mat4

#include "../math/Vec4x.h"

//This is needed, since SDL defines main.
#ifdef main
#undef main
#endif

using namespace BR2;

void glmTest() {
  //Evaluating GLM vs BR2.
  uint_fast64_t count = 0;
#ifdef _DEBUG
  count = 10000000;
#else
  count = 10000000000;
#endif
  {
    t_timeval a = Gu::getMilliSeconds();
    glm::vec4 v2 = {0, 0, 0, 0};
    for (size_t i = 0; i < count; ++i) {
      glm::vec4 v = {1, 0, 0, 0};
      glm::mat4 m = glm::mat4(1.0);
      v2 = (v2 + (m * v)) / 2.0f;
    }
    std::cout << "glm (val=" << v2.x << "," << v2.y << "," << v2.z << "," << v2.w << "): " << (Gu::getMilliSeconds() - a) << "ms" << std::endl;
  }
  {
    t_timeval a = Gu::getMilliSeconds();
    vec4 v2(0, 0, 0, 0);
    for (size_t i = 0; i < count; ++i) {
      vec4 v(1, 0, 0, 0);
      mat4 m;
      m.setIdentity();
      v2 = (v2 + (m * v)) / 2.0f;
    }
    std::cout << "br2 (val=" + v2.toString() + "): " << (Gu::getMilliSeconds() - a) << "ms" << std::endl;
  }
}

#define MSSFFFILE(x) BR_SRC_FLINE
#define BOOBAR MSSFFFILE(3)

int main(int argc, char **argv) {
  // glmTest();
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
  std::cout << BR_SRC_FNAME << " " << BR_SRC_FLINE << std::endl;
  std::cout << BOOBAR << std::endl;
  std::cout << BRTestFL() << std::endl;

  DebugHelper::debugHeapBegin(false);
  {
    //BR2::DebugHelper::setBreakAlloc(221975);
    std::shared_ptr<AppRunner> ar = std::make_shared<AppRunner>();
    ar->runApp(Gu::argsToVectorOfString(argc, argv),
               std::vector<std::function<bool()>>({[]() {
                 //const string_t strtest = "using System; namespace MyNS { class MyCL{int[] x = new int[]; int b(float a ){ return b + a*(2*(3+.0006d); }} }";

                 //std::vector<CSToken*> toks = CSharpScript::lexTest(strtest);
                 //string_t str = CSharpScript::tokensToString(toks);
                 //std::cout << str << std::endl;

                 //Gu::debugBreak();
                 return true;
               }}));
  }
  DebugHelper::debugHeapEnd();

  return 0;
}
