#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{

	typedef struct tagEngine_Desc
	{
		HWND hWnd;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		unsigned int iNumLevels;
		WINMODE		winMode;

	}ENGINE_DESC;


	struct VertexColorData
	{
		_float3 position = { 0,0,0 };
		_float4 color = { 0,0,0,0 };


		static vector<D3D11_INPUT_ELEMENT_DESC> desc;

	};


	struct VertexTextureData
	{
		_float3 position = { 0,0,0 };
		_float2 uv = { 0,0 };


		static vector<D3D11_INPUT_ELEMENT_DESC> desc;

	};

}
#endif // Engine_Struct_h__
