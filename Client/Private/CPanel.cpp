#include "CPanel.h"

#include "CGameInstance.h"
#include "CTexture.h"
#include "CConstantBuffer.h"
#include "GeometryHelper.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CGeometry.h"
#include "CVertexShader.h"
#include "CPixelShader.h"
#include "CShader.h"
#include "CInputLayout.h"

#include "Client_Defines.h"




USING(Client)
CPanel::CPanel(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CUI(_pDevice,_pDeviceContext)
{
}
CPanel::CPanel(const CPanel& rhs)
	:CUI(rhs)
{
}
HRESULT CPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPanel::Initialize_Copytype(void* pArg)
{
	/*부모 컴포넌트 값세팅 */
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_Piepline()))
		return E_FAIL;



	PANEL_DESC* pPanel_desc = static_cast<PANEL_DESC*>(pArg);
	
	CreateGeometry();
	VertexShader();
	CreateInputLayout();
	PixelShader();

	CreateRasterizerState();
	CreateSamplerState();
	CreateBlendState();

	m_Pipeline.texture1->Create(pPanel_desc->ImgPath);
	m_Pipeline.constantBuffer->Create();



	return S_OK;
}

void CPanel::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	CheckNull(m_pTransformCom);


	m_transformData.matworld = m_pTransformCom->Get_World();
	m_Pipeline.constantBuffer->CopyData(m_transformData);

}

void CPanel::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CPanel::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CPanel::Render()
{
	__super::Render();

	Set_IA();
	Set_VS();
	Set_RS();
	Set_PS();
	Set_OM();


	return S_OK;
}

HRESULT CPanel::Initialize_Piepline()
{
	m_Pipeline.vertexBuffer = make_shared<CVertexBuffer>(m_pDevice);
	m_Pipeline.indexBuffer = make_shared<CIndexBuffer>(m_pDevice);
	m_Pipeline.inputLayout = make_shared<CInputLayout>(m_pDevice);
	m_Pipeline.geometry = make_shared <CGeometry<VertexTextureData>>();

	m_Pipeline.vertexShader = make_shared<CVertexShader>(m_pDevice);
	m_Pipeline.pixelShader = make_shared<CPixelShader>(m_pDevice);
	m_Pipeline.constantBuffer = make_shared<CConstantBuffer<TransformData>>(m_pDevice, m_pContext);

	m_Pipeline.texture1 = make_shared<CTexture>(m_pDevice);


	return S_OK;
}



void CPanel::CreateGeometry()
{
	GeometryHelper::CreateRectangle(m_Pipeline.geometry);

	m_Pipeline.vertexBuffer->Create(m_Pipeline.geometry->GetVertices());
	m_Pipeline.indexBuffer->Create(m_Pipeline.geometry->GetIndices());
}

void CPanel::CreateInputLayout()
{
	m_Pipeline.inputLayout->Create(VertexTextureData::desc, m_Pipeline.vertexShader->GetBlob());
}

void CPanel::VertexShader()
{
	m_Pipeline.vertexShader->Create(L"../../EngineSDK/inc/Default.hlsl", "main", "vs_5_0");

}

void CPanel::PixelShader()
{
	m_Pipeline.pixelShader->Create(L"../../EngineSDK/inc/Default.hlsl", "PS", "ps_5_0");

}

HRESULT CPanel::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.FillMode = D3D11_FILL_SOLID;//WIREFRAME of SOLID
	desc.CullMode = D3D11_CULL_BACK;//CULLMODE: 반시계 컬링
	desc.FrontCounterClockwise = false;

	HRESULT hr = m_pDevice->CreateRasterizerState(&desc, m_Pipeline._rasterizerState.GetAddressOf());
	CHECK(hr);

	return S_OK;
}

HRESULT CPanel::CreateSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	//RGBA (빨간색)
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;


	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;

	m_pDevice->CreateSamplerState(&desc, m_Pipeline._samplerState.GetAddressOf());

	return S_OK;
}

HRESULT CPanel::CreateBlendState()
{

	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = m_pDevice->CreateBlendState(&desc, m_Pipeline._BlendState.GetAddressOf());
	CHECK(hr);
	return S_OK;
}

void CPanel::Set_IA()
{
	UINT32 stride = sizeof(VertexTextureData);
	UINT32 offset = 0;

	m_pContext.Get()->IASetVertexBuffers(0, 1, m_Pipeline.vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	m_pContext.Get()->IASetIndexBuffer(m_Pipeline.indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
	m_pContext.Get()->IASetInputLayout(m_Pipeline.inputLayout->Get_ComPtr().Get());
	m_pContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void CPanel::Set_VS()
{
	m_pContext.Get()->VSSetShader(m_Pipeline.vertexShader->GetComPtr().Get(), nullptr, 0);

	ID3D11Buffer* buffer = m_Pipeline.constantBuffer->GetComPtr().Get();
	m_pContext.Get()->VSSetConstantBuffers(0, 1, &buffer);

}

void CPanel::Set_RS()
{
	m_pContext.Get()->RSSetState(m_Pipeline._rasterizerState.Get());

}

void CPanel::Set_PS()
{
	m_pContext.Get()->PSSetShader(m_Pipeline.pixelShader->GetComPtr().Get(), nullptr, 0);
	m_pContext.Get()->PSSetShaderResources(0, 1, m_Pipeline.texture1->GetComPtr().GetAddressOf());
	m_pContext.Get()->PSSetSamplers(0, 1, m_Pipeline._samplerState.GetAddressOf());

}

void CPanel::Set_OM()
{
	m_pContext.Get()->OMSetBlendState(m_Pipeline._BlendState.Get(), nullptr, 0xFFFFFFFF);
	m_pContext.Get()->DrawIndexed(m_Pipeline.geometry->GetIndexCount(), 0, 0);

}

CPanel* CPanel::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CPanel* pInstance = new CPanel(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CPanel ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CPanel::Clone(void* pArg)
{
	CPanel* pInstance = new CPanel(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CPanel ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CPanel::Free()
{
	__super::Free();
	
}
