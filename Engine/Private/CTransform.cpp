#include "CTransform.h"
#include "MathUtils.h"
#include "CShader.h"
#include "CNavigation.h"


CTransform::CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{
	
}

CTransform::CTransform(const CTransform& Prototype)
	:CComponent(Prototype), m_fSpeedPerSec{Prototype.m_fSpeedPerSec},
	m_fRotationPerSec{ Prototype.m_fRotationPerSec}, m_fEularDegree{Prototype.m_fEularDegree},
	m_pParent{nullptr},m_LocalWorldMatrix{Prototype.m_LocalWorldMatrix},m_WorldMatrix{ Prototype.m_WorldMatrix}
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

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);
	
	XMStoreFloat4x4(&m_LocalWorldMatrix, XMMatrixIdentity());
	m_WorldMatrix = m_LocalWorldMatrix;


	if (pDesc)
	{
		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;

		Set_State(STATE::POSITION, pDesc->vLocalPosition);
		Set_Scale(pDesc->vLocalScale);
		Rotation(_float3(pDesc->vLocalRotation.x, pDesc->vLocalRotation.y, pDesc->vLocalRotation.z));
	}
	
	else
	{
		m_fSpeedPerSec = 5.f;
		m_fRotationPerSec = 0.f;

		_float4 vLocalPosition = { 0.f,0.f,0.f,1.f };
		_float4 vLocalScale = { 1.f,1.f,1.f,1.f };
		_float4 vLocalRotation = { 0.f,0.f,0.f,1.f };


		Set_State(STATE::POSITION, vLocalPosition);
		Set_Scale(vLocalScale);
		Rotation(_float3(vLocalRotation.x, vLocalRotation.y, vLocalRotation.z));
	}




	m_vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

void CTransform::Update_Matrix()
{
	
	if (m_pParent)
	{
		//부모행렬 가져와서 계산
		_matrix LocalMatrix = XMLoadFloat4x4(&m_LocalWorldMatrix);
		_matrix ParentMatrix = XMLoadFloat4x4(&m_pParent->Get_World());


		_matrix ResultMatrix = XMMatrixMultiply(LocalMatrix, ParentMatrix);

		XMStoreFloat4x4(&m_WorldMatrix, ResultMatrix);
		
		

	}

	else
		m_WorldMatrix = m_LocalWorldMatrix;
}

_vector CTransform::Get_State(STATE eState, TransformScope eScope)
{
	//부모가있다면, escope를 선택해서 리턴
	//mat행렬 전체를 연산용 행렬로 만든 뒤, 그 중 eState번째 행만 가져오기
	if (m_pParent)
	{
		switch (eScope)
		{
		case Engine::TransformScope::LOCAL:
			return XMLoadFloat4x4(&m_LocalWorldMatrix).r[ENUM_TO_UINT(eState)];;

		case Engine::TransformScope::WORLD:
			return XMLoadFloat4x4(&m_WorldMatrix).r[ENUM_TO_UINT(eState)];;


		}

	}

	//부모가 지정되지 않았다면, 그냥 localMatrix리턴
	else
		return XMLoadFloat4x4(&m_LocalWorldMatrix).r[ENUM_TO_UINT(eState)];;

	return XMLoadFloat4x4(&m_LocalWorldMatrix).r[ENUM_TO_UINT(eState)];;

}

