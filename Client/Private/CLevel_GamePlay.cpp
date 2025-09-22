#include "CLevel_GamePlay.h"
#include "CLoader.h"
#include "CGameInstance.h"


USING(Client)

CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    return S_OK;
}

HRESULT CLevel_GamePlay::Update(const _float fTimeDelta)
{
    if (GetKeyState(VK_SPACE) & 0x8000)
        m_pGameInstance->Pop_Level();

    return S_OK;
}

void CLevel_GamePlay::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"게임플레이 씬입니다.");

}



CLevel_GamePlay* CLevel_GamePlay::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CLevel_GamePlay* pInstance = new CLevel_GamePlay(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize()))
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
