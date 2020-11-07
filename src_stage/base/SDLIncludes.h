/**
*  @file SDLIncludes.h
*  @date 20200221
*  @author MetalMario971
*/
#pragma once
#ifndef __SDLINCLUDES_15823157603355975189_H__
#define __SDLINCLUDES_15823157603355975189_H__

#include "../base/BuildConfig.h"

#if defined(BR2_OS_WINDOWS)
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_net.h>
#elif defined(BR2_OS_LINUX)
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_net.h>
#else
  OS_NOT_IMPLEMENTED
#endif

#endif
