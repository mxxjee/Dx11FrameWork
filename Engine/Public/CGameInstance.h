#pragma once
#include "CBase.h"
#include "Engine_LevelTypes.h"
#include "UIGroup.h"

/*엔진의 모든 매니저들을 관리한다.
1. 엔진의 모든 매니저들을 업데이트/렌더한다.
2. 여러 매니저들의 함수를 호출해주는 역할 (링크)
3. 알아서 Update한다. 
4. 알아서 Render한다.

*/


NS_BEGIN(Engine)
class CConstantBuffer;

class ENGINE_DLL CGameInstance final: public CBase
{
	 DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region Engine
public:
	HRESULT Initialize_Engine(const ENGINE_DESC & EngineDesc,ComPtr<ID3D11Device>*pDevice, ComPtr<ID3D11DeviceContext>*pContext);

	void    Update_Priority_Engine(_float fTimedelta);
	void    Update_Engine(_float fTimedelta);
	void    LateUpdate_Engine(float fTimedelta);
	void    Update_Render(float fTimedelta);

	HRESULT     Draw_Begin(const _float4 * pClearColor);
	HRESULT     Draw();
	HRESULT     Draw_End();
	void        Clear(_uint iLevelID);        /*레벨의 자원 삭제.*/


	/*랜덤값 나오는 함수.*/
	_float		Random(_float fMin, _float fMax);
	//const ENGINE_DESC& Get_EngineDesc() { return m_EngineDesc; }
#pragma endregion


#pragma region LevelManager
public:
	HRESULT			Level_Changer(_uint iSceneID, LevelArgs & args);
	void            Pop_Level();
	class CLevel* Get_CurrentLevel();
	const vector<CLevel*>* Get_LevelStack();
	_uint           Get_CurrentLevelID();
	bool			Get_IsLoading();
	void			Set_IsLoading(bool b);
#pragma endregion

#pragma region LevelFactory
	void            Register_Level(_uint iSceneID, LevelCreator Creator);
	CLevel* Create_Level(_uint iSceneID, LevelArgs _Arg);
#pragma endregion

#pragma region TimerManager
public:
	_float			Get_TimeDelta(const _tchar * pTimerTag);
	HRESULT			Add_Timer(const _tchar * pTimerTag);
	void			Compute_TimeDelta(const _tchar * pTimerTag);
	_float			Get_FPS(const _tchar * pTimerTag);
#pragma endregion

#pragma region ProtoManager
public:
	HRESULT        Add_Prototype(_uint iLevelIndex, const _wstring & strProtoTag, CBase * pPrototype);
	CBase*			Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelIndex, const _wstring & strPrototag, void* pArg);
#pragma endregion

#pragma region ObjectManager
public:
	const unordered_map<_wstring, class CLayer*>& Get_Layers(_uint iLevel);

	//생성과 동시에 추가해주는 함수
	HRESULT Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring & strPrototypeTag,
		_uint iLayerLevelIndex, const _wstring & strLayerTag, void* pArg = nullptr);

					//레이어 생성
	HRESULT Make_New_Layer(_uint iLayerLevelIndex, const _wstring & strLayerTag);

	void        Update_Priority_Static(_float fTimeDelta);
	void        Update_Static(_float fTimeDelta);
	void        Update_Late_Static(_float fTimeDelta);
	void        Update_Render_Static(_float fTimeDelta);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _wstring & LayerTag, const _wstring & Tag);
	
	//이미생성한 것을 추가하는 함수
	HRESULT     Add_GameObject_To_Layer(_uint iLayerLevelIndex, const _wstring & strLayerTag, CGameObject * pObject);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _wstring & LayerTag);


#pragma endregion

#pragma region Renderer
public:
	HRESULT         Initialize_Renderer(_uint RenderGroupCount);
	HRESULT         Add_RenderObject(_uint eID, class CGameObject* pRenderObject);
	HRESULT         Add_SortFunc(_uint eID, function<bool(class CGameObject*, class CGameObject*)> _Fun);
	void            Render_Group(_uint eType);
	void            Clear_RenderGroups();
	int             Get_RenderGroupCount();


	void			Bind_And_Render_Lights();
	void			Bind_Rect_Matricies();
	void			Render_Combined();

#ifdef _DEBUG
	void			Render_Debug();

	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif
#pragma endregion

#pragma region GraphicDevice
	HRESULT     Get_Buffer(ComPtr<ID3D11Texture2D>*pBuffer, UINT iFlag = 0);
	ComPtr<ID3D11RenderTargetView>	    Get_BackBuffer_RTV();
	ComPtr<ID3D11DepthStencilView>	    Get_BackBuffer_DSV();
