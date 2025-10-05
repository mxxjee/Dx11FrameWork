#include "CCamera_Manager.h"
#include "CPerspectiveCameraComponent.h"


void CCamera_Manager::RegisterCamera(const _wstring& Tag, CCameraComponent* pComp, bool isOrtho)
{
	if (!isOrtho)
		m_mapPerspectiveCams.emplace(Tag, pComp);

	else
		m_mapOrthoCams.emplace(Tag, pComp);
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

const Matrix& CCamera_Manager::GetViewMatrix(bool isOrtho) const
{
	//나중에수정
	return m_pMainPerspectiveCamera->Get_ViewMatrix();
}

const Matrix& CCamera_Manager::GetProjMatrix(bool isOrtho) const
{
	return m_pMainPerspectiveCamera->Get_ProjMatrix();
}

CCameraComponent* CCamera_Manager::GetMainPerspectiveCamera()
{
	return m_pMainPerspectiveCamera;
}

CCameraComponent* CCamera_Manager::GetMainOrthoCamera()
{
	return m_pMainOrthoCamera;
}



CCamera_Manager* CCamera_Manager::Create()
{
	return new CCamera_Manager;
}
