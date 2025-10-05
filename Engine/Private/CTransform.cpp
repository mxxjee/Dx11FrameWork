#include "CTransform.h"
#include "MathUtils.h"

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
	
	vLocalPosition = pDesc->vLocalPosition;
	vLocalScale = pDesc->vLocalScale;
	vLocalRotation = pDesc->vLocalRotation;
	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());

	Set_State(STATE::POSITION, vLocalPosition);
	Set_Scale(vLocalScale);
	return S_OK;
}

void CTransform::Move(DIRECTION eDir, float fTimeDelta, Space space)
{
	_vector vPosition = Get_State(STATE::POSITION);

	STATE TargetState=STATE::END;
	_float fTargetSpeed = (eDir > DIRECTION::UP) ? m_fSpeedPerSec * (-1) : m_fSpeedPerSec;
	_vector vTargetAxis;

	if (space == Space::Local)
	{
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

		vTargetAxis = Get_State(TargetState);
	}

	else
	{
		switch (eDir)
		{
		case Engine::DIRECTION::FORWARD:
		case Engine::DIRECTION::BACKWARD:
			vTargetAxis = WORLD_UP;
			break;

		case Engine::DIRECTION::RIGHT:
		case Engine::DIRECTION::LEFT:
			vTargetAxis = WORLD_RIGHT;
			break;

		case Engine::DIRECTION::UP:
		case Engine::DIRECTION::DOWN:
			vTargetAxis = WORLD_UP;
			break;

		default:
			break;
		}
	}
	

	
	vPosition += XMVector3Normalize(vTargetAxis) * fTargetSpeed * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);

}

_float3 CTransform::Get_Scale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),//float 추출
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		);

}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	
	//특정 축을 통한 x ,y, z 회전.
	_matrix AxisRotationMat = XMMatrixRotationAxis(vAxis, fRadian);
	_float3 vScale = Get_Scale();

	//방향벡터이므로 , w=0
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, AxisRotationMat));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, AxisRotationMat));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, AxisRotationMat));


	
}

void CTransform::Set_Scale(_float4 vScale)
{
	_vector vRight = Get_State(STATE::RIGHT);
	_vector vUp = Get_State(STATE::UP);
	_vector vLook = Get_State(STATE::LOOK);


	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);


}

void CTransform::Rotation(_float3 fRadian)
{
	//x,y,z 쿼터니온 4원수회전
	Matrix QuaternionMat = XMMatrixRotationRollPitchYaw(fRadian.x, fRadian.y, fRadian.z);
	_float3 vScale= Get_Scale();			//크기유지
	
	//크기 유지
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f)*vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f)*vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f)*vScale.z;

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, QuaternionMat));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, QuaternionMat));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, QuaternionMat));


}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	//누적회전
	//1.회전행렬구하기
	_matrix		RotationMat = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
	
	//2. 나의 right/up/look구하기
	_vector	vRight = Get_State(STATE::RIGHT);
	_vector	vUp = Get_State(STATE::RIGHT);
	_vector	vLook = Get_State(STATE::RIGHT);


	//3.회전행렬과 계산
	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMat));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMat));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMat));


}


void CTransform::LookAt(_fvector vWorldPoint)
{
	_float3 vScale = Get_Scale();

	//new look 구하기
	_vector vPosition = Get_State(STATE::POSITION);
	
	_vector vNewLook = vWorldPoint - vPosition;
	_vector vNewRight = XMVector3Cross(WORLD_UP, vNewLook);
	_vector vNewUp = XMVector3Cross(vNewLook, vNewRight);


	vNewRight = XMVector3Normalize(vNewRight) * vScale.x;
	vNewUp = XMVector3Normalize(vNewUp) * vScale.y;
	vNewLook = XMVector3Normalize(vNewLook) * vScale.z;


	Set_State(STATE::RIGHT, vNewRight);
	Set_State(STATE::UP, vNewUp);
	Set_State(STATE::LOOK, vNewLook);


}

void CTransform::LookAt(_fvector vAxis, _fvector vWorldPoint, _float fTimeDelta, _float fSpeed)
{
	// 1. 회전축 정규화 (ex: (0,1,0) → Y축 회전)
	_vector NormalAxis = XMVector3Normalize(vAxis);

	// 2. 내 위치와 타겟 방향
	_vector vPos = Get_State(STATE::POSITION);
	_vector vToTarget = vWorldPoint - vPos;

	// 3. "회전축 평면"에 투영
	_vector vProjTarget = vToTarget - XMVector3Dot(vToTarget, NormalAxis) * NormalAxis;
	_vector vCurLook = Get_State(STATE::LOOK);
	_vector vProjLook = vCurLook - XMVector3Dot(vCurLook, NormalAxis) * NormalAxis;

	vProjTarget = XMVector3Normalize(vProjTarget);
	vProjLook = XMVector3Normalize(vProjLook);

	// 4. 각도 계산
	float fDot = XMVectorGetX(XMVector3Dot(vProjLook, vProjTarget));
	fDot = MathUtils::Clamp(fDot, -1.0f, 1.0f);
	float fAngle = acosf(fDot);

	// 거의 일치하면 회전 중지 (빙글빙글 방지)
	if (fAngle < 0.1f)
		return;

	// 5. 회전 방향 판별 (cross vs 축)
	_vector vCross = XMVector3Cross(vProjLook, vProjTarget);
	float fDir = XMVectorGetX(XMVector3Dot(vCross, NormalAxis));
	if (fDir < 0)
		fAngle = -fAngle;

	// 6. 실제 회전 각도 (보간)
	float fDeltaAngle = MathUtils::Clamp<float>(fAngle, -fSpeed * fTimeDelta, fSpeed * fTimeDelta);

	// 7. 회전행렬 생성
	_matrix RotationMat = XMMatrixRotationAxis(NormalAxis, fDeltaAngle);

	// 8. 내 로컬 축들 변환
	_vector vRight = XMVector3TransformNormal(Get_State(STATE::RIGHT), RotationMat);
	_vector vUp = XMVector3TransformNormal(Get_State(STATE::UP), RotationMat);
	_vector vLook = XMVector3TransformNormal(Get_State(STATE::LOOK), RotationMat);

	// 9. 직교화
	vRight = XMVector3Normalize(vRight);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	// 10. 최종 세팅 (스케일 유지)
	Set_State(STATE::RIGHT, vRight * Get_Scale().x);
	Set_State(STATE::UP, vUp * Get_Scale().y);
	Set_State(STATE::LOOK, vLook * Get_Scale().z);
}


void CTransform::Chase(_fvector vPoint, _float fTimeDelta, _float MinDistance)
{
	//쫓아가는 방향구하기
	_vector vPosition = Get_State(STATE::POSITION);
	
	_vector vNewLook = vPoint - vPosition;
	_float vLookLength = XMVectorGetX(XMVector3Length(vNewLook));

	
	if (vLookLength >= MinDistance)
		vPosition += XMVector3Normalize(vNewLook) * fTimeDelta * m_fSpeedPerSec;

		
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

void CTransform::LookAt(CTransform* target)
{
	CheckNull(target);
	LookAt(target->Get_State(STATE::POSITION));

}

void CTransform::Free()
{
	__super::Free();
}