#pragma endregion

#pragma region CameraManager
	//카메라 등록 및 제거..
	void        RegisterCamera(CAMERA_TYPE eType, CGameObject * pObj);
	void        UnRegisterCamera(CAMERA_TYPE eType);
	void        Set_MainCamera(CAMERA_TYPE eType);

	void        Update_Cameras(_float fTimeDelta);
	void        LateUpdate_Cameras(_float fTimeDelta);


	//카메라 가져오기
	class CCamera_Base* Find_Camera(CAMERA_TYPE eType);
	class CCamera_Base* Get_MainCamera();


	//메인카메라의 뷰,투영행렬 관련
	//const _float4x4& Get_Main_ViewMatrix();
	//const _float4x4& Get_Main_ProjMatrix();
	// _matrix Get_Main_MulViewProjMatrix();
	 //void    Bind_Main_ViewProjMatrix() const;

	 class CShader* Get_RenderShader();
	 const string& Get_RenderPassName();
	 class CCamera_Base* Get_RenderCamera();


 #pragma endregion


 #pragma region Shader_Manager
	 HRESULT     Register_Shader(const _wstring & Tag, class CShader* pInstance);
	 class CShader* Find_Shader(const _wstring & Tag);

	 HRESULT		 Bind_GlobalLightData();

	 //모든 셰이더파일들에게 Sampler값 바인딩
	 HRESULT         Bind_SamplerState(_uint iRenderGroup);
	 

	 //상수버퍼 갱신용
	 void            CopyData_Buffer(string Key, const void* pData, _uint iSize);

 #pragma endregion


 #pragma region ScreenShot_Manager
	 void                    ScreenShot(const _wstring & Key);
	 class CTexture* Find_ScreenTexture(const _wstring & Key);
	 HRESULT                 SaveTextureToFile(const _wstring & Key, const _wstring & filePath);

 #pragma endregion

#pragma region RenderState_Manager
	 HRESULT        Register_RenderStates(_uint iRenderGroup, const RenderStates & States);
	 const RenderStates& Get_RenderStates(_uint iRenderGroup);
	 HRESULT             Bind_SamplerState(class CShader* pShader, _uint iRenderGroup);

#pragma endregion


#pragma region Texture_Manager
	 HRESULT       Register_Texture(const _wstring & Tag, CTexture * pInstance);
	 CTexture* Find_Texture(const _wstring & Tag);
	 //폴더안의 텍스처한장씩 로드해서 각가의 texture객체로만든다.(폴더경로, 확장자)
	 HRESULT         Load_Textures(const _wstring& FolderPath, const _wstring& Extension);

#pragma endregion

#pragma region Pipeline
public:
	void		Set_Transform(_uint CameraType, D3DTS eTransformMatrix, _fmatrix TransformMatrix);
	void		Set_CamPosition(_uint CameraType, const _float4& Position);

	//뷰,투영을 골라서 바인드가능
	HRESULT Bind_PipeLineMatrix(class CShader* pShader, const _char * pConstant, _uint iCameraType, D3DTS eTransformMatrix);

	//뷰와 투영을 한번에 바인드
	HRESULT Bind_PipeLineMatrixAll(class CShader* pShader, const _char * pConstant, _uint iCameraType);

	HRESULT Bind_PipeLineInverseMatrix(class CShader* pShader, const _char * pConstant, _uint iCameraType, D3DTS eTransformMatrix);
	
	const _float4x4* Get_InverseTransform(_uint iCameraType, D3DTS eTransformMatrix);

	HRESULT Update_CamBuffer(_uint CameraType);
	HRESULT Bind_CamBuffer();


	_matrix					Get_ViewProjMatrix(_uint CameraType);

	public:
		const _float4x4& Get_ViewMatrix(_uint CameraType);
		const _float4x4& Get_ProjMatrix(_uint CameraType);
		const _float4& Get_CamPosition(_uint CameraType);;
#pragma endregion



