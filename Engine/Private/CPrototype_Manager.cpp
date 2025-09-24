#include "CPrototype_Manager.h"
#include "CGameObject.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevels = iNumLevels;
    m_Prototypes.resize(iNumLevels);

    return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strProtoTag, CBase* pPrototype)
{
    if (iLevelIndex >= m_iNumLevels ||
        nullptr != Find_Prototype(iLevelIndex, strProtoTag))
        return E_FAIL;

    m_Prototypes[iLevelIndex].emplace(strProtoTag, pPrototype);

    return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelIndex, const _wstring& strPrototag, void* pArg)
{
    CBase* pPrototype = Find_Prototype(iLevelIndex, strPrototag);
    CheckNullResult(pPrototype, nullptr);


    return ePrototypeID == PROTOTYPE::GAMEOBJECT ? dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg) : /*나중에는 컴포넌트로 캐스팅*/nullptr;

}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const _wstring& strProtoTag)
{
    auto iter = m_Prototypes[iLevelIndex].find(strProtoTag);

    //만약 컨테이너내에 존재하지않는다면
    if(iter==m_Prototypes[iLevelIndex].end())
        return nullptr;

    return iter->second;
  
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
    CPrototype_Manager* pInstance = new CPrototype_Manager();
    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Create : CPrototype_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPrototype_Manager::Free()
{
    __super::Free();

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
