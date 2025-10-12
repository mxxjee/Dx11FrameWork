#include "CLevel_Logo.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "MathUtils.h"

#include "CInput_Manager.h"

#include "CBackGround.h"
#include "CPlayer.h"
#include "CMainCamera.h"
#include "CUICamera.h"
#include "CPerspectiveCameraComponent.h"
#include "COrthographicCameraComponent.h"




USING(Client)

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}


HRESULT CLevel_Logo::Initialize(LevelArgs& args)
{

    __super::Initialize(args);

    if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
        return E_FAIL;


    if(FAILED(Ready_Layer_UI(L"UI_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(L"Player_Layer")))
        return E_FAIL;

    /*부모행렬 테스트*/
    if (FAILED(Reday_Layer_Test(L"Test_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_MainCamera(L"Camera_Layer")))
        return E_FAIL;


    return S_OK;
}

void CLevel_Logo::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::P))
    {
        LevelArgs args;
        args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::UI);
        args.changeType = LEVELCHANGETYPE::PUSH;
        //args.loadingChangeType = LEVELCHANGETYPE::PUSH;
        args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::UI);


        if (FAILED(m_pGameInstance->Level_Changer(
            ENUM_TO_UINT(LEVEL_ID::UI),
            args)))
            return;
    }
}

void CLevel_Logo::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    

    return;
}

void CLevel_Logo::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    
    /*타겟 바꾸기 테스트*/
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Tab))
    {
        if (iTargetIdx == 1)
            iTargetIdx = 0;
        else
            ++iTargetIdx;
      
        iTargetIdx = MathUtils::Clamp(iTargetIdx, 0, 1);


        CGameObject* pMainCamera = m_pGameInstance->GetMainPerspectiveCamera();
        CheckNull(pMainCamera);
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        switch (iTargetIdx)
        {
        case 0:
            ppMainCamera->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC),
                L"Player_Layer",
                L"Player"));
            break;

        case 1:
            ppMainCamera->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC),
                L"Enviroment_Layer",
                L"Floor"));

            break;

        case 2:
            break;
        }

    }

    /*카메라 변경 테스트*/
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::C))
    {
        if (iTargetIdx == 1)
            iTargetIdx = 0;
        else
            ++iTargetIdx;

        iTargetIdx = MathUtils::Clamp(iTargetIdx, 0, 1);

        switch (iTargetIdx)
        {
        case 0:
            m_pGameInstance->SetMainPerspectiveCamera(L"MainCamera");
            break;

        case 1:
            m_pGameInstance->SetMainPerspectiveCamera(L"FreeCamera");

            break;
        }

    }

    /*UI Screen좌펴 변환 테스트*/
   // Set_UIPos_ByWorld(_float3(0.f,-100.f,0.f));

    /*부모행렬테스트*/
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Q))
    {
      /*  CGameObject* pTestObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Test_Layer", L"Test");
        CGameObject* pPlayerObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");

        if (pTestObject)
        {
            if (pTestObject->Get_Transform()->Get_Parent() == nullptr)
                pTestObject->Get_Transform()->Set_Parent(pPlayerObject->Get_Transform());

            else
                pTestObject->Get_Transform()->Set_Parent(nullptr);

        }*/
    }
}

void CLevel_Logo::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"로고 씬입니다.");

}

HRESULT CLevel_Logo::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
    CQuad::QUAD_DESC desc;
    desc.ObjTag = L"Floor";
    desc.ImgPath = L"../../Resource/Terrain0.png";


    CTransform::TRANSFORM_DESC TransDesc = {};

    TransDesc.fRotationPerSec = 0.f;
    TransDesc.fSpeedPerSec = 1.f;
    TransDesc.vLocalPosition = { 0.f,-2.f,0.f,1.f };

    TransDesc.vLocalScale = { 30.f,30.f,1.f,1.f };
    TransDesc.vLocalRotation = { -90.f,0.f,0.f,1.f };



    desc.TransformDesc = &TransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Floor"),
        ENUM_TO_UINT(LEVEL_ID::LOGO),
        strLayerTag, &desc)))
        return E_FAIL;
        
    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
    CUI::tagUIDesc        Desc = {};

    Desc.ObjTag = L"Hp_UI";
    Desc.ImgPath = L"../../Resource/Hp.png";

    Desc.fSizeX = 38.f;
    Desc.fSizeY = 38.f;
    Desc.fX = 50.f;
    Desc.fY = 50.f;

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;

    Desc.TransformDesc = &TransDesc;

    if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Panel"),
        ENUM_TO_UINT(LEVEL_ID::LOGO),
        strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_MainCamera(const _wstring& strLayerTag)
{
    Create_MainCamera();

    /////////////////UICamera
    Create_UICamera();

    Create_FreeCamera();
   
    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CQuad::QUAD_DESC        Desc = {};

    Desc.ObjTag = L"Player";
    Desc.ImgPath = L"../../Resource/Keroro.png";


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
    TransDesc.vLocalScale = { 3.f,3.f,1.f,1.f };

    Desc.TransformDesc = &TransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Player"),
        ENUM_TO_UINT(LEVEL_ID::LOGO),
        strLayerTag, &Desc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_Logo::Reday_Layer_Test(const _wstring& strLayerTag)
{
    CQuad::QUAD_DESC        Desc = {};

    Desc.ObjTag = L"Test";
    Desc.ImgPath = L"../../Resource/Character.png";
    Desc.eRenderGroup = RENDERGROUP::ALPHA;


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
    TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };

    Desc.TransformDesc = &TransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Quad"),
        ENUM_TO_UINT(LEVEL_ID::LOGO),
        strLayerTag, &Desc)))
        return E_FAIL;


    return S_OK;
}

