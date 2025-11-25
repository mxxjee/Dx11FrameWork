#include "CCamera_Manager.h"
#include "CPerspectiveCameraComponent.h"
#include "COrthographicCameraComponent.h"
#include "CGameObject.h"
#include "CCamera_Base.h"
#include "CGameInstance.h"

#include "ColorUtils.h"
#include "GraphicUtil.h"

#include "CShader.h"
#include "CConstantBuffer.h"



CCamera_Manager::CCamera_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice(_pDevice),m_pContext(_pContext)
{
	XMStoreFloat4x4(&g_Identityfloat4x4, XMMatrixIdentity());
	g_IdentityMatrix = XMMatrixIdentity();
	m_pGameInstance = CGameInstance::GetInstance();
}

HRESULT CCamera_Manager::Initialize()
{
	for (int i = 0; i < ENUM_TO_UINT(CAMERA_TYPE::END); ++i)
		m_Cameras[i] = nullptr;

	
	//이벤트등록
	//NPC 이벤트 시작시 필요한 콜백 등록(카메라 역할)
	m_pGameInstance->RegisterListners("Enter_Interaction_NPC", [this](const GameEvent& evt)
		{
			CGameObject* pNpc = static_cast<CGameObject*>(evt.Payload.Ptrs.at("NPC"));
			CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(Get_MainCamera());

			pCameraBase->Set_Target(pNpc);
			pCameraBase->Set_Offset(_float3(
				evt.Payload.Floats.at("Float_X"),
				evt.Payload.Floats.at("Float_Y"),
				evt.Payload.Floats.at("Float_Z")));

		});

	m_pGameInstance->RegisterListners("Exit_Interaction_NPC", [this](const GameEvent& evt)
		{
			CGameObject* pPlayer = static_cast<CGameObject*>(evt.Payload.Ptrs.at("Player"));
			CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(Get_MainCamera());
			
			pCameraBase->Set_Target(pPlayer);
			pCameraBase->Set_Offset(pCameraBase->Get_InitOffset());

		});

	m_pGameInstance->RegisterListners("Fix_Camera", [this](const GameEvent& evt)
		{
			CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(Get_MainCamera());
			pCameraBase->Set_Target(nullptr);
			

		});

	m_pGameInstance->RegisterListners("Init_Camera", [this](const GameEvent& evt)
		{
			CGameObject* pPlayer = static_cast<CGameObject*>(evt.Payload.Ptrs.at("Player"));
			CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(Get_MainCamera());

			pCameraBase->Set_Target(pPlayer);
			pCameraBase->Set_Offset(pCameraBase->Get_InitOffset());

		});
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


CCamera_Base* CCamera_Manager::Find_Camera(CAMERA_TYPE eType)
{
	return m_Cameras[ENUM_TO_UINT(eType)];
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


	for (int i = 0; i < ENUM_TO_UINT(CAMERA_TYPE::END); ++i)
	{
		CCamera_Base* pCam = m_Cameras[i];

		if (!pCam || !pCam->Is_Active())
			continue;


		m_pRenderCamera = pCam;

	
		m_pRenderCamera->Bind_RenderTarget();
		m_pRenderCamera->Clear_RenderTargetView(&ClearColor);
		

		//모든셰이더에게 이 카메라의 뷰,투영,카메라위치를 바인딩한다.
		m_pGameInstance->Update_CamBuffer(ENUM_TO_UINT(m_pRenderCamera->Get_CameraType()));
		//m_pGameInstance->Bind_CamBuffer();		//전역 상수버퍼 바인드
		m_pGameInstance->Bind_GlobalLightData();


		//각 카메라가 렌더할 렌더그룹을 접근해서 Render()호출
		vector<uint8_t> RenderMask = m_pRenderCamera->Get_RenderMask();
		for (int i = 0; i < RenderMask.size(); ++i)
		{
			if (RenderMask[i])
				m_pGameInstance->Render_Group(i);

		}



 		
		m_pRenderCamera->UnBind_RenderTarget();
		UnbindAllShaderResources(m_pContext.Get());   // 혹시 모를 잔여 바인딩 제거(HAZARD오류 방지)
	}

	
	
	m_pGameInstance->Clear_RenderGroups();
}



CCamera_Manager* CCamera_Manager::Create(ComPtr<ID3D11Device>  _pDevice, ComPtr<ID3D11DeviceContext>  _pContext)
{
	CCamera_Manager* pInstance = new CCamera_Manager(_pDevice,_pContext);
	if (FAILED(pInstance->Initialize()))
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

//const _float4x4& CCamera_Manager::GetViewMatrix(CAMERA_TYPE eType) const
//{
//	//뷰행렬
//	if (m_Cameras[ENUM_TO_UINT(eType)])
//	{
//		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
//		return pBase->Get_CameraComp()->Get_ViewMatrix();
//	}
//	
//}
//
//const _float4x4& CCamera_Manager::GetProjMatrix(CAMERA_TYPE eType) const
//{
//	//투영행렬
//	if (m_Cameras[ENUM_TO_UINT(eType)])
//	{
//		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
//		return pBase->Get_CameraComp()->Get_ProjMatrix();
//	}
//
//}
//
//const _matrix CCamera_Manager::GetMulViewProjMatrix(CAMERA_TYPE eType) const
//{
//	//뷰 x 투영행렬
//	if (m_Cameras[ENUM_TO_UINT(eType)])
//	{
//		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
//		return pBase->Get_CameraComp()->Get_MulViewProjMatrix();
//	}
//	
//}
//
//void CCamera_Manager::Bind_ViewProjMatrix(CAMERA_TYPE eType)
//{
//	if (m_Cameras[ENUM_TO_UINT(eType)])
//	{
//		CCamera_Base* pBase = m_Cameras[ENUM_TO_UINT(eType)];
//		pBase->Bind_ViewProjMatrix();
//	}
//
//	
//}
//
//CCamera_Base* CCamera_Manager::Find_Camera(CAMERA_TYPE eType)
//{
//	return m_Cameras[ENUM_TO_UINT(eType)];
//}
//
//const _float4x4& CCamera_Manager::Get_RenderCamera_ViewMatrix() const
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//	CheckNullResult(m_pRenderCamera, g_Identityfloat4x4);
//	return m_pRenderCamera->Get_CameraComp()->Get_ViewMatrix();
//}
//
//const _float4x4& CCamera_Manager::Get_RenderCamera_GetProjMatrix() const
//{
//	CheckNullResult(m_pRenderCamera, g_Identityfloat4x4);
//	return m_pRenderCamera->Get_CameraComp()->Get_ProjMatrix();
//}
//
//const _matrix CCamera_Manager::Get_RenderCamera_GetMulViewProjMatrix() const
//{
//	CheckNullResult(m_pRenderCamera, g_IdentityMatrix);
//	return m_pRenderCamera->Get_CameraComp()->Get_MulViewProjMatrix();
//}
