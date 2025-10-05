#include "CPerspectiveCameraComponent.h"

CPerspectiveCameraComponent::CPerspectiveCameraComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCameraComponent(pDevice,pContext)
{
}

CPerspectiveCameraComponent::CPerspectiveCameraComponent(const CCameraComponent& Prototype)
	: CCameraComponent(Prototype)
{
}

HRESULT CPerspectiveCameraComponent::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPerspectiveCameraComponent::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;
	//수정필요!!
	PERSPECTIVE_DESC* pDesc = static_cast<PERSPECTIVE_DESC*>(pArg);
	
	m_fFov = pDesc->fFov;
	m_fAspect = pDesc->Aspect;

	return S_OK;
}

void CPerspectiveCameraComponent::Update_ViewMatrix(_float fTimeDelta)
{
	__super::Update_ViewMatrix(fTimeDelta);
	Update_ProjectionMatrix();

	
}

void CPerspectiveCameraComponent::Update_ProjectionMatrix()
{
	XMStoreFloat4x4(&m_matProj, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));
}

CPerspectiveCameraComponent* CPerspectiveCameraComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CPerspectiveCameraComponent* pInstance = new CPerspectiveCameraComponent(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPerspectiveCameraComponent");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent* CPerspectiveCameraComponent::Clone(void* pArg)
{
	CPerspectiveCameraComponent* pInstance = new CPerspectiveCameraComponent(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPerspectiveCameraComponent");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CPerspectiveCameraComponent::Free()
{
	__super::Free();
}