void CTransform::Move(DIRECTION eDir, float fTimeDelta, Space space, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE::POSITION);
	bool	m_bCross = false;

	STATE TargetState=STATE::END;
	_float fTargetSpeed = (eDir > DIRECTION::UP) ? m_fSpeedPerSec * (-1) : m_fSpeedPerSec;
	_vector vTargetAxis = {};

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

		case Engine::DIRECTION::RIGHTUP:
		case Engine::DIRECTION::LEFTDOWN:
		{
			m_bCross = true;

			_vector vUp = Get_State(STATE::LOOK);
			_vector vRight = Get_State(STATE::RIGHT);

			vTargetAxis = vUp + vRight;

			fTargetSpeed = m_fSpeedPerSec;
			float fValue = eDir == DIRECTION::RIGHTUP ? 1.f : -1.f;

			fTargetSpeed *= fValue;

		}
			break;

		case Engine::DIRECTION::LEFTUP:
		case Engine::DIRECTION::RIGHTDOWN:
		{
			m_bCross = true;

			_vector vUp = Get_State(STATE::LOOK);
			_vector vLeft = Get_State(STATE::RIGHT) * (-1);

			vTargetAxis = vUp + vLeft;

			fTargetSpeed = m_fSpeedPerSec;
			float fValue = eDir == DIRECTION::LEFTUP ? 1.f : -1.f;

			fTargetSpeed *= fValue;

		}
		break;

		default:
			break;
		}
		
		if(!m_bCross)
			vTargetAxis = Get_State(TargetState);
	}

	else
	{
		switch (eDir)
		{
		case Engine::DIRECTION::FORWARD:
		case Engine::DIRECTION::BACKWARD:
			vTargetAxis = WORLD_LOOK;
			break;

		case Engine::DIRECTION::RIGHT:
		case Engine::DIRECTION::LEFT:
			vTargetAxis = WORLD_RIGHT;

			break;

		case Engine::DIRECTION::UP:
		case Engine::DIRECTION::DOWN:
			vTargetAxis = WORLD_UP;
			break;

		case Engine::DIRECTION::RIGHTUP:
		case Engine::DIRECTION::LEFTDOWN:
		{
			m_bCross = true;

			_vector vUp = WORLD_LOOK;
			_vector vRight = WORLD_RIGHT;

			vTargetAxis = vUp + vRight;

			fTargetSpeed = m_fSpeedPerSec;
			float fValue = eDir == DIRECTION::RIGHTUP ? 1.f : -1.f;

			fTargetSpeed *= fValue;

		}
		break;

		case Engine::DIRECTION::LEFTUP:
		case Engine::DIRECTION::RIGHTDOWN:
		{
			m_bCross = true;

			_vector vUp = WORLD_LOOK;
			_vector vLeft = WORLD_RIGHT * (-1);

			vTargetAxis = vUp + vLeft;

			fTargetSpeed = m_fSpeedPerSec;
			float fValue = eDir == DIRECTION::LEFTUP ? 1.f : -1.f;

			fTargetSpeed *= fValue;

		}
		break;
		default:
			break;
		}
	}
	

	
	vPosition += XMVector3Normalize(vTargetAxis) * fTargetSpeed * fTimeDelta;
	m_vMoveDir = XMVectorScale(XMVector3Normalize(vTargetAxis), fTargetSpeed);


	//m_vMoveResultPos = vPosition;

	if(pNavigation==nullptr || pNavigation->isMove(vPosition))
		Set_State(STATE::POSITION, vPosition);


}

void CTransform::Go_Astar(CNavigation* pNavigation, _int GoalIdx, _float fTimeDelta)
{
	const list<_vector>* Rout = pNavigation->Make_Route(GoalIdx);




}

void CTransform::MoveLerp(_vector vTargetPos, float fLerpSpeed, float fTimeDelta,bool bUpdateLook)
{
	float t = 1.0f - expf(-fTimeDelta * fLerpSpeed);
	_vector vCur = Get_State(STATE::POSITION);
	_vector vNew = XMVectorLerp(vCur, vTargetPos, t);
	Set_State(STATE::POSITION, vNew);
}

void CTransform::ScaleLerp(_vector vTargetPos, float fLerpSpeed, float fTimeDelta)
{
	float t = 1.0f - expf(-fTimeDelta * fLerpSpeed);
	_vector vCur = Get_Scale_ByVector();
	_vector vNew = XMVectorLerp(vCur, vTargetPos, t);

	_float4 vScale;
	XMStoreFloat4(&vScale, vNew);

	Set_Scale(vScale);
}

