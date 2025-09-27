#include "CRenderer.h"
#include "CGameObject.h"

CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
	for (size_t i = 0; i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
		m_RenderObjects[i].clear();
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eID, CGameObject* pRenderObject)
{
	if (eID >= RENDERGROUP::END)
		return E_FAIL;


	m_RenderObjects[ENUM_TO_UINT(eID)].push_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

void CRenderer::Draw()
{
	Render_Priority();
	Render_NonBlend();
	Render_Blend();
	Render_UI();
}

void CRenderer::Render_Priority()
{
	
	for (auto& i : m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::PRIORITY)])
	{
		if(nullptr!=i)
			i->Render();

		Safe_Release(i);
	}

	//매프레임이후 삭제
	m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::PRIORITY)].clear();
}

void CRenderer::Render_NonBlend()
{
	for (auto& i : m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::NONALPHA)])
	{
		if (nullptr != i)
			i->Render();

		Safe_Release(i);
	}

	//매프레임이후 삭제
	m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::NONALPHA)].clear();
}

void CRenderer::Render_Blend()
{
	for (auto& i : m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::ALPHA)])
	{
		if (nullptr != i)
			i->Render();

		Safe_Release(i);
	}

	//매프레임이후 삭제
	m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::ALPHA)].clear();
}

void CRenderer::Render_UI()
{
	for (auto& i : m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::UI)])
	{
		if (nullptr != i)
			i->Render();

		Safe_Release(i);
	}

	//매프레임이후 삭제
	m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::UI)].clear();
}

CRenderer* CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create :CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pObj : RenderObjects)
		{
			Safe_Release(pObj);

		}

		RenderObjects.clear();
	}


}
