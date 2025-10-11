#include "CCamera_Manager.h"
#include "CPerspectiveCameraComponent.h"
#include "COrthographicCameraComponent.h"
#include "CGameObject.h"
#include "CCamera_Base.h"


void CCamera_Manager::RegisterCamera(const _wstring& Tag, CGameObject* pObj, bool isOrtho)
{
	if (!isOrtho)
		m_mapPerspectiveCams.emplace(Tag, pObj);

	else
		m_mapOrthoCams.emplace(Tag, pObj);
}

void CCamera_Manager::UnRegisterCamera(const _wstring& Tag, bool isOrtho)
{
	auto& Map = isOrtho ? m_mapOrthoCams : m_mapPerspectiveCams;

	auto iter = Map.find(Tag);
	if (iter != Map.end())
	{
		if (!isOrtho)
		{
			if (iter->second == m_pMainPerspectiveCamera)
				m_pMainPerspectiveCamera = nullptr;
		}

		else
		{
			if (iter->second == m_pMainOrthoCamera)
				m_pMainOrthoCamera = nullptr;
		}

		Safe_Release(iter->second);
		Map.erase(iter);

	}
}

bool CCamera_Manager::SetMainPerspectiveCamera(const _wstring& tag)
{
	auto it = m_mapPerspectiveCams.find(tag);
	if (it != m_mapPerspectiveCams.end())
	{
		m_pMainPerspectiveCamera = it->second;
		return true;
	}

	return false;
}

bool CCamera_Manager::SetMainOrthoCamera(const _wstring& tag)
{
	auto it = m_mapOrthoCams.find(tag);
	if (it != m_mapOrthoCams.end())
	{
		m_pMainOrthoCamera = it->second;
		return true;
	}

	return false;
}

const _float4x4& CCamera_Manager::GetViewMatrix(bool isOrtho) const
{
	//뷰행렬
	if (!isOrtho)
	{
		CComponent* pComp = m_pMainPerspectiveCamera->Get_Component(L"PerspectiveCamera");
		if (pComp)
		{
			CPerspectiveCameraComponent* pPers = dynamic_cast<CPerspectiveCameraComponent*>(pComp);
			return pPers->Get_ViewMatrix();
		}

	

	}

	else
	{
		CComponent* pComp = m_pMainOrthoCamera->Get_Component(L"OrthographicCamera");
		if (pComp)
		{
			COrthographicCameraComponent* pOrtho = dynamic_cast<COrthographicCameraComponent*>(pComp);

			return pOrtho->Get_ViewMatrix();
		}
		
	}
}

const _float4x4& CCamera_Manager::GetProjMatrix(bool isOrtho) const
{
	if (!isOrtho)
	{
		CComponent* pComp = m_pMainPerspectiveCamera->Get_Component(L"PerspectiveCamera");
		CPerspectiveCameraComponent* pPers = dynamic_cast<CPerspectiveCameraComponent*>(pComp);

		return pPers->Get_ProjMatrix();
	}
	else
	{
		CComponent* pComp = m_pMainOrthoCamera->Get_Component(L"OrthographicCamera");
		COrthographicCameraComponent* pOrtho = dynamic_cast<COrthographicCameraComponent*>(pComp);

		return pOrtho->Get_ProjMatrix();
	}

}

const _matrix CCamera_Manager::GetMulViewProjMatrix(bool isOrtho) const
{
	// TODO: 여기에 return 문을 삽입합니다.
	if (!isOrtho)
	{
		CComponent* pComp = m_pMainPerspectiveCamera->Get_Component(L"PerspectiveCamera");
		CPerspectiveCameraComponent* pPers = dynamic_cast<CPerspectiveCameraComponent*>(pComp);

		return pPers->Get_MulViewProjMatrix();
	}
	else
	{
		CComponent* pComp = m_pMainOrthoCamera->Get_Component(L"OrthographicCamera");
		COrthographicCameraComponent* pOrtho = dynamic_cast<COrthographicCameraComponent*>(pComp);

		return pOrtho->Get_MulViewProjMatrix();
	}
	
}

void CCamera_Manager::Bind_ViewProjMatrix(bool isOrtho)
{
	
	if (!isOrtho)
	{
		CCamera_Base* pBase = dynamic_cast<CCamera_Base*>(m_pMainPerspectiveCamera);
		if (pBase)
			pBase->Bind_ViewProjMatrix();
	}

	else
	{
		CCamera_Base* pBase = dynamic_cast<CCamera_Base*>(m_pMainOrthoCamera);
		if (pBase)
			pBase->Bind_ViewProjMatrix();
	}
}

CGameObject* CCamera_Manager::GetMainPerspectiveCamera()
{
	return m_pMainPerspectiveCamera;
}

CGameObject* CCamera_Manager::GetMainOrthoCamera()
{
	return m_pMainOrthoCamera;
}

void CCamera_Manager::Update_MainCamera(_float fTimeDelta)
{
	if (m_pMainPerspectiveCamera)
		m_pMainPerspectiveCamera->Update(fTimeDelta);

	if (m_pMainOrthoCamera)
		m_pMainOrthoCamera->Update(fTimeDelta);
}

void CCamera_Manager::LateUpdate_MainCamera(_float fTimeDelta)
{
	if (m_pMainPerspectiveCamera)
		m_pMainPerspectiveCamera->Update_Late(fTimeDelta);

	if (m_pMainOrthoCamera)
		m_pMainOrthoCamera->Update_Late(fTimeDelta);
}



CCamera_Manager* CCamera_Manager::Create()
{
	return new CCamera_Manager;
}

void CCamera_Manager::Free()
{
	for (auto& pair : m_mapPerspectiveCams)
	{
		Safe_Release(pair.second);
		
	}

	for (auto& pair : m_mapOrthoCams)
	{
		Safe_Release(pair.second);

	}

	m_mapPerspectiveCams.clear();
	m_mapOrthoCams.clear();
}
