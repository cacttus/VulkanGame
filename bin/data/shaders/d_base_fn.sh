//Base functions for shaders
// 20160501 Sw
//1) Version 420 onward is required for multidimensional C style arrays.
//2) Version 400 required per spec for varying array dimensions e.g. we get
//        error: sampler arrays indexed with non-constant expressions are forbidden in GLSL 1.30 and later
//  With 330 core (embedded) so we need to figure out how to fix this. 
//  For now we must stick to at least 400 or 420
#version 420 core


#define length2(x) dot(x,x)



