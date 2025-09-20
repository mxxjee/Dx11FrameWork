#include "CGameInstance.h"

#include "CTimer_Manager.h"
#include "CGraphic_Device.h"
#include "CLevel_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
	 
}
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스 초기화 */
	m_pGraphicDev = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.winMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	CheckNullResult(m_pGraphicDev, E_FAIL);

	/*레벨 매니저 초기화*/
	m_pLevelManager = CLevel_Manager::Create();
	CheckNullResult(m_pLevelManager, E_FAIL);


	/* 인풋 디바이스 초기화 */
	/* 사운드  디바이스 초기화 */
	/* 타이머 매니져 초기화 */
	m_pTimerManager = CTimer_Manager::Create();
	CheckNullResult(m_pTimerManager, E_FAIL);



	

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimedelta)
{
	m_pLevelManager->Update(fTimedelta);
}

HRESULT CGameInstance::Draw_Begin(const _float4* pClearColor)
{
	CheckNullResult(m_pGraphicDev,E_FAIL);

	if(FAILED(m_pGraphicDev->Clear_BackBuffer_View(pClearColor)))
		return E_FAIL;

	if (FAILED(m_pGraphicDev->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	//Renderer->Draw();
	m_pLevelManager->Render();
	return S_OK;
}

HRESULT CGameInstance::Draw_End()
{
	CheckNullResult(m_pGraphicDev,E_FAIL);

	m_pGraphicDev->Present();

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelID)
{

}

HRESULT CGameInstance::Level_Changer(_uint iSceneID, CLevel* pNewLevel, LEVELCHANGETYPE eChangeType)
{
	CheckNullResult(m_pLevelManager,E_FAIL);
	return m_pLevelManager->Level_Changer(iSceneID,pNewLevel, eChangeType);
}

void CGameInstance::Pop_Level()
{
	CheckNull(m_pLevelManager);
	return m_pLevelManager->Pop_Level();

}

CLevel* CGameInstance::Get_CurrentLevel()
{
	CheckNullResult(m_pLevelManager,nullptr);
	return m_pLevelManager->Get_CurrentLevel();
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

	Safe_Release(m_pLevelManager);
	Safe_Release(m_pTimerManager);
	Safe_Release(m_pGraphicDev);
	

}
