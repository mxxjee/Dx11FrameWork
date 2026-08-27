#include "CLevel_Dungeon.h"
#include "CGameInstance.h"
#include "CFadeScreen.h"
#include "Client_Defines.h"
#include "CRoom.h"
#include "CStaticBody.h"
#include "CMapLoader.h"

#include "CPlayer.h"
#include "CGameManager.h"
#include "CCell.h"

#include "CMonster.h"
#include "CM_GreenZol.h"
#include "CMonster_Body.h"

#include "CTreasureChest.h"
#include "CItem_Manager.h"

#include "CEventTrigger.h"
#include "CLayer.h"
#include "CM_Gidbos.h"
#include "CDungeonBlock.h"


#include "CMainCamera.h"
#include "CBoxColliderComponent.h"
#include "CInteraction_Manager.h"



USING(Client)

CLevel_Dungeon::CLevel_Dungeon(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Dungeon::Initialize(LevelArgs& args)
{
	__super::Initialize(args);

	m_pGameManager = CGameManager::GetInstance();

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
		return E_FAIL;

#if !defined(_DEBUG)
	if (FAILED(Ready_Layer_Monster(L"Monster_Layer")))
		return E_FAIL;
#endif

	if (FAILED(Ready_Layer_InteractionObject(L"Interaction_Layer")))
		return E_FAIL;
	 
	if (FAILED(Ready_Layer_Trigger(L"Trigger_Layer")))
		return E_FAIL;  

	if (FAILED(Ready_Events()))
		return E_FAIL;




	UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
	pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));

	return S_OK;
}

void CLevel_Dungeon::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CLevel_Dungeon::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CLevel_Dungeon::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

}

void CLevel_Dungeon::Render()
{
	wchar_t szTitle[256];
	swprintf_s(szTitle, L"Dungeon 씬입니다. FPS : %.1f", m_pGameInstance->Get_FPS(L"Timer_60"));

	SetWindowText(g_hWnd, szTitle);
}

HRESULT CLevel_Dungeon::Ready_Lights()
{
	LIGHT_DESC      LightDesc{};
	LightDesc.LightName = L"Dungeon_DirectionLight";
	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.715f, 0.715f, 0.715f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
	wstring ModelName[] = { L"RichardCave_01A",L"RichardCave_02A",L"RichardCave_01B",L"RichardCave_01C" };
	_float4 vPos[] = {
		_float4(9.178f,0.0f,19.523f,1.f),
		_float4(9.168f,0.f,7.77f,1.f),
		_float4(67.07f,0.f,17.583f,1.f),
		_float4(82.015f,0.f,17.567f,1.f)
	};

	for (int i = 0; i < 4; ++i)
	{
		CRoom::MODELOBJECT_DESC     RoomDesc;
		RoomDesc.ObjTag = ModelName[i];

		CStaticBody::BODY_DESC BodyDesc;
		BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
		BodyDesc.modelName = ModelName[i];
		RoomDesc.BodyDesc = &BodyDesc;

		CTransform::TRANSFORM_DESC TransDesc;
		TransDesc.vLocalPosition = vPos[i];
		RoomDesc.TransformDesc = &TransDesc;

		CBase* pBaseRoom = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Room"), &RoomDesc);
		m_Rooms[i] = dynamic_cast<CGameObject*>(pBaseRoom);
		
		if (m_Rooms[i])
		{
			if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::DUNGEON), strLayerTag, m_Rooms[i])))
				return E_FAIL;



		}
	}

	if (FAILED(CMapLoader::Load_Dungeon()))
		return E_FAIL;
	

	m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::DUNGEON));


