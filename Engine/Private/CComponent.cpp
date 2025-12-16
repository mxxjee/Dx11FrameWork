#include "CComponent.h"
#include "CGameInstance.h"

CComponent::CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice{pDevice},
	m_pContext{pContext},
	m_isClone{false},m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CComponent::CComponent(const CComponent& Prototype)
	:m_pDevice{Prototype.m_pDevice},
	m_pContext{Prototype.m_pContext},
	m_isClone{true},m_pGameInstance{Prototype.m_pGameInstance}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize_Copytype(void* pArg)
{
	COMPONENT_DESC* pDesc = static_cast<COMPONENT_DESC*>(pArg);
	m_pOwner = pDesc->pOwner;

	return S_OK;
}



void CComponent::Free()
{
	Safe_Release(m_pGameInstance);
}
