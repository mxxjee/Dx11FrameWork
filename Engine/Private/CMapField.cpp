
#include "CMapField.h"
#include "CBoxColliderComponent.h"
CMapField::CMapField(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMapModel(pDevice,pContext)
{
}

CMapField::CMapField(const CMapField& rhs)
	: CMapModel(rhs)
{
}

HRESULT CMapField::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapField::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;


	pColliderComp->Set_ScaleOffset(_float3(10.f, 10.f, 10.f));

	return S_OK;
}

void CMapField::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMapField::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CMapField::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CMapField::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CMapField::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CMapField* CMapField::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CMapField* pInstance = new CMapField(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CMapField ");
		Safe_Release(pInstance);

	}
	 


	return pInstance;
}

CGameObject* CMapField::Clone(void* pArg)
{
	CMapField* pInstance = new CMapField(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CMapField ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CMapField::Free()
{
	__super::Free();
}