void CTransform::RotateLerp(_vector vTargetRot, float fLerpSpeed, float fTimeDelta)
{
	float t = 1.0f - expf(-fTimeDelta * fLerpSpeed);
	_vector vCur = XMLoadFloat3(&m_fEularDegree);
	_vector vNew = XMVectorLerp(vCur, vTargetRot, t);

	_float3 vRotation;
	XMStoreFloat3(&vRotation, vNew);

	AddRotation(vRotation);
}

void CTransform::AddPosition(_float3 vPos)
{
	_vector vCurPos = Get_State(STATE::POSITION);
	_vector vMovePower = XMLoadFloat3(&vPos);

	Set_State(STATE::POSITION, vCurPos + vMovePower);

}

bool CTransform::IsFront(_vector vDir)
{
	//나의 룩벡터와 vDir을 내적한 결과가
	//  
	//양수일경우 앞(true)
	//음수일 경우 (false)

	
	_vector vLook = XMVector3Normalize(Get_State(STATE::LOOK));
	vDir = XMVector3Normalize(vDir);

	float Dot = XMVectorGetX(XMVector3Dot(vLook, vDir));

	return Dot >= 0.f ? true : false;

}

void CTransform::AddImpulse(float fPower, const _float3 direction, CNavigation* pNavigation)
{
	_float3 impulse;

	m_vVelocity = XMVectorSet(0.f,0.f,0.f,0.f);
	XMStoreFloat3(&impulse, XMVector3Normalize(XMLoadFloat3(&direction)) * fPower);
	m_vVelocity += XMLoadFloat3(&impulse);

	XMVectorSetW(m_vMoveResultPos, 0.f);

	m_bAddImpulse = true;

	//즉시 1회적용
	UpdateImpulse(0.f, pNavigation);
}

void CTransform::UpdateImpulse(_float fTimeDelta, CNavigation* pNavigation)
{
	if (m_bAddImpulse)
	{
		_vector vPosition = Get_State(STATE::POSITION) + m_vVelocity;
		if (pNavigation == nullptr || pNavigation->isMove(vPosition))
			Set_State(STATE::POSITION, vPosition);

		m_vVelocity *= 0.9f;
		if (XMVector3Equal(m_vVelocity, XMVectorSet(0.f, 0.f, 0.f, 1.f)))
			m_bAddImpulse = false;

	}


}

bool CTransform::IsInSight(_float SightFov, _vector vToTargetDir)
{
	_float fCosFov = cosf(SightFov / 2.f);

	//시선벡터 정규화

	
	_vector vLook= XMVector3Normalize(Get_State(STATE::LOOK));
	_vector vToTarget = XMVector3Normalize(vToTargetDir);

	
	_float vTargetDot = XMVectorGetX(XMVector3Dot(vLook, vToTarget));

	
	return vTargetDot >= fCosFov;

}



HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const string& Variable)
{
	CheckNullResult(pShader, E_FAIL);
	return pShader->Bind_Matrix(Variable, m_WorldMatrix);

}

const _float4x4& CTransform::Get_World(TransformScope eScope)
{

	//부모가있다면, escope를 선택해서 리턴
	if (m_pParent)
	{
		switch (eScope)
		{
		case Engine::TransformScope::LOCAL:
			return m_LocalWorldMatrix;

		case Engine::TransformScope::WORLD:
			return m_WorldMatrix;

		}

	}

	//부모가 지정되지 않았다면, 그냥 localMatrix리턴
	else
		return m_LocalWorldMatrix;

	return m_LocalWorldMatrix;
}

_matrix CTransform::Get_WorldInverse(TransformScope eScope)
{
	//부모가있다면, escope를 선택해서 리턴

	if (m_pParent)
	{
		switch (eScope)
		{
		case Engine::TransformScope::LOCAL:
			return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LocalWorldMatrix));


		case Engine::TransformScope::WORLD:
			return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));


		}

	}

	//부모가 지정되지 않았다면, 그냥 localMatrix리턴
	else
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LocalWorldMatrix));

	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LocalWorldMatrix));
}

