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


	CAMERABASE_DESC* pCameraDesc = static_cast<CAMERABASE_DESC*>(pArg);
	if (pCameraDesc)
	{
		CTransform::TRANSFORM_DESC* pTransDesc = static_cast<CTransform::TRANSFORM_DESC*>(pCameraDesc->TransformDesc);
		if (pTransDesc)
			m_vLocalRotation = pTransDesc->vLocalRotation;
	}
	m_bPerspective = true;
	m_pTransformCom->Set_Scale(_float4{ 1.f, 1.f, 1.f,1.f });
	m_pTransformCom->Set_Parent(nullptr);
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
	
	if (m_bLerpRotation)
	{

		_float3 vFRotation = m_pTransformCom->Get_Rotation_ByEular();
		_vector vRot = XMLoadFloat3(&vFRotation);
		XMStoreFloat4(&m_vLocalRotation, vRot);

		m_pTransformCom->RotateLerp(XMLoadFloat3(&m_vTargetRotation), m_fRotationSpeed, fTimeDelta);

	}
	else
		m_pTransformCom->Rotation(_float3(m_vLocalRotation.x, m_vLocalRotation.y, m_vLocalRotation.z));


	Update_PipeLine();
	
	
}

void CMainCamera::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	
	
}

void CMainCamera::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	

}

HRESULT CMainCamera::Render()
{

	
	return S_OK;
}

void CMainCamera::PreRenderGroup(_uint iRenderGroup)
{
	switch ((RENDERGROUP)iRenderGroup)
	{
	case RENDERGROUP::NONALPHA:
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObject"))))
			return;

		break;

	case RENDERGROUP::LIGHT:
		m_pGameInstance->Bind_And_Render_Lights();

		break;

	case RENDERGROUP::COMBINED:
		m_pGameInstance->Render_Combined();
		break;
	default:
		break;
	}
}

void CMainCamera::PostRenderGroup(_uint iRenderGroup)
{
	switch ((RENDERGROUP)iRenderGroup)
	{
	case RENDERGROUP::NONALPHA:
	{
		if (FAILED(m_pGameInstance->End_MRT()))
			return;
	}
	break;

	case RENDERGROUP::LIGHT:
	{
		if (FAILED(m_pGameInstance->End_MRT()))
			return;
	}
		break;


	default:
		break;
	}
}

void CMainCamera::Update_PipeLine()
{
	//pipeline에게 정보업데이트

	m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::VIEW, ViewMatrix);
	m_pGameInstance->Set_CamPosition(ENUM_TO_UINT(m_eCameraType), Get_CameraPosition());
	m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovy), (m_fWidth / m_fHeight), m_fNearZ, m_fFarZ));

}




void CMainCamera::Set_Target(CGameObject* pTarget,  bool bInit)
{
	CheckNull(pTarget);
	m_pTarget = pTarget;

	CheckFalse(bInit);
	if (m_pTarget)
	{
		CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(COMPONENT_TYPE::TRANSFORM));
		if (pTargetTransform && m_pTransformCom)
		{
			_vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION,TransformScope::WORLD) ;
			
			_vector vStartPos = vTargetPos + XMLoadFloat3(&m_vOffset);
			m_pTransformCom->Set_State(STATE::POSITION, vStartPos);

			//// 필요하다면 view 행렬도 바로 계산
			//m_pCameraCom->Update_ViewMatrix(0.f);
		}
	}
}

void CMainCamera::Follow_Target(_float fTimeDelta)
{
	if (!m_pTarget)
		return;

	
	CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(COMPONENT_TYPE::TRANSFORM));
	if (!m_pTransformCom || !pTargetTransform)
		return;

	

	const _vector TargetPos = pTargetTransform->Get_State(STATE::POSITION,TransformScope::WORLD);
	m_pTransformCom->MoveLerp(TargetPos + XMLoadFloat3(&m_vOffset), 3.f, fTimeDelta);


	//??????? 왜이상한곳을보고있니
	_vector eye = m_pTransformCom->Get_State(STATE::POSITION);
	_vector look = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
	XMVECTOR up = m_pTransformCom->Get_State(STATE::UP);

	ViewMatrix = XMMatrixLookToLH(eye, look, up);

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
