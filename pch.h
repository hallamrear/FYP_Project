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

#endif //PCH_H
