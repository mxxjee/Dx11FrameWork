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
		EngineMode	eEngineMode;
		_uint		colGroupMax = 0;

	}ENGINE_DESC;

	typedef struct HoldKeyData
	{
		KeyCode Code;
		float	m_fTHoldTime = 0.f;		//누른시간
		float	m_fThresHold = 1.f;		//눌렀다고 판정되는 시간
		bool	m_bHeld = false;

		void Reset()
		{
			m_fTHoldTime = 0.f;
			m_bHeld = false;
		}
	}HOLDKEY_DATA;

	typedef struct tagLoadTerrainData
	{
		int		iTileID = -1;
		_float4 vPosition;
		_float4 vScale;
		_float4 vRotation;
		_float2 vIndex;
		string ModelName;

	}LOADTERRAINDATA;


	struct RenderStates
	{
		ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;
		ComPtr<ID3D11SamplerState>		_samplerState = nullptr;
		ComPtr<ID3D11BlendState> _BlendState = nullptr;
		ComPtr<ID3D11DepthStencilState> _DepthStencilState = nullptr;

		RenderStates() {};
		void		Reset()
		{
			_rasterizerState.Reset();
			_samplerState.Reset();
			_BlendState.Reset();
			_DepthStencilState.Reset();
		}
		~RenderStates()
		{
			

		}
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
		XMFLOAT4		fRange;			//빛 범위(point light시 사용)
	
	}LIGHT_DESC;


	
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


	typedef struct tagAssetInfo
	{
		MapObjType              ObjType;    //설치하려는 오브젝트타입
		wstring TexKey = L"";   //Texmgr에 이미등록해논 키
		wstring FullPath = L""; //전체경로
	}AssetInfo;

	typedef struct tagPriviewPoint
	{
		_float3 vPos=_float3(-999.f,-999.f,-999.f);
		int		m_iDrawIdx = 0;
		_bool   vRegister = false;      //true일경우 진짜로 찍은점.
	}PreviewPoint;


	struct DefaultCellInfo
	{
		/*이값들은 모두 정렬된 이후를 기준으로.(실제연산에 쓸값들)*/
		_float3         m_vPoints[ENUM_TO_UINT(POINTType::END)] = {};   //각 정점의 좌표(월드)
		_float3         m_vNormals[ENUM_TO_UINT(LINE::END)] = {};   //각 선분에 대한 법선벡터
		_int            m_iNeighbors[ENUM_TO_UINT(LINE::END)] = { -1,-1,-1 };   //각 선분에 대해서 인접한 삼각형의 인덱스(존재하지 않으면 -1로 채움)
		_uint           m_iIndex;           //현재 이 삼각형이 네브메쉬 배열의 몇번째 인덱스인가요
		_float4			m_Plane=_float4(0.f,0.f,0.f,0.f);			//현재 이 삼각형의 평면정보(a,b,c,d) // ax+by+cz+d

		_uint			CellType=0;			//enum(AreaType)
	
	};

	struct DefaultInteractionData
	{
		string ModelName = "";
		_uint InteractionType = 0;

		_float3 vPos;
		_float3 vScale;
		_float3 vRotation;

		_float3 ColliderCenter;
		_float3 ColliderExtent;


	};
	
	//게임이벤트 구조체에 담길 부가 데이터, 이벤트 전달시 이용할 데이터들
	struct EventPayload
	{
		UMap<string, float>		Floats;
		UMap<string, int>		Ints;
		UMap<string, void*>		Ptrs;

		EventPayload() {}
		EventPayload(const EventPayload& Prototpye)
		{
			Floats = Prototpye.Floats;
			Ints = Prototpye.Ints;
			Ptrs = Prototpye.Ptrs;
		}
	};


	//실제 이벤트 버스에 등록할 이벤트타입
	struct GameEvent
	{
		string	Name = "";
		EventPayload	Payload;		//부가데이터( 타겟지정, float값들 등)

		GameEvent() {}
		GameEvent(const GameEvent& Prototype)
			:Name(Prototype.Name),
			Payload(Prototype.Payload)
		{
		}

		GameEvent operator=(const GameEvent& Other)
		{
			Name = Other.Name;
			Payload = Other.Payload;

			return (*this);
		}
	};

	using EventCallBack = function<void(const GameEvent&)>;

	

	///청크렌더링용
	struct Bound
	{
		_float3 MinBound = { 0.f,0.f,0.f };
		_float3 MaxBound = { 0.f,0.f,0.f };



	};

	struct Chunk
	{
		Bound ChunkBound;
		_float3 vCenter = { 0.f,0.f,0.f };
		_float2 iIdxZX = { 0.f,0.f };
	};



}


#endif // Engine_Struct_h__