#pragma region UI_Manager
public:
		//새로운  UIGroup 구조체를 만들어서 등록
	HRESULT     Register_UIGroup(const UIGroup& Group, const _wstring& Key = L"");

	//이미 매니저에 존재하는 그룹에 추가로 등록
	HRESULT     AddUIToGroup(const _wstring & Key, CGameObject * pGameObject);

	//이벤트 등록.. 
	HRESULT     RegisterEvent(const _wstring & Key, function<void(void*)> _function);
	HRESULT     UnRegisterEvent(const _wstring& Key);


	//이벤트 뿌리기 , pData=전달하고싶은 데이터들
	HRESULT     BroadCastEvent(const _wstring & Key, void* pData);

	//그룹단위로 setactive/false처리
	HRESULT        SetActiveGroup(const _wstring & Key, bool bActive);


	UIGroup* Get_UIGroup(const _wstring Key);
	function<void(void*)> Get_EventFunction(const _wstring & Key);
#pragma endregion


#pragma region Terrain_Manager
	HRESULT						Register_Terrain(const _wstring& Key, class CTerrain_Base* pTerrain);
	HRESULT						UnRegister_Terrain(const _wstring& Key);
	class CTerrain_Base*		Find_Terrain(const _wstring& Key);
	_float3						Get_PickingWorldPos();
	CTerrain_Base*				Get_PickTerrain();
	CTerrain_Base*				Check_Picking();
	Triangle*					PickTerrain(const _wstring& Key);
	void						Set_EnableUpdate(bool b);
	void						Set_EnalbeUpdateRender(bool b);
	void						Set_EnableUpdateMinimap(bool b);

	void						Set_Active_Group(int StartTileID, int EndTiileID, bool _bActive);

	HRESULT						Save_All_Terrains(const string& path, int iNum=-1);
	HRESULT             Load_Terrains_MapTool(const string& LoadPath);
	const vector<tagLoadTerrainData>& Load_Terrains_Runtime(const string& LoadPath);

	float           Get_PickDist();
	const UMap<_wstring, CTerrain_Base*>& Get_TerrainMap();

	
	void                        RequestDestroy(class CTerrain_Base* pObj);



#pragma endregion



