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
#include "CLight.h"

#include "CModelObject.h"
#include "CModel.h"
#include "CBody.h"
#include "CMonster.h"






USING(Client)

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}


HRESULT CLevel_Logo::Initialize(LevelArgs& args)
{

    __super::Initialize(args);
 

    if (FAILED(Ready_Lights()))
        return E_FAIL;


    if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(L"Player_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(L"Monster_Layer")))
        return E_FAIL;


    if (FAILED(Ready_Layer_MainCamera(L"Camera_Layer")))
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

    CGameObject* pTestObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"UI_Layer", L"Player_Marker");
    CGameObject* pPlayerObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Player_Layer", L"Player");

    if (pTestObject)
    {
        pTestObject->Get_Transform()->Rotation(_float3(90.f, 0.f, 0.f));
        _vector vPos = pPlayerObject->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD);
        pTestObject->Get_Transform()->Set_State(STATE::POSITION, vPos);

    }

    return;
}

void CLevel_Logo::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    
   /*점조명테스트*/
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::PageUp))
    {
        CLight* pLight = m_pGameInstance->Get_Light(m_iLevelID, 0);
        LIGHT_DESC LightDesc = (*pLight->Get_LightDesc());
        LightDesc.vPosition.y += 0.1f;
        pLight->Set_LightDesc(LightDesc);

    }

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::PageDown))
    {
        CLight* pLight = m_pGameInstance->Get_Light(m_iLevelID, 0);
        LIGHT_DESC LightDesc = (*pLight->Get_LightDesc());
        LightDesc.vPosition.y -= 0.1f;
        pLight->Set_LightDesc(LightDesc);

    }

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
            m_pGameInstance->Set_MainCamera(CAMERA_TYPE::FREE);

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

HRESULT CLevel_Logo::Ready_Lights()
{
    LIGHT_DESC      LightDesc{};
    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f,0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID,LightDesc)))
        return E_FAIL;


    ////////////빨간 점조명 테스트//////////
    LIGHT_DESC       RedLight;
    RedLight.eType = LIGHT::POINT;

    RedLight.vDiffuse = _float4(10.f, 0.f, 0.f, 1.f);//빨간색
    RedLight.fRange =_float4(10.f,0.f,0.f,1.f); 
    RedLight.vPosition = _float4(0.f,3.f,0.f,1.f);
    RedLight.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    RedLight.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);


    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, RedLight)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{

    /////////////////////////////////////
    CTerrain::TERRAIN_DESC pDesc;
    pDesc.TextureKey = L"Terrain";
    pDesc.ShaderName = L"VtxNorTex";
    pDesc.passName = "Default";
    pDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);

    CTransform::TRANSFORM_DESC TransDesc = {};

    TransDesc.fRotationPerSec = 0.f;
    TransDesc.fSpeedPerSec = 1.f;
    TransDesc.vLocalPosition = { -64.f,-5.f,-64.f,1.f };

    TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };

    pDesc.TransformDesc = &TransDesc;


    CBase* pTerrain = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Terrain"), &pDesc);
    if (pTerrain)
    {
        CTerrain_Base* ppTerrain = dynamic_cast<CTerrain_Base*>(pTerrain);
        if (ppTerrain)
            m_pGameInstance->Register_Terrain(L"Terrain", ppTerrain);
    }


    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
    UIGroup     HeartGroup;
    HeartGroup.Key = L"HeartGroup";
    
    for (int i = 0; i < 5; ++i)
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"Hp_UI" + to_wstring(i);
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"Hp";

        Desc.iIdx = i;
        
        Desc.fSizeX = 38.f;
        Desc.fSizeY = 38.f;
        Desc.fX = 50.f + (i*45.f);
        Desc.fY = 50.f;

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.fRotationPerSec = 10.f;
        TransDesc.fSpeedPerSec = 5.f;
        Desc.TransformDesc = &TransDesc;
        
        //AlphaAnim등록
        CUIComponent::UICOMP_DESC UIDesc = {};
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].fStart = _float4(Desc.fX,Desc.fY,1.f,1.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].fTarget = _float4(Desc.fX -10.f, Desc.fY, 1.f, 1.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].m_fSpeed = 0.1f;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].bLoop =true;
        
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fStart = _float4(1.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fTarget = _float4(0.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].m_fSpeed = 5.f;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bLoop = true;
        Desc.UICompDesc = &UIDesc;




        
        CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Desc);
        if (pObj)
        {
            CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), strLayerTag, pInstance)))
                return E_FAIL;


            HeartGroup.Objects.push_back(pInstance);

        }

    }

    m_pGameInstance->Register_UIGroup(HeartGroup);
    m_pGameInstance->RegisterEvent(L"OnHeartDamaged", [this](void* pData)
        {
            int* iHp = reinterpret_cast<int*>(pData);
            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"HeartGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i);
                    if (pUI)
                    {
                        if (pUI->Get_Idx() == *iHp)
                        {
                            pUI->Set_ActiveAnim(1, [pUI]()
                                {
                                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA);
                                    pUI->Get_UIComp()->PlayAnim(UIAnimType::POSITION);
                                });


                            pUI->OnActivated(false);
                        }
                    }
                }
            }
        });
   

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


    /*플레이어 마커 생성*/
    CQuad::QUAD_DESC        Desc = {};

    Desc.ObjTag = L"Player_Marker";
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
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::NONALPHA), false);
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), false);

    }
    
    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Player(const _wstring& strLayerTag)
{



    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Player"),
        ENUM_TO_UINT(LEVEL_ID::LOGO),
        strLayerTag, nullptr)))
        return E_FAIL;

    return S_OK;

}

HRESULT CLevel_Logo::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    for (int i = 0; i < 20; ++i)
    {
        CMonster::MonsterDesc desc;
        
        CBody::BODY_DESC bodyDesc;
        bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        bodyDesc.modelName = L"MoriblinSword";

        desc.BodyDesc = &bodyDesc;
        
        desc.iAttack = 10;
        desc.MaxHp = 5;

        desc.ObjTag = L"Monster"+to_wstring(i);
        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.vLocalPosition = { 10.f,0.f,10.f,1.f };
        TransDesc.fSpeedPerSec = 5.f;
        TransDesc.fRotationPerSec = 10.f;

        desc.TransformDesc = &TransDesc;

   

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
            PROTO_OBJ_NAME(L"Monster"),
            ENUM_TO_UINT(LEVEL_ID::LOGO),
            strLayerTag, &desc)))
            return E_FAIL;

    }
   
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

    /*_vector vOffset = XMLoadFloat3(&OffSet);

    }
   */


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
    Desc.vOffset= _float3(0.f, 4.f, -3.f);
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
