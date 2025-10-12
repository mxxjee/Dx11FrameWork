#include "CCamera_Manager.h"
#include "CPerspectiveCameraComponent.h"
#include "COrthographicCameraComponent.h"
#include "CGameObject.h"
#include "CCamera_Base.h"


HRESULT CCamera_Manager::Initialize()
{
	for (int i = 0; i < ENUM_TO_UINT(CAMERA_TYPE::END); ++i)
		m_Cameras[i] = nullptr;

	XMStoreFloat4x4(&g_Identityfloat4x4, XMMatrixIdentity());
	g_IdentityMatrix = XMMatrixIdentity();
	
	return S_OK;
}

void CCamera_Manager::RegisterCamera(CAMERA_TYPE eType, CGameObject* pObj)
{

	CCamera_Base* pCamera = dynamic_cast<CCamera_Base*>(pObj);
	if (pCamera)
		m_Cameras[ENUM_TO_UINT(eType)] = pCamera;

}

void CCamera_Manager::UnRegisterCamera(CAMERA_TYPE eType)
{
	Safe_Release(m_Cameras[ENUM_TO_UINT(eType)]);
}

void CCamera_Manager::Set_MainCamera(CAMERA_TYPE eType)
{
	m_pMainCamera = m_Cameras[ENUM_TO_UINT(eType)];
}

const _float4x4& CCamera_Manager::GetViewMatrix(CAMERA_TYPE eType) const
{
	//뷰행렬
	if (m_Cameras[ENUM_TO_UINT(eType)])
	{
		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
		return pBase->Get_CameraComp()->Get_ViewMatrix();
	}
	
}

const _float4x4& CCamera_Manager::GetProjMatrix(CAMERA_TYPE eType) const
{
	//투영행렬
	if (m_Cameras[ENUM_TO_UINT(eType)])
	{
		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
		return pBase->Get_CameraComp()->Get_ProjMatrix();
	}

}

const _matrix CCamera_Manager::GetMulViewProjMatrix(CAMERA_TYPE eType) const
{
	//뷰 x 투영행렬
	if (m_Cameras[ENUM_TO_UINT(eType)])
	{
		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
		return pBase->Get_CameraComp()->Get_MulViewProjMatrix();
	}
	
}

void CCamera_Manager::Bind_ViewProjMatrix(CAMERA_TYPE eType)
{
	if (m_Cameras[ENUM_TO_UINT(eType)])
	{
		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
		pBase->Bind_ViewProjMatrix();
	}

	
}

CCamera_Base* CCamera_Manager::Find_Camera(CAMERA_TYPE eType)
{
	return m_Cameras[ENUM_TO_UINT(eType)];
}

const _float4x4& CCamera_Manager::Get_Main_ViewMatrix()
{
	if (!m_pMainCamera)
		return g_Identityfloat4x4;

	return m_pMainCamera->Get_CameraComp()->Get_ViewMatrix();
}

const _float4x4& CCamera_Manager::Get_Main_ProjMatrix()
{
	if (!m_pMainCamera)
		return g_Identityfloat4x4;

	return m_pMainCamera->Get_CameraComp()->Get_ProjMatrix();
}

_matrix CCamera_Manager::Get_Main_MulViewProjMatrix()
{
	if (!m_pMainCamera)
		return g_IdentityMatrix;

	return m_pMainCamera->Get_CameraComp()->Get_MulViewProjMatrix();
}

void CCamera_Manager::Bind_Main_ViewProjMatrix() const
{
	CheckNull(m_pMainCamera);
	m_pMainCamera->Bind_ViewProjMatrix();
}

void CCamera_Manager::Update_Cameras(_float fTimeDelta)
{
	for (int i = 0; i < ENUM_TO_UINT(CAMERA_TYPE::END); ++i)
	{
		if (m_Cameras[i] != nullptr)
		{
			if (m_Cameras[i]->Is_Active())
				m_Cameras[i]->Update(fTimeDelta);
		}
		
	}
		

}

void CCamera_Manager::LateUpdate_Cameras(_float fTimeDelta)
{
	for (int i = 0; i < ENUM_TO_UINT(CAMERA_TYPE::END); ++i)
	{
		if (m_Cameras[i] != nullptr)
		{
			if (m_Cameras[i]->Is_Active())
				m_Cameras[i]->Update_Late(fTimeDelta);
		}
	
	}
		
}



CCamera_Manager* CCamera_Manager::Create()
{
	CCamera_Manager* pInstance = new CCamera_Manager;
	if (HRESULT(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : Camera_manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Manager::Free()
{
	for (int i = 0; i < ENUM_TO_UINT(CAMERA_TYPE::END); ++i)
		Safe_Release(m_Cameras[i]);

}
