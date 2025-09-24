#include "CLevel.h"
#include "CGameInstance.h"
#include "CLayer.h"


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

void CLevel::Update(const _float fTimeDelta)
{
    auto& layers=m_pGameInstance->Get_Layers(m_eLevelArgs.m_iLevelID);
    for (auto& layer : layers)
    {
        layer.second->Update_Priority(fTimeDelta);
        layer.second->Update(fTimeDelta);
        layer.second->Update_Late(fTimeDelta);
    }

}

void CLevel::Render()
{
}


void CLevel::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

}
