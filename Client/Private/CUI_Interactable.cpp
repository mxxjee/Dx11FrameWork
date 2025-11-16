#include "CUI_Interactable.h"

USING(Client)
CUI_Interactable::CUI_Interactable(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
}

CUI_Interactable::CUI_Interactable(const CUI_Interactable& rhs)
{
}

HRESULT CUI_Interactable::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CUI_Interactable::Initialize_Copytype(void* pArg)
{
	return E_NOTIMPL;
}

void CUI_Interactable::Update_Priority(_float fTimeDelta)
{
}

void CUI_Interactable::Update(_float fTimeDelta)
{
}

void CUI_Interactable::Update_Late(_float fTimeDelta)
{
}

void CUI_Interactable::Update_Render(_float fTimeDelta)
{
}

HRESULT CUI_Interactable::Render()
{
	return E_NOTIMPL;
}

CUI_Interactable* CUI_Interactable::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	return nullptr;
}

CGameObject* CUI_Interactable::Clone(void* pArg)
{
	return nullptr;
}

void CUI_Interactable::Free()
{
}

void CUI_Interactable::OnHoverEnter()
{
}

void CUI_Interactable::OnHoverExit()
{
}

void CUI_Interactable::OnPress()
{
}

void CUI_Interactable::OnRelease()
{
}

void CUI_Interactable::OnDisabled()
{
}
