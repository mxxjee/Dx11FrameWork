#include "CLevel.h"
#include "CGameInstance.h"
#include "CLayer.h"


CLevel::CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext*_pDeviceContext)
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

void CLevel::Update_Priority(_float fTimeDelta)
{
    auto& layers = m_pGameInstance->Get_Layers(m_eLevelArgs.m_iLevelID);
    for (auto& layer : layers)
    {
        layer.second->Update_Priority(fTimeDelta);
    }
}

void CLevel::Update(const _float fTimeDelta)
{
    auto& layers=m_pGameInstance->Get_Layers(m_eLevelArgs.m_iLevelID);
    for (auto& layer : layers)
    {
    
        layer.second->Update(fTimeDelta);
      
    }

}

void CLevel::Update_Late(_float fTimeDelta)
{
    auto& layers = m_pGameInstance->Get_Layers(m_eLevelArgs.m_iLevelID);
    for (auto& layer : layers)
    {
        layer.second->Update_Late(fTimeDelta);
    }
}

void CLevel::Update_Render(_float fTimeDelta)
{
    auto& layers = m_pGameInstance->Get_Layers(m_eLevelArgs.m_iLevelID);
    for (auto& layer : layers)
    {
        layer.second->Update_Render(fTimeDelta);
    }
}

void CLevel::Render()
{
    /*auto& layers = m_pGameInstance->Get_Layers(m_eLevelArgs.m_iLevelID);
    for (auto& layer : layers)
    {
        layer.second->
    }*/
}


void CLevel::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

}