void CLevel_Logo::OnEnter()
{    
    //메인카메라 등록
    m_pGameInstance->SetMainOrthoCamara(L"UICamera");
    m_pGameInstance->SetMainPerspectiveCamera(L"MainCamera");


    CGameObject* pTestObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Test_Layer", L"Test");
    CGameObject* pPlayerObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Player_Layer", L"Player");
    pPlayerObject->Set_Target(m_pGameInstance->Find_Camera(L"FreeCamera"));


    if (pTestObject)
    {
        if (pTestObject->Get_Transform()->Get_Parent() == nullptr)
        {
            pTestObject->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, -1.f, 1.f));
            pTestObject->Get_Transform()->Set_Parent(pPlayerObject->Get_Transform());
        }


        else
            pTestObject->Get_Transform()->Set_Parent(nullptr);

    }

    //소켓을 카메라 타겟으로
    CGameObject* pMainCamera = m_pGameInstance->GetMainPerspectiveCamera();
    if (pMainCamera)
    {
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        ppMainCamera->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO),
            L"Test_Layer",
            L"Test"),true);
    }
    
    
  /*  CGameObject* pMainCamera = m_pGameInstance->GetMainPerspectiveCamera();
    if (pMainCamera)
    {
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        ppMainCamera->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO),
            L"Player_Layer",
            L"Player"), true);
    }*/

 
}

void CLevel_Logo::OnResume()
{

}

void CLevel_Logo::OnPause()
{
    /*2회 호출막기..*/
    if (Get_State() == LEVELSTATE::HIDDEN || Get_State() == LEVELSTATE::PAUSE)
        return;

  
}

void CLevel_Logo::OnExit()
{
    int A = 0;
}


CLevel_Logo* CLevel_Logo::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(args)))
    {
        MSG_BOX("Failed to Create : CLevel_Logo");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Logo::Free()
{
    __super::Free();


}

void CLevel_Logo::Set_UIPos_ByWorld(_float3 OffSet)
{
    /*UI를 플레이어 위치 머리위에 띄워보자!!*/

    _vector vOffset = XMLoadFloat3(&OffSet);

    CGameObject* pHeartUI = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"UI_Layer", L"Hp_UI");
    CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");


    if (pHeartUI && pPlayer)
    {
        CTransform* pTransform = pHeartUI->Get_Transform();
        CTransform* pPlayerTransform = pPlayer->Get_Transform();

        _vector vPlayerPos = pPlayerTransform->Get_State(STATE::POSITION);

        if (pTransform && pPlayerTransform)
        {
            _vector vWorldPos = MathUtils::WorldToScreen(
                vPlayerPos ,
                m_pGameInstance->GetViewMatrix(),
                m_pGameInstance->GetProjMatrix(),
                g_iWinSizeX, g_iWinSizeY
            );

            vWorldPos += vOffset;

            pTransform->Set_State(STATE::POSITION, 
                MathUtils::ScreenToWorld_UI(
                vWorldPos, g_iWinSizeX, g_iWinSizeY));

        }
    }
}

void CLevel_Logo::Create_MainCamera()
{
    CMainCamera::GAMEOBJECT_DESC Desc = {};
    Desc.ObjTag = L"MainCamera";

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalRotation = { 30.f,0.f,0.f,1.f };

    CPerspectiveCameraComponent::PERSPECTIVE_DESC CameraDesc = {};
    CameraDesc.Aspect = (float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear =0.1f;
    CameraDesc.fFar = 1000.f;

    //CameraDesc.pTarget= m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"BackGround_Layer", L"BackGround");
    CameraDesc.vOffset = _float3(0.f, 2.f, -2.f);


    Desc.CameraDesc = &CameraDesc;
    Desc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MainCamera"), &Desc));
    m_pGameInstance->RegisterCamera(L"MainCamera", pInstance, false);

}

void CLevel_Logo::Create_UICamera()
{
    CUICamera::GAMEOBJECT_DESC UIDesc = {};
    UIDesc.ObjTag = L"UICamera";

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;

    COrthographicCameraComponent::ORTHOGRAPHIC_DESC UICameraDesc = {};

    UICameraDesc.fNear = 0.1f;
    UICameraDesc.fFar = 1.f;

    UICameraDesc.ViewWdith = (float)g_iWinSizeX;
    UICameraDesc.ViewHeight = (float)g_iWinSizeY;

    UIDesc.CameraDesc = &UICameraDesc;
    UIDesc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"UICamera"), &UIDesc));
    m_pGameInstance->RegisterCamera(L"UICamera", pInstance, true);
}

void CLevel_Logo::Create_FreeCamera()
{
    CMainCamera::GAMEOBJECT_DESC Desc = {};
    Desc.ObjTag = L"FreeCamera";

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    //TransDesc.vLocalPosition = _float4(0.f, 2.f, -2.f,1.f);
  
    CPerspectiveCameraComponent::PERSPECTIVE_DESC CameraDesc = {};
    CameraDesc.Aspect = (float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 1000.f;


    Desc.CameraDesc = &CameraDesc;
    Desc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FreeCamera"), &Desc));
    m_pGameInstance->RegisterCamera(L"FreeCamera", pInstance, false);
}
