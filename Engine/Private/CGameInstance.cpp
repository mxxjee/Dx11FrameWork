#include "CGameInstance.h"

#include "CTimer_Manager.h"
#include "CGraphic_Device.h"
#include "CLevel_Manager.h"
#include "CLevelFactroy.h"
#include "CPrototype_Manager.h"
#include "CObject_Manager.h"
#include "CRenderer.h"
#include "CCamera_Manager.h"
#include "CInput_Manager.h"
#include "CShader_Manager.h"
#include "CScreenShot_Manager.h"
#include "CRenderState_Manager.h"
#include "CShader.h"




IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
	 
}
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>* pDevice, ComPtr<ID3D11DeviceContext>*pContext)
{
	m_EngineDesc = EngineDesc;



	/* 그래픽 디바이스 초기화 */
	m_pGraphicDev = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.winMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pDevice, pContext);
	CheckNullResult(m_pGraphicDev, E_FAIL);

	D3D11_VIEWPORT          ViewportDesc{};
	_uint           iNumViewports = { 1 };
	(*pContext)->RSGetViewports(&iNumViewports, &ViewportDesc);
	m_ViewPorts.push_back(ViewportDesc);

	/*레벨 매니저 초기화*/
	m_pLevelManager = CLevel_Manager::Create();
	CheckNullResult(m_pLevelManager, E_FAIL);

	/*레벨 팩토리 초기화*/
	m_pLevelFactory = CLevelFactroy::Create();
	CheckNullResult(m_pLevelFactory, E_FAIL);


	/* 사운드  디바이스 초기화 */

	/* 타이머 매니져 초기화 */
	m_pTimerManager = CTimer_Manager::Create();
	CheckNullResult(m_pTimerManager, E_FAIL);


	/*프로토타입매니져 초기화*/
	m_pProtoManager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	CheckNullResult(m_pProtoManager, E_FAIL);

	/*오브젝트매니져 초기화*/
	m_pObjectManager = CObject_Manager::Create(*pDevice, *pContext, EngineDesc.iNumLevels);
	CheckNullResult(m_pObjectManager, E_FAIL);


	/*렌더러 초기화*/
	m_pRenderer = CRenderer::Create(*pDevice, *pContext);
	CheckNullResult(m_pRenderer, E_FAIL);

	/*카메라 매니져 초기화*/
	m_pCameraManager = CCamera_Manager::Create(*pDevice,*pContext);
	CheckNullResult(m_pCameraManager, E_FAIL);

	/*쉐이더 매니져 초기화*/
	m_pShaderManager = CShader_Manager::Create(*pDevice, *pContext);
	CheckNullResult(m_pShaderManager, E_FAIL);

	/*스크린샷매니저 초기화*/
	m_pScreenshotManager = CScreenShot_Manager::Create(*pDevice, *pContext);
	CheckNullResult(m_pScreenshotManager, E_FAIL);

	/*렌더스테이트 매니져 초기화*/
	m_pRenderStateManager = CRenderState_Manager::Create(*pDevice, *pContext);
	CheckNullResult(m_pRenderStateManager, E_FAIL);

	return S_OK;
}

void CGameInstance::Update_Priority_Engine(_float fTimedelta)
{
	/*지연삭제 / Scenechange 용*/
	m_pLevelManager->Update_Priority(fTimedelta);
}

void CGameInstance::Update_Engine(_float fTimedelta)
{
	/*m_pObjectManager->Update_Priority(fTimedelta);
	m_pObjectManager->Update(fTimedelta);
	m_pObjectManager->Update_Late(fTimedelta);*/

	m_pLevelManager->Update(fTimedelta);
	Update_Cameras(fTimedelta);
	


}

void CGameInstance::LateUpdate_Engine(float fTimedelta)
{
	m_pLevelManager->Update_Late(fTimedelta);
	LateUpdate_Cameras(fTimedelta);
}

