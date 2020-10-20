/**
*  @file SDLGLIncludes.h
*  @date 20200224
*  @author MetalMario971
*/
#pragma once
#ifndef __SDLGLINCLUDES_15825791622018333782_H__
#define __SDLGLINCLUDES_15825791622018333782_H__

#include "../base/BuildConfig.h"

//#define NO_SDL_GLEXT //--TODO: for linux we should define this.
#include <SDL_opengl.h>
#if defined (BR2_OS_LINUX)
  #include <GL/glext.h>  
#endif


#endif
