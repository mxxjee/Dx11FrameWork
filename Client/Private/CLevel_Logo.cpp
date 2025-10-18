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
#include "CTerrain.h"


#include "CMinimapCamera.h"
#include "CMinimapQuad.h"

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

    if (FAILED(Ready_Layer_Player(L"Player_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_MainCamera(L"Camera_Layer")))
        return E_FAIL;

    if (FAILED(Reday_Layer_Test(L"Test_Layer")))
        return E_FAIL;

    
    if (FAILED(Ready_Layer_UI(L"UI_Layer")))
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

    CGameObject* pTestObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Test_Layer", L"Test");
    CGameObject* pPlayerObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Player_Layer", L"Player");

    if (pTestObject)
    {
        pTestObject->Get_Transform()->Rotation(_float3(90.f, 0.f, 0.f));
        pTestObject->Get_Transform()->Set_State(STATE::POSITION, pPlayerObject->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD));

    }

    return;
}

void CLevel_Logo::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    
   

    /*타겟 바꾸기 테스트*/
  /*  if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Tab))
    {
        if (iTargetIdx == 1)
            iTargetIdx = 0;
        else
            ++iTargetIdx;
      
        iTargetIdx = MathUtils::Clamp(iTargetIdx, 0, 1);


        CGameObject* pMainCamera = m_pGameInstance->Get_MainCamera();
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

    }*/

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
            m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);
            break;

        case 1:
            m_pGameInstance->Set_MainCamera(CAMERA_TYPE::MINIMAP);

            break;
        }

    }

    /*UI Screen좌펴 변환 테스트*/
    //Set_UIPos_ByWorld(_float3(0.f,-100.f,0.f));

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
#pragma region 이전지형(일자지형)
    //CQuad::QUAD_DESC desc;
    //desc.ObjTag = L"Floor";
    //desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
    //desc.ImgPath = L"../../Resource/Terrain0.png";


    //CTransform::TRANSFORM_DESC TransDesc = {};

    //TransDesc.fRotationPerSec = 0.f;
    //TransDesc.fSpeedPerSec = 1.f;
    //TransDesc.vLocalPosition = { 0.f,-2.f,0.f,1.f };

    //TransDesc.vLocalScale = { 30.f,30.f,1.f,1.f };
    //TransDesc.vLocalRotation = { -90.f,0.f,0.f,1.f };



    //desc.TransformDesc = &TransDesc;

    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
    //    PROTO_OBJ_NAME(L"Floor"),
    //    ENUM_TO_UINT(LEVEL_ID::LOGO),
    //    strLayerTag, &desc)))
    //    return E_FAIL;

#pragma endregion

    /////////////////////////////////////
    CTerrain::TERRAIN_DESC pDesc;
    pDesc.TextureKey = L"Terrain";
    pDesc.ShaderName = L"VtxNorTex";
    pDesc.passName = "Default";

    CTransform::TRANSFORM_DESC TransDesc = {};

    TransDesc.fRotationPerSec = 0.f;
    TransDesc.fSpeedPerSec = 1.f;
    TransDesc.vLocalPosition = { -64.f,-5.f,-64.f,1.f };

    TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };

    pDesc.TransformDesc = &TransDesc;


    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
       PROTO_OBJ_NAME(L"Terrain"),
       ENUM_TO_UINT(LEVEL_ID::LOGO),
       strLayerTag, &pDesc)))
       return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
    CUI::tagUIDesc        Desc = {};

    Desc.ObjTag = L"Hp_UI";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    Desc.TextureKey = L"Hp";

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



    ///////////////////Minimapquad생성
    CUI::tagUIDesc        MinimapDesc = {};
    MinimapDesc.ObjTag = L"MinimapQuad";
    MinimapDesc.passName = "Blur";
    MinimapDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    MinimapDesc.fSizeX = 150;
    MinimapDesc.fSizeY = 150;
    MinimapDesc.fX = g_iWinSizeX - 100;
    MinimapDesc.fY =  100;

    CTransform::TRANSFORM_DESC MinimapTransDesc = {};
    MinimapDesc.TransformDesc = &MinimapTransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"MinimapQuad"),
        ENUM_TO_UINT(LEVEL_ID::LOGO),
        strLayerTag, &MinimapDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_MainCamera(const _wstring& strLayerTag)
{
    Create_MainCamera();

    /////////////////UICameraonenter
    Create_UICamera();

    Create_FreeCamera();
   
    Create_MiniMapCamera();

    CCamera_Base* pTargetCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    if (pTargetCam)
    {
        pTargetCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP), false);
        pTargetCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), false);

    }


    CCamera_Base* pFreeCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::FREE);
    if (pFreeCam)
    {
        pFreeCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP), false);
        pFreeCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), false);

    }


    CCamera_Base* pUICam = m_pGameInstance->Find_Camera(CAMERA_TYPE::UI);
    if (pUICam)
    {
        pUICam->Set_RenderAllRenderMask(false);
        pUICam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), true);
        
    }


    CCamera_Base* pMinimapCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);
    if (pMinimapCam)
    {
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), false);

    }
    
    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CQuad::QUAD_DESC        Desc = {};

    Desc.ObjTag = L"Player";
    Desc.TextureKey = L"Keroro";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::ALPHA);

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
    TransDesc.vLocalScale = {3.f,3.f,1.f,1.f };

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
    Desc.TextureKey = L"Player_Marker";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP);


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
    TransDesc.vLocalScale = { 5.f,5.f,1.f,1.f };

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
    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

