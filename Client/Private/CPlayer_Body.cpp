#include "CPlayer_Body.h"
#include "CModel.h"
#include "CModelObject.h"
#include "CAnimation.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CShader.h"

#include "CInteraction_Manager.h"
#include "Client_Defines.h"

#include "CPlayer_AnimNotifyTypes.h"




USING(Client)
CPlayer_Body::CPlayer_Body(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CAnimBody(pDevice,pContext)
{
}

CPlayer_Body::CPlayer_Body(const CPlayer_Body& rhs)
	: CAnimBody(rhs)
{
}

HRESULT CPlayer_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	
	return S_OK;
}

HRESULT CPlayer_Body::Initialize_Copytype(void* pArg)
{
	if(FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	//모든애니메이션 loop처리
	for (auto& pair : m_pModel->Get_Anims())
	{
		if (pair.second)
			pair.second->Set_Loop(true);
	}

	if (FAILED(Ready_Animation_Speed()))
		return E_FAIL;

	if (FAILED(Ready_Animation_Notify()))
		return E_FAIL;

	if (FAILED(Ready_Animation_Listner()))
		return E_FAIL;

	
	m_pModel->Set_TransitionTime(m_fInitTransitionTime);

#pragma region Set Visible Mesh
	m_pModel->Set_VisibleMesh(L"flipperL_low__MI_flippers", false);
	m_pModel->Set_VisibleMesh(L"MagicRod_magicRodJem_low__MagicRod_MI_magicRod", false);
	m_pModel->Set_VisibleMesh(L"Shovel_handle_low__Shovel_MI_shovel", false);
	m_pModel->Set_VisibleMesh(L"linkHookShot_handl_low__linkHookShot_MI_hookShot", false);
	m_pModel->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldBMirror", false);
	m_pModel->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldB", false);
	m_pModel->Set_VisibleMesh(L"Ocarina_ocarina_low__Ocarina_MI_ocarina", false);
#pragma endregion


	CPlayer_Body::PLAYER_BODY_DESC* pDesc = static_cast<CPlayer_Body::PLAYER_BODY_DESC*>(pArg);
	m_pDamageRender = pDesc->pDamgeRender;
	m_fDamageTime = pDesc->pDamageTime;

	return S_OK;
}

void CPlayer_Body::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CPlayer_Body::Update(_float fTimeDelta)
{



	__super::Update(fTimeDelta);	//Model->PlayAnimtion
}

void CPlayer_Body::Update_Late(_float fTimeDelta)
{
	if (m_pModel)
		m_pModel->Set_Animation(m_NextAnimKey, m_NextAnimLoop);

	__super::Update_Late(fTimeDelta);
	
	

	
}

void CPlayer_Body::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CPlayer_Body::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CPlayer_Body::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Float("b_Damage", (*m_pDamageRender))))
		return E_FAIL;

  	if (FAILED(m_pShader->Bind_Float("g_Time", (*m_fDamageTime))))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Body::Motion_Change()
{

}

