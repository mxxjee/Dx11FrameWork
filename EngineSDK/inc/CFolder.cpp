#pragma once
#include "CFolder.h"
#include "CGameInstance.h"
#include "CTexture.h"

HRESULT CFolder::Initialize(void* pArg)
{
	tagFolderDesc* pDesc = static_cast<tagFolderDesc*>(pArg);
	m_pGameInstance = CGameInstance::GetInstance();

	//폴더아이콘찾기
	m_pTexture = m_pGameInstance->Find_Texture(L"Folder");
	m_iIdx = iIdx;

	//폴더이름설정
	strcpy_s(m_szName, MAX_PATH, pDesc->Name);

	m_Category = pArg->Category;


	//사이즈 설정
	m_iSize = pDesc->Size;

	Safe_AddRef(m_pGameInstance);
}


bool CFolder::Update()
{
	//폴더 아이콘표시
	if (ImGui::ImageButton(m_szName, m_pTexture->Get_SRV(0).Get(), m_iSize))
	{
		return true;
	}
}

void CFolder::Render()
{
}


void CFolder::Show_Folder()
{

}

CFolder* CFolder::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	CFolder* pInstance = new CFolder(pDevice,pContext);
	if (pInstance->Initialize(pArg))
	{
		MSG_BOX("Failed to Create:CFolder");
		Safe_Release(pInstance);
	
	}

	return pInstance;
}

void CFolder::Free()
{
	Safe_Release(m_pGameInstance);
}
