#include "CLevel.h"
#include "CGameInstance.h"



CLevel::CLevel(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :m_pGameInstance{ CGameInstance::GetInstance() }, 
    m_pDevice{_pDevice},
    m_pDeviceContext{_pDeviceContext}
{
    Safe_AddRef(m_pGameInstance);
}


HRESULT CLevel::Initialize(LevelArgs& args)
{
    m_eLevelArgs = args;
    return S_OK;
}

HRESULT CLevel::Update(const _float fTimeDelta)
{
    return S_OK;
}

void CLevel::Render()
{
}


void CLevel::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

}
