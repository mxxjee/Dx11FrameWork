#include "CLevel_Ending.h"
#include "CGameManager.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CCell.h"
#include "CPlayer.h"
#include "CNPC.h"
#include "CRoom_Manager.h"
#include "CInteraction_Manager.h"
#include "CFadeScreen.h"
#include "CMapLoader.h"
#include "CWeatherCock.h"
#include "CUI_Credit.h"



USING(Client)
CLevel_Ending::CLevel_Ending(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CLevel(_pDevice, _pDeviceContext)
{
}

HRESULT CLevel_Ending::Initialize(LevelArgs& args)
{
	__super::Initialize(args);

	m_pGameManager = CGameManager::GetInstance();

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(L"NPC_Layer")))
		return E_FAIL;
	

	if (FAILED(Ready_Layer_UI(L"UI_Layer")))
		return E_FAIL;

	UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
	pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));


	return S_OK;
}

void CLevel_Ending::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CLevel_Ending::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CLevel_Ending::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CLevel_Ending::Render()
{
	wchar_t szTitle[256];
	swprintf_s(szTitle, L"Ending 씬입니다. FPS : %.1f ", m_pGameInstance->Get_FPS(L"Timer_60"));



	SetWindowText(g_hWnd, szTitle);
}

void CLevel_Ending::Play_LevelBGM()
{
	m_pGameInstance->PlayBGM(L"BGM/Ending.wav", g_BGMVolume);
}

