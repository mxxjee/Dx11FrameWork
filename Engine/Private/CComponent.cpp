#include "CComponent.h"

CComponent::CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice{pDevice},
	m_pContext{pContext}
{
}

CComponent::CComponent(const CComponent& Prototype)
	:m_pDevice{Prototype.m_pDevice},
	m_pContext{Prototype.m_pContext}
{
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize_Copytype(void* pArg)
{
	return S_OK;
}


void CComponent::Free()
{
	
}
