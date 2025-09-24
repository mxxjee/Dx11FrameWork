#include "CLevel_GamePlay.h"
#include "CLoader.h"
#include "CGameInstance.h"


USING(Client)

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
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

void CLevel_GamePlay::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (GetKeyState(VK_SPACE) & 0x8000)
        m_pGameInstance->Pop_Level();

    return;
}

void CLevel_GamePlay::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"게임플레이 씬입니다.");

}

HRESULT CLevel_GamePlay::Ready_Layer_Background(const _wstring& strLayerTag)
{
    if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"BackGround"), ENUM_TO_UINT(LEVEL_ID::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    return S_OK;
}



CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LevelArgs& args)
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
