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

	template <typename TVertex,typename TBuffer>
	struct RenderPipelineResource
	{
		// 버텍스/인덱스 데이터
		shared_ptr<class CVertexBuffer>   vertexBuffer;
		shared_ptr<class CIndexBuffer>    indexBuffer;
		shared_ptr<class CInputLayout > inputLayout;
		shared_ptr<class CGeometry<TVertex>> geometry;

		// 셰이더
	/*	shared_ptr<class CVertexShader>   vertexShader;
		shared_ptr<class CPixelShader>    pixelShader;*/

		shared_ptr<class CTexture> texture1 = { nullptr };
		
		// 상수 버퍼
		shared_ptr<class CConstantBuffer<TBuffer>> constantBuffer;



		ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;
		ComPtr<ID3D11SamplerState>		_samplerState = nullptr;
		ComPtr<ID3D11BlendState> _BlendState = nullptr;
	};



	
}


#endif // Engine_Struct_h__
