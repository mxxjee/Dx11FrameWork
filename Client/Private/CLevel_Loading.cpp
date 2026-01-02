#include "CLevel_Loading.h"
#include "CLoader.h"
#include "CGameInstance.h"

#include "CLevel_Town.h"
#include "CLevel_Logo.h"

#include "CInput_Manager.h"
#include "CUICreator.h"
#include "CCamera_Base.h"

#include "CGameManager.h"
#include "CUIComponent.h"
#include "CQuad.h"
#include "CTransform.h"
#include "CPanel.h"
#include "CVIBuffer_Rect.h"
#include "CUICamera.h"
#include "CFadeScreen.h"







USING(Client)

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL_ID iLevelID, LEVELCHANGETYPE eChangeType, LevelArgs& args)
{
    /*로더 : 로딩을 위한 서브스레드를 생성하고 실제 로딩 수행.*/
    //로더 생성
    
    __super::Initialize(args);

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, iLevelID);
    CheckNullResult(m_pLoader, E_FAIL);

    m_eNextLevelID = iLevelID;
    m_eChangeType = eChangeType;

    Set_Flag(LEVELFLAG::TRANSIENT);

    Ready_Prototypes();
        
                         
    Create_UICamera();

    //로딩할동안 보여줄 UI생성
    if (FAILED(Ready_UI_Layer()))
        return E_FAIL;

    UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
    pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));


    return S_OK;
}

void CLevel_Loading::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    if (m_pLoader->IsFinished())
    {
        m_fTime += fTimeDelta;

    }


    if (m_fTime>=m_fNextTime)
    {
        LevelArgs args;
        args.changeType = m_eChangeType;
        args.m_iLevelID = ENUM_TO_UINT(m_eNextLevelID);


        if (SUCCEEDED(m_pGameInstance->Level_Changer(ENUM_TO_UINT(m_eNextLevelID), args)))
            return;

        MSG_BOX("다음 씬 불러오기 실패");
        m_fTime = 0.f;
    }
}

void CLevel_Loading::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    /*로더야 로딩 다 되었니?*/
    CheckNull(m_pLoader);

   

}

void CLevel_Loading::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

void CLevel_Loading::Render()
{
    //UI렌더. (로딩바)
    CheckNull(m_pLoader);
    m_pLoader->Output();

}



HRESULT CLevel_Loading::Ready_UI_Layer()
{
    //UI 요소 추가.
    if (FAILED(UICreator::Create_Loading_UI(L"UI_Layer")))
        return E_FAIL;




    return S_OK;
}

void CLevel_Loading::OnEnter()
{
    m_pGameInstance->Set_IsLoading(true);
    CheckNull(pFadeScreen);

}

void CLevel_Loading::OnResume(_uint iPreLevel)
{
    int A = 0;
}

void CLevel_Loading::OnPause(_uint iNextLevel)
{
    int A = 0;

}

void CLevel_Loading::OnExit()
{
    CheckNull(pFadeScreen);

    pFadeScreen->PlayFadeOut();
    m_pGameInstance->Set_IsLoading(false);
}


CLevel_Loading* CLevel_Loading::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pDeviceContext);

    if (FAILED(pInstance->Initialize(
        static_cast<LEVEL_ID>(args.iNextLevelID), 
        static_cast<LEVELCHANGETYPE>(args.changeType), args
    )))
    {
        MSG_BOX("Failed to Create : Level_Loading");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Loading::Free()
{
    __super::Free();
    Safe_Release(m_pLoader);


}

void CLevel_Loading::Ready_Prototypes()
{

    CheckFalse(CGameManager::GetInstance()->Get_IsFirstLoading());

    //필요한 컴포넌트 및 오브젝트 먼저 원형생성

   
    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"UICamera", CUICamera::Create(m_pDevice, m_pContext))))
        return;



    m_pGameInstance->Load_Textures(L"../../Resource/UI/Loading/", L".dds");

    CGameManager::GetInstance()->Set_FirstLoading(false);

    return;
}

void CLevel_Loading::Create_UICamera()
{
    CCamera_Base* pUICam=m_pGameInstance->Find_Camera(CAMERA_TYPE::UI);
    if (!pUICam)
    {
        CCamera_Base::CAMERABASE_DESC UIDesc = {};
        UIDesc.ObjTag = L"UICamera";



        UIDesc.eCameraType = CAMERA_TYPE::UI;
        UIDesc.eCameraFlag = CAMERA_FLAG::NONE;
        UIDesc.fNear = 0.1f;
        UIDesc.fFar = 1.f;
        UIDesc.m_bDynamic = false;
        UIDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

        UIDesc.fWidth = (float)g_iWinSizeX;
        UIDesc.fHeight = (float)g_iWinSizeY;

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.fRotationPerSec = 10.f;
        TransDesc.fSpeedPerSec = 5.f;
        UIDesc.TransformDesc = &TransDesc;


        CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"UICamera"), &UIDesc));
        m_pGameInstance->RegisterCamera(CAMERA_TYPE::UI, pInstance);

        CCamera_Base* pUICam = m_pGameInstance->Find_Camera(CAMERA_TYPE::UI);
        if (pUICam)
        {
            pUICam->Set_RenderAllRenderMask(false);
            pUICam->Set_RenderMask(ENUM_TO_UINT(RENDERGROUP::UI), true);

        }

    }
}