#pragma region 메인카메라 등록및 transform 부모설정
   

    //CGameObject* pTestObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Test_Layer", L"Test");
    //CGameObject* pPlayerObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Player_Layer", L"Player");
    ////pPlayerObject->Set_Target(m_pGameInstance->Find_Camera(L"FreeCamera"));


    //if (pTestObject)
    //{
    //    if (pTestObject->Get_Transform()->Get_Parent() == nullptr)
    //    {
    //        pTestObject->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(0.f, 1.f, 0.f, 1.f));
    //        pTestObject->Get_Transform()->Rotation(_float3(-90.f, 0.f, 0.f));
    //        pTestObject->Get_Transform()->Set_Parent(pPlayerObject->Get_Transform());
    //    }


    //    else
    //        pTestObject->Get_Transform()->Set_Parent(nullptr);

    //}

#pragma endregion

    //미니맵 타겟 = 플레이어
    CCamera_Base* pMinimapCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);
    if (pMinimapCamera)
    {
        pMinimapCamera->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO),
            L"Player_Layer",
            L"Player"));
    }


     

    CGameObject* pMainCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    if (pMainCamera)
    {
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        ppMainCamera->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO),
            L"Player_Layer",
            L"Player"), true);
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

void CLevel_Logo::Set_UIPos_ByWorld(_float3 OffSet)
{
    /*UI를 플레이어 위치 머리위에 띄워보자!!*/

    _vector vOffset = XMLoadFloat3(&OffSet);

    CGameObject* pMarkerUI = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"UI_Layer", L"Player_Marker");
    CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");


    //if (pHeartUI && pPlayer)
    //{
    //    CTransform* pTransform = pHeartUI->Get_Transform();
    //    CTransform* pPlayerTransform = pPlayer->Get_Transform();

    //    _vector vPlayerPos = pPlayerTransform->Get_State(STATE::POSITION);

    //    if (pTransform && pPlayerTransform)
    //    {
    //        _vector vWorldPos = MathUtils::WorldToScreen(
    //            vPlayerPos ,
    //            //m_pGameInstance->Get_Main_ViewMatrix(),
    //            //m_pGameInstance->Get_Main_ProjMatrix(),
    //            g_iWinSizeX, g_iWinSizeY
    //        );

    //        vWorldPos += vOffset;

    //        pTransform->Set_State(STATE::POSITION, 
    //            MathUtils::ScreenToWorld_UI(
    //            vWorldPos, g_iWinSizeX, g_iWinSizeY));

    //    }
    //}
}

void CLevel_Logo::Create_MainCamera()
{
    CMainCamera::CAMERABASE_DESC Desc = {};
    Desc.ObjTag = L"MainCamera";

    Desc.eCameraType = CAMERA_TYPE::TARGET;
    Desc.eCameraFlag = CAMERA_FLAG::NONE;
    Desc.fWidth = (float)g_iWinSizeX;
    Desc.fHeight = (float)g_iWinSizeY;
    Desc.fFovy = 90.f;
    Desc.fNear = 0.1f;
    Desc.vPosition = _float3(0.f, 2.f, -2.f);
    Desc.vOffset= _float3(0.f, 2.f, -2.f);
    Desc.fFar = 1000.f;
    Desc.pTarget = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Player_Layer", L"Player");


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalRotation = { 30.f,0.f,0.f,1.f };
    Desc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MainCamera"), &Desc));
    m_pGameInstance->RegisterCamera(CAMERA_TYPE::TARGET, pInstance);
    

    //pInstance->Set_Active(false);
}

void CLevel_Logo::Create_UICamera()
{
    CCamera_Base::CAMERABASE_DESC UIDesc = {};
    UIDesc.ObjTag = L"UICamera";
    


    UIDesc.eCameraType = CAMERA_TYPE::UI;
    UIDesc.eCameraFlag = CAMERA_FLAG::NONE;
    UIDesc.fNear = 0.1f;
    UIDesc.fFar = 1.f;
    UIDesc.m_bDynamic = false;

    UIDesc.fWidth = (float)g_iWinSizeX;
    UIDesc.fHeight = (float)g_iWinSizeY;

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    UIDesc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"UICamera"), &UIDesc));
    m_pGameInstance->RegisterCamera(CAMERA_TYPE::UI , pInstance);
}

void CLevel_Logo::Create_FreeCamera()
{
    CCamera_Base::CAMERABASE_DESC Desc = {};
    Desc.ObjTag = L"FreeCamera";

    Desc.eCameraType = CAMERA_TYPE::FREE;
    Desc.eCameraFlag = CAMERA_FLAG::NONE;
    Desc.fWidth = (float)g_iWinSizeX;
    Desc.fHeight = (float)g_iWinSizeY;
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    //TransDesc.vLocalPosition = _float4(0.f, 2.f, -2.f,1.f);

    Desc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FreeCamera"), &Desc));
    m_pGameInstance->RegisterCamera(CAMERA_TYPE::FREE, pInstance);
    pInstance->Set_Active(false);
}

void CLevel_Logo::Create_MiniMapCamera()
{
    CMainCamera::CAMERABASE_DESC Desc = {};
    Desc.ObjTag = L"MinimapCamera";
    Desc.m_bCreateNewRenderTarget = true;

    Desc.vOffset = _float3(0.f, 10.f, 0.f);
    Desc.vUp = _float3(0.f, 0.f, -1.f);

    Desc.fWidth = (float)20.f;
    Desc.fHeight = (float)20.f;


    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;
 
    Desc.eCameraType = CAMERA_TYPE::MINIMAP;


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
   // TransDesc.vLocalRotation = _float4(90.f, 180.f, 0.f,1.f);
    Desc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MinimapCamera"), &Desc));
    m_pGameInstance->RegisterCamera(CAMERA_TYPE::MINIMAP, pInstance);
    //pInstance->Set_Active(false);
}
