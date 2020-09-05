#include <SDL2/SDL.h>
#include "./SDLVulkan.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

using namespace VulkanGame;

int main(int argc, char **argv) { 
  (void)argc;
  (void)argv;

  std::cout<< "hello worldy world." <<std::endl;

  SDLVulkan sdl;

  sdl.makeSDLWindow(800, 600);

  // Wait two seconds
  SDL_Delay(3000);

  sdl.cleanup();

  return 0;
}
