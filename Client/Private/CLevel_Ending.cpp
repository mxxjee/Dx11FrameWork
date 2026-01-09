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

HRESULT CLevel_Ending::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
	m_pGameInstance->Set_Active_Group(48, 127, true);
	m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::TOWN));

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
		_float4(27.7f,10.5f,35.1f,1.f),
		_float4(29.5f,10.5f,35.07f,1.f),
		_float4(31.4f,10.5f,35.1f,1.f),
		_float4(34.4f,10.5f,35.4f,1.f),
		_float4(37.2f,10.5f,34.8f,1.f),

		_float4(29.9f,10.5f,32.81f,1.f),
		_float4(32.5f,10.5f,33.26f,1.f),



	};


	int iSize = 7;

	for (int i = 0; i < iSize; ++i)
	{
		CNPC::NPC_DESC pDesc;

		CTransform::TRANSFORM_DESC pTransDesc;

		pTransDesc.vLocalPosition = vPos[i];

		pDesc.ObjTag = L"NPC_"+ NPCModelName[i];
		pDesc.pTarget = nullptr;
		pDesc.ModelName = NPCModelName[i];
		pDesc.SceneName = "Level_Town";
		pDesc.m_iLevelID = m_iLevelID;
		pDesc.TalkRange = 3.f;

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
			if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::ENDING), strLayerTag, pNpc)))
				return E_FAIL;

		}

	}

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Player(const _wstring& strLayerTag)
{
	return E_NOTIMPL; 
}

void CLevel_Ending::OnEnter()
{
	__super::OnEnter();

	/*카메라 바로 스냅 및 초기화*/
	{
		GameEvent Event;
		Event.Name = "Init_Camera";
		Event.Payload.Ptrs["Player"] = CGameManager::GetInstance()->Get_MainPlayer();

		m_pGameInstance->Emit(Event);
	}


	CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();
	CheckNull(pPlayer);

	vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
	if (m_Cells)
	{
		CCell* pSpawnCell = (*m_Cells)[0];//[1033];
		_vector vSpawnPoint = pSpawnCell->Get_CenterPos();


		pPlayer->Get_Transform()->Set_State(STATE::POSITION, vSpawnPoint);
		pPlayer->Change_MainNavMesh();

		m_pGameManager->Set_DefaultPosition(vSpawnPoint);
		m_pGameManager->Set_LastPosition(vSpawnPoint);

	}

	//////현재씬의 itneraction 등록
	CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::TOWN));

	CheckNull(pFadeScreen);
	pFadeScreen->PlayFadeOut();

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
