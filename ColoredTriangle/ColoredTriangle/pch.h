#pragma once

// Windows and standard libraries
#include <Windows.h>
#include <wrl.h>
#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <codecvt>
#include <cstdint>
#include <vector>

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

// Library
#include "GameException.h"
#include "Game.h"
#include "GameClock.h"
#include "GameTime.h"
#include "GameComponent.h"
#include "DrawableGameComponent.h"
#include "Utility.h"
#include "VertexDeclarations.h"
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"

// Local
#include "RenderingGame.h"
#include "ColoredTriangleDemo.h"
#include "CubeDemo.h"