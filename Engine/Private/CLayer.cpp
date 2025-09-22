#include "CLayer.h"
#include "CGameObject.h"


CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
    m_GameObjects.push_back(pGameObject);
    return S_OK;
}

void CLayer::Update_Priority(_float fTimeDelta)
{
    for (auto& i : m_GameObjects)
    {
        if (nullptr != i)
            i->Update_Priority(fTimeDelta);
    }
        
}

void CLayer::Update(_float fTimeDelta)
{
    for (auto& i : m_GameObjects)
    {
        if (nullptr != i)
            i->Update(fTimeDelta);
    }
        
}

void CLayer::Update_Late(_float fTimeDelta)
{
    for (auto& i : m_GameObjects)
    {
        if (nullptr != i)
            i->Update_Late(fTimeDelta);
    }
       
}

CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();

    for (auto& i : m_GameObjects)
        Safe_Release(i);

    m_GameObjects.clear();
}
