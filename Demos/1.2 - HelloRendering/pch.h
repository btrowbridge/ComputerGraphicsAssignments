#pragma once

// Windows and standard libraries
#include <Windows.h>
#include <wrl.h>
#include <string>
#include <sstream>
#include <memory>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// DirectX
#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// Local
#include "GameException.h"
#include "Game.h"