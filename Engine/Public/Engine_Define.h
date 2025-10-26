#ifndef Engine_Define_h__
#define Engine_Define_h__

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <memory>
using namespace Microsoft::WRL;


#include <DirectXMath.h>
#include "SimpleMath/SimpleMath.h"

#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/DirectXTex.inl"

#include "Fx11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"

#include <wincodec.h>

using namespace DirectX;
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <array>
using namespace std;
#include <unordered_map>
#include <ctime>

#include <DirectXCollision.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifdef _DEBUG
#pragma comment(lib,"../../Libraries/Lib/DirectXTex_Debug.lib")

#else
#pragma comment(lib,"../../Libraries/Lib/DirectXTex.lib")

#endif

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

#include "Engine_Macro.h"
#include "Engine_Enum.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

using namespace Engine;

#endif // Engine_Define_h__
