#include "../base/DebugHelper.h"
#include "../base/Logger.h"
#include "../base/UnitTests.h"
#include "../base/Base.h"
#include "../core/SDLApp.h"

//unit test includes
//#include "../world/CSharpScript.h"
#include <iostream>

//This is needed, since SDL defines main.
#ifdef main
#undef main
#endif

using namespace VG;

int main(int argc, char **argv) {
//#ifdef _DEBUG
//  UnitTests::run(true);
//#endif

  DebugHelper::debugHeapBegin(false);
  {
    //VG::DebugHelper::setBreakAlloc(221975);
    auto ar = std::make_shared<SDLApp>();
    ar->runApp(Base::argsToVectorOfString(argc, argv),
               std::vector<std::function<bool()>>({[]() {
                 //const string_t strtest = "using System; namespace MyNS { class MyCL{int[] x = new int[]; int b(float a ){ return b + a*(2*(3+.0006d); }} }";

                 //std::vector<CSToken*> toks = CSharpScript::lexTest(strtest);
                 //string_t str = CSharpScript::tokensToString(toks);
                 //std::cout << str << std::endl;

                 //Base::debugBreak();
                 return true;
               }}));
  }
  DebugHelper::debugHeapEnd();

  return 0;
}
