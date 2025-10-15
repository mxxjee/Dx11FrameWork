#include "CMainCamera.h"
#include "CPerspectiveCameraComponent.h"
#include "CGameInstance.h"
#include "CShader.h"


CMainCamera::CMainCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCamera_Base(pDevice,pContext)
{
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	: CCamera_Base(rhs)
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainCamera::Initialize_Copytype(void* pArg)
{
	if(FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;


	/*컴포넌트 세팅*/
	CComponent* pPerspectiveCam= dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype
	(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"PerspectiveCamera"), pArg));
	
	if(FAILED(Add_Component(COMPONENT_TYPE::PERSPECTIVE_CACM,pPerspectiveCam,(CComponent**)(&m_pCameraCom))))
		return E_FAIL;


	return S_OK;
}

void CMainCamera::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	
}

void CMainCamera::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
	Follow_Target(fTimeDelta);

	
	
}

void CMainCamera::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	
	m_pCameraCom->Update_ViewMatrix(fTimeDelta);

	
}

void CMainCamera::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	

}

HRESULT CMainCamera::Render()
{

	
	return S_OK;
}


void CMainCamera::Set_Target(CGameObject* pTarget, bool bInit)
{
	m_pTarget = pTarget;
	
	m_pCameraCom->Set_Target(m_pTarget);

	CheckFalse(bInit);
	if (m_pTarget)
	{
		CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(COMPONENT_TYPE::TRANSFORM));
		if (pTargetTransform && m_pTransformCom)
		{
			_vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION,TransformScope::WORLD);
			_float3 vOffset = m_pCameraCom->Get_OffSet();

			_vector vStartPos = vTargetPos + XMLoadFloat3(&vOffset);
			m_pTransformCom->Set_State(STATE::POSITION, vStartPos);

			// 필요하다면 view 행렬도 바로 계산
			m_pCameraCom->Update_ViewMatrix(0.f);
		}
	}
}

void CMainCamera::Follow_Target(_float fTimeDelta)
{
	if (!m_pTarget)
		return;
	//OutputDebugString(L"[CAMERA] Update Tick\n");

	CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(COMPONENT_TYPE::TRANSFORM));
	if (!m_pTransformCom || !pTargetTransform)
		return;

	

	const _vector TargetPos = pTargetTransform->Get_State(STATE::POSITION,TransformScope::WORLD);
	const _float3 Offset = m_pCameraCom->Get_OffSet();
	
	m_pTransformCom->MoveLerp(TargetPos + XMLoadFloat3(&Offset), 3.f, fTimeDelta);

	
	
	//m_pTransformCom->LookAtSmooth(TargetPos, 5, fTimeDelta);
	
}


CMainCamera* CMainCamera::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CMainCamera* pInstance = new CMainCamera(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CMainCamera ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

CGameObject* CMainCamera::Clone(void* pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CMainCamera ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CMainCamera::Free()
{
	

	__super::Free();
	

}
