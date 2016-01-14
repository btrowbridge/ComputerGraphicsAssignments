#pragma once

#include <Windows.h>
#include <sstream>
#include <string>


#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

//DirectX
#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "GameException.h"

#define ReleaseObject(object) if((object) != NULL) { object-> Release(); object = NULL; }
#define DeleteObjects(objects) if ((objects)!= NULL){delete[]objects; objects = NULL;}
#define DeleteObject(object) if ((object)!= NULL){delete object; object = NULL;}

namespace GameLibrary 
{
	typedef unsigned char byte;
}