#pragma region Light_Manager
	HRESULT						Add_Light(_uint iLevelID,const LIGHT_DESC& LightDesc);
	const						LIGHT_DESC* Get_LightDesc(_uint iLevelID,_uint iIndex);
	HRESULT					 Bind_Lights(class CShader* pShader);
	class CLight*			Get_Light(_uint iLevelID, _uint iIndex);
	class CLight*			Get_DirectionLight(_uint iLevelID);
	class CLight*			Get_Light(_uint iLevelID, wstring LightName);

	HRESULT					Bind_Directional_Light(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, const LIGHT_DESC* pLightDesc);
	void					Render_LightManager(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

	list<class CLight*>     Get_Lights(_uint iLevelID);

	HRESULT         Save_LightData();
	HRESULT         Load_LightData(_uint iLevelID, string FilePath);
#pragma endregion

#pragma region Model_Manager
	HRESULT       Register_Model(const _wstring& Tag, class CModel* pInstance);
	class CModel*		Find_Model(const _wstring& ProtoModelName);
	class CModel*		 Clone_Model(const _wstring& ProtoModelName, void* pArg);
	_uint           Get_ModelCount();
	HRESULT			Load_Model(const string& FilePath, _matrix PreMatrix);
	const			UMap<_wstring, class CModel*>& Get_MapModel();
public:
	//모든 모델 로드..(폴더안의 모델 모두로드)
	HRESULT Load_All_Models(const string& FilePath, _matrix PreMatrix);


#pragma endregion

#pragma region MaterialManager
	HRESULT       Register_Material(const _wstring& Tag, class CMaterial* pInstance);
	class CMaterial* Find_Material(const _wstring& Tag);
#pragma endregion

 #pragma region Default
	 const vector<D3D11_VIEWPORT>& Get_Viewports() { return m_ViewPorts; }
	 const tagEngine_Desc& Get_EngineDesc() const { return m_EngineDesc; }
 #pragma endregion


#pragma region HotKey
	 HRESULT         Register_HotKey(KeyCode eKode, bool bCtrl, bool bShift, bool bAlt, function<void()> Func);

#pragma endregion

#pragma region NavMeshManager
	 HRESULT							Load_NavMesh(_uint iLevelIdx, const string& Filepath);
	 vector<class CCell*>*				Find_Cells(_uint iLevelIdx);
	 void								Set_MainCells(_uint LevelID);
	 vector<class CCell*>*				Get_MainCells();
	 void								Set_DrawDebug(bool b);
	 void								Reset_NaveMesh(_uint iLevelIdx);
	 _vector							Get_CellPos_By_MainCells(_uint iIdx);
#ifdef _DEBUG
	 void								Set_NavMeshShader(class CShader* pShader);
	 HRESULT							    Render_NavMeshManager();
#endif
	 
	 _float4x4*								Get_ParentMatrix();
#pragma endregion



#pragma region EventBusManager
	 void Emit(const GameEvent& Event);
	 _uint RegisterListners(const string& CBName, EventCallBack Callback);
	 void    UnRegisterListenrs(const string& CBName, _uint iHandle);

#pragma endregion


#pragma region CollisionManager
		// 콜라이더 등록
	 HRESULT     Register_Collider(class CCollider_Base* pCollider, _uint iSceneID);
		
	 //객체 삭제 시 호출해줘야함. 콜라이더 등록해제
	 HRESULT     UnRegister_Collider(CCollider_Base* pCollider, _uint iSceneID);
	 void		Clear_SceneColliders(_uint iSceneID);

		//각 그룹간의 충돌 설정 
	 void        Set_Enable_Collision(_uint iSrcGroup, _uint iDstGroup, bool bEnable);


#pragma endregion

#pragma region FontManager
	 HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	 class	SpriteFont* Find_Font(const _wstring& strFontTag);
	 SpriteBatch* Get_Batch();
#pragma endregion


#pragma region TimerTask_Manager
	 HRESULT         Invoke(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner);
	 HRESULT         Repeat(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner);
	 HRESULT         CancelTaskOf(CGameObject* pOwner);

#pragma endregion

#pragma region Target_Manager
	 //렌더타겟을 등록하는함수
	 HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);

	 //MRT : RenderTarget들을 그룹으로묶어 한번에 바인딩
	 HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);

	 //MRT를 찾아서 바인딩
	 HRESULT Begin_MRT(const _wstring& strMRTTag);

	 //다시 복원
	 HRESULT End_MRT();


	 //쉐이더에 렌더타겟의 SRV를바인딩
	 HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	 HRESULT Unbind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
	 HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	 HRESULT Debug_RT_Render(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

#pragma endregion
	 void		StopSoundFade(CHANNELID eID, float fDuration);
	 void PlaySound(const wstring& pSoundKey, CHANNELID eID, float fVolume,bool bLoop=false);
	 void PlayBGM(const std::wstring& soundKey, float fVolume);
	 void StopSound(CHANNELID eID);
	 void StopAll();
	 void SetChannelVolume(CHANNELID eID, float fVolume);

#pragma region Sound_Manager
#pragma endregion
 private:
	 class CLevel_Manager* m_pLevelManager = { nullptr };
	 class CTimer_Manager* m_pTimerManager = { nullptr };
	 class CGraphic_Device* m_pGraphicDev = { nullptr };
	 class CLevelFactroy* m_pLevelFactory = { nullptr };
	 class CPrototype_Manager* m_pProtoManager = { nullptr };
	 class CObject_Manager* m_pObjectManager = { nullptr };
	 class CRenderer* m_pRenderer = nullptr;
	 class CCamera_Manager* m_pCameraManager = { nullptr };
	 class CShader_Manager* m_pShaderManager = { nullptr };
	 class CScreenShot_Manager* m_pScreenshotManager = { nullptr };
	 class CRenderState_Manager* m_pRenderStateManager = { nullptr };

	 class CPipeLine* m_pPipeLine = { nullptr };
	 class CTexture_Manager* m_pTextureManager = { nullptr };
	 class CUI_Manager* m_pUIManager = { nullptr };
	 class CTerrain_Manager* m_pTerrainManager = { nullptr };
	 class CLight_Manager* m_pLightManager = { nullptr };
	 
	 class CMaterial_Manager* m_pMaterialManager = { nullptr };
	 class CModel_Manager* m_pModelManager = { nullptr };
	 class CHotKey_Manager* m_pHotKeyManager = { nullptr };

	 class CNavMesh_Manager* m_pNavMeshManager = { nullptr };
	 class CEventBus_Manager* m_pEventBusManager = { nullptr };
	 class CCollision_Manager* m_pCollisionManager = { nullptr };
	 class CFont_Manager* m_pFont_Manager = { nullptr };
	 class CTimerTask_Manager* m_pTimerTask_Manager = { nullptr };


	 class CTarget_Manager* m_pTarget_Manager = { nullptr };

	 class CSoundMgr* m_pSound_Manager = nullptr;

private:
	vector<D3D11_VIEWPORT>          m_ViewPorts;
	tagEngine_Desc                  m_EngineDesc;

public:
	void                Release_Engine();
	virtual void        Free() override;

#ifdef _DEBUG
	static  bool		m_bDrawDebug;
#endif 
};

NS_END

