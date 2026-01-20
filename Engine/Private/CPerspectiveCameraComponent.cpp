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


	return S_OK;
}

void CPerspectiveCameraComponent::Update_ViewMatrix(_float fTimeDelta)
{
	__super::Update_ViewMatrix(fTimeDelta);
	
	
}

void CPerspectiveCameraComponent::Set_ProjectionMatrix()
{
	XMStoreFloat4x4(&m_matProj, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));
}


void CPerspectiveCameraComponent::Set_CameraValue(float _fNear, float _fFar, float _fFov)
{
	m_fNear = _fNear;
	m_fFar = _fFar;
	m_fFov = _fFov;

	Set_ProjectionMatrix();
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
