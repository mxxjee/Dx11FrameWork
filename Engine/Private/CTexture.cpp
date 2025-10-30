#include "CTexture.h"
#include "Engine_Define.h"
#include "CShader.h"


CTexture::CTexture(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice(_pDevice),m_pContext(_pContext)
{
}

CTexture::CTexture(const CTexture& Prototype)
	:m_iNumTextures(Prototype.m_iNumTextures),
	m_Textures(Prototype.m_Textures)
{
}




ComPtr<ID3D11ShaderResourceView> CTexture::Get_SRV(_uint iIdx)
{
	if (iIdx > m_Textures.size())
		return nullptr;

	return m_Textures[iIdx];
}

HRESULT CTexture::Add_SRV(ComPtr<ID3D11ShaderResourceView> pSRV)
{
	m_Textures.push_back(pSRV);
	m_iNumTextures = static_cast<_uint>(m_Textures.size());

	return S_OK;
}

HRESULT CTexture::Add_TextureToSRV(ComPtr<ID3D11Texture2D> pTex)
{
	if (!pTex)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC desc{};
	pTex->GetDesc(&desc);

	// ShaderResourceView 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;

	ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;
	HRESULT hr = m_pDevice->CreateShaderResourceView(pTex.Get(), &srvDesc, pSRV.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// 벡터에 저장
	m_Textures.push_back(pSRV);

	m_iNumTextures = static_cast<_uint>(m_Textures.size());
	return S_OK;
}

HRESULT CTexture::Set_SRV(ComPtr<ID3D11ShaderResourceView> pSRV, _uint iIdx)
{
	if (iIdx >= m_iNumTextures)
		return E_FAIL;

	m_Textures[iIdx] = pSRV;
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{

	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_Textures[iTextureIndex]);

	return S_OK;
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName,&m_Raws.front(),m_iNumTextures);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	if (m_iNumTextures == 1)
		return Load_Texture(pTextureFilePath);

	else
		return Load_TextureSequence(pTextureFilePath, iNumTextures);
	
	
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Load_TextureSequence(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	/* D:\Burger\156\Framework\Engine\Private\Test.jpg */

	_tchar szDrive[MAX_PATH] = {};		/* D:\ */
	_tchar szDir[MAX_PATH] = {};		 /* Burger\156\Framework\Engine\Private\ */
	_tchar szEXT[MAX_PATH] = {};		/* .jpg */

	//경로 분할 함수
	_wsplitpath_s(pTextureFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, szEXT, MAX_PATH);


	for (size_t i = 0; i < iNumTextures; ++i)
	{
		ComPtr<ID3D11ShaderResourceView> pSRV;
		_tchar szFilePath[MAX_PATH] = {};

		/*pTextureFilePath = L"Texture_%d.png";
		szFilePath[256];
		i = 3;

		szFilePath == L"Texture_3.png"
		*/
		wsprintf(szFilePath, pTextureFilePath, i);

		HRESULT hr = {};
		if (!lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice.Get(), szFilePath, nullptr, pSRV.GetAddressOf());

		else if (!lstrcmp(szEXT, TEXT(".tga")))
			return E_FAIL;

		else
			hr = CreateWICTextureFromFile(m_pDevice.Get(), szFilePath, nullptr, pSRV.GetAddressOf());


		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pSRV);
		m_Raws.push_back(pSRV.Get());
	}

	return S_OK;
}

HRESULT CTexture::Load_Texture(const _tchar* pTextureFilePath)
{

	_tchar szDrive[MAX_PATH] = {};		/* D:\ */
	_tchar szDir[MAX_PATH] = {};		 /* Burger\156\Framework\Engine\Private\ */
	_tchar szEXT[MAX_PATH] = {};		/* .jpg */


	//경로 분할 함수
	_wsplitpath_s(pTextureFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, szEXT, MAX_PATH);

	ComPtr<ID3D11ShaderResourceView> pSRV;

	HRESULT hr = {};
	if (!lstrcmp(szEXT, TEXT(".dds")))
		hr = CreateDDSTextureFromFile(m_pDevice.Get(), pTextureFilePath, nullptr, pSRV.GetAddressOf());

	else if (!lstrcmp(szEXT, TEXT(".tga")))
		return E_FAIL;

	else
		hr = CreateWICTextureFromFile(m_pDevice.Get(), pTextureFilePath, nullptr, pSRV.GetAddressOf());


	if (FAILED(hr))
		return E_FAIL;

	m_Textures.push_back(pSRV);
	m_Raws.push_back(pSRV.Get());
	
	return S_OK;
}

CTexture* CTexture::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{

		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CTexture* CTexture::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, ComPtr <ID3D11ShaderResourceView> pSRV)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);
	if (FAILED(pInstance->Add_SRV(pSRV)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CTexture* CTexture::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, ComPtr<ID3D11Texture2D> pTex)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);
	if (FAILED(pInstance->Add_TextureToSRV(pTex)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CTexture* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	m_Textures.clear();
}
