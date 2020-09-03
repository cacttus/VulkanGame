// #include <algorithm>
// #include <future>
// #include <iostream>
// #include <mutex>
// #include <numeric>
// #include <string>
// #include <thread>
// #include <vector>

#include <SDL2/SDL.h>
#include "../app/SDL.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

void doStuff() {}

using namespace VulkanGameDemo;

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  SDL sdl;

  sdl.makeSDLWindow(800, 600);

  // Wait two seconds
  SDL_Delay(3000);

  sdl.cleanup();

  return 0;
}