const _float4x4* CTransform::Get_WorldMatrixPtr(TransformScope eScope)
{
	//부모가있다면, escope를 선택해서 리턴
	if (m_pParent)
	{
		switch (eScope)
		{
		case Engine::TransformScope::LOCAL:
			return &m_LocalWorldMatrix;

		case Engine::TransformScope::WORLD:
			return &m_WorldMatrix;

		}

	}

	//부모가 지정되지 않았다면, 그냥 localMatrix리턴
	else
		return &m_LocalWorldMatrix;

	return &m_LocalWorldMatrix;
}

_float3 CTransform::Get_Scale_ByFloat3()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),//float 추출
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		);

}

_vector CTransform::Get_Scale_ByVector()
{
	return XMVectorSet(
		XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),//float 추출
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK))),
		1.f
	);
}

_vector CTransform::Get_SRT(SRTType eType)
{
	_vector vScale, vTrans, vQuat;


	XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_LocalWorldMatrix));
	switch (eType)
	{
	case Engine::SRTType::SCALE:
		return vScale;
		break;
	case Engine::SRTType::ROTATION:
		return vQuat;
		break;
	case Engine::SRTType::TRANSFORM:
		return vTrans;
		break;
	case Engine::SRTType::END:
		break;
	default:
		break;
	}

	return vScale;
}



void CTransform::Rotation(_vector vAxis, _float fRadian)
{
	
	//특정 축을 통한 x ,y, z 회전.
	_matrix AxisRotationMat = XMMatrixRotationAxis(vAxis, fRadian);
	_float3 vScale = Get_Scale_ByFloat3();

	//방향벡터이므로 , w=0
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, AxisRotationMat));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, AxisRotationMat));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, AxisRotationMat));
	

}

void CTransform::Set_WorldMatrix(const _float4x4& Mat, TransformScope eScope)
{
	//부모가있다면, escope를 선택해서 저장가능.
	if (m_pParent)
	{
		switch (eScope)
		{
		case Engine::TransformScope::LOCAL:
			m_LocalWorldMatrix = Mat;
			break;

		case Engine::TransformScope::WORLD:
			m_WorldMatrix = Mat;
			break;

		}

	}

	//부모가 지정되지 않았다면, LocalMatrix == WorldMatrix==Mat
	else
		m_LocalWorldMatrix = m_WorldMatrix = Mat;
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

void CTransform::Set_Parent(CTransform* pParent)
{
	if (pParent != nullptr && !m_pParent)
	{
		m_pParent = pParent;

	
		//부모의 상대적인 위치로 변환
		_matrix ParentWorldInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&pParent->Get_World(TransformScope::WORLD)));
		_matrix NewLocalMatrix = XMLoadFloat4x4(&Get_World()) * ParentWorldInv;
		XMStoreFloat4x4(&m_LocalWorldMatrix, NewLocalMatrix);

	}

	//nullptr인경우, 부모를 끊는다.
	else
	{
		m_pParent = nullptr;
		m_LocalWorldMatrix = m_WorldMatrix;

		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	}

}

void CTransform::Rotation(_float3 fEularDegree)
{
	//x,y,z 쿼터니온 4원수회전
	
	m_fEularDegree.x = fEularDegree.x;
	m_fEularDegree.y = fEularDegree.y;
	m_fEularDegree.z = fEularDegree.z;

	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_fEularDegree.x),
		XMConvertToRadians(m_fEularDegree.y),
		XMConvertToRadians(m_fEularDegree.z));

	_float3 vScale= Get_Scale_ByFloat3();			//크기유지
	
	//크기 유지
	_vector vRight = XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f),vQuaternion)*vScale.x;
	_vector vUp = XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), vQuaternion) *vScale.y;
	_vector vLook = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), vQuaternion) *vScale.z;

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);


}

