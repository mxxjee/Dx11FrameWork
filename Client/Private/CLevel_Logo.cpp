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

#include "CUI.h"
#include "CUIComponent.h"
#include "CFadeScreen.h"
#include "CButton.h"

#include "MathUtils.h"
#include "CUICreator.h"








USING(Client)

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}


HRESULT CLevel_Logo::Initialize(LevelArgs& args)
{

    __super::Initialize(args);
 

    if (FAILED(Ready_Layer_MainCamera(L"Camera_Layer")))
        return E_FAIL;

    
    if (FAILED(Ready_Layer_UI(L"UI_Layer")))
        return E_FAIL;


    UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
    pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));

    return S_OK;
}

void CLevel_Logo::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

    
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Enter))
    {
        pFadeScreen->PlayFadeIn();

        pFadeScreen->Set_FadeInEndFunc([FadeScreen=pFadeScreen]()
            {
                /*씬이동*/
                LevelArgs args;
                args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::SPAWN);
                args.changeType = LEVELCHANGETYPE::REPLACETOP;
                //args.loadingChangeType = LEVELCHANGETYPE::PUSH;
                args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);


                if (FAILED(CGameInstance::GetInstance()->Level_Changer(
                    ENUM_TO_UINT(LEVEL_ID::LOADING),
                    args)))
                    return;
            });

    

    

   
       
    }

 
}

void CLevel_Logo::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CLevel_Logo::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    

}

void CLevel_Logo::Render()
{
    //UI렌더. (로딩바)
    wchar_t szTitle[256];
    swprintf_s(szTitle, L"로고 씬입니다. FPS : %.1f", m_pGameInstance->Get_FPS(L"Timer_60"));

    SetWindowText(g_hWnd, szTitle);
}



HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
    //타이틀 배경사진
    CUI::tagUIDesc BackGroundDesc = {};
    CTransform::TRANSFORM_DESC TransDesc;
    BackGroundDesc.ObjTag = L"Title_Background";
    BackGroundDesc.TextureKey = L"Title";
    BackGroundDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    BackGroundDesc.m_iLevelID = m_iLevelID;

    BackGroundDesc.fSizeX = g_iWinSizeX;
    BackGroundDesc.fSizeY = g_iWinSizeY;

    BackGroundDesc.fX = g_iWinSizeX >> 1;
    BackGroundDesc.fY = g_iWinSizeY >> 1;


    BackGroundDesc.TransformDesc = &TransDesc;

    CBase* pBackGroundImg = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &BackGroundDesc);
    pTitleBackGround = dynamic_cast<CGameObject*>(pBackGroundImg);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::LOGO), strLayerTag, pTitleBackGround)))
        return E_FAIL;

    

#pragma region 로고생성
    /////////////////////////////로고 생성//////////////////////////////////
    UIGroup LogoGroup;
    LogoGroup.Key = L"LogoGroup";


    //////////타이틀그냥 흰색로고
    CUI::tagUIDesc Title_Desc = {};
    Title_Desc.ObjTag = L"Title";
    Title_Desc.TextureKey = L"Logo_White";
    Title_Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    Title_Desc.m_iLevelID = m_iLevelID;

    Title_Desc.fSizeX = 1092*0.7f;
    Title_Desc.fSizeY = 546 * 0.7f;
    Title_Desc.Depth = 0.49f;

    Title_Desc.fX = 400;
    Title_Desc.fY = 255;


    Title_Desc.TransformDesc = &TransDesc;
    //AlphaAnim등록
    CUIComponent::UICOMP_DESC UIDesc = {};
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fStart = _float4(0.f, 0.f, 0.f, 0.f);
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fTarget = _float4(1.f, 0.f, 0.f, 0.f);
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].m_fSpeed = 2.f;
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bLoop = false;
    Title_Desc.UICompDesc = &UIDesc;


    CBase* pLogoObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Title_Desc);
    CGameObject*    pInstance= dynamic_cast<CGameObject*>(pLogoObj);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::LOGO), strLayerTag, pInstance)))
        return E_FAIL;
    LogoGroup.push_back(pInstance);


    /////////마스크
    CUI::tagUIDesc Logo_MaskDesc = {};
    Logo_MaskDesc.ObjTag = L"Title_LogoMask";
    Logo_MaskDesc.passName = "Logo";
    Logo_MaskDesc.TextureKey = L"Logo_Mask";
    Logo_MaskDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    Logo_MaskDesc.m_iLevelID = m_iLevelID;

    Logo_MaskDesc.fSizeX = 1092 * 0.7f;
    Logo_MaskDesc.fSizeY = 546 * 0.7f;

    Logo_MaskDesc.fX = 400;
    Logo_MaskDesc.fY = 255;


    Logo_MaskDesc.TransformDesc = &TransDesc;
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].m_fSpeed = 0.5f;
    Logo_MaskDesc.UICompDesc = &UIDesc;

    CBase* pLogoMaskObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Logo_MaskDesc);
    CGameObject* pInstance2 = dynamic_cast<CGameObject*>(pLogoMaskObj);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::LOGO), strLayerTag, pInstance2)))
        return E_FAIL;

    LogoGroup.push_back(pInstance2);
    m_pGameInstance->Register_UIGroup(LogoGroup);

    for (auto& i : LogoGroup.Objects)
    {
        CUI* pUI = dynamic_cast<CUI*>(i.second);
        if (pUI)
            pUI->Set_ActiveAnim(0,[pUI]()
                {
                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA);
                });

        pUI->Set_Active(false);

    }
#pragma endregion



    if (FAILED(UICreator::Create_LevelUI(strLayerTag)))
        return E_FAIL;



    if (FAILED(UICreator::Create_Interaction_GetUI(strLayerTag)))
        return E_FAIL;


    if (FAILED(UICreator::Create_ItemGet_UI(strLayerTag)))
        return E_FAIL;

    if (FAILED(UICreator::Create_ItemGet_Desc_UI(strLayerTag)))
        return E_FAIL;

    if (FAILED(UICreator::Create_Interaction_SeeUI(strLayerTag)))
        return E_FAIL;

    if (FAILED(UICreator::Create_Interaction_OpenUI(strLayerTag)))
        return E_FAIL;

    if (FAILED(UICreator::Create_See_Desc_UI(strLayerTag)))
        return E_FAIL;

 

    if (FAILED(UICreator::Create_InvenSlot(strLayerTag)))
        return E_FAIL;

    return S_OK;

  
}

HRESULT CLevel_Logo::Ready_Layer_MainCamera(const _wstring& strLayerTag)
{
    //로고씬에서 각종 카메라를 먼저 생성한다.
    Create_MainCamera();

    /////////////////UICameraonenter
   // Create_UICamera();

    Create_FreeCamera();
   
    Create_MiniMapCamera();

    CCamera_Base* pTargetCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    if (pTargetCam)
    {
        pTargetCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::PRIORITY_MINIMAP), false);
        pTargetCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP), false);
        pTargetCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), false);
        pTargetCam->Set_Distance(55.f);

    }


    CCamera_Base* pFreeCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::FREE);
    if (pFreeCam)
    {
        pFreeCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::PRIORITY_MINIMAP), false);
        pFreeCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP), false);
        pFreeCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), false);

    }


    //CCamera_Base* pUICam = m_pGameInstance->Find_Camera(CAMERA_TYPE::UI);
    //if (pUICam)
    //{
    //    pUICam->Set_RenderAllRenderMask(false);
    //    pUICam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), true);
    //    
    //}


    CCamera_Base* pMinimapCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);
    if (pMinimapCam)
    {
        pMinimapCam->Set_Distance(50.f);
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::NONALPHA), false);
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), false);
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::PRIORITY), false);
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::LIGHT), false);
        pMinimapCam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::COMBINED), false);
     
    }
    


    return S_OK;
}

void CLevel_Logo::OnEnter()
{
    pFadeScreen->PlayFadeOut();

    //타겟카메라
    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

    ///UIGroup활성화(페이드인아웃)
    m_pGameInstance->SetActiveGroup(L"LogoGroup", true);

 
    //페이드스크린이벤트 추가설정..
    m_pGameInstance->RegisterEvent(L"FadeOutEnd", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"ButtonSlotGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    i.second->Set_Active(true);
                }
            }
        });




  

}

void CLevel_Logo::OnResume(_uint iPreLevel)
{

}

void CLevel_Logo::OnPause(_uint iNextLevel)
{
    /*2회 호출막기..*/
    if (Get_State() == LEVELSTATE::HIDDEN || Get_State() == LEVELSTATE::PAUSE)
        return;

  
}

void CLevel_Logo::OnExit()
{

    m_pGameInstance->UnRegisterEvent(L"FadeOutEnd");


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
    Desc.fFovy = 60.f;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
    Desc.fNear = 0.1f;
    Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);


    Desc.vOffset = _float3(0.f, 9.f, -6.f);//_float3(0.f, 7.5f, -4.f);
    Desc.fFar = 100.f;
    Desc.pTarget = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Player_Layer", L"Player");


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalRotation = { 56.f,0.f,0.f,1.f }; //{65.f, 0.f, 0.f, 0.f};
    TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };
    Desc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MainCamera"), &Desc));
    m_pGameInstance->RegisterCamera(CAMERA_TYPE::TARGET, pInstance);
    

    CCamera_Base* pCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    CheckNull(pCam);
    
    pCam->Set_InitRotation(_float3(56.f, 0.f, 0.f));
    pCam->Set_TargetRotation(_float3(56.f, 0.f, 0.f)); 

    //pInstance->Set_Active(false);
}

void CLevel_Logo::Create_UICamera()
{
   /*로딩맨처음에 생성으로 바꾸기*/
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
    Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

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
    Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);


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