#pragma region 떨어지는바닥설치
	CDungeonBlock::MODELOBJECT_DESC BlockDesc;
	BlockDesc.m_iLevelID = m_iLevelID;
	BlockDesc.ObjTag = L"DungeonBlock";

	CTransform::TRANSFORM_DESC TransDesc;
	TransDesc.vLocalPosition = _float4(5.414f, 0.f, 16.988f, 1.f);
	TransDesc.vLocalScale = _float4(1.5f, 1.5f, 1.f, 1.f);
	BlockDesc.TransformDesc = &TransDesc;

	CDungeonBlock* pBlock = CDungeonBlock::Create(m_pDevice, m_pContext, &BlockDesc);
	if (pBlock)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevelID, strLayerTag, pBlock)))
			return E_FAIL;

	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Player(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	_float4 vPos[] = {
		_float4(11.211f,0.f,9.88f,1.f),
		_float4(7.675f,0.f,11.350f,1.f),
		_float4(5.472f,0.f,19.44f,1.f),
		_float4(12.909f,0.f,17.425f,1.f),

	};

	for (int i = 0; i < 4; ++i)
	{
		CMonster::MonsterDesc desc;

		CMonster_Body::MONSTER_BODY_DESC bodyDesc;
		bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
		bodyDesc.modelName = L"ZolGreenAnim";

		desc.BodyDesc = &bodyDesc;


		desc.iAttack = 10;
		desc.MaxHp = 1;
		desc.fActionRange = 10.f;
		desc.m_iLevelID = m_iLevelID;

		desc.ObjTag = L"CM_GreenZol" + to_wstring(i);
		CTransform::TRANSFORM_DESC TransDesc = {};
		TransDesc.vLocalPosition = vPos[i];
		TransDesc.vLocalRotation = { 0.f,180.f,0.f,1.f };

		TransDesc.fSpeedPerSec = 3.f;
		TransDesc.fRotationPerSec = 10.f;

		desc.TransformDesc = &TransDesc;



		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
			PROTO_OBJ_NAME(L"CM_GreenZol"),
			ENUM_TO_UINT(LEVEL_ID::DUNGEON),
			strLayerTag, &desc)))
			return E_FAIL;
	}



#pragma region gibos
	bool b = true;

	_float3 PointA[] = {
		_float3(70.39f,0.f,16.497f),
		_float3(68.813f,0.f,14.595f),
	};


	_float3 PointB[] = {
		_float3(84.151f,0.f,16.674f),
		_float3(84.927f,0.f,14.876f),
	};


	for (int i = 0; i < 2; ++i)
	{
		CM_Gidbos::GIDBOS_DESC desc;

		CMonster_Body::MONSTER_BODY_DESC bodyDesc;
		bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
		bodyDesc.modelName = L"Gidbos";

		desc.BodyDesc = &bodyDesc;
		desc.m_StartLeft = b;

		desc.iAttack = 10;
		desc.MaxHp = 2;
		desc.fActionRange = 10.f;
		desc.m_iLevelID = m_iLevelID;

		desc.m_vPointA = PointA[i];
		desc.m_vPointB = PointB[i];

		desc.ObjTag = L"Gidbos" + to_wstring(0);
		CTransform::TRANSFORM_DESC TransDesc = {};

		if (b)
		{
			TransDesc.vLocalRotation = { 0.f,90.f,0.f,1.f };
			TransDesc.fSpeedPerSec = 1.5f;
		}
		else
		{
			TransDesc.vLocalRotation = { 0.f,-90.f,0.f,1.f };
			TransDesc.fSpeedPerSec = 3.f;
		}

		TransDesc.fRotationPerSec = 10.f;

		desc.TransformDesc = &TransDesc;



		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
			PROTO_OBJ_NAME(L"CM_Gidbos"),
			ENUM_TO_UINT(LEVEL_ID::DUNGEON),
			strLayerTag, &desc)))
			return E_FAIL;

		b = !b;
	}

#pragma endregion
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
	CTreasureChest::CTreasureChest_Desc Desc;
	Desc.m_iLevelID = m_iLevelID;
	Desc.SceneName = "Level_Dungeon";
	Desc.bAnimated = false;

	Desc.ObjTag = L"Treausurebox" + to_wstring(0);
	Desc.m_pInnerItem = CItem_Manager::GetInstance()->Get_ItemInfo(ItemType::FEATHER);

	CTransform::TRANSFORM_DESC TransDesc;
	TransDesc.vLocalPosition = _float4(5.559f, 0.f, 21.266f,1.f);
	Desc.TransformDesc = &TransDesc;

	
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
		PROTO_OBJ_NAME(L"TreasureChest"),
		m_iLevelID,
		L"Interaction_Layer", &Desc)))
		return E_FAIL;

	CGameObject* pObj = m_pGameInstance->Find_GameObject(m_iLevelID, L"Interaction_Layer", L"Treausurebox0");
	if (pObj)
	{
		CTreasureChest* pChest = dynamic_cast<CTreasureChest*>(pObj);
		if (pChest)
			pChest->Set_Event([this]()
				{
					CGameObject* pObj = m_pGameInstance->Find_GameObject(m_iLevelID, L"Enviroment_Layer", L"DungeonBlock");
					
					if (pObj)
					{
						CDungeonBlock* pBlock = dynamic_cast<CDungeonBlock*>(pObj);
						if (pBlock)
							pBlock->Drop();


					}
					
				});

	}


	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Particle(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Trigger(const _wstring& strLayerTag)
{
#pragma region 2단계던전 가는 트리거
	CEventTrigger::EventTriggerDesc Goto_Second_Desc;
	Goto_Second_Desc.vCenter = _float3(0.f, 0.f, 0.f);
	Goto_Second_Desc.vExtents = _float3(1.f,1.f, 1.f);
	Goto_Second_Desc.ObjTag = L"Dungeon_Goto_Two_Chapter";
	Goto_Second_Desc.m_iLevelID = m_iLevelID;

	CTransform::TRANSFORM_DESC Second_EventTransform;
	Second_EventTransform.vLocalPosition = _float4(15.5f, 0.7f, 23.39f, 1.f);

	Goto_Second_Desc.TransformDesc = &Second_EventTransform;
	Goto_Second_Desc.EnterFunc = [this]()
	{
		CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
		if(pPlayer)
			pPlayer->Set_CanMove(false);


		Teleport(TELEPORT::GOTO_2ND);



	};

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), ENUM_TO_UINT(LEVEL_ID::DUNGEON), strLayerTag, &Goto_Second_Desc)))
		return E_FAIL;

