#include "CRenderer.h"
#include "CGameObject.h"
#include "CGameInstance.h"
#include "CUI.h"
#include "CMainCamera.h"
#include "CUICamera.h"
#include "CCameraComponent.h"



CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT CRenderer::Initialize(_uint RenderGroupCount)
{
	m_RenderMaxCount = RenderGroupCount;

	m_RenderGroups.resize(m_RenderMaxCount);
	m_SortFuncMap.resize(m_RenderMaxCount);

	return S_OK;
}


HRESULT CRenderer::Add_RenderObject(_uint eID, CGameObject* pRenderObject)
{
	if (eID > m_RenderMaxCount)
		return E_FAIL;

	auto& iter = m_RenderGroups[eID];
	iter.push_back(pRenderObject);
	
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Add_SortFunc(_uint eID, function<bool(CGameObject*, CGameObject*)> _Fun)
{
	m_SortFuncMap[eID] = _Fun;
	return S_OK;
}

void CRenderer::Draw()
{

	//Render_Priority();
	//Render_NonBlend();
	//Render_Blend();
	//Render_UI();
}

void CRenderer::Render_Group(_uint eType)
{
	BindRenderState(eType);
	SortByDepth(eType);
	RenderGroupObjects(eType);
}



void CRenderer::BindRenderState(_uint eGroup)
{
	RenderStates Renderstate = CGameInstance::GetInstance()->Get_RenderStates(eGroup);


	m_pContext->RSSetState(Renderstate._rasterizerState.Get());
	m_pContext->PSSetSamplers(0, 1, Renderstate._samplerState.GetAddressOf());
	m_pContext->OMSetBlendState(Renderstate._BlendState.Get(), nullptr, 0xFFFFFFFF);
	m_pContext->OMSetDepthStencilState(Renderstate._DepthStencilState.Get(), 0);

}

void CRenderer::SortByDepth(_uint eGroup)
{
	if (m_SortFuncMap[eGroup])
		m_RenderGroups[eGroup].sort(m_SortFuncMap[eGroup]);


}

void CRenderer::RenderGroupObjects(_uint eGroup)
{
	auto iter = m_RenderGroups[eGroup];

	for (auto& i : iter)
	{
		if (nullptr != i)
			i->Render();
	}

}



void CRenderer::Clear_RenderGroups()
{
	for (int i = 0; i < m_RenderMaxCount; ++i)
	{
		list<CGameObject*>& ObjList = m_RenderGroups[i];
		for (auto& i : ObjList)
			Safe_Release(i);

		ObjList.clear();

	}
}


CRenderer* CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);
	
	return pInstance;

}

void CRenderer::Free()
{
	__super::Free();

	for (int i = 0; i < m_RenderMaxCount; ++i)
	{
		list<CGameObject*>& ObjList = m_RenderGroups[i];
		for (auto& i : ObjList)
			Safe_Release(i);

		ObjList.clear();

	}


}
