#include "CPlayerFallState.h"
#include "CPlayer.h"
#include "CCamera_Base.h"
#include "CGameInstance.h"



USING(Client)
CPlayerFallState::CPlayerFallState()
	:m_pGameInstance(CGameInstance::GetInstance())
{
}

CPlayerFallState::~CPlayerFallState()
{
}

void CPlayerFallState::Enter(CPlayer* pPlayer)
{

	pPlayerInput = pPlayer->Get_Input();
	pActionControl = pPlayer->Get_ActionControl();


	pPlayer->Reserve_Animation_To_Body(L"fall", true);


	pPlayer->Set_CanMove(false);

	CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());
	pCameraBase->Set_Target(nullptr);

	m_fTime = 0.f;

}

void CPlayerFallState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	m_fTime += fTimeDelta;

	/// 5초동안 떨어져라..

	if (m_fTime <= 2.f)
	{
		_vector vPos = pPlayer->Get_Transform()->Get_State(STATE::POSITION);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - (fTimeDelta * 10.f));

		pPlayer->Get_Transform()->Set_State(STATE::POSITION, vPos);
	}

	else
	{
		//이후엔 이웃셀에 위치해라(이런거이벤트로모아놓으면 될듯)
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
	}
	
}

void CPlayerFallState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
	

}

void CPlayerFallState::Exit(CPlayer* pPlayer)
{
	CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());
	pCameraBase->Set_Target(pPlayer);


	pPlayer->Respawn();
    pActionControl->m_bFall = false;
}

CPlayerFallState* CPlayerFallState::Create()
{
    return new CPlayerFallState;
}
