#include "CTexture_Manager.h"
#include "CTexture.h"

CTexture_Manager::CTexture_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice(_pDevice),m_pContext(_pContext)
{
}

HRESULT CTexture_Manager::Initialize()
{
	//임의의 흰색 texutre를 만들어서 저장하자
	ComPtr<ID3D11Texture2D> pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc = {};
	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;


	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	_uint* pInitialPixels = new _uint[TextureDesc.Width * TextureDesc.Height];
	for(int i=0;i< TextureDesc.Width * TextureDesc.Height;++i)
		pInitialPixels[i]= D3DCOLOR_ARGB(255, 255, 255, 255);

	D3D11_SUBRESOURCE_DATA		InitialData{};
	InitialData.pSysMem = pInitialPixels;
	InitialData.SysMemPitch = sizeof(_uint) * TextureDesc.Width;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;


	CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, pTexture2D);
	if (pTexture)
		m_mapTex.emplace(L"Default", pTexture);




	///페이드스크린 전용 검은색이미지
	ComPtr<ID3D11Texture2D> pTextureBlack2D = { nullptr };
	for (int i = 0; i < TextureDesc.Width * TextureDesc.Height; ++i)
		pInitialPixels[i] = D3DCOLOR_ARGB(255, 0, 0, 0);

	InitialData.pSysMem = pInitialPixels;
	InitialData.SysMemPitch = sizeof(_uint) * TextureDesc.Width;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTextureBlack2D)))
		return E_FAIL;

	pTexture = CTexture::Create(m_pDevice, m_pContext, pTextureBlack2D);
	if (pTexture)
		m_mapTex.emplace(L"Black", pTexture);

	Safe_Delete_Array(pInitialPixels);
	return S_OK;
}

HRESULT CTexture_Manager::Register_Texture(const _wstring& Tag, CTexture* pInstance)
{
	CTexture* pTex = Find_Texture(Tag);
	if (pTex)
	{
		//MSG_BOX("Texture is alreday exist");
		return E_FAIL;
	}

	else
	{
		m_mapTex.emplace(Tag, pInstance);
	}


	return S_OK;
}

HRESULT CTexture_Manager::Load_Textures(const _wstring& FolderPath, const _wstring& Extension)
{
	for (const auto& entry : fs::recursive_directory_iterator(FolderPath))
	{
		if (entry.path().extension() == Extension)
		{
			string FullPath = entry.path().string();
			string Name = entry.path().stem().string();

			CTexture* pInstance = CTexture::Create(m_pDevice, m_pContext, entry.path().wstring().c_str(),1);
			if (!pInstance)
				return E_FAIL;

			m_mapTex.emplace(wstring(Name.begin(), Name.end()), pInstance);
		}
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

