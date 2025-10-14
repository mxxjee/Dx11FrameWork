#include "CCamera_Manager.h"
#include "CPerspectiveCameraComponent.h"
#include "COrthographicCameraComponent.h"
#include "CGameObject.h"
#include "CCamera_Base.h"
#include "CGameInstance.h"
#include "ColorUtils.h"
#include "CShader.h"




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
	{
		m_Cameras[ENUM_TO_UINT(eType)] = pCamera;
	}


}

void CCamera_Manager::UnRegisterCamera(CAMERA_TYPE eType)
{
	Safe_Release(m_Cameras[ENUM_TO_UINT(eType)]);
}

void CCamera_Manager::Set_MainCamera(CAMERA_TYPE eType)
{
	//기존 카메라 비활성화
	if(m_pMainCamera)
		m_pMainCamera->Set_Active(false);

	//새로운 카메라 활성화
	m_pMainCamera = m_Cameras[ENUM_TO_UINT(eType)];
	m_pMainCamera->Set_Active(true);

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

void CCamera_Manager::Render_Cameras()
{
	auto m_pGameInstance = CGameInstance::GetInstance();
	_float4		ClearColor = COLOR_YELLOW;

#pragma region 고치기전(모든카메라순회)
	//for (auto& i : m_Cameras)
	//{
	//	if (i == nullptr||!i->Is_Active())
	//		continue;

	//	m_pRenderCamera = i;
	//	m_pRenderShader = i->Get_Shader();

	//	m_pRenderPassName = i->Get_PassName();


	//	i->Bind_ViewProjMatrix();

	//	bool HasRenderTarget;
	//	if (HasRenderTarget = i->HasRenderTarget())
	//	{
	//		//이전 백버퍼를 복원하고 새로운 렌더타겟으로 설정
	//		i->Bind_RenderTarget();
	//		i->Clear_RenderTargetView(&ClearColor);
	//	}

	//	

	//	
	//	switch (i->Get_CameraType())
	//	{
	//		

	//	case CAMERA_TYPE::TARGET:
	//	case CAMERA_TYPE::FREE:
	//	case CAMERA_TYPE::MINIMAP:
	//	case CAMERA_TYPE::CUTSCENE:
	//		m_pGameInstance->Render_Group(RENDERGROUP::PRIORITY);
	//		m_pGameInstance->Render_Group(RENDERGROUP::NONALPHA);
	//		m_pGameInstance->Render_Group(RENDERGROUP::ALPHA);
	//		break;


	//	}
	//	

	//	//되돌려놓기
	//	if(HasRenderTarget)
	//		i->UnBind_RenderTarget();
	//	
	//	if(i->Get_CameraType() ==CAMERA_TYPE::UI)
	//		m_pGameInstance->Render_Group(RENDERGROUP::UI);
	//}

#pragma endregion

	//먼저 렌더타겟뷰 가지는거부터 렌더
	for (auto& cam : m_Cameras)
	{
		if (!cam || !cam->Is_Active() || !cam->HasRenderTarget())
			continue;


		m_pRenderCamera = cam;
		m_pRenderShader = cam->Get_Shader();
		m_pRenderPassName = cam->Get_PassName();

		cam->Bind_RenderTarget();
		cam->Clear_RenderTargetView(&ClearColor);

		cam->Bind_ViewProjMatrix();
		m_pGameInstance->Render_Group(RENDERGROUP::PRIORITY);
		m_pGameInstance->Render_Group(RENDERGROUP::NONALPHA);
		m_pGameInstance->Render_Group(RENDERGROUP::ALPHA);

		cam->UnBind_RenderTarget();
	}

	//이후 백버퍼카메라 렌더
	for (auto& cam : m_Cameras)
	{
		if (!cam || !cam->Is_Active() || cam->HasRenderTarget()||cam->Get_CameraType()==CAMERA_TYPE::UI)
			continue;

		m_pRenderCamera = cam;
		m_pRenderShader = cam->Get_Shader();
		m_pRenderPassName = cam->Get_PassName();

		cam->Bind_ViewProjMatrix();
		m_pGameInstance->Render_Group(RENDERGROUP::PRIORITY);
		m_pGameInstance->Render_Group(RENDERGROUP::NONALPHA);
		m_pGameInstance->Render_Group(RENDERGROUP::ALPHA);
	}

	//마지막 ui렌더
	CCamera_Base* pUICam = Find_Camera(CAMERA_TYPE::UI);
	if (pUICam)
	{
		m_pRenderCamera = pUICam;
		m_pRenderShader = pUICam->Get_Shader();
		m_pRenderPassName = pUICam->Get_PassName();
		pUICam->Bind_ViewProjMatrix();
		m_pGameInstance->Render_Group(RENDERGROUP::UI);

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
