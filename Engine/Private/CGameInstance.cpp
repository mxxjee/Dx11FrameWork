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
#include "CTexture_Manager.h"
#include "CUI_Manager.h"
#include "CShader.h"
#include "CPipeLine.h"

#include "CTerrain_Manager.h"
#include "CTerrain_Base.h"

#include "CMapObject_Manager.h"
#include "CLight_Manager.h"

#include "CMapObject.h"
#include "CLayer.h"




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

	/*텍스쳐 매니저 초기화*/
	m_pTextureManager = CTexture_Manager::Create(*pDevice, *pContext);
	CheckNullResult(m_pTimerManager, E_FAIL);

	/*스크린샷매니저 초기화*/
	m_pScreenshotManager = CScreenShot_Manager::Create(*pDevice, *pContext);
	CheckNullResult(m_pScreenshotManager, E_FAIL);

	/*렌더스테이트 매니져 초기화*/
	m_pRenderStateManager = CRenderState_Manager::Create(*pDevice, *pContext);
	CheckNullResult(m_pRenderStateManager, E_FAIL);

	/*파이프라인*/
	m_pPipeLine = CPipeLine::Create();
	CheckNullResult(m_pPipeLine, E_FAIL);

	/*UI매니저*/
	m_pUIManager = CUI_Manager::Create();
	CheckNullResult(m_pUIManager, E_FAIL);

	/*Terrain매니저*/
	m_pTerrainManager = CTerrain_Manager::Create(*pDevice, *pContext);
	
	CheckNullResult(m_pTerrainManager, E_FAIL);

	/*맵툴오브젝트 매니저 */
	m_pMapObjectManager = CMapObject_Manager::Create(*pDevice, *pContext);
	CheckNullResult(m_pMapObjectManager, E_FAIL);


	/*라이트 매니저*/
	m_pLightManager = CLight_Manager::Create();
	CheckNullResult(m_pLightManager, E_FAIL);

	return S_OK;
}

void CGameInstance::Update_Priority_Engine(_float fTimedelta)
{
	/*지연삭제 / Scenechange 용*/

	m_pMapObjectManager->Update_Priority(fTimedelta);

	
	m_pTerrainManager->Update_Priority(fTimedelta);
	m_pLevelManager->Update_Priority(fTimedelta);
}

void CGameInstance::Update_Engine(_float fTimedelta)
{
	/*m_pObjectManager->Update_Priority(fTimedelta);
	m_pObjectManager->Update(fTimedelta);
	m_pObjectManager->Update_Late(fTimedelta);*/


	m_pMapObjectManager->Update(fTimedelta);

	m_pTerrainManager->Update(fTimedelta);
	m_pLevelManager->Update(fTimedelta);
	Update_Cameras(fTimedelta);
	
	//카메라의 이동이모두 끝난 후 계산
	m_pPipeLine->Update();


}

void CGameInstance::LateUpdate_Engine(float fTimedelta)
{


	m_pMapObjectManager->Update_Late(fTimedelta);


	m_pTerrainManager->Update_Late(fTimedelta);
	m_pLevelManager->Update_Late(fTimedelta);
	LateUpdate_Cameras(fTimedelta);


}

