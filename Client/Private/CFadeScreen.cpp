#include "CFadeScreen.h"
#include "CGameInstance.h"
#include "Client_Defines.h"

USING(Client)

CFadeScreen::CFadeScreen(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CPanel(_pDevice, _pDeviceContext)
{
}

CFadeScreen::CFadeScreen(const CFadeScreen& rhs)
	: CPanel(rhs)
{
}

HRESULT CFadeScreen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFadeScreen::Initialize_Copytype(void* pArg)
{
	/*값세팅 */


	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;



	return S_OK;
}

void CFadeScreen::PlayFadeIn()
{
	CheckTrue(m_eState ==END_FADEIN || m_eState==LOOP_FADE_IN);

	m_eState = START_FADEIN;

	m_pGameInstance->BroadCastEvent(L"PlayFadeIn", nullptr);
	m_bEnd = false;//Auto체크를 위한 끝남표시,, m_bEnd가 false여야 auto가 작동된다.

}

void CFadeScreen::PlayFadeOut()
{
	m_eState = START_FADEOUT;
	m_pGameInstance->BroadCastEvent(L"PlayFadeOut", nullptr);


}



void CFadeScreen::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CFadeScreen::Update(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CFadeScreen::START_FADEIN:
	{	
		m_eState = LOOP_FADE_IN;
	
	}
		
		break;
	case Client::CFadeScreen::LOOP_FADE_IN:
	{
		if (Get_UIComp()->Is_AnimEnd(UIAnimType::ALPHA))
			m_eState = END_FADEIN;
	}
		break;

	case Client::CFadeScreen::END_FADEIN:
	{

		if (Get_UIComp()->Is_AnimEnd(UIAnimType::ALPHA))
			m_eState = WAIT;
	}
		break;

	case Client::CFadeScreen::START_FADEOUT:
	{

		m_eState = LOOP_FADEOUT;
		
	}
		break;
	case Client::CFadeScreen::LOOP_FADEOUT:
	{
		
		if (Get_UIComp()->Is_AnimEnd(UIAnimType::ALPHA))
			m_eState = END_FADEOUT;
	}
	break;
	case Client::CFadeScreen::END_FADEOUT:
	{
		IfFadeOutStart();
		if (Get_UIComp()->Is_AnimEnd(UIAnimType::ALPHA))
			m_eState = WAIT;
	}
		
		break;


	}

	
	Change_State();
	__super::Update(fTimeDelta);


}

void CFadeScreen::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	if (m_bAutoMode && !m_bEnd)
	{
		m_fCurTime += fTimeDelta;
		if (m_fCurTime >= m_fTime)
		{
			m_bEnd = true;
			m_fCurTime = 0.f;
			PlayFadeOut();
		}

	}

}

void CFadeScreen::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CFadeScreen::Render()
{
	RenderStates RS = m_pGameInstance->Get_RenderStates(ENUM_TO_UINT(RENDERGROUP::UI));

	m_pContext->OMSetBlendState(RS._BlendState.Get(), nullptr, 0xFFFFFFFF);

	__super::Render();
	return S_OK;
}

void CFadeScreen::IfFadeOutEnd()
{
	m_pGameInstance->BroadCastEvent(L"FadeOutEnd", nullptr);

}

void CFadeScreen::IfFadeOutStart()
{
	m_pGameInstance->BroadCastEvent(L"FadeOutStart", nullptr);

}

void CFadeScreen::IfFadeInEnd()
{
	m_pGameInstance->BroadCastEvent(L"FadeInEnd", nullptr);
	if (m_FadeInEndFunction)
	{
		m_FadeInEndFunction();
		m_FadeInEndFunction = nullptr;
	}
}

void CFadeScreen::IfFadeInStart()
{
	m_pGameInstance->BroadCastEvent(L"FadeInStart", nullptr);
}

void CFadeScreen::Change_State()
{
	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case Client::CFadeScreen::START_FADEIN:
			IfFadeInStart();
			break;
		case Client::CFadeScreen::END_FADEIN:
			IfFadeInEnd();
			break;
		case Client::CFadeScreen::START_FADEOUT:
			IfFadeOutStart();
			break;
		case Client::CFadeScreen::END_FADEOUT:
			IfFadeOutEnd();
			break;
		}


		m_ePreState = m_eState;
	}
}

CFadeScreen* CFadeScreen::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CFadeScreen* pInstance = new CFadeScreen(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CFadeScreen ");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CGameObject* CFadeScreen::Clone(void* pArg)
{
	CFadeScreen* pInstance = new CFadeScreen(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CFadeScreen ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CFadeScreen::Free()
{
	__super::Free();
}
