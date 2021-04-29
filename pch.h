// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

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

//DirectX11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

//DirectXTK
#include "Audio.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

//namespaces
using namespace DirectX;
using std::string;

#define PARTICLE_COUNT 250
#define PARTICLE_SEARCH_DISTANCE 5.0f
#define WORLD_SIZE XMFLOAT2(1000, 1000)
#define GRID_SIZE XMINT2(100, 100)
#define WORLD_EDGE 100.0f

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.

#endif //PCH_H