HRESULT CPlayer_Body::Ready_Animation_Speed()
{

	m_pModel->Set_Animation(L"Idle", true);
	m_pModel->Set_Loop(L"slash", false);


	m_pModel->Set_Animation_Speed(L"Idle", 60.f);
	m_pModel->Set_Animation_Speed(L"run", 60.f);
	m_pModel->Set_Animation_Speed(L"slash", 90.f);

	m_pModel->Set_Animation_Speed(L"slash_hold_st", 80.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_lp", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_ed", 80.f);


	m_pModel->Set_Animation_Speed(L"slash_hold_l", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_r", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_b", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_f", 60.f);


	m_pModel->Set_Animation_Speed(L"shield_st", 100.f);
	m_pModel->Set_Animation_Speed(L"shield_lp", 100.f);
	m_pModel->Set_Animation_Speed(L"shield_ed", 150.f);

	m_pModel->Set_Animation_Speed(L"shield_hold_f", 60.f);

	m_pModel->Set_Animation_Speed(L"slash_hold_shield_b", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_shield_f", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_shield_r", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_shield_l", 60.f);

	m_pModel->Set_Animation_Speed(L"slash_hold_shield_l", 60.f);


	m_pModel->Set_Animation_Speed(L"jump", 50.f);
	m_pModel->Set_Animation_Speed(L"land", 80.f);


	m_pModel->Set_Animation_Speed(L"item_get_st", 50.f);
	m_pModel->Set_Animation_Speed(L"item_get_lp", 50.f);
	m_pModel->Set_Animation_Speed(L"item_get_ed", 50.f);


	m_pModel->Set_Animation_Speed(L"carry", 60.f);

	m_pModel->Set_Animation_Speed(L"jump_carry", 70.f);
	m_pModel->Set_Animation_Speed(L"talk", 70.f);

	m_pModel->Set_Animation_Speed(L"dmg_f", 60.f);
	m_pModel->Set_Animation_Speed(L"dmg_b", 60.f);


	m_pModel->Set_Animation_Speed(L"push_st", 70.f);
	m_pModel->Set_Animation_Speed(L"push_lp", 60.f);
	m_pModel->Set_Animation_Speed(L"push_ed", 60.f);

	m_pModel->Set_Animation_Speed(L"item_get_st", 50.f);
	m_pModel->Set_Animation_Speed(L"item_get_lp", 50.f);
	m_pModel->Set_Animation_Speed(L"item_get_ed", 50.f);

	m_pModel->Set_Animation_Speed(L"powder", 60.f);


	return S_OK;
}

HRESULT CPlayer_Body::Ready_Animation_Notify()
{
	///////////Notify///////
	GameEvent Event;
	EventPayload payload;
	Event.Payload = payload;
	if (m_pOwner)
	{
		CContainerObject* pPlayer = m_pOwner;
		Event.Payload.Ptrs["Player"] = pPlayer;
	}



	CAnimation* pAnim = m_pModel->Find_Animation(L"slash");
	if (pAnim)
	{
		Event.Name = "AttackBegin";
		pAnim->AddNotify(2, Event);

		Event.Name = "Play_SlashSound";
		pAnim->AddNotify(5, Event);

		Event.Name = "SlashBegin";
		pAnim->AddNotify(0, Event);

		Event.Name = "SlashEnd";
		pAnim->AddNotify(18, Event);
	}
	
	pAnim = m_pModel->Find_Animation(L"shield_st");
	if (pAnim)
	{
		Event.Name = "Shield_Start";
		pAnim->AddNotify(2, Event);
	}


	pAnim = m_pModel->Find_Animation(L"shield_lp");
	if (pAnim)
	{
		Event.Name = "Shield_Loop";
		pAnim->AddNotify(2, Event);
	}


	pAnim = m_pModel->Find_Animation(L"shield_hold_f");
	if (pAnim)
	{
		Event.Name = "Shield_Loop";
		pAnim->AddNotify(2, Event);
	}

	pAnim = m_pModel->Find_Animation(L"slash_hold_shield_lp");
	if (pAnim)
	{
		Event.Name = "slash_hold_shield_lp";
		pAnim->AddNotify(2, Event);
	}

	pAnim = m_pModel->Find_Animation(L"slash_hold_shield_lp");
	if (pAnim)
	{
		Event.Name = "slash_hold_shield_lp";
		pAnim->AddNotify(2, Event);
	}

	pAnim = m_pModel->Find_Animation(L"dmg_f");
	if (pAnim)
	{
		Event.Name = "PlayerOnDamage";
		pAnim->AddNotify(2, Event);

		Event.Name = "PlayerFlash";
		pAnim->AddNotify(10, Event);

	}

	pAnim = m_pModel->Find_Animation(L"dmg_b");
	if (pAnim)
	{
		Event.Name = "PlayerOnDamage";
		pAnim->AddNotify(2, Event);

		Event.Name = "PlayerFlash";
		pAnim->AddNotify(10, Event);
	}


	pAnim = m_pModel->Find_Animation(L"carry");
	if (pAnim)
	{
		Event.Name = "PlayerOnCarry";
		pAnim->AddNotify(23, Event);

	}

	/*pAnim = m_pModel->Find_Animation(L"throw");
	if (pAnim)
	{
		Event.Name = "PlayerOnThrow";
		pAnim->AddNotify(3, Event);

	}*/

	//item_get_st
	pAnim = m_pModel->Find_Animation(L"item_get_st");
	if (pAnim)
	{
		Event.Name = "PlayerOnItemGet";
		pAnim->AddNotify(20, Event);

	}

	pAnim = m_pModel->Find_Animation(L"item_get_ed");
	if (pAnim)
	{
		Event.Name = "PlayerExitItemGet";
		pAnim->AddNotify(6, Event);

	}

	pAnim = m_pModel->Find_Animation(L"powder");
	if (pAnim)
	{
		Event.Name = "PlayerPowder";
		pAnim->AddNotify(6, Event);

	}

	pAnim = m_pModel->Find_Animation(L"slash_hold_ed");
	if (pAnim)
	{
		Event.Name = "PlayerSlashHoldEnd_Start";
		pAnim->AddNotify(1, Event);

		Event.Name = "Play_hold_ed_Sound";
		pAnim->AddNotify(7, Event);

	}

	//pAnim = m_pModel->Find_Animation(L"slash_hold_st");
	//if (pAnim)
	//{
	//	Event.Name = "PlayerSlashHoldStart_Start";
	//	pAnim->AddNotify(10, Event);

	//}

	//slash_hold_lp
	//pAnim = m_pModel->Find_Animation(L"slash_hold_st");
	//if (pAnim)
	//{
	//	Event.Name = "PlayerSlashHoldStart_End";
	//	pAnim->AddNotify(23, Event);

	//}

	return S_OK;
}

HRESULT CPlayer_Body::Ready_Animation_Listner()
{
	m_pGameInstance->RegisterListners("AttackBegin", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->OnAttackBegin();
		});

	//Play_SlashSound
	m_pGameInstance->RegisterListners("Play_SlashSound", [](const GameEvent& event)
		{
			bool bPlaySound = rand() % 2;

			if (bPlaySound)
			{
				int iRandom = rand() % 2;
				CGameInstance::GetInstance()->PlaySoundW(L"LinkVoice/Link_Swish" + to_wstring(iRandom) + L".wav", CHANNELID::SOUND_PLAYER_SFX1, g_VoiceVolume);

			}

			CGameInstance::GetInstance()->PlaySoundW(L"Effects/Sword_Slash.wav", CHANNELID::SOSUND_PLAYER_SFX2, g_EffectVolume);


		});

	m_pGameInstance->RegisterListners("Play_hold_ed_Sound", [](const GameEvent& event)
		{
			int iRandom = rand() % 2;

			CGameInstance::GetInstance()->PlaySoundW(L"LinkVoice/Link_ChargingEnd" + to_wstring(iRandom)+L".wav", CHANNELID::SOUND_PLAYER_SFX1, g_VoiceVolume);

		});


	/*m_pGameInstance->RegisterListners("Shield_Start", [](const GameEvent& event)
		{
			CGameInstance::GetInstance()->PlaySoundW(L"Effects/Shield.wav", CHANNELID::SOSUND_PLAYER_SFX2, g_EffectVolume);

		});*/




	m_pGameInstance->RegisterListners("AttackBegin", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->OnAttackBegin();
		});

	m_pGameInstance->RegisterListners("SlashBegin", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->AnimNotify_Start(PLAYER_ANIMNOTIFY_TYPE::SLASH_START);

		});

	m_pGameInstance->RegisterListners("SlashEnd", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->AnimNotify_End(PLAYER_ANIMNOTIFY_TYPE::SLASH_END);
		});

	m_pGameInstance->RegisterListners("Shield_Loop", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->Set_ShieldEnable(true);
		});


	m_pGameInstance->RegisterListners("slash_hold_shield_lp", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->Set_ShieldEnable(true);
		});

	m_pGameInstance->RegisterListners("PlayerOnDamage", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->Damage_Behavior();     
		});


	//깜빡깜빡거리기
	m_pGameInstance->RegisterListners("PlayerFlash", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->Set_Flash(true);
		
			

		});


	// 프레임맞춰서 들게하기
	m_pGameInstance->RegisterListners("PlayerOnCarry", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));

			CIInteractable* pObj =CInteraction_Manager::GetInstance()->Get_CurrentTarget();
			if (pPlayer)
				pPlayer->Set_CarryAndThrowState(dynamic_cast<CInteractionObject*>(pObj));




		});

	m_pGameInstance->RegisterListners("PlayerSlashHoldEnd_Start", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->AnimNotify_Start(PLAYER_ANIMNOTIFY_TYPE::SLASH_HOLD_ED_START);





		});

	//m_pGameInstance->RegisterListners("PlayerSlashHoldStart_Start", [](const GameEvent& event)
	//	{
	//		CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
	//		if (pPlayer)
	//			pPlayer->AnimNotify_Start(PLAYER_ANIMNOTIFY_TYPE::SLASH_HOLD_ST_START);





	//	});

	m_pGameInstance->RegisterListners("PlayerSlashHoldStart_End", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			if (pPlayer)
				pPlayer->AnimNotify_End(PLAYER_ANIMNOTIFY_TYPE::SLASH_HOLD_ST_END);





		});

	// 프레임맞춰서 던지기
	//m_pGameInstance->RegisterListners("PlayerOnThrow", [](const GameEvent& event)
	//	{
	//		CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));

	//		CIInteractable* pObj = CInteraction_Manager::GetInstance()->Get_PreTarget();
	//		if (pPlayer)
	//			pPlayer->Set_CarryAndThrowState(dynamic_cast<CInteractionObject*>(pObj));




	//	});

	//파우더
	m_pGameInstance->RegisterListners("PlayerPowder", [](const GameEvent& event)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
			CheckNull(pPlayer);

			pPlayer->Create_PowderParticle();


		});

	return S_OK;
}



CPlayer_Body* CPlayer_Body::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CPlayer_Body* pInstance = new CPlayer_Body(_pDevice, _pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Body::Clone(void* pArg)
{
	CPlayer_Body* pInstance = new CPlayer_Body(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Body::Free()
{
	__super::Free();
}
