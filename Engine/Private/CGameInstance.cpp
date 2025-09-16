#include "CGameInstance.h"

#include "CTimer_Manager.h"
#include "CGraphic_Device.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
	 
}
HRESULT CGameInstance::Initialize_Engine(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY,
	_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스 초기화 */
	m_pGraphicDev = CGraphic_Device::Create(hWnd, isWindowed, iWinSizeX, iWinSizeY, ppDevice, ppContext);
	CheckNullResult(m_pGraphicDev, E_FAIL);


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

_float CGameInstance::Clear_BackBuffer_View(const _float4* pClearColor)
{
	CheckNullResult(m_pGraphicDev,0.f);
	return m_pGraphicDev->Clear_BackBuffer_View(pClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	CheckNullResult(m_pGraphicDev,E_FAIL);
	return m_pGraphicDev->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	CheckNullResult(m_pGraphicDev,E_FAIL);
	return m_pGraphicDev->Present();
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
	Safe_Release(m_pGraphicDev);
}
