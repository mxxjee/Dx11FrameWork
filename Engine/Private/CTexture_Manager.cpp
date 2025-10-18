#include "CTexture_Manager.h"
#include "CTexture.h"

CTexture_Manager::CTexture_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice(_pDevice),m_pContext(_pContext)
{
}

HRESULT CTexture_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTexture_Manager::Register_Texture(const _wstring& Tag, CTexture* pInstance)
{
	CTexture* pTex = Find_Texture(Tag);
	if (pTex)
	{
		MSG_BOX("Texture is alreday exist");
		return E_FAIL;
	}

	else
	{
		m_mapTex.emplace(Tag, pInstance);
	}


	return S_OK;
}

CTexture_Manager* CTexture_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CTexture_Manager* pInstance = new CTexture_Manager(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CTexture_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture_Manager::Free()
{
	for (auto& pair : m_mapTex)
	{
		Safe_Release(pair.second);

	}
}

CTexture* CTexture_Manager::Find_Texture(const _wstring& Tag)
{

	auto iter = m_mapTex.find(Tag);

	if (iter != m_mapTex.end())
		return iter->second;


	return nullptr;
}

