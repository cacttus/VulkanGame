
#include "../app/SDL.h"

#include <SDL2/SDL.h>

namespace VulkanGameDemo {
class SDL_Internal { 
public:
  SDL_Window* _pWindow = nullptr;
};

SDL::SDL() {
  _pint  = std::make_unique<SDL_Internal>();
}
SDL::~SDL() {
  SDL::cleanup();
  _pint  = nullptr;
}
void SDL::makeSDLWindow(int32_t width, int32_t height) {
  SDL_Surface *screenSurface = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  } else {
    _pint->_pWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width,
                              height, SDL_WINDOW_SHOWN);
    if (_pint->_pWindow  == nullptr) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
      screenSurface = SDL_GetWindowSurface(_pint->_pWindow);
      SDL_FillRect(screenSurface, NULL,
                   SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
      SDL_UpdateWindowSurface(_pint->_pWindow);
    }
  }
}

void SDL::cleanup(){
  if(_pint->_pWindow){
    SDL_DestroyWindow(_pint->_pWindow);
    _pint->_pWindow = nullptr;
  }

  SDL_Quit();
}

} // namespace VulkanGameDemo