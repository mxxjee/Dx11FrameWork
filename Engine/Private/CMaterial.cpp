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

HRESULT CMaterial::Initialize_Prototype(const char* BasePath, map<MaterialMapType, string>& _TextureData)
{

	if(FAILED(Register_MaterialTexture(BasePath, _TextureData)))	//먼저 mateiral에 쓸 텍스처들을 준비,등록
		return E_FAIL;

	
	return S_OK;
}

HRESULT CMaterial::Register_MaterialTexture(const char* BasePath, map<MaterialMapType, string>& _TextureData)
{
	// 이름에 맞는 texture생성
	wstring wBasePath = wstring(BasePath, BasePath + strlen(BasePath));

	wstring MaterialTexName[ENUM_TO_UINT(MaterialMapType::END)];
	CTexture* pTextures[ENUM_TO_UINT(MaterialMapType::END)] = { nullptr };

	//경로와 이름을 통해서 텍스처 매니저에 텍스처를 등록한다.
	for (int i = 0; i < ENUM_TO_UINT(MaterialMapType::END); ++i)
	{
		auto it = _TextureData.find(MaterialMapType(i));
		if (it == _TextureData.end() || it->second.empty())
			continue;

		MaterialTexName[i] = wstring(_TextureData[(MaterialMapType)i].begin(), _TextureData[(MaterialMapType)i].end());
		
		pTextures[i] = m_pGameInstance->Find_Texture(MaterialTexName[i]);

		//못찾앗을때만 . 생성
		if (!pTextures[i])
		{
			pTextures[i] = CTexture::Create(m_pDevice, m_pContext, wstring(wBasePath + MaterialTexName[i]).c_str());
			if (FAILED(m_pGameInstance->Register_Texture(MaterialTexName[i], pTextures[i])))
			{
				Safe_Release(pTextures[i]);
				continue;
			}
		}
	
	
	}


	///material data에 이를 채워준다.
	for (int i = 0; i < ENUM_TO_UINT(MaterialMapType::END); ++i)
	{
		CTexture* pFindTex = m_pGameInstance->Find_Texture(MaterialTexName[i]);
		if(pFindTex)
			m_MatData.m_Textures.emplace(MaterialMapType(i), pFindTex);
	}

	
	for (auto& Tex : m_MatData.m_Textures)
		Safe_AddRef(Tex.second);
	return S_OK;
}

CMaterial* CMaterial::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const char* BasePath, map<MaterialMapType, string>& _TextureData)
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

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const string& Variable, MaterialMapType eType)
{
	CheckNullResult(pShader, E_FAIL);
	
	auto it = m_MatData.m_Textures.find(eType);
	if (it == m_MatData.m_Textures.end())
		return E_FAIL;

	CTexture* pTex = m_MatData.m_Textures[eType];
	CheckNullResult(pTex, E_FAIL);

	ComPtr<ID3D11ShaderResourceView> SRV = pTex->Get_SRV(0);
	CheckNullResult(SRV.Get(), E_FAIL);

	return pShader->Bind_SRV(Variable, SRV);
}