void CGameInstance::Update_Render(float fTimedelta)
{
	m_pLevelManager->Update_Render(fTimedelta);
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
	//m_pRenderer->Draw();
	m_pCameraManager->Render_Cameras();
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

	m_pProtoManager->Clear(iLevelID);
	m_pObjectManager->Clear(iLevelID);


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
const vector<CLevel*>* CGameInstance::Get_LevelStack()
{
	return m_pLevelManager->Get_LevelStack();
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

void CGameInstance::Update_Priority_Static(_float fTimeDelta)
{
	CheckNull(m_pObjectManager);
	return m_pObjectManager->Update_Priority_Static(fTimeDelta);

}

void CGameInstance::Update_Static(_float fTimeDelta)
{

	CheckNull(m_pObjectManager);
	return m_pObjectManager->Update_Static(fTimeDelta);
}

void CGameInstance::Update_Late_Static(_float fTimeDelta)
{
	CheckNull(m_pObjectManager);
	return m_pObjectManager->Update_Late_Static(fTimeDelta);
}

void CGameInstance::Update_Render_Static(_float fTimeDelta)
{
	CheckNull(m_pObjectManager);
	return m_pObjectManager->Update_Render_Static(fTimeDelta);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _wstring& LayerTag, const _wstring& Tag)
{
	return m_pObjectManager->Find_GameObject(iLevelIndex,LayerTag,Tag);
}

#pragma endregion

#pragma region Renderer
HRESULT CGameInstance::Add_RenderObject(_uint eID, CGameObject* pRenderObject)
{
	CheckNullResult(m_pRenderer, E_FAIL);
	return m_pRenderer->Add_RenderObject(eID, pRenderObject);
}
HRESULT CGameInstance::Add_SortFunc(_uint eID, function<bool(CGameObject*, CGameObject*)> _Fun)
{
	CheckNullResult(m_pRenderer, E_FAIL);
	return m_pRenderer->Add_SortFunc(eID,_Fun);
}
void CGameInstance::Render_Group(_uint eType)
{
	return m_pRenderer->Render_Group(eType);
}
HRESULT CGameInstance::Initialize_Renderer(_uint RenderGroupCount)
{
	CheckNullResult(m_pRenderer, E_FAIL);
	return m_pRenderer->Initialize(RenderGroupCount);
}
void CGameInstance::Clear_RenderGroups()
{
	return m_pRenderer->Clear_RenderGroups();
}
int CGameInstance::Get_RenderGroupCount()
{
	return m_pRenderer->Get_RenderGroupCount();
}
HRESULT CGameInstance::Get_Buffer(ComPtr<ID3D11Texture2D>* pBuffer, UINT iFlag)
{
	return m_pGraphicDev->Get_Buffer(pBuffer,iFlag);
}
ComPtr<ID3D11RenderTargetView> CGameInstance::Get_BackBuffer_RTV()
{
	return m_pGraphicDev->Get_BackBuffer_RTV();
}
ComPtr<ID3D11DepthStencilView> CGameInstance::Get_BackBuffer_DSV()
{
	return m_pGraphicDev->Get_BackBuffer_DSV();
}
#pragma endregion


#pragma region CameraManager
void CGameInstance::RegisterCamera(CAMERA_TYPE eType, CGameObject* pObj)
{
	CheckNull(m_pCameraManager);
	m_pCameraManager->RegisterCamera(eType, pObj);
}
void CGameInstance::UnRegisterCamera(CAMERA_TYPE eType)
{
	CheckNull(m_pCameraManager);
	m_pCameraManager->UnRegisterCamera(eType);
}

void CGameInstance::Set_MainCamera(CAMERA_TYPE eType)
{
	CheckNull(m_pCameraManager);
	return m_pCameraManager->Set_MainCamera(eType);

}

const _float4x4& CGameInstance::GetViewMatrix(CAMERA_TYPE eType) const
{
	return m_pCameraManager->GetViewMatrix(eType);
}
const _float4x4& CGameInstance::GetProjMatrix(CAMERA_TYPE eType) const
{
	return m_pCameraManager->GetProjMatrix(eType);
}
const _matrix CGameInstance::GetMulViewProjMatrix(CAMERA_TYPE eType) const
{
	return m_pCameraManager->GetMulViewProjMatrix(eType);
}
const _float4x4& CGameInstance::Get_RenderCamera_ViewMatrix() const
{
	return m_pCameraManager->Get_RenderCamera_ViewMatrix();
}
const _float4x4& CGameInstance::Get_RenderCamera_GetProjMatrix() const
{
	return m_pCameraManager->Get_RenderCamera_GetProjMatrix();
}
const _matrix CGameInstance::Get_RenderCamera_GetMulViewProjMatrix() const
{
	return m_pCameraManager->Get_RenderCamera_GetMulViewProjMatrix();
}
void CGameInstance::Bind_ViewProjMatrix(CAMERA_TYPE eType)
{
	CheckNull(m_pCameraManager);
	return m_pCameraManager->Bind_ViewProjMatrix(eType);
}

void CGameInstance::Update_Cameras(_float fTimeDelta)
{
	CheckNull(m_pCameraManager);
	m_pCameraManager->Update_Cameras(fTimeDelta);
}

void CGameInstance::LateUpdate_Cameras(_float fTimeDelta)
{
	CheckNull(m_pCameraManager);
	m_pCameraManager->LateUpdate_Cameras(fTimeDelta);
}

CCamera_Base* CGameInstance::Find_Camera(CAMERA_TYPE eType)
{
	CheckNullResult(m_pCameraManager, nullptr);
	return m_pCameraManager->Find_Camera(eType);
}

CCamera_Base* CGameInstance::Get_MainCamera()
{
	CheckNullResult(m_pCameraManager, nullptr);
	return m_pCameraManager->Get_MainCamera();
}

//const _float4x4& CGameInstance::Get_Main_ViewMatrix()
//{
//	return m_pCameraManager->Get_Main_ViewMatrix();
//}
//
//const _float4x4& CGameInstance::Get_Main_ProjMatrix()
//{
//	return m_pCameraManager->Get_Main_ProjMatrix();
//}

//_matrix CGameInstance::Get_Main_MulViewProjMatrix()
//{
//	return m_pCameraManager->Get_Main_MulViewProjMatrix();
//}

//void CGameInstance::Bind_Main_ViewProjMatrix() const
//{
//	return m_pCameraManager->Bind_Main_ViewProjMatrix();
//}

CShader* CGameInstance::Get_RenderShader()
{
	return m_pCameraManager->Get_RenderShader();
}

const string& CGameInstance::Get_RenderPassName()
{
	return m_pCameraManager->Get_RenderPassName();
}

CCamera_Base* CGameInstance::Get_RenderCamera()
{
	return m_pCameraManager->Get_RenderCamera();
}



#pragma endregion


HRESULT CGameInstance::Register_Shader(const _wstring& Tag, CShader* pInstance)
{
	CheckNullResult(m_pShaderManager, E_FAIL);
	return m_pShaderManager->Register_Shader(Tag, pInstance);
}

CShader* CGameInstance::Find_Shader(const _wstring& Tag)
{
	CheckNullResult(m_pShaderManager, nullptr);
	return m_pShaderManager->Find_Shader(Tag);
}

void CGameInstance::ScreenShot(const _wstring& Key)
{
	CheckNull(m_pScreenshotManager);
	return m_pScreenshotManager->ScreenShot(Key);
}
 
CTexture* CGameInstance::Find_ScreenTexture(const _wstring& Key)
{
	CheckNullResult(m_pScreenshotManager,nullptr);
	return m_pScreenshotManager->Find_ScreenTexture(Key);
}

HRESULT CGameInstance::SaveTextureToFile(const _wstring& Key, const _wstring& filePath)
{
	CheckNullResult(m_pScreenshotManager,E_FAIL);
	return m_pScreenshotManager->SaveTextureToFile(Key, filePath);
}

HRESULT CGameInstance::Register_RenderStates(_uint iRenderGroup, const RenderStates& States)
{
	CheckNullResult(m_pRenderStateManager, E_FAIL);
	return m_pRenderStateManager->Register_RenderStates(iRenderGroup, States);
}

const RenderStates& CGameInstance::Get_RenderStates(_uint iRenderGroup)
{
	return m_pRenderStateManager->Get_RenderStates(iRenderGroup);
}

#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pLevelManager);
	Safe_Release(m_pTimerManager);
	Safe_Release(m_pLevelFactory);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pProtoManager);
	Safe_Release(m_pObjectManager);
	Safe_Release(m_pCameraManager);
	Safe_Release(m_pShaderManager);
	Safe_Release(m_pScreenshotManager);
	Safe_Release(m_pRenderStateManager);

	Safe_Release(m_pGraphicDev);

	DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();



}
