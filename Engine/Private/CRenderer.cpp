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
	m_EngineDesc = CGameInstance::GetInstance()->Get_EngineDesc();

	m_RenderMaxCount = RenderGroupCount;

	m_RenderGroups.resize(m_RenderMaxCount);
	m_SortFuncTable.resize(m_RenderMaxCount);

	//각 렌더타겟들생성
	_uint		iNumViewports = { 1 };
	D3D11_VIEWPORT ViewPort = {};
	m_pContext->RSGetViewports(&iNumViewports, &ViewPort);


	/*Diffuse기록할 렌더타겟*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewPort.Width, (_uint)ViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Normal 기록할 렌더타겟*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewPort.Width, (_uint)ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*깊이를 기록하는 렌더타겟*/																		//near~Far사이의 값을 저장해야하므로,타입변경
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewPort.Width, (_uint)ViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/*Shade렌더타겟*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewPort.Width, (_uint)ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*specular 렌더타겟 - light계산이후의 specular을 기록*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;



	/*MRT , 렌더타겟 여러개를 묶는다.*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Depth"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPort.Width, ViewPort.Height, 1.f));
	m_WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f));


	//후처리 보여줄 Quad
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	CheckNullResult(m_pVIBuffer, E_FAIL);

	m_pShader = CShader::Create(m_pDevice,
		m_pContext, VTXPOSTEX::desc, L"../../Resource/Shader/Shader_Deferred.hlsl",
		"DefaultTechnique");
	m_pGameInstance->Register_Shader(L"Deferred", m_pShader);

	CheckNullResult(m_pVIBuffer, E_FAIL);

	
#ifdef _DEBUG
	if (m_EngineDesc.eEngineMode == EngineMode::CLIENT)
	{
		if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.f, 150.f, 300.f, 300.f)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.f, 450.f, 300.f, 300.f)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.f, 150.f, 300.f, 300.f)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 450.f, 450.f, 300.f, 300.f)))
			return E_FAIL;
	}
	

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

HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);


	return S_OK;
}
#endif

void CRenderer::Bind_And_Render_Lights()
{
	ID3D11ShaderResourceView* nullSRVs[16] = { nullptr };
	m_pContext->VSSetShaderResources(0, 16, nullSRVs); // Vertex Shader
	m_pContext->PSSetShaderResources(0, 16, nullSRVs); // Pixel Shader
	// 필요하다면 GS, DS, HS 등 다른 쉐이더 단계도 추가

	// 2. 모든 Unordered Access View (UAV) 해제 (Compute Shader를 쓴다면 필요)
	// m_pContext->CSSetUnorderedAccessViews(0, 1, nullptr, nullptr); 

	// 3. 모든 Render Target View (RTV) 및 Depth Stencil View (DSV) 해제
	//    (Begin_MRT가 처리하지만, 명시적으로 비우는 것도 안전합니다.)



	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;


	Bind_Rect_Matricies();

	


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", *m_pGameInstance->Get_InverseTransform(ENUM_TO_UINT(CAMERA_TYPE::TARGET),D3DTS::VIEW))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", *m_pGameInstance->Get_InverseTransform(ENUM_TO_UINT(CAMERA_TYPE::TARGET), D3DTS::PROJ))))
		return;

	if (FAILED(m_pShader->Bind_Vector("g_MainCamPosition", m_pGameInstance->Get_CamPosition(ENUM_TO_UINT(CAMERA_TYPE::TARGET)))))
		return;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;


	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	

	//라이트 값 바인딩.
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

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;

	if (FAILED(m_pShader->Begin("Combined")))
		return;

	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return;

	if (FAILED(m_pVIBuffer->Render()))
		return;

}

