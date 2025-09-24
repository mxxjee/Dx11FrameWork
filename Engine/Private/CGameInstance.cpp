#include "CGameInstance.h"

#include "CTimer_Manager.h"
#include "CGraphic_Device.h"
#include "CLevel_Manager.h"
#include "CLevelFactroy.h"
#include "CPrototype_Manager.h"
#include "CObject_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
	 
}
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
{
	/* 그래픽 디바이스 초기화 */
	m_pGraphicDev = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.winMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pDevice.GetAddressOf(), pContext.GetAddressOf());
	CheckNullResult(m_pGraphicDev, E_FAIL);

	/*레벨 매니저 초기화*/
	m_pLevelManager = CLevel_Manager::Create();
	CheckNullResult(m_pLevelManager, E_FAIL);

	/*레벨 팩토리 초기화*/
	m_pLevelFactory = CLevelFactroy::Create();
	CheckNullResult(m_pLevelFactory, E_FAIL);


	/* 인풋 디바이스 초기화 */
	/* 사운드  디바이스 초기화 */

	/* 타이머 매니져 초기화 */
	m_pTimerManager = CTimer_Manager::Create();
	CheckNullResult(m_pTimerManager, E_FAIL);


	/*프로토타입매니져 초기화*/
	m_pProtoManager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	CheckNullResult(m_pProtoManager, E_FAIL);

	/*오브젝트매니져 초기화*/
	m_pObjectManager = CObject_Manager::Create(pDevice.Get(), pContext.Get(), EngineDesc.iNumLevels);
	CheckNullResult(m_pObjectManager, E_FAIL);

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimedelta)
{
	/*m_pObjectManager->Update_Priority(fTimedelta);
	m_pObjectManager->Update(fTimedelta);
	m_pObjectManager->Update_Late(fTimedelta);*/

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

#pragma region LevelManager
HRESULT CGameInstance::Level_Changer(_uint iSceneID, LevelArgs& args)
{
	CheckNullResult(m_pLevelManager,E_FAIL);
	return m_pLevelManager->Level_Changer(iSceneID, args);
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
#pragma endregion

#pragma region LevelFactory
void CGameInstance::Register_Level(_uint iSceneID, LevelCreator Creator)
{
	CheckNull(m_pLevelFactory);
	m_pLevelFactory->Register(iSceneID, Creator);
}

CLevel* CGameInstance::Create_Level(_uint iSceneID, LevelArgs _Arg)
{
	CheckNullResult(m_pLevelFactory,nullptr);
	return m_pLevelFactory->Create(iSceneID, _Arg);
}
#pragma endregion

#pragma region TimerManager
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
	CheckNullResult(m_pTimerManager, E_FAIL);
	return m_pTimerManager->Add_Timer(pTimerTag);
}

#pragma endregion

#pragma region ProtoManager
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strProtoTag, CBase* pPrototype)
{
	CheckNullResult(m_pProtoManager, E_FAIL);
	return m_pProtoManager->Add_Prototype(iLevelIndex,strProtoTag,pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelIndex, const _wstring& strPrototag, void* pArg)
{
	CheckNullResult(m_pProtoManager, nullptr);
	return m_pProtoManager->Clone_Prototype(ePrototypeID, iLevelIndex, strPrototag,pArg);
}
#pragma endregion

#pragma region ObjectManager
const unordered_map<_wstring, CLayer*>& CGameInstance::Get_Layers(_uint iLevel)
{
	return m_pObjectManager->Get_Layers(iLevel);
}

HRESULT CGameInstance::Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CheckNullResult(m_pObjectManager, E_FAIL);
	return m_pObjectManager->Add_GameObject_To_Layer(iProtoLevelIndex,strPrototypeTag,iLayerLevelIndex,strLayerTag,pArg);
}
#pragma endregion



void CGameInstance::Release_Engine()
{
	Safe_Release(m_pLevelManager);
	Safe_Release(m_pTimerManager);
	Safe_Release(m_pLevelFactory);
	Safe_Release(m_pProtoManager);
	Safe_Release(m_pObjectManager);
	Safe_Release(m_pGraphicDev);

	DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();



}
