#include "CMapObject.h"
#include "CGameInstance.h"


CMapObject::CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject{pDevice,pContext}
{

}

CMapObject::CMapObject(const CMapObject& rhs)
	:CGameObject{rhs}
	,m_bSelected{rhs.m_bSelected}
	,m_eRenderGroup{rhs.m_eRenderGroup}
{
	
}


HRESULT CMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Initialize_Copytype(void* pArg)
{
	if(FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMapObject::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMapObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	CheckNull(m_pTransformCom);

}

void CMapObject::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}


void CMapObject::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CMapObject::Render()
{
	__super::Render();

	return S_OK;
}
