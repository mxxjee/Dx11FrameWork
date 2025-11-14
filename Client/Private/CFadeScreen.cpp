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
	m_pGameInstance->BroadCastEvent(L"PlayFadeIn", nullptr);
	m_bEnd = false;//Auto체크를 위한 끝남표시,, m_bEnd가 false여야 auto가 작동된다.
}

void CFadeScreen::PlayFadeOut()
{
	m_pGameInstance->BroadCastEvent(L"PlayFadeOut", nullptr);

}



void CFadeScreen::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CFadeScreen::Update(_float fTimeDelta)
{
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
	__super::Update(fTimeDelta);


}

void CFadeScreen::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	

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
