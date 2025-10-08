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
    HRESULT Initialize_Engine(const ENGINE_DESC & EngineDesc,ComPtr<ID3D11Device>* pDevice, ComPtr<ID3D11DeviceContext>* pContext);
    
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
    class CLevel*   Get_CurrentLevel();
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
    CBase*          Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelIndex, const _wstring & strPrototag, void* pArg);
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
    HRESULT         Add_RenderObject(RENDERGROUP eID, class CGameObject * pRenderObject);

#pragma endregion

#pragma region CameraManager
    //카메라 등록 및 제거..
    void        RegisterCamera(const _wstring & Tag, class CGameObject * pObj, bool isOrtho);
    void        UnRegisterCamera(const _wstring & Tag, bool isOrtho);
    
    //메인카메라 설정
    bool        SetMainPerspectiveCamera(const _wstring & tag);
    bool        SetMainOrthoCamara(const _wstring & tag);

    //뷰,투영행렬가져오기
    const _float4x4& GetViewMatrix(bool isOrtho = false) const;
    const _float4x4& GetProjMatrix(bool isOrtho = false) const;

    //메인 카메라 가져오기
    class CGameObject* GetMainPerspectiveCamera();
    class CGameObject* GetMainOrthoCamera();

    void        Update_MainCamera(_float fTimeDelta);
    void        LateUpdate_MainCamera(_float fTimeDelta);

#pragma endregion

#pragma region Input_Manager
public:
    bool    IsKeyPressed(KeyCode key) const;
    bool    IsKeyHeld(KeyCode key) const;
    bool     IsKeyReleased(KeyCode key) const;
    
    bool    IsMouseButtonHeld(int button) const;
    POINT     GetMouseDelta() const;

#pragma endregion

#pragma region Shader_Manager
    HRESULT     Register_Shader(const _wstring& Tag, class CShader* pInstance);
    class CShader*     Find_Shader(const _wstring& Tag);
#pragma endregion
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
    class CInput_Manager* m_pInputManager = { nullptr };
    class CShader_Manager* m_pShaderManager = { nullptr };


private:
    vector<D3D11_VIEWPORT>          m_ViewPorts;
    tagEngine_Desc                  m_EngineDesc;
public:
    void                Release_Engine();
    virtual void        Free() override;

};

NS_END

