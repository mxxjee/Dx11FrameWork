#include "CMaterial_Manager.h"
#include "CMaterial.h"


CMaterial_Manager::CMaterial_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice),m_pContext(_pContext)
{
}

HRESULT CMaterial_Manager::Initialize()
{
    return S_OK;
}

HRESULT CMaterial_Manager::Register_Material(const _wstring& Tag, CMaterial* pInstance)
{
	CMaterial* pMaterial = Find_Material(Tag);
	if (pMaterial)
	{
#ifdef _DEBUG 
		//MSG_BOX("Material is alreday exist");
#endif
		return E_FAIL;
	}

	else
	{
		m_mapMat.emplace(Tag, pInstance);
		pInstance->Set_Name(Tag);
	}



    return S_OK;
}

CMaterial_Manager* CMaterial_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CMaterial_Manager* pInstance = new CMaterial_Manager(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CMaterial_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaterial_Manager::Free()
{
	for (auto& pair : m_mapMat)
	{
		Safe_Release(pair.second);

	}
}

CMaterial* CMaterial_Manager::Find_Material(const _wstring& Tag)
{
	auto iter = m_mapMat.find(Tag);

	if (iter != m_mapMat.end())
		return iter->second;


	return nullptr;
}
