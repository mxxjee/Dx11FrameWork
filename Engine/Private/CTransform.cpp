#include "CTransform.h"

CTransform::CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{
	
}

CTransform::CTransform(const CTransform& Prototype)
	:CComponent(Prototype)
{
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize_Copytype(void* pArg)
{
	if(FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	COMPONENT_DESC* pComponentDest = static_cast<COMPONENT_DESC*>(pArg);
	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pComponentDest->TransformDesc);
	
	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());

	return S_OK;
}

void CTransform::Move(DIRECTION eDir, float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);

	STATE TargetState=STATE::END;
	_float fTargetSpeed = (eDir > DIRECTION::UP) ? m_fSpeedPerSec * (-1) : m_fSpeedPerSec;


	switch (eDir)
	{
	case Engine::DIRECTION::FORWARD:
	case Engine::DIRECTION::BACKWARD:
		TargetState = STATE::LOOK;

		break;

	case Engine::DIRECTION::RIGHT:
	case Engine::DIRECTION::LEFT:
		TargetState = STATE::RIGHT;

		break;

	case Engine::DIRECTION::UP:
	case Engine::DIRECTION::DOWN:
		TargetState = STATE::UP;

		break;

	default:                 
		break;
	}

	_vector vTargetAxis = Get_State(TargetState);
	vPosition += XMVector3Normalize(vTargetAxis) * fTargetSpeed * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);

}


CTransform* CTransform::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTransform");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}


	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}
