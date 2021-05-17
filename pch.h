// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define NOMINMAX
#include "framework.h"

#define _USE_MATH_DEFINES
#ifndef APSTUDIO_INVOKED
#include <SDKDDKVer.h>
#endif

#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//system
#include <string>
#include <vector>
#include <exception>
#include <math.h>
#include <cmath>
#include <algorithm>

//namespaces
using std::string;

//Particle Info
#define DEFAULT_PARTICLE_MASS 10.0F
#define DEFAULT_PARTICLE_DRAG_COEFFICIENT 0.43f
#define PARTICLE_COLLIDER_SIZE 10.0f
#define PARTICLE_SEARCH_DISTANCE (10.0f * PARTICLE_COLLIDER_SIZE)
#define WORLD_SIZE Vector2f(1000.0f, 1000.0f)
#define GRID_SIZE Vector2i(40, 40)
#define WORLD_EDGE 123.0f

//Simulation stuff
#define STARTING_PARTICLE_COUNT 2
#define MAX_PARTICLE_COUNT 500
#define KERNEL_HEIGHT 16.0f
#define GRAVITY Vector2f(0.0f, 9.81f)
#define REST_LENGTH 1.0f
#define DAMPENING 0.75f

//#define GAS_CONSTANT  8.314
//#define REST_DENSITY 100.0f
//#define VISCOSITY_CONSTANT 1000.0f

#define GAS_CONSTANT  2000.0f
#define REST_DENSITY 1000.0f
#define VISCOSITY_CONSTANT 1000.0f

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.

#endif //PCH_H
