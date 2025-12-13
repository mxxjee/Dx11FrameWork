#include "CLevel.h"
#include "CGameInstance.h"
#include "CLayer.h"


CLevel::CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pGameInstance{ CGameInstance::GetInstance() }, 
    m_pDevice{ pDevice },
    m_pContext{ pContext }
{
    Safe_AddRef(m_pGameInstance);
}


HRESULT CLevel::Initialize(LevelArgs& args)
{
    m_eLevelArgs = args;
    m_iLevelID = args.m_iLevelID;

  


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

void CLevel::OnEnter()
{
    if (m_pGameInstance->Get_IsLoading())
        m_pGameInstance->Set_IsLoading(false);
}

void CLevel::OnResume(_uint iPreLevel)
{
    if (m_pGameInstance->Get_IsLoading())
        m_pGameInstance->Set_IsLoading(false);


}
void CLevel::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

}