HRESULT CLevel_Ending::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
	m_pGameInstance->Set_Active_Group(48, 127, true);
	
	CMapLoader::Load_Ending();
	m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::ENDING));


	CWeatherCock::MODELOBJECT_DESC ModelDesc;
	ModelDesc.m_iLevelID = m_iLevelID;
	ModelDesc.ObjTag = L"WeatherCock";

	CTransform::tagTransformDesc TransDesc;
	TransDesc.vLocalPosition = _float4(30.8f, 10.f, 39.f, 1.f);
	TransDesc.vLocalScale = _float4(1.5f, 1.5f, 1.5f, 1.f);

	ModelDesc.TransformDesc = &TransDesc;

	CGameObject* pObj = CWeatherCock::Create(m_pDevice, m_pContext, &ModelDesc);
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevelID, strLayerTag, pObj)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Ending::Ready_Lights()
{
	LIGHT_DESC      LightDesc{};
	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_NPC(const _wstring& strLayerTag)
{
	//모든 npc들 다 소환
	wstring NPCModelName[] = {
		L"Dad", L"Mom",L"Kid_Blue",L"Kid_Green",L"Marin",L"Tarin",L"Witch"
	};

	_float4 vPos[] = {
	_float4(32.44f, 10.50f, 37.12f, 1.f), // CaveRock
	_float4(34.07f, 10.51f, 35.41f, 1.f), // Rock
	_float4(37.10f, 10.50f, 37.46f, 1.f), // Lawn
	_float4(40.37f, 10.52f, 35.94f, 1.f), // Grass
	_float4(31.38f, 10.50f, 34.60f, 1.f), // BigRock
	_float4(37.38f, 10.50f, 35.30f, 1.f), // ClosedDoor
	_float4(34.16f, 10.50f, 33.54f, 1.f), // Rock
	_float4(38.20f, 10.50f, 33.56f, 1.f)  // Lawn
	};

	// --- Interaction Objects Rotation Data (Degree) ---
	_float4 vRotation[] = {
		_float4(0.0f, 24.20f, 0.0f, 0.f),  // CaveRock
		_float4(0.0f, 27.30f, 0.0f, 0.f),  // Rock
		_float4(0.0f, -38.00f, 0.0f, 0.f), // Lawn
		_float4(0.0f, -28.30f, 0.0f, 0.f), // Grass
		_float4(0.0f, 42.20f, 0.0f, 0.f),  // BigRock
		_float4(0.0f, -24.40f, 0.0f, 0.f), // ClosedDoor
		_float4(0.0f, 20.90f, 0.0f, 0.f),  // Rock
		_float4(0.0f, -22.30f, 0.0f, 0.f)  // Lawn
	};

	int iSize = 7;

	for (int i = 0; i < iSize; ++i)
	{
		CNPC::NPC_DESC pDesc;

		CTransform::TRANSFORM_DESC pTransDesc;

		pTransDesc.vLocalPosition = vPos[i];
		pTransDesc.vLocalRotation = vRotation[i];

		pDesc.ObjTag = L"NPC_"+ NPCModelName[i];
		pDesc.pTarget = nullptr;
		pDesc.ModelName = NPCModelName[i];
		pDesc.SceneName = "Level_Town";
		pDesc.m_iLevelID = m_iLevelID;
		pDesc.TalkRange =0.2f;

		_uint Expression_Eye[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = {};
		_uint Expression_Mouth[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = {};
		_uint Expression_Mouth_Open[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = {};

		CRoom_Manager::GetInstance()->Load_NPC_ExpressionData(NPCModelName[i], Expression_Eye, Expression_Mouth, Expression_Mouth_Open);

		memcpy(&pDesc.iExpressionIdxEye, Expression_Eye, sizeof(_uint) * CNPC::EXPRESSION::END);
		memcpy(&pDesc.iExpressionIdx_Mouth, Expression_Mouth, sizeof(_uint) * CNPC::EXPRESSION::END);
		memcpy(&pDesc.iOpenIdx_Mouth, Expression_Mouth_Open, sizeof(_uint) * CNPC::EXPRESSION::END);


		pDesc.TransformDesc = &pTransDesc;

		CNPC* pNpc = CNPC::Create(m_pDevice, m_pContext, &pDesc);
		if (pNpc)
		{
			//pNpc->Set_TriggerBoxEnable(false);
			if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::ENDING), strLayerTag, pNpc)))
				return E_FAIL;

		}

	}

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_UI(const _wstring& strLayerTag)
{
	//////////타이틀그냥 흰색로고
	CUI_Credit::CreditDesc Credit_Desc = {};
	CTransform::TRANSFORM_DESC TransDesc;
	Credit_Desc.ObjTag = L"Title";
	Credit_Desc.TextureKey = L"Credit";
	Credit_Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
	Credit_Desc.m_iLevelID = m_iLevelID;

	Credit_Desc.fSizeX = 256 * 0.7f;
	Credit_Desc.fSizeY = 2046 * 0.7f;
	Credit_Desc.Depth = 0.49f;

	Credit_Desc.fX = g_iWinSizeX-100.f;
	Credit_Desc.fY = g_iWinSizeY+700.f;
	Credit_Desc.fSpeed = 10.f;



	Credit_Desc.TransformDesc = &TransDesc;
	//AlphaAnim등록
	CUIComponent::UICOMP_DESC UIDesc = {};

	Credit_Desc.UICompDesc = &UIDesc;


	CBase* pLogoObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"UI_Credit"), &Credit_Desc);
	CGameObject* pInstance = dynamic_cast<CGameObject*>(pLogoObj);
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::ENDING), strLayerTag, pInstance)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Ending::OnEnter()
{
	__super::OnEnter();

	//플레이어 ui비활성화
	m_pGameInstance->SetActiveGroup(L"HeartGroup", false);
	m_pGameInstance->SetActiveGroup(L"QuickSlotGroup", false);


	m_pGameInstance->Set_EnableUpdate(true);
	m_pGameInstance->Set_EnalbeUpdateRender(true);
	/*카메라 바로 스냅 및 초기화*/
	{
		GameEvent Event;
		Event.Name = "Init_Camera_With_NewValue";
		Event.Payload.Ptrs["Player"] = CGameManager::GetInstance()->Get_MainPlayer();

		Event.Payload.Floats["Float_X"] = 0.f;
		Event.Payload.Floats["Float_Y"] = 7.f;
		Event.Payload.Floats["Float_Z"] = -10.f;

		Event.Payload.Floats["Rotate_X"] = 42.f;
		Event.Payload.Floats["Rotate_Y"] = 0.f;
		Event.Payload.Floats["Rotate_Z"] = 0.f;



		m_pGameInstance->Emit(Event);
	}


	CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();
	CheckNull(pPlayer);

	vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
	if (m_Cells)
	{
		//계단오르기
		CCell* pSpawnCell = (*m_Cells)[16];//[1033];
		_vector vSpawnPoint = pSpawnCell->Get_CenterPos();


		pPlayer->Get_Transform()->Set_State(STATE::POSITION, vSpawnPoint);
		pPlayer->Change_MainNavMesh();

		//애기 위치 정하기
		CGameObject* pRed_Kid = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red");
		if (pRed_Kid)
		{
			
			pRed_Kid->Get_Transform()->Set_State(STATE::POSITION, XMVectorSetW(vSpawnPoint + XMVectorSet(3.f, 0.f, 0.f, 0.f), 1.f));
			CNPC* pNPC = dynamic_cast<CNPC*>(pRed_Kid);
			if (pNPC)
				pNPC->Set_TriggerBoxEnable(true);

		}
		m_pGameManager->Set_DefaultPosition(vSpawnPoint);
		m_pGameManager->Set_LastPosition(vSpawnPoint);

	}


	//////현재씬의 itneraction 등록
	CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::ENDING));

	CheckNull(pFadeScreen);



	//1초뒤 ending step 변경
	m_pGameManager->Set_EndingStep(EndingStep::TOWN_ARRIVAL);

	m_pGameInstance->Invoke(0.5f, 0.f, false, false, [this]()
		{
			pFadeScreen->PlayFadeOut();
			Play_LevelBGM();

		},CGameManager::GetInstance()->Get_MainPlayer());
	
}

void CLevel_Ending::OnResume(_uint iPreLevel)
{
}

void CLevel_Ending::OnPause(_uint iNextLeve)
{
}

void CLevel_Ending::OnExit()
{
}

CLevel_Ending* CLevel_Ending::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
	CLevel_Ending* pInstance = new CLevel_Ending(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize(args)))
	{
		MSG_BOX("Failed to Create : CLevel_Ending");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();
}