#pragma endregion


#pragma region 1단계이전
	CEventTrigger::EventTriggerDesc Goto_First_Desc;
	Goto_First_Desc.vCenter = _float3(0.f, 0.f, 0.f);
	Goto_First_Desc.vExtents = _float3(1.f, 1.f, 0.5f);
	Goto_First_Desc.ObjTag = L"Dungeon_Goto_First_Chapter";
	Goto_First_Desc.m_iLevelID = m_iLevelID;

	CTransform::TRANSFORM_DESC First_EventTransform;
	First_EventTransform.vLocalPosition = _float4(15.5f, 2.f, 4.03f, 1.f);

	Goto_First_Desc.TransformDesc = &First_EventTransform;
	Goto_First_Desc.EnterFunc = [this]()
	{
		Teleport(TELEPORT::GOTO_1PRE);
		pFadeScreen->PlayFadeIn();
	};

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), ENUM_TO_UINT(LEVEL_ID::DUNGEON), strLayerTag, &Goto_First_Desc)))
		return E_FAIL;
#pragma endregion



#pragma region 마지막
	CEventTrigger::EventTriggerDesc Goto_Third_Desc;
	Goto_Third_Desc.vCenter = _float3(0.f, 0.f, 0.f);
	Goto_Third_Desc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	Goto_Third_Desc.ObjTag = L"Dungeon_Goto_Exit";
	Goto_Third_Desc.m_iLevelID = m_iLevelID;

	CTransform::TRANSFORM_DESC Third_EventTransform;
	Third_EventTransform.vLocalPosition = _float4(71.f, 0.5f, 10.f, 1.f);

	Goto_Third_Desc.TransformDesc = &Third_EventTransform;
	Goto_Third_Desc.EnterFunc = [this]()
	{
		Teleport(TELEPORT::GOTO_EXIT);
	
	};

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), ENUM_TO_UINT(LEVEL_ID::DUNGEON), strLayerTag, &Goto_Third_Desc)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Events()
{

	CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();

	m_EnterFirstEvent.Payload.Ptrs["Player"] = pPlayer;
	m_EnterFirstEvent.Payload.Floats["OffSet_X"] = 0.f;
	m_EnterFirstEvent.Payload.Floats["OffSet_Y"] = 10.f;
	m_EnterFirstEvent.Payload.Floats["OffSet_Z"] = -3.f;

	m_EnterFirstEvent.Payload.Floats["Rot_X"] = 60.f;
	m_EnterFirstEvent.Payload.Floats["Rot_Y"] = 0.f;
	m_EnterFirstEvent.Payload.Floats["Rot_Z"] = 0.f;

	m_EnterFirstEvent.Name = "Enter_DungeonRoom";



	m_EnterSecondEvent.Payload.Ptrs["Player"] = pPlayer;
	m_EnterSecondEvent.Payload.Floats["OffSet_X"] = 0.f;
	m_EnterSecondEvent.Payload.Floats["OffSet_Y"] = 10.f;
	m_EnterSecondEvent.Payload.Floats["OffSet_Z"] = -7.f;

	m_EnterSecondEvent.Payload.Floats["Rot_X"] = 60.f;
	m_EnterSecondEvent.Payload.Floats["Rot_Y"] = 0.f;
	m_EnterSecondEvent.Payload.Floats["Rot_Z"] = 0.f;

	m_EnterSecondEvent.Name = "Enter_DungeonRoom";


	m_pGameInstance->RegisterListners("Go_Boss", [this](const GameEvent& evt)
		{
			m_pGameInstance->StopSoundFade(CHANNELID::SOUND_BGM, 1.f);
			pFadeScreen->Set_FadeInEndFunc([this]()
				{
					LevelArgs args;
					args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::BOSS);
					args.changeType = LEVELCHANGETYPE::PUSH;
					args.loadingChangeType = LEVELCHANGETYPE::PUSH;
					args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);


					if (FAILED(m_pGameInstance->Level_Changer(
						ENUM_TO_UINT(LEVEL_ID::LOADING),
						args)))
						return;

				});

			pFadeScreen->PlayFadeIn();
		});

	return S_OK;
}

