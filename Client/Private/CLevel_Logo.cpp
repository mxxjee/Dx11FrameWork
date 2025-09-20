#include "CLevel_Logo.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"


USING(Client)

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}


HRESULT CLevel_Logo::Initialize()
{
    return S_OK;
}

HRESULT CLevel_Logo::Update(const _float fTimeDelta)
{
   
    if (GetKeyState(VK_UP) & 0x8000)
    {
        if (FAILED(m_pGameInstance->Level_Changer(
            ENUM_TO_UINT(LEVEL_ID::LOADING),
            CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_ID::GAMEPLAY,LEVELCHANGETYPE::OVERLAY),
            LEVELCHANGETYPE::LOADING)))

            return E_FAIL;
    }
    return S_OK;
}

void CLevel_Logo::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"로고 씬입니다.");

}

void CLevel_Logo::Clear()
{
    /*뭔가 클리어할게있남*/
}



CLevel_Logo* CLevel_Logo::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize()))
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
