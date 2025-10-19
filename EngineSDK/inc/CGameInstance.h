#pragma once
#include "CBase.h"
#include "Engine_LevelTypes.h"


/*엔진의 모든 매니저들을 관리한다.
1. 엔진의 모든 매니저들을 업데이트/렌더한다.
2. 여러 매니저들의 함수를 호출해주는 역할 (링크)
3. 알아서 Update한다.
4. 알아서 Render한다.

*/


NS_BEGIN(Engine)

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

	//const ENGINE_DESC& Get_EngineDesc() { return m_EngineDesc; }
#pragma endregion


#pragma region LevelManager
public:
	HRESULT			Level_Changer(_uint iSceneID, LevelArgs & args);
	void            Pop_Level();
	class CLevel* Get_CurrentLevel();
	const vector<CLevel*>* Get_LevelStack();
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
#pragma endregion

#pragma region ProtoManager
public:
	HRESULT        Add_Prototype(_uint iLevelIndex, const _wstring & strProtoTag, CBase * pPrototype);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelIndex, const _wstring & strPrototag, void* pArg);
#pragma endregion

#pragma region ObjectManager
public:
	const unordered_map<_wstring, class CLayer*>& Get_Layers(_uint iLevel);

	HRESULT Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring & strPrototypeTag,
		_uint iLayerLevelIndex, const _wstring & strLayerTag, void* pArg = nullptr);


	void        Update_Priority_Static(_float fTimeDelta);
	void        Update_Static(_float fTimeDelta);
	void        Update_Late_Static(_float fTimeDelta);
	void        Update_Render_Static(_float fTimeDelta);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _wstring & LayerTag, const _wstring & Tag);
#pragma endregion

#pragma region Renderer
public:
	HRESULT         Initialize_Renderer(_uint RenderGroupCount);
	HRESULT         Add_RenderObject(_uint eID, class CGameObject* pRenderObject);
	HRESULT         Add_SortFunc(_uint eID, function<bool(class CGameObject*, class CGameObject*)> _Fun);
	void            Render_Group(_uint eType);
	void            Clear_RenderGroups();
	int             Get_RenderGroupCount();
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

	 //모든 셰이더파일들에게 전역변수들을 바인딩(카메라 뷰관련,, 수치등)
	 HRESULT         Bind_GlobalPipelineData(_uint CameraType);

 #pragma endregion


 #pragma region ScreenShot_Manager
	 void                    ScreenShot(const _wstring & Key);
	 class CTexture* Find_ScreenTexture(const _wstring & Key);
	 HRESULT                 SaveTextureToFile(const _wstring & Key, const _wstring & filePath);

 #pragma endregion

#pragma region RenderState_Manager
	 HRESULT        Register_RenderStates(_uint iRenderGroup, const RenderStates & States);
	 const RenderStates& Get_RenderStates(_uint iRenderGroup);
#pragma endregion


#pragma region Texture_Manager
	 HRESULT       Register_Texture(const _wstring & Tag, CTexture * pInstance);
	 CTexture* Find_Texture(const _wstring & Tag);
#pragma endregion

#pragma region Pipeline
public:
	void		Set_Transform(_uint CameraType, D3DTS eTransformMatrix, _fmatrix TransformMatrix);

	//뷰,투영을 골라서 바인드가능
	HRESULT Bind_PipeLineMatrix(class CShader* pShader, const _char * pConstant, _uint iCameraType, D3DTS eTransformMatrix);

	//뷰와 투영을 한번에 바인드
	HRESULT Bind_PipeLineMatrixAll(class CShader* pShader, const _char * pConstant, _uint iCameraType);

	HRESULT Bind_PipeLineInverseMatrix(class CShader* pShader, const _char * pConstant, _uint iCameraType, D3DTS eTransformMatrix);
	HRESULT Bind_CamPosition(class CShader* pShader, const _char * pConstant, _uint iCameraType);


public:
	const _float4x4& Get_ViewMatrix(_uint CameraType);
	const _float4x4& Get_ProjMatrix(_uint CameraType);
	const _float4& Get_CamPosition(_uint CameraType);;
#pragma region


#pragma region UI_Manager
public:
		//새로운  UIGroup 구조체를 만들어서 등록
	HRESULT     Register_UIGroup(const _wstring & Key, const UIGroup & Group);

	//이미 매니저에 존재하는 그룹에 추가로 등록
	HRESULT     AddUIToGroup(const _wstring & Key, CGameObject * pGameObject);

	//이벤트 등록.. 
	HRESULT     RegisterEvent(const _wstring & Key, function<void(void*)> _function);


	//이벤트 뿌리기 , pData=전달하고싶은 데이터들
	HRESULT     BroadCastEvent(const _wstring & Key, void* pData);

	//그룹단위로 setactive/false처리
	HRESULT        SetActiveGroup(const _wstring & Key, bool bActive);


	UIGroup* Get_UIGroup(const _wstring Key);
	function<void(void*)> Get_EventFunction(const _wstring & Key);
#pragma region


 public:
 #pragma region Default
	 const vector<D3D11_VIEWPORT>& Get_Viewports() { return m_ViewPorts; }
	 const tagEngine_Desc& Get_EngineDesc() const { return m_EngineDesc; }
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


private:
	vector<D3D11_VIEWPORT>          m_ViewPorts;
	tagEngine_Desc                  m_EngineDesc;
public:
	void                Release_Engine();
	virtual void        Free() override;

};

NS_END

