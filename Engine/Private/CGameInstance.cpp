#include "CGameInstance.h"
#include "CTimer_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
	 
}
HRESULT CGameInstance::Initialize_Engine()
{
	/* 그래픽 디바이스 초기화 */
	/* 인풋 디바이스 초기화 */
	/* 사운드  디바이스 초기화 */
	/* 타이머 매니져 초기화 */
	m_pTimerManager = CTimer_Manager::Create();
	if (nullptr == m_pTimerManager)
		return E_FAIL;


	

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimedelta)
{
}

HRESULT CGameInstance::Draw()
{
	return S_OK;
}

_float CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	CheckNullResult(m_pTimerManager, 0.f);

	return m_pTimerManager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _tchar* pTimerTag)
{
	CheckNull(m_pTimerManager);
	return m_pTimerManager->Compute_TimeDelta(pTimerTag);

}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	CheckNullResult(m_pTimerManager,E_FAIL);
	return m_pTimerManager->Add_Timer(pTimerTag);
}

void CGameInstance::Free()
{
	__super::Free();

	Safe_Release(m_pTimerManager);

}
