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
#include <Shlwapi.h>

// DirectX
#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <GamePad.h>
#include <Keyboard.h>
#include <Mouse.h>

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

// Local
#include "GameException.h"
#include "Game.h"
#include "GameClock.h"
#include "GameTime.h"
#include "GameComponent.h"
#include "DrawableGameComponent.h"
#include "Utility.h"
#include "VertexDeclarations.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "FirstPersonCamera.h"
#include "GamePadComponent.h"
#include "KeyboardComponent.h"
#include "MouseComponent.h"
#include "RenderStateHelper.h"
#include "ServiceContainer.h"
#include "StreamHelper.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "Mesh.h"
#include "ColorHelper.h"
#include "SamplerStates.h"