#include "CPanel.h"

#include "CGameInstance.h"
#include "Client_Defines.h"




USING(Client)
CPanel::CPanel(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CUI(_pDevice,_pDeviceContext)
{
}
CPanel::CPanel(const CPanel& rhs)
	:CUI(rhs)
{
}
HRESULT CPanel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPanel::Initialize_Copytype(void* pArg)
{
	/*부모 컴포넌트 값세팅 */
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPanel::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);



}

void CPanel::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	
}

void CPanel::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CPanel::Render()
{
	RenderStates RS = m_pGameInstance->Get_RenderStates(ENUM_TO_UINT(RENDERGROUP::UI));

	m_pContext->OMSetBlendState(RS._BlendState.Get(), nullptr, 0xFFFFFFFF);

	__super::Render();

	

	return S_OK;
}



CPanel* CPanel::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CPanel* pInstance = new CPanel(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CPanel ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CPanel::Clone(void* pArg)
{
	CPanel* pInstance = new CPanel(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CPanel ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CPanel::Free()
{
	__super::Free();
	
}