void CGameInstance::Update_Render(float fTimedelta)
{

	m_pMapObjectManager->Update_Render(fTimedelta);


	m_pTerrainManager->Update_Render(fTimedelta);
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

HRESULT CGameInstance::Add_GameObject_To_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pObject)
{
	CheckNullResult(m_pObjectManager, E_FAIL);
	return m_pObjectManager->Add_GameObject_To_Layer(iLayerLevelIndex, strLayerTag, pObject);
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

#pragma region ShaderManager
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

HRESULT CGameInstance::Bind_GlobalPipelineData(_uint CameraType)
{
	CheckNullResult(m_pShaderManager, E_FAIL);
	return m_pShaderManager->Bind_GlobalPipelineData(CameraType);
}

HRESULT CGameInstance::Bind_GlobalLightData()
{
	CheckNullResult(m_pShaderManager, E_FAIL);
	return m_pShaderManager->Bind_GlobalLightData();
}

HRESULT CGameInstance::Bind_SamplerState(_uint iRenderGroup)
{
	CheckNullResult(m_pShaderManager, E_FAIL);
	return m_pShaderManager->Bind_SamplerState(iRenderGroup);
}

#pragma endregion

#pragma region ScreenShotManager
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

#pragma endregion

HRESULT CGameInstance::Register_RenderStates(_uint iRenderGroup, const RenderStates& States)
{
	CheckNullResult(m_pRenderStateManager, E_FAIL);
	return m_pRenderStateManager->Register_RenderStates(iRenderGroup, States);
}

const RenderStates& CGameInstance::Get_RenderStates(_uint iRenderGroup)
{
	return m_pRenderStateManager->Get_RenderStates(iRenderGroup);
}

HRESULT CGameInstance::Bind_SamplerState(CShader* pShader, _uint iRenderGroup)
{
	CheckNullResult(m_pRenderStateManager, E_FAIL);
	return m_pRenderStateManager->Bind_SamplerState(pShader, iRenderGroup);
}

HRESULT CGameInstance::Register_Texture(const _wstring& Tag, CTexture* pInstance)
{
	CheckNullResult(m_pTextureManager, E_FAIL);
	return m_pTextureManager->Register_Texture(Tag,pInstance);
}

CTexture* CGameInstance::Find_Texture(const _wstring& Tag)
{
	CheckNullResult(m_pTextureManager, nullptr);
	return m_pTextureManager->Find_Texture(Tag);
}

#pragma region Pipeline
void CGameInstance::Set_Transform(_uint CameraType, D3DTS eTransformMatrix, _fmatrix TransformMatrix)
{
	CheckNull(m_pPipeLine);
	return m_pPipeLine->Set_Transform(CameraType, eTransformMatrix, TransformMatrix);

}

HRESULT CGameInstance::Bind_PipeLineMatrix(CShader* pShader, const _char* pConstant, _uint iCameraType, D3DTS eTransformMatrix)
{
	CheckNullResult(m_pPipeLine,E_FAIL);
	return m_pPipeLine->Bind_PipeLineMatrix(pShader, pConstant, iCameraType, eTransformMatrix);

}

HRESULT CGameInstance::Bind_PipeLineMatrixAll(CShader* pShader, const _char* pConstant, _uint iCameraType)
{
	return m_pPipeLine->Bind_PipeLineMatrixAll(pShader,pConstant,iCameraType);
}

HRESULT CGameInstance::Bind_PipeLineInverseMatrix(CShader* pShader, const _char* pConstant, _uint iCameraType, D3DTS eTransformMatrix)
{
	CheckNullResult(m_pPipeLine, E_FAIL);
	return m_pPipeLine->Bind_PipeLineInverseMatrix(pShader, pConstant, iCameraType, eTransformMatrix);
}

HRESULT CGameInstance::Bind_CamPosition(CShader* pShader, const _char* pConstant, _uint iCameraType)
{
	CheckNullResult(m_pPipeLine, E_FAIL);
	return m_pPipeLine->Bind_CamPosition(pShader, pConstant, iCameraType);
}
const _float4x4& CGameInstance::Get_ViewMatrix(_uint CameraType)
{
	return m_pPipeLine->Get_ViewMatrix(CameraType);
}
const _float4x4& CGameInstance::Get_ProjMatrix(_uint CameraType)
{
	return m_pPipeLine->Get_ProjMatrix(CameraType);
}
const _float4& CGameInstance::Get_CamPosition(_uint CameraType)
{
	return m_pPipeLine->Get_CamPosition(CameraType);
}

#pragma endregion

#pragma region UI_Manager
HRESULT CGameInstance::Register_UIGroup(const UIGroup& Group, const _wstring& Key)
{
	CheckNullResult(m_pUIManager, E_FAIL);
	return m_pUIManager->Register_UIGroup(Group, Key);
}
HRESULT CGameInstance::AddUIToGroup(const _wstring& Key, CGameObject* pGameObject)
{
	CheckNullResult(m_pUIManager, E_FAIL);
	return m_pUIManager->AddUIToGroup(Key, pGameObject);
}
HRESULT CGameInstance::RegisterEvent(const _wstring& Key, function<void(void*)> _function)
{
	CheckNullResult(m_pUIManager, E_FAIL);
	return m_pUIManager->RegisterEvent(Key, _function);
}
HRESULT CGameInstance::BroadCastEvent(const _wstring& Key, void* pData)
{
	CheckNullResult(m_pUIManager, E_FAIL);
	return m_pUIManager->BroadCastEvent(Key, pData);
}
HRESULT CGameInstance::SetActiveGroup(const _wstring& Key, bool bActive)
{
	CheckNullResult(m_pUIManager, E_FAIL);
	return m_pUIManager->SetActiveGroup(Key, bActive);
}
UIGroup* CGameInstance::Get_UIGroup(const _wstring Key)
{
	CheckNullResult(m_pUIManager, nullptr);
	return m_pUIManager->Get_UIGroup(Key);
}
function<void(void*)> CGameInstance::Get_EventFunction(const _wstring& Key)
{
	CheckNullResult(m_pUIManager, nullptr);
	return m_pUIManager->Get_EventFunction(Key);
}

#pragma endregion

#pragma region Terrain_Manager
HRESULT CGameInstance::Register_Terrain(const _wstring& Key, CTerrain_Base* pTerrain)
{
	CheckNullResult(m_pTerrainManager, E_FAIL);
	return m_pTerrainManager->Register_Terrain(Key,pTerrain);
}
HRESULT CGameInstance::UnRegister_Terrain(const _wstring& Key)
{
	CheckNullResult(m_pTerrainManager, E_FAIL);
	return m_pTerrainManager->UnRegister_Terrain(Key);
}
CTerrain_Base* CGameInstance::Find_Terrain(const _wstring& Key)
{
	CheckNullResult(m_pTerrainManager, nullptr);
	return m_pTerrainManager->Find_Terrain(Key);
}

Triangle* CGameInstance::PickTerrain(const _wstring& Key)
{
	return m_pTerrainManager->PickTerrain(Key);
}

#pragma endregion

#pragma region MapObjectManager

HRESULT CGameInstance::Add_MapObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag, const _wstring& strLayerTag, void* pArg)
{
	CheckNullResult(m_pMapObjectManager, E_FAIL);
	return m_pMapObjectManager->Add_MapObject_To_Layer(iProtoLevelIndex,strPrototypeTag, strLayerTag, pArg);
}
HRESULT CGameInstance::Add_MapObject_To_Layer(const _wstring& LayerTag, CMapObject* pObj)
{
	CheckNullResult(m_pMapObjectManager, E_FAIL);
	return m_pMapObjectManager->Add_MapObject_To_Layer(LayerTag, pObj);
}
CMapObject* CGameInstance::Find_MapObject(const _wstring& LayerTag, const _wstring& ObjTag)
{
	CheckNullResult(m_pMapObjectManager, nullptr);
	return m_pMapObjectManager->Find_MapObject(LayerTag, ObjTag);
}
void CGameInstance::Clear(const _wstring& LayerTag)
{
	return m_pMapObjectManager->Clear(LayerTag);
}
CLayer* CGameInstance::Find_MapLayer(const _wstring& LayerTag)
{
	CheckNullResult(m_pMapObjectManager, nullptr);
	return m_pMapObjectManager->Find_Layer(LayerTag);
}
const UMap<_wstring, CLayer*>& CGameInstance::Get_Layers()
{
	return m_pMapObjectManager->Get_Layers();
}
void CGameInstance::Set_SelectObject(CMapObject* pObj)
{
	return m_pMapObjectManager->Set_SelectObject(pObj);
}
CMapObject* CGameInstance::Get_SelectObject()
{
	CheckNullResult(m_pMapObjectManager, nullptr);
	return m_pMapObjectManager->Get_SelectObject();
}

#pragma endregion

#pragma region Light_Manager
HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	CheckNullResult(m_pLightManager, E_FAIL);
	return m_pLightManager->Add_Light(LightDesc);
}
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	CheckNullResult(m_pLightManager, nullptr);
	return m_pLightManager->Get_LightDesc(iIndex);
}
HRESULT CGameInstance::Bind_Lights(CShader* pShader)
{
	CheckNullResult(m_pLightManager, E_FAIL);

	return m_pLightManager->Bind_Lights(pShader);
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
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTextureManager);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pTerrainManager);
	Safe_Release(m_pMapObjectManager);
	Safe_Release(m_pLightManager);


	Safe_Release(m_pGraphicDev);

	DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();



}
