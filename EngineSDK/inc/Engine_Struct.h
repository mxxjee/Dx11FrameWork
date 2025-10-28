#pragma once

#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	struct MouseState
	{
		POINT position = { 0, 0 };
		bool leftButton = false;
		bool rightButton = false;
		bool middleButton = false;
		short wheelDelta = 0;
	};

	typedef struct tagEngine_Desc
	{
		HWND hWnd;
		HINSTANCE hInst;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		unsigned int iNumLevels;
		WINMODE		winMode;

	}ENGINE_DESC;



	struct RenderStates
	{
		ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;
		ComPtr<ID3D11SamplerState>		_samplerState = nullptr;
		ComPtr<ID3D11BlendState> _BlendState = nullptr;
		ComPtr<ID3D11DepthStencilState> _DepthStencilState = nullptr;

	};


	struct RENDER_TARGET
	{
		ComPtr<ID3D11Texture2D>				pColorTex;//실제로 GPU가 사용하는 내부적인 texture buffer
		ComPtr<ID3D11DepthStencilView>		DSV;
		ComPtr<ID3D11RenderTargetView>		RTV;		//그릴 곳, 대상 렌더타겟뷰
		ComPtr<ID3D11ShaderResourceView>	SRV;	//현재 렌더타겟으로 만든 셰이더리소스뷰

	};

	typedef struct tagLightDesc
	{
		LIGHT		eType;			//라이트 종류 (방향성/포인트)
		
		XMFLOAT4	vDiffuse;		//Diffuse색
		XMFLOAT4	vAmbient;		//주변광 색
		XMFLOAT4	vSpecular;		//정반사광 색

		XMFLOAT4	vDirection;		//빛의 방향
		XMFLOAT4	vPosition;		//빛 위치(point light시 사용)
		float		fRange;			//빛 범위(point light시 사용)
	
	}LIGHT_DESC;


	struct UIGroup
	{
		_wstring Key;       //그룹이름
		vector<class CGameObject*>        Objects;        //그룹요소들
		bool        isActive = true;                //활성화 여부
	};

	struct Ray
	{
		_vector Origin;		//시작점
		_vector Dir;		//방향
	};
	
	struct Triangle
	{
		_float3 v0 = {};
		_float3 v1 = {};
		_float3 v2 = {};
	};


	
}


#endif // Engine_Struct_h__
