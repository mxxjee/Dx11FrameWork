#include "CRenderer.h"
#include "CGameObject.h"
#include "CGameInstance.h"
#include "CUI.h"
#include "CMainCamera.h"
#include "CUICamera.h"


CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
	for (size_t i = 0; i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
		m_RenderObjects[i].clear();
}

HRESULT CRenderer::Initialize()
{
	CreateSamplerStates();
	CreateBlendStates();
	CreateRasterizerStates();
	CreateDepthStencilStates();

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
	Bind_PerspectiveCamera();
	Render_Priority();
	Render_NonBlend();
	Render_Blend();

	Bind_OrthoCamera();
	Render_UI();
}

void CRenderer::Render_Priority()
{
	CheckTrue(m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::PRIORITY)].empty());
	
	m_pContext->RSSetState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._rasterizerState.Get());
	m_pContext->PSSetSamplers(0,1,m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._samplerState.GetAddressOf());
	m_pContext->OMSetBlendState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._BlendState.Get(),nullptr,0xFFFFFFFF);
	m_pContext->OMSetDepthStencilState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._DepthStencilState.Get(), 0);

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
	CheckTrue(m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::NONALPHA)].empty());


	m_pContext->RSSetState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._rasterizerState.Get());
	m_pContext->PSSetSamplers(0, 1, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._samplerState.GetAddressOf());
	m_pContext->OMSetBlendState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._BlendState.Get(), nullptr, 0xFFFFFFFF);
	m_pContext->OMSetDepthStencilState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._DepthStencilState.Get(), 0);

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
	CheckTrue(m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::ALPHA)].empty());



	m_pContext->RSSetState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._rasterizerState.Get());
	m_pContext->PSSetSamplers(0, 1, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._samplerState.GetAddressOf());
	m_pContext->OMSetBlendState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._BlendState.Get(), nullptr, 0xFFFFFFFF);
	m_pContext->OMSetDepthStencilState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._DepthStencilState.Get(), 0);

	
	m_MainCameraView = XMLoadFloat4x4(&CGameInstance::GetInstance()->GetViewMatrix(false));

	m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::ALPHA)].sort([&](CGameObject* a, CGameObject* b)
		{
			CTransform* aTrans = dynamic_cast<CTransform*>(a->Get_Component(L"Transform"));
			CTransform* bTrans = dynamic_cast<CTransform*>(b->Get_Component(L"Transform"));

			_vector aView=XMVector3TransformCoord(aTrans->Get_State(STATE::POSITION, TransformScope::WORLD),m_MainCameraView);
			_vector bView = XMVector3TransformCoord(bTrans->Get_State(STATE::POSITION, TransformScope::WORLD), m_MainCameraView);

			//내림차순정렬, 먼것부터 그려야함
			return XMVectorGetZ(aView) > XMVectorGetZ(bView);
		});



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
	CheckTrue(m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::UI)].empty());


	m_pContext->RSSetState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._rasterizerState.Get());
	m_pContext->PSSetSamplers(0, 1, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._samplerState.GetAddressOf());
	m_pContext->OMSetBlendState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._BlendState.Get(), nullptr, 0xFFFFFFFF);
	m_pContext->OMSetDepthStencilState(m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._DepthStencilState.Get(), 0);

	m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::UI)].sort([&](CGameObject* a, CGameObject* b)
		{
			CUI* pUI_A = dynamic_cast<CUI*>(a);
			CUI* pUI_B = dynamic_cast<CUI*>(b);

			if (a && b)
			{
				return pUI_A->Get_Depth() > pUI_B->Get_Depth();
			}
		});

	for (auto& i : m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::UI)])
	{
		if (nullptr != i)
			i->Render();

		Safe_Release(i);
	}

	//매프레임이후 삭제
	m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::UI)].clear();
}

void CRenderer::Bind_PerspectiveCamera()
{
	CGameObject* pMainCam = CGameInstance::GetInstance()->GetMainPerspectiveCamera();
	if (pMainCam)
	{
		CMainCamera* ppMainCam = dynamic_cast<CMainCamera*>(pMainCam);
		if (ppMainCam)
			ppMainCam->Bind_ViewProjMatrix();
	}
}

void CRenderer::Bind_OrthoCamera()
{
	CGameObject* pMainCam = CGameInstance::GetInstance()->GetMainOrthoCamera();
	if (pMainCam)
	{
		CUICamera* ppMainCam = dynamic_cast<CUICamera*>(pMainCam);
		if (ppMainCam)
			ppMainCam->Bind_ViewProjMatrix();
	}
}

void CRenderer::CreateSamplerStates()
{
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 0.0f;


	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;


	m_pDevice->CreateSamplerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._samplerState.GetAddressOf());
	m_pDevice->CreateSamplerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._samplerState.GetAddressOf());
	m_pDevice->CreateSamplerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._samplerState.GetAddressOf());
	m_pDevice->CreateSamplerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._samplerState.GetAddressOf());


}

void CRenderer::CreateBlendStates()
{
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = FALSE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pDevice->CreateBlendState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._BlendState.GetAddressOf());
	m_pDevice->CreateBlendState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._BlendState.GetAddressOf());
	
	desc.RenderTarget[0].BlendEnable = TRUE;

	m_pDevice->CreateBlendState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._BlendState.GetAddressOf());
	m_pDevice->CreateBlendState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._BlendState.GetAddressOf());

}

void CRenderer::CreateRasterizerStates()
{
	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.FillMode = D3D11_FILL_SOLID;//WIREFRAME of SOLID
	desc.CullMode = D3D11_CULL_NONE;//CULLMODE: 반시계 컬링
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;

	m_pDevice->CreateRasterizerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._rasterizerState.GetAddressOf());
	m_pDevice->CreateRasterizerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._rasterizerState.GetAddressOf());
	m_pDevice->CreateRasterizerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._rasterizerState.GetAddressOf());
	m_pDevice->CreateRasterizerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._rasterizerState.GetAddressOf());

}

void CRenderer::CreateDepthStencilStates()
{
	
	D3D11_DEPTH_STENCIL_DESC desc= CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);

	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._DepthStencilState.GetAddressOf());
	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._DepthStencilState.GetAddressOf());
	
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._DepthStencilState.GetAddressOf());
	
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._DepthStencilState.GetAddressOf());

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