void CLevel_Dungeon::Teleport(TELEPORT eType)
{
	CheckTrue(m_bTeleport);

	CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
	CheckNull(pPlayer);
	m_bTeleport = true;
	pFadeScreen->PlayFadeIn();


	//왠지모르지만 room트리거들이 밟히는거같아서 콜리전비홀성화
	CLayer* pTriggerLayer = m_pGameInstance->Find_Layer(m_iLevelID, L"Trigger_Layer");
	for (auto& pObj : pTriggerLayer->Get_ObjList())
	{
		pObj->Set_Active(false);

	}

	_vector vTargetPos;

	switch (eType)
	{
	case TELEPORT::GOTO_2ND:
	{
		vTargetPos = XMVectorSet(76.5f, 1.5f, 21.630f, 1.f);

		
		m_eCurrentPhase = CLevel_Dungeon::PHASE::SECOND;



		m_pGameInstance->Emit(m_EnterSecondEvent);
		break;
	}

	case TELEPORT::GOTO_EXIT:
	{	
		GameEvent gameEvent;
		gameEvent.Name = "Go_Boss";

		m_pGameInstance->Emit(gameEvent);
		return;
	}
		break;
	}

	//옮기고나서 페이드아웃
	m_pGameInstance->Invoke(2.f, false, false, false, [pPlayer, vTargetPos, this]()
		{
			pPlayer->Get_Transform()->Set_State(STATE::POSITION, vTargetPos);
			pPlayer->Change_MainNavMesh();

		}, pPlayer);



	m_pGameInstance->Invoke(3.5f, false, false, false, [pPlayer, vTargetPos,this]()
		{
			pFadeScreen->PlayFadeOut();
			m_bTeleport = false;
			pPlayer->Set_CanMove(true);

			CLayer* pTriggerLayer = m_pGameInstance->Find_Layer(m_iLevelID, L"Trigger_Layer");
			for (auto& pObj : pTriggerLayer->Get_ObjList())
			{
				pObj->Set_Active(true);

			}


		},pPlayer);



}

void CLevel_Dungeon::OnEnter()
{
	__super::OnEnter();
	CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::DUNGEON));
	m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

	CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();
	CheckNull(pPlayer);

	pPlayer->Show_Weapons();
	vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
	if (m_Cells)
	{
		CCell* pSpawnCell = (*m_Cells)[3];
		_vector vSpawnPoint = pSpawnCell->Get_CenterPos();


		pPlayer->Get_Transform()->Set_State(STATE::POSITION, vSpawnPoint);
		pPlayer->Change_MainNavMesh();
	
		m_pGameManager->Set_DefaultPosition(vSpawnPoint);
		m_pGameManager->Set_LastPosition(vSpawnPoint);

		
		m_eCurrentPhase = CLevel_Dungeon::PHASE::FIRST;

	}

	CheckNull(pFadeScreen);
	pFadeScreen->PlayFadeOut();
	CGameInstance::GetInstance()->Set_EnalbeUpdateRender(false);

	m_pGameInstance->Emit(m_EnterFirstEvent);

	wstring strKey = L"Dungeon";
	CGameInstance::GetInstance()->BroadCastEvent(L"UpdateLevelUI", &strKey);
	m_pGameInstance->Invoke(6.f, 0.f, false, false, []()
		{
			CGameInstance::GetInstance()->BroadCastEvent(L"OnLevelUIHide", nullptr);


		}, pPlayer);

	Play_LevelBGM();
}

void CLevel_Dungeon::OnResume(_uint iPreLevel)
{
}

void CLevel_Dungeon::OnPause(_uint iNextLeve)
{
}

void CLevel_Dungeon::OnExit()
{
	if (Get_State() == LEVELSTATE::HIDDEN || Get_State() == LEVELSTATE::PAUSE)
		return;

	m_pGameInstance->Clear_SceneColliders(m_iLevelID);
	CheckNull(pFadeScreen);
	pFadeScreen->PlayFadeIn();


}

void CLevel_Dungeon::Play_LevelBGM()
{
	m_pGameInstance->PlayBGM(L"BGM/Cave.wav", g_BGMVolume);

}

CLevel_Dungeon* CLevel_Dungeon::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
	CLevel_Dungeon* pInstance = new CLevel_Dungeon(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize(args)))
	{
		MSG_BOX("Failed to Create : CLevel_Dungeon");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CLevel_Dungeon::Free()
{
	__super::Free();

}
