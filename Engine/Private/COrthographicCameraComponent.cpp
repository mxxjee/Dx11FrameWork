#include "COrthographicCameraComponent.h"

COrthographicCameraComponent::COrthographicCameraComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCameraComponent(pDevice,pContext)
{
}

COrthographicCameraComponent::COrthographicCameraComponent(const COrthographicCameraComponent& Prototype)
	: CCameraComponent(Prototype)
{
}

HRESULT COrthographicCameraComponent::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COrthographicCameraComponent::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;


	COMPONENT_DESC* pCompDesc = static_cast<COMPONENT_DESC*>(pArg);
	ORTHOGRAPHIC_DESC* pDesc = static_cast<ORTHOGRAPHIC_DESC*>(pCompDesc->CameraDesc);

	m_vViewHeight = pDesc->ViewHeight;
	m_vViewWidth = pDesc->ViewWdith;


	Set_ProjectionMatrix();

	return S_OK;
}

void COrthographicCameraComponent::Update_ViewMatrix(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_matView,XMMatrixIdentity());
	
}

void COrthographicCameraComponent::Set_ProjectionMatrix()
{
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(m_vViewWidth, m_vViewHeight, m_fNear, m_fFar));
}

COrthographicCameraComponent* COrthographicCameraComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	COrthographicCameraComponent* pInstance = new COrthographicCameraComponent(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : COrthographicCameraComponent");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent* COrthographicCameraComponent::Clone(void* pArg)
{
	COrthographicCameraComponent* pInstance = new COrthographicCameraComponent(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : COrthographicCameraComponent");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void COrthographicCameraComponent::Free()
{
	__super::Free();
}
