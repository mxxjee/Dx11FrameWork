#include "CModel_Manager.h"
#include "CModel.h"


CModel_Manager::CModel_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice), m_pContext(_pContext)
{
}

HRESULT CModel_Manager::Initialize()
{
    return S_OK;
}

HRESULT CModel_Manager::Register_Model(const _wstring& Tag, CModel* pInstance)
{
    CModel* pModel = Find_Model(Tag);
    if (pModel)
    {
#ifdef _DEBUG 
        MSG_BOX("Material is alreday exist");
#endif
        return E_FAIL;

    }

    else
    {
        m_mapModel.emplace(Tag, pInstance);
        m_iCount += 1;
        
    }
    return S_OK;
}

CModel_Manager* CModel_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
    CModel_Manager* pInstance = new CModel_Manager(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CModel_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel_Manager::Free()
{
    __super::Free();
    for (auto& pair : m_mapModel)
    {
        if (pair.second)
            Safe_Release(pair.second);

    }
}

CModel* CModel_Manager::Find_Model(const _wstring& ProtoModelName)
{

    auto iter = m_mapModel.find(ProtoModelName);

    if (iter != m_mapModel.end())
        return iter->second;


    return nullptr;
}

CModel* CModel_Manager::Clone_Model(const _wstring& ProtoModelName,void *pArg)
{
    auto iter = m_mapModel.find(ProtoModelName);
    if (iter != m_mapModel.end())
        return (iter->second)->Clone(pArg);

    return nullptr;
}

HRESULT CModel_Manager::Load_All_Models(const string& FilePath,_matrix PreMatrix)
{
    for (const auto& entry : fs::recursive_directory_iterator(FilePath))
    {
        if (entry.path().extension() == ".json")
        {
            string FullPath = entry.path().string();
            string Name = entry.path().stem().string();

            CModel* pInstance = CModel::Create(m_pDevice, m_pContext, PreMatrix, FullPath.c_str());
            if (!pInstance)
                return E_FAIL;

            m_mapModel.emplace(wstring(Name.begin(), Name.end()), pInstance);
        }
    }
    return S_OK;
}
