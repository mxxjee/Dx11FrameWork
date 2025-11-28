
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING


#ifndef Engine_Define_h__
#define Engine_Define_h__


#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <memory>
#include <dxgi1_3.h>
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;


#include <DirectXMath.h>
#include "SimpleMath/SimpleMath.h"

#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/DirectXTex.inl"

#include "Fx11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/ScreenGrab.h"

#ifdef _DEBUG

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/VertexTypes.h>

#endif // _DEBUG


#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"


#include <wincodec.h>


using namespace DirectX;
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <array>
#include <fstream>
#include <filesystem>
#include <queue>



#include <experimental\filesystem>
namespace fs = std::experimental::filesystem;

using namespace std;
#include <unordered_map>
#include <ctime>

#include <DirectXCollision.h>


#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifdef _DEBUG
#pragma comment(lib,"../../EngineSDK/lib/DirectXTex_Debug.lib")

#else
#pragma comment(lib,"../../EngineSDK/lib/DirectXTex.lib")

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

namespace Engine
{
	static const unsigned int g_iMaxNumBones = 512;
}
#include "Engine_Macro.h"
#include "Engine_Enum.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

using namespace Engine;

#endif // Engine_Define_h__
