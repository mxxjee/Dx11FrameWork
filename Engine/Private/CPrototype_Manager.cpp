#include "CPrototype_Manager.h"
#include "CGameObject.h"
#include "CLayer.h"


CPrototype_Manager::CPrototype_Manager()
{
}


HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevels = iNumLevels;
    m_Prototypes.reserve(iNumLevels);

    return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint m_iNumLevel, const _wstring& strPrototypeTag, CBase* _base)
{
    //1. 원본검색
    if (m_iNumLevel >= m_iNumLevels || nullptr != Find_Prototype(m_iNumLevel, strPrototypeTag))
    {
        //이미존재하거나, 숫자가 그 이상일경우
        return E_FAIL;
    }


    else
        m_Prototypes[m_iNumLevel].emplace(strPrototypeTag, _base);

    return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE eType, _uint iNumLevel, const _wstring& strPrototypeTag, void* pArg)
{
    //1.원본찾기
    CBase* pInstance = Find_Prototype(iNumLevel, strPrototypeTag);
    if (pInstance)
    {
        return (eType == PROTOTYPE::GAMEOBJECT) ? dynamic_cast<CGameObject*>(pInstance)->Clone(pArg) : /*dynamic_cast<CComponent*>(pInstance)->Clone()*/(nullptr);
    }

    else
        return nullptr;
}

CBase* CPrototype_Manager::Find_Prototype(_uint m_iNumLevel, const _wstring tag)
{
    auto iter = m_Prototypes[m_iNumLevel].find(tag);
    if(iter==m_Prototypes[m_iNumLevel].end())
        return nullptr;


    return iter->second;

}


CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
    CPrototype_Manager* pInstance = new CPrototype_Manager();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CPrototype_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPrototype_Manager::Free()
{
    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& pair : m_Prototypes[i])
        {
            Safe_Release(pair.second);
        }

        m_Prototypes[i].clear();
    }

    m_Prototypes.clear();
}


