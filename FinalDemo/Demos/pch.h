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
#include <cassert>

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
#include <GamePad.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#include "GameException.h"
#include "Game.h"
#include "GameClock.h"
#include "GameTime.h"
#include "GameComponent.h"
#include "DrawableGameComponent.h"
#include "Utility.h"
#include "GamePadComponent.h"
#include "KeyboardComponent.h"
#include "MouseComponent.h"
#include "VertexDeclarations.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "FirstPersonCamera.h"
#include "FpsComponent.h"
#include "RenderStateHelper.h"
#include "ServiceContainer.h"
#include "StreamHelper.h"
#include "..\Library\Model.h"
#include "..\Library\Mesh.h"
#include "..\Library\ModelMaterial.h"
#include "ColorHelper.h"
#include "SamplerStates.h"
#include "RasterizerStates.h"
#include "BlendStates.h"
#include "Grid.h"
#include "ProxyModel.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

// Local
#include "RenderingGame.h"
#include "DiffuseLightingDemo.h"
#include "PointLightingDemo.h"
#include "SpecularLightingDemo.h"
#include "SpotlightLightingDemo.h"