void CTransform::AddRotation(_float3 fEularDegree)
{
	//x,y,z 쿼터니온 4원수회전, 누적회전

	m_fEularDegree.x += fEularDegree.x;
	m_fEularDegree.y += fEularDegree.y;
	m_fEularDegree.z += fEularDegree.z;


	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_fEularDegree.x),
		XMConvertToRadians(m_fEularDegree.y),
		XMConvertToRadians(m_fEularDegree.z));

	_matrix RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);
	_float3 vScale = Get_Scale_ByFloat3();			//크기유지

	//크기 유지
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void CTransform::Turn(_vector vAxis, _float fTimeDelta)
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


void CTransform::LookAt(_vector vWorldPoint)
{
	_float3 vScale = Get_Scale_ByFloat3();

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

void CTransform::LookAt(_vector vAxis, _vector vWorldPoint, _float fTimeDelta, _float fSpeed)
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
	Set_State(STATE::RIGHT, vRight * Get_Scale_ByFloat3().x);
	Set_State(STATE::UP, vUp * Get_Scale_ByFloat3().y);
	Set_State(STATE::LOOK, vLook * Get_Scale_ByFloat3().z);
}

void CTransform::LookAtSmooth(_vector vTargetPos, float fLerpSpped, float fTimeDelta)
{
	//new look
	
	_vector vPos = Get_State(STATE::POSITION);
	_float3 vScale = Get_Scale_ByFloat3();

	_vector vDir = vTargetPos - vPos;
	vDir = XMVectorSetY(vDir, 0.f);
	vDir = XMVector3Normalize(vDir);


	_vector vCurLook = XMVector3Normalize(Get_State(STATE::LOOK));
	vCurLook = XMVectorSetY(vCurLook, 0.f);

	_vector vNewLook = XMVectorLerp(vCurLook, vDir, fTimeDelta * fLerpSpped);
	vNewLook = XMVector3Normalize(vNewLook);

	//float dot = XMVectorGetX(XMVector3Dot(vCurLook, vNewLook));
	//if (dot < 0.0f)
	//	vNewLook = XMVectorNegate(vNewLook);

	// Up, Right 갱신
	_vector vRight = XMVector3Normalize(XMVector3Cross(WORLD_UP, vNewLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));


	Set_State(STATE::RIGHT, vRight * vScale.x);
	Set_State(STATE::UP, vUp * vScale.y);
	Set_State(STATE::LOOK, vNewLook * vScale.z);



}


bool CTransform::Chase(_vector vPoint, _float fTimeDelta, CNavigation* pNavigation,_float MinDistance)
{
	//쫓아가는 방향구하기
	bool Result = false;

	_vector vPosition = Get_State(STATE::POSITION);
	
	_vector vNewLook = vPoint - vPosition;
	_float vLookLength = XMVectorGetX(XMVector3Length(vNewLook));

	
	if (vLookLength >= MinDistance)
	{
		vPosition += XMVector3Normalize(vNewLook) * fTimeDelta * m_fSpeedPerSec;
		Result = false;
	}

	else
		Result = true;

	if (pNavigation == nullptr || pNavigation->isMove(vPosition))
		Set_State(STATE::POSITION, vPosition);

	return Result;
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

void CTransform::LookAtWithUpVector(_vector vWorldPoint, _vector vUp)
{
	_float3 vScale = Get_Scale_ByFloat3();

	//new look 구하기
	_vector vPosition = Get_State(STATE::POSITION);

	_vector vNewLook = vWorldPoint - vPosition;
	_vector vNewRight = XMVector3Cross(vUp, vNewLook);
	_vector vNewUp = XMVector3Cross(vNewLook, vNewRight);


	vNewRight = XMVector3Normalize(vNewRight) * vScale.x;
	vNewUp = XMVector3Normalize(vNewUp) * vScale.y;
	vNewLook = XMVector3Normalize(vNewLook) * vScale.z;


	Set_State(STATE::RIGHT, vNewRight);
	Set_State(STATE::UP, vNewUp);
	Set_State(STATE::LOOK, vNewLook);

}

void CTransform::Free()
{
	__super::Free();
}


