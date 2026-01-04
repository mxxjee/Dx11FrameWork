#include "CMaterial.h"
#include "CTexture_Manager.h"
#include "CTexture.h"
#include "CGameInstance.h"
#include "CShader.h"

CMaterial::CMaterial(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice(_pDevice),m_pContext(_pContext), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);

}

CMaterial::CMaterial(const CMaterial& Prototype)
	:m_pGameInstance(Prototype.m_pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMaterial::Initialize_Prototype(const char* BasePath, map<aiTextureType, string>& _TextureData)
{

	if(FAILED(Register_MaterialTexture(BasePath, _TextureData)))	//먼저 mateiral에 쓸 텍스처들을 준비,등록
		return E_FAIL;

	
	return S_OK;
}

HRESULT CMaterial::Register_MaterialTexture(const char* BasePath, map<aiTextureType, string>& _TextureData)
{
	// 이름에 맞는 texture생성
	wstring wBasePath = wstring(BasePath, BasePath + strlen(BasePath));


	//경로와 이름을 통해서 텍스처 매니저에 텍스처를 등록한다.
	for (int i = 0; i <AI_TEXTURE_TYPE_MAX; ++i)
	{

		auto it = _TextureData.find(aiTextureType(i));
		if (it == _TextureData.end() || it->second.empty())
			continue;


		fs::path path = BasePath;
		
		wstring TexName = wstring(it->second.begin(), it->second.end());
		fs::path TexPath = TexName;

		TexName=TexPath.stem().wstring()+L".dds";

		wstring TexKey= path.parent_path().parent_path().filename().wstring() + L"/" + TexName;
		
		
		CTexture* pTex = m_pGameInstance->Find_Texture(TexKey);

		//못찾앗을때만 . 생성
		if (!pTex)
		{
			pTex = CTexture::Create(m_pDevice, m_pContext, wstring(path .wstring()+TexName).c_str());
			if (FAILED(m_pGameInstance->Register_Texture(TexKey, pTex)))
			{
				Safe_Release(pTex);
				continue;
			}
		}
	
		m_MatData.m_Textures.emplace(static_cast<aiTextureType>(i), pTex);

	
	}


	///레퍼런스관리
	for (auto& Tex : m_MatData.m_Textures)
		Safe_AddRef(Tex.second);
	return S_OK;
}

CMaterial* CMaterial::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const char* BasePath, map<aiTextureType, string>& _TextureData)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(BasePath,_TextureData)))
	{
		MSG_BOX("Failed to Created : v");
		Safe_Release(pInstance);

	}

	return pInstance;
}

void CMaterial::Free()
{
	__super::Free();
	for (auto& pair : m_MatData.m_Textures)
	{
		if (pair.second)
			Safe_Release(pair.second);
	}

	Safe_Release(m_pGameInstance);
}

CTexture* CMaterial::Get_Texture(aiTextureType eType)
{

	return m_MatData.Get_Texture(eType);
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const string& Variable, aiTextureType eType, int idx )
{
	CheckNullResult(pShader, E_FAIL);
	
	auto it = m_MatData.m_Textures.find(eType);
	if (it == m_MatData.m_Textures.end())
	{

		//1.1.1.1 사진 떤져주기
		CTexture* pDefaulTex = m_pGameInstance->Find_Texture(L"Default");
		if (pDefaulTex)
		{
			ComPtr<ID3D11ShaderResourceView> pSRV = pDefaulTex->Get_SRV(idx);
			return pShader->Bind_SRV(Variable,pSRV );

		}
	}

	else
	{
		CTexture* pTex = m_MatData.m_Textures[eType];
		CheckNullResult(pTex, E_FAIL);

		ComPtr<ID3D11ShaderResourceView> SRV = pTex->Get_SRV(0);
		CheckNullResult(SRV.Get(), E_FAIL);

		return pShader->Bind_SRV(Variable, SRV);
	}
	
	return S_OK;
}

