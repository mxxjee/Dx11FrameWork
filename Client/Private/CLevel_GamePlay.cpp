#include "CLevel_GamePlay.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CBackGround.h"




USING(Client)

CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_GamePlay::Initialize(LevelArgs& args)
{
    __super::Initialize(args);
    if(FAILED(Ready_Layer_Background(L"Background_Layer")))
        return E_FAIL;

    return S_OK;
}

void CLevel_GamePlay::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    if (GetKeyState(VK_SPACE) & 0x8000)
        m_pGameInstance->Pop_Level();
}

void CLevel_GamePlay::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

 

    return;
}

void CLevel_GamePlay::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

void CLevel_GamePlay::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"게임플레이 씬입니다.");

}

HRESULT CLevel_GamePlay::Ready_Layer_Background(const _wstring& strLayerTag)
{
    CPanel::PANEL_DESC desc;
    desc.ObjTag = L"BackGround_Gameplay";
    desc.ImgPath = L"../../Resource/Skeleton.png";

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 0.f;
    TransDesc.fSpeedPerSec = 1.f;

    desc.TransformDesc = &TransDesc;

    if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::GAMEPLAY),
        PROTO_OBJ_NAME(L"Panel"), 
        ENUM_TO_UINT(LEVEL_ID::GAMEPLAY), 
        strLayerTag,&desc)))
        return E_FAIL;

    return S_OK;
}

void CLevel_GamePlay::OnEnter()
{
    int A = 0;
}

void CLevel_GamePlay::OnResume()
{ 
    int A=0;
}

void CLevel_GamePlay::OnPause()
{
    /*2회 호출막기..*/
    if (Get_State() == LEVELSTATE::HIDDEN || Get_State() == LEVELSTATE::PAUSE)
        return;
    int A=0;
}

void CLevel_GamePlay::OnExit()
{
    int A=0;
}



CLevel_GamePlay* CLevel_GamePlay::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_GamePlay* pInstance = new CLevel_GamePlay(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(args)))
    {
        MSG_BOX("Failed to Create : Level_Logo");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_GamePlay::Free()
{
    __super::Free();
 

}
