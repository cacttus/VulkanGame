/**
*  @file GlobalIncludes.h
*  @date November 12, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __GLOBALINCLUDES_14789744473709794265_H__
#define __GLOBALINCLUDES_14789744473709794265_H__

#include "../base/BuildConfig.h"

//////////////////////////////////////////////////////////////////////////
//
// This is the root of all *system and api* include files.
// Note: This file is included everywhere, so we must spare the heavy includes!
//
//////////////////////////////////////////////////////////////////////////

//STL
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <bitset>

//for GCC the _MAX are delcared in these files.
#include <climits>
#include <cfloat>
//for std::sqrt
#include <cmath>
//Enable_Shared_from_this, shared_ptr, unique_ptr
#include <memory>

#include <optional>

//int_fast32_t
//#include <typeinfo>

//For memcpy / memmove
#include <cstring>

#endif