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

	if (FAILED(Ready_Layer_Monster(L"Monster_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Layer_InteractionObject(L"Interaction_Layer")))
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
	swprintf_s(szTitle, L"Dungeon ¾ÀÀÔ´Ï´Ù. FPS : %.1f", m_pGameInstance->Get_FPS(L"Timer_60"));

	SetWindowText(g_hWnd, szTitle);
}

HRESULT CLevel_Dungeon::Ready_Lights()
{
	LIGHT_DESC      LightDesc{};
	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
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
		CGameObject* pRoom = dynamic_cast<CGameObject*>(pBaseRoom);
		
		if (pRoom)
		{
			if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::DUNGEON), strLayerTag, pRoom)))
				return E_FAIL;



		}
	}

	if (FAILED(CMapLoader::Load_Dungeon()))
		return E_FAIL;
	

	m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::DUNGEON));


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
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
	CInteractionObject::Interaction_DESC Desc;
	Desc.m_iLevelID = m_iLevelID;
	Desc.SceneName = "Level_Dungeon";
	Desc.bAnimated = false;

	Desc.ObjTag = L"Treausurebox" + to_wstring(0);

	CTransform::TRANSFORM_DESC TransDesc;
	TransDesc.vLocalPosition = _float4(5.559f, 0.f, 21.266f,1.f);
	Desc.TransformDesc = &TransDesc;

	
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
		PROTO_OBJ_NAME(L"TreasureChest"),
		m_iLevelID,
		L"Interaction_Layer", &Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Particle(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Trigger(const _wstring& strLayerTag)
{
	return S_OK;
}

void CLevel_Dungeon::OnEnter()
{
	__super::OnEnter();
	m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

	CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();
	CheckNull(pPlayer);

	pPlayer->Show_Weapons();
	vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
	if (m_Cells)
	{
		CCell* pSpawnCell = (*m_Cells)[0];
		_vector vSpawnPoint = pSpawnCell->Get_CenterPos();


		pPlayer->Get_Transform()->Set_State(STATE::POSITION, vSpawnPoint);
		pPlayer->Change_MainNavMesh();

		m_pGameManager->Set_DefaultPosition(vSpawnPoint);
		m_pGameManager->Set_LastPosition(vSpawnPoint);

	}

	CheckNull(pFadeScreen);
	pFadeScreen->PlayFadeOut();
	CGameInstance::GetInstance()->Set_EnalbeUpdateRender(false);

	GameEvent Event;
	EventPayload payload;
	Event.Payload = payload;

	Event.Payload.Ptrs["Player"] = pPlayer; 
	Event.Payload.Floats["OffSet_X"] = 0.f;
	Event.Payload.Floats["OffSet_Y"] = 10.f;
	Event.Payload.Floats["OffSet_Z"] = -3.f;

	Event.Payload.Floats["Rot_X"] = 60.f;
	Event.Payload.Floats["Rot_Y"] = 0.f;
	Event.Payload.Floats["Rot_Z"] = 0.f;

	Event.Name = "Enter_DungeonRoom";
	m_pGameInstance->Emit(Event);
}

void CLevel_Dungeon::OnResume(_uint iPreLevel)
{
}

void CLevel_Dungeon::OnPause(_uint iNextLeve)
{
}

void CLevel_Dungeon::OnExit()
{
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
