#include "CLevel_Logo.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CBackGround.h"



USING(Client)

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}


HRESULT CLevel_Logo::Initialize(LevelArgs& args)
{
    __super::Initialize(args);

    if(FAILED(Ready_Layer_Background(L"BackGround_Layer")))
        return E_FAIL;


    return S_OK;
}

void CLevel_Logo::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    if (GetKeyState(VK_UP) & 0x8000)
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
}

void CLevel_Logo::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"로고 씬입니다.");

}

HRESULT CLevel_Logo::Ready_Layer_Background(const _wstring& strLayerTag)
{
    CPanel::PANEL_DESC        Desc = {};

    Desc.ObjTag = L"BackGround";
    Desc.ImgPath = L"../../Resource/Character.png";

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    
    Desc.TransformDesc = &TransDesc;

    if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::LOGO),
        PROTO_OBJ_NAME(L"BackGround"),
        ENUM_TO_UINT(LEVEL_ID::LOGO),
        strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::OnEnter()
{
    int A = 0;
}

void CLevel_Logo::OnResume()
{
    int A = 0;
}

void CLevel_Logo::OnPause()
{
    /*2회 호출막기..*/
    if (Get_State() == LEVELSTATE::HIDDEN || Get_State() == LEVELSTATE::PAUSE)
        return;


    int A = 0;
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
