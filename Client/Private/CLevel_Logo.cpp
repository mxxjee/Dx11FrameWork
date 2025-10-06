#include "CLevel_Logo.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "MathUtils.h"

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

    if (FAILED(Ready_Layer_MainCamera(L"Camera_Layer")))
        return E_FAIL;



    return S_OK;
}

void CLevel_Logo::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    if (GetKeyState('P') & 0x8000)
    {
        LevelArgs args;
        args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::GAMEPLAY);
        args.changeType = LEVELCHANGETYPE::OVERLAY;
        args.loadingChangeType = LEVELCHANGETYPE::PUSH;
        args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);


        if (FAILED(m_pGameInstance->Level_Changer(
            ENUM_TO_UINT(LEVEL_ID::LOADING),
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
    if (GetAsyncKeyState(VK_TAB) & 0x8000)
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
        ENUM_TO_UINT(LEVEL_ID::STATIC),
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
    Desc.fX =  50;
    Desc.fY = 50;

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;

    Desc.TransformDesc = &TransDesc;

    if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Panel"),
        ENUM_TO_UINT(LEVEL_ID::STATIC),
        strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_MainCamera(const _wstring& strLayerTag)
{
    CMainCamera::GAMEOBJECT_DESC Desc = {};
    Desc.ObjTag = L"MainCamera";

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalRotation = { 30.f,0.f,0.f,1.f };

    CPerspectiveCameraComponent::PERSPECTIVE_DESC CameraDesc = {};
    CameraDesc.Aspect = (float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 1000.f;

    //CameraDesc.pTarget= m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"BackGround_Layer", L"BackGround");
    CameraDesc.vOffset = _float3(0.f,2.f, -2.f);


    Desc.CameraDesc = &CameraDesc;
    Desc.TransformDesc = &TransDesc;

   
    CGameObject* pInstance=dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MainCamera"), &Desc));
    m_pGameInstance->RegisterCamera(L"MainCamera", pInstance,false);
    

    /////////////////UICamera
    CUICamera::GAMEOBJECT_DESC UIDesc = {};
    UIDesc.ObjTag = L"UICamera";

    TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
   
    COrthographicCameraComponent::ORTHOGRAPHIC_DESC UICameraDesc = {};
   
    UICameraDesc.fNear = 0.1f;
    UICameraDesc.fFar = 1000.f;

    UICameraDesc.ViewWdith = (float)g_iWinSizeX;
    UICameraDesc.ViewHeight = (float)g_iWinSizeY;

    UIDesc.CameraDesc = &UICameraDesc;
    UIDesc.TransformDesc = &TransDesc;


    pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"UICamera"), &UIDesc));
    m_pGameInstance->RegisterCamera(L"UICamera", pInstance, true);
    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CQuad::QUAD_DESC        Desc = {};

    Desc.ObjTag = L"Player";
    Desc.ImgPath = L"../../Resource/Character.png";


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
    TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };

    Desc.TransformDesc = &TransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Player"),
        ENUM_TO_UINT(LEVEL_ID::STATIC),
        strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::OnEnter()
{
    //메인카메라 등록
    m_pGameInstance->SetMainOrthoCamara(L"UICamera");

	m_pGameInstance->SetMainPerspectiveCamera(L"MainCamera");
   

    CGameObject* pMainCamera = m_pGameInstance->GetMainPerspectiveCamera();
    if (pMainCamera)
    {
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        ppMainCamera->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC),
            L"Player_Layer",
            L"Player"),true);
    }
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
