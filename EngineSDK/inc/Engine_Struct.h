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


	
	struct TransformData
	{
			Matrix	matworld; //= DirectX::SimpleMath::Matrix::Identity;
			Matrix	view;// = DirectX::SimpleMath::Matrix::Identity;
			Matrix  proj; //= DirectX::SimpleMath::Matrix::Identity;

	};



	template<typename T>
	class CGeometry;


	template<typename T>
	class CConstantBuffer;


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
	
}


#endif // Engine_Struct_h__
