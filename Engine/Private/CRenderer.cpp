#include "CRenderer.h"
#include "CGameObject.h"
#include "CGameInstance.h"
#include "CUI.h"
#include "CMainCamera.h"
#include "CUICamera.h"
#include "CCameraComponent.h"
#include "CNavMesh_Manager.h"
#include "CVIBuffer_Rect.h"
#include "CShader.h"

#include "CComponent.h"	



CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CRenderer::Initialize(_uint RenderGroupCount)
{
	m_RenderMaxCount = RenderGroupCount;

	m_RenderGroups.resize(m_RenderMaxCount);
	m_SortFuncTable.resize(m_RenderMaxCount);

	//°¢ ·»´õÅ¸°Ùµé»ý¼º
	_uint		iNumViewports = { 1 };
	D3D11_VIEWPORT ViewPort = {};
	m_pContext->RSGetViewports(&iNumViewports, &ViewPort);


	/*Diffuse±â·ÏÇÒ ·»´õÅ¸°Ù*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Normal ±â·ÏÇÒ ·»´õÅ¸°Ù*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Shade·»´õÅ¸°Ù*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;



	/*MRT , ·»´õÅ¸°Ù ¿©·¯°³¸¦ ¹­´Â´Ù.*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;



	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPort.Width, ViewPort.Height, 1.f));
	//m_WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f));


	//ÈÄÃ³¸® º¸¿©ÁÙ Quad
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	CheckNullResult(m_pVIBuffer, E_FAIL);

	m_pShader = CShader::Create(m_pDevice,
		m_pContext, VTXPOSTEX::desc, L"../../Resource/Shader/Shader_Deferred.hlsl",
		"DefaultTechnique");
	m_pGameInstance->Register_Shader(L"Deferred", m_pShader);

	CheckNullResult(m_pVIBuffer, E_FAIL);

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.f, 150.f, 300.f, 300.f)))
		return E_FAIL;

#endif // _DEBUG


	return S_OK;
}


HRESULT CRenderer::Add_RenderObject(_uint eID, CGameObject* pRenderObject)
{
	if (eID > (_uint)m_RenderMaxCount)
		return E_FAIL;

	auto& iter = m_RenderGroups[eID];
	iter.push_back(pRenderObject);
	
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Add_SortFunc(_uint eID, function<bool(CGameObject*, CGameObject*)> _Fun)
{
	if (eID >= (_uint)m_RenderMaxCount|| !_Fun)
		return E_FAIL;


	m_SortFuncTable[eID] = _Fun;
	return S_OK;
}

void CRenderer::Draw()
{

	//Render_Priority();
	//Render_Non
	// ();
	//Render_Blend();
	//Render_UI();
}

void CRenderer::Render_Group(_uint eType)
{
	SortByDepth(eType);
	RenderGroupObjects(eType);

#ifdef _DEBUG
	if(eType==1)
		m_pGameInstance->Render_NavMeshManager();
#endif
}



void CRenderer::BindRenderState(_uint eGroup)
{
	RenderStates Renderstate = CGameInstance::GetInstance()->Get_RenderStates(eGroup);
	
	m_pGameInstance->Bind_SamplerState(eGroup);
	m_pContext->RSSetState(Renderstate._rasterizerState.Get());
	m_pContext->OMSetBlendState(Renderstate._BlendState.Get(), nullptr, 0xFFFFFFFF);
	m_pContext->OMSetDepthStencilState(Renderstate._DepthStencilState.Get(), 0);

}

void CRenderer::SortByDepth(_uint eGroup)
{
	if (m_SortFuncTable[eGroup])
		m_RenderGroups[eGroup].sort(m_SortFuncTable[eGroup]);


}

void CRenderer::RenderGroupObjects(_uint eGroup)
{
	auto iter = m_RenderGroups[eGroup];

	for (auto& i : iter)
	{
		if (nullptr != i)
			i->Render();
	}

	/*if (eGroup == 0)
	{
		wchar_t buffer[128];
		swprintf_s(buffer, L"Priority: %d\n", iter.size());

		OutputDebugStri
		 ng(buffer);
	}*/
		
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

	Safe_Release(m_pVIBuffer);

	for (int i = 0; i < m_RenderMaxCount; ++i)
	{
		list<CGameObject*>& ObjList = m_RenderGroups[i];
		for (auto& i : ObjList)
			Safe_Release(i);

		ObjList.clear();

	}


}

#ifdef _DEBUG
void CRenderer::Render_Debug()
{
	CheckFalse(CGameInstance::m_bDrawDebug);

	for (auto& pComp : m_DebugComponents)
		if (nullptr != pComp)
		{
			pComp->Render();
			Safe_Release(pComp);
		}

	m_DebugComponents.clear();

	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return;


	/*if (FAILED(m_pShader->Bind_Matrix("g_ViewProjMatrix", m_ViewProjMatrix)))
		return;*/

	if (FAILED(m_pGameInstance->Debug_RT_Render(TEXT("MRT_GameObject"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Debug_RT_Render(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return;


}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);


	return S_OK;
}
#endif

#endif

void CRenderer::Bind_And_Render_Lights()
{
	

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;


	Bind_Rect_Matricies();


	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	//¶óÀÌÆ® °ª ¹ÙÀÎµù.
	m_pGameInstance->Render_LightManager(m_pShader, m_pVIBuffer);


}

void CRenderer::Bind_Rect_Matricies()
{

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return;
}

void CRenderer::Render_Combined()
{
	Bind_Rect_Matricies();

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;

	if (FAILED(m_pShader->Begin("Combined")))
		return;

	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return;

	if (FAILED(m_pVIBuffer->Render()))
		return;

}

