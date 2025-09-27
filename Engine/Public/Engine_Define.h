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


using namespace DirectX;
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
using namespace std;
#include <unordered_map>
#include <ctime>

#include "Engine_Macro.h"
#include "Engine_Enum.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"


#ifdef _DEBUG
#pragma comment(lib,"../../Libraries/Lib/DirectXTex_Debug.lib")

#else
#pragma comment(lib,"../../Libraries/Lib/DirectXTex.lib")

#endif

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

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


using namespace Engine;

#endif // Engine_Define_h__
