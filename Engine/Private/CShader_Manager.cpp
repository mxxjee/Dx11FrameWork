#include "CShader_Manager.h"
#include "CShader.h"

CShader_Manager::CShader_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice{ _pDevice }, m_pContext{ _pContext }
{
}

HRESULT CShader_Manager::Initialize()
{
    return S_OK;
}



HRESULT CShader_Manager::Register_Shader(const _wstring& Tag, CShader* pInstance)
{
	CShader* pShader = Find_Shader(Tag);
	if (pShader)
	{
		MSG_BOX("Shader is alreday exist");
		return E_FAIL;
	}

	else
	{
		m_mapShader.emplace(Tag, pInstance);
	}
	

	return S_OK;
}

CShader_Manager* CShader_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CShader_Manager* pInstance = new CShader_Manager(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CShader_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CShader_Manager::Free()
{
	for (auto& pair : m_mapShader)
	{
		Safe_Release(pair.second);

	}

}

CShader* CShader_Manager::Find_Shader(const _wstring& Tag)
{
	auto iter = m_mapShader.find(Tag);

	if (iter != m_mapShader.end())
		return iter->second;


    return nullptr;
}
