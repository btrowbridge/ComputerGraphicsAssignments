#pragma once

#include <Windows.h>
#include <string>
#include <sstream>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// DirectX
#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>

// Local
#include "GameException.h"

#define ReleaseObject(object) if((object) != nullptr) { object->Release(); object = nullptr; }