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
        
       
        if (!m_bPressEnter)
        {
            m_bPressEnter = true;
            CUI* pUI = dynamic_cast<CUI*>(pTitleBackGround);
            if (pUI)
                pUI->Set_Texture(L"Title_Blur");

            ///UIGroup비활성화
            m_pGameInstance->SetActiveGroup(L"LogoGroup", false);

            pFadeScreen->PlayFadeIn();
        }

   
       
    }

    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::S))
    {
        /*씬이동*/
        LevelArgs args;
        args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::TOWN);
        args.changeType = LEVELCHANGETYPE::REPLACETOP; 
        //args.loadingChangeType = LEVELCHANGETYPE::PUSH;
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
    
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::UpArrow))
        --m_ButtonIdx;

    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::DownArrow))
        ++m_ButtonIdx;

    m_ButtonIdx = MathUtils::Clamp<unsigned int>(m_ButtonIdx, 0, 2);



    UIGroup* pButtonGroup = m_pGameInstance->Get_UIGroup(L"ButtonSlotGroup");
    int i = 0;
    if (pButtonGroup)
    {
        for (auto& pair : pButtonGroup->Objects)
        {
            CGameObject* pTarget = pair.second;
            if (pTarget)
            {
                CButton* pButton = dynamic_cast<CButton*>(pTarget);

                if (pButton)
                {
                    if (i == m_ButtonIdx)
                        pButton->Set_Hover(true);

                    else
                        pButton->Set_Hover(false);
                    ++i;
                }


            }
        }
  
        
    }

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

    ///*카메라 변경 테스트*/
    //if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::C))
    //{
    //    if (iTargetIdx == 1)
    //        iTargetIdx = 0;
    //    else
    //        ++iTargetIdx;

    //    iTargetIdx = MathUtils::Clamp(iTargetIdx, 0, 1);

    //    switch (iTargetIdx)
    //    {
    //    case 0:
    //        m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);
    //        break;

    //    case 1:
    //        m_pGameInstance->Set_MainCamera(CAMERA_TYPE::FREE);

    //        break;
    //    }

    //}

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



#pragma region 버튼생성
    ///////////////////////////////슬롯버튼//////////////////////////////////
    UIGroup ButtonSlotGroup;
    ButtonSlotGroup.Key = L"ButtonSlotGroup";


    //////////슬롯(버튼)///////
    for (int i = 0; i < 3; ++i)
    {
        CButton::tagButtonDesc Button_Desc = {};
        Button_Desc.ObjTag = L"SaveSlot"+to_wstring(i);
        Button_Desc.passName = "SaveSlot";
        Button_Desc.m_iLevelID = m_iLevelID;

        Button_Desc.TextureKey = L"SaveSlot";
        Button_Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

        Button_Desc.fSizeX = 800 * 0.9f;
        Button_Desc.fSizeY = 140 * 0.9f;
        Button_Desc.Depth = 0.49f;

        Button_Desc.fX = g_iWinSizeX>>1;
        Button_Desc.fY = 170 + (i*200.f);

        Button_Desc.eKeyCode = KeyCode::Enter;
        Button_Desc.SelectActionFunc = [&]()
        {
            /*씬이동*/
            LevelArgs args;
            args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::SPAWN);
            args.changeType = LEVELCHANGETYPE::REPLACETOP;
            //args.loadingChangeType = LEVELCHANGETYPE::PUSH;
            args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);

            pFadeScreen->Set_AutoMode(false);
            if (FAILED(m_pGameInstance->Level_Changer(
                ENUM_TO_UINT(LEVEL_ID::LOADING),
                args)))
                return;
        };


        Button_Desc.TransformDesc = &TransDesc;
        //AlphaAnim등록
        CUIComponent::UICOMP_DESC UIDesc = {};
        Button_Desc.UICompDesc = &UIDesc;


        CBase* pSlotObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Button"), &Button_Desc);
        CGameObject* ppSlotObj = dynamic_cast<CGameObject*>(pSlotObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::LOGO), strLayerTag, ppSlotObj)))
            return E_FAIL;

        ButtonSlotGroup.push_back(ppSlotObj);

    }
  
    m_pGameInstance->Register_UIGroup(ButtonSlotGroup);
    m_pGameInstance->SetActiveGroup(ButtonSlotGroup.Key, false);


    return S_OK;
#pragma endregion
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
        pMinimapCam->Set_Distance(30.f);
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
    //타겟카메라
    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

    ///UIGroup활성화(페이드인아웃)
    m_pGameInstance->SetActiveGroup(L"LogoGroup", true);
    m_pGameInstance->SetActiveGroup(L"ButtonSlotGroup", false);

 
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

    pFadeScreen->PlayFadeIn();
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


    Desc.vOffset= _float3(0.f, 8.5f, -7.f);
    Desc.fFar = 100.f;
    Desc.pTarget = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOGO), L"Player_Layer", L"Player");


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalRotation = {52.f,0.f,0.f,1.f };
    TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };
    Desc.TransformDesc = &TransDesc;


    CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MainCamera"), &Desc));
    m_pGameInstance->RegisterCamera(CAMERA_TYPE::TARGET, pInstance);
    

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
