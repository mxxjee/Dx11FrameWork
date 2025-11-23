#include "CNPC_Body.h"
#include "CModel.h"
#include "CModelObject.h"
#include "CAnimation.h"


USING(Client)
CNPC_Body::CNPC_Body(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBody(pDevice,pContext)
{
}

CNPC_Body::CNPC_Body(const CNPC_Body& rhs)
	: CBody(rhs)
{
}

HRESULT CNPC_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CNPC_Body::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	//葛电局聪皋捞记 loop贸府
	for (auto& pair : m_pModel->Get_Anims())
	{
		if (pair.second)
			pair.second->Set_Loop(true);
	}


	m_NextAnimKey = L"wait";
	m_NextAnimLoop = true;

	m_pModel->Set_Animation_Speed(L"wait", 50.f);
	m_pModel->Set_Animation_Speed(L"talk", 50.f);

	return S_OK;
}

void CNPC_Body::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CNPC_Body::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);	//Model->PlayAnimtion
}

void CNPC_Body::Update_Late(_float fTimeDelta)
{
	if (m_pModel)
		m_pModel->Set_Animation(m_NextAnimKey, m_NextAnimLoop);

	__super::Update_Late(fTimeDelta);

}

void CNPC_Body::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CNPC_Body::Render()
{
	__super::Render();

	return S_OK;
}

CNPC_Body* CNPC_Body::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CNPC_Body* pInstance = new CNPC_Body(_pDevice, _pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPC_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNPC_Body::Clone(void* pArg)
{
	CNPC_Body* pInstance = new CNPC_Body(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPC_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Body::Free()
{
	__super::Free();
}
