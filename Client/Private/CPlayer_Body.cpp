#include "CPlayer_Body.h"
#include "CModel.h"
#include "CModelObject.h"
#include "CAnimation.h"


USING(Client)
CPlayer_Body::CPlayer_Body(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBody(pDevice,pContext)
{
}

CPlayer_Body::CPlayer_Body(const CPlayer_Body& rhs)
	:CBody(rhs)
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

	//葛电局聪皋捞记 loop贸府
	for (auto& pair : m_pModel->Get_Anims())
	{
		if (pair.second)
			pair.second->Set_Loop(true);
	}



	m_pModel->Set_Animation(L"Idle", true);
	//m_pModel->Set_TransitionTime(0.02f);


	return S_OK;
}

void CPlayer_Body::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CPlayer_Body::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (*m_pParentState & CModelObject::IDLE)
		m_pModel->Set_Animation(L"Idle", true);


	if (*m_pParentState & CModelObject::RUN)
	{
		m_pModel->Set_Animation(L"run", true);
	}

	if (*m_pParentState & CModelObject::ATTACK)
	{
		m_pModel->Set_Animation(L"slash", true);
	}
}

void CPlayer_Body::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	m_pModel->Play_Animation(fTimeDelta);
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
