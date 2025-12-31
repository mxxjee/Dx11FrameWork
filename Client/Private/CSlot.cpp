#include "CSlot.h"

USING(Client)


CSlot::CSlot(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CPanel(_pDevice, _pDeviceContext)
{
}

CSlot::CSlot(const CSlot& rhs)
	: CPanel(rhs)
{
}

HRESULT CSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlot::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;



	return S_OK;
}

void CSlot::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CSlot::Update(_float fTimeDelta)
{
	Enter_State();
	Update_State();
	__super::Update(fTimeDelta);

}

void CSlot::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CSlot::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);

}

HRESULT CSlot::Render()
{
	__super::Render();
	return S_OK;
}

void CSlot::Enter_State()
{
}

void CSlot::Update_State()
{
	
}
