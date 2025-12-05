#include "CMonster_Body.h"
#include "CModel.h"
#include "CModelObject.h"
#include "CAnimation.h"
#include "CShader.h"


USING(Client)
CMonster_Body::CMonster_Body(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBody(pDevice, pContext)
{
}

CMonster_Body::CMonster_Body(const CMonster_Body& rhs)
	: CBody(rhs)
{
}

HRESULT CMonster_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CMonster_Body::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	//모든애니메이션 loop처리
	for (auto& pair : m_pModel->Get_Anims())
	{
		if (pair.second)
			pair.second->Set_Loop(true);
	}


	CMonster_Body::MONSTER_BODY_DESC* pDesc = static_cast<CMonster_Body::MONSTER_BODY_DESC*>(pArg);
	m_pActionControl = pDesc->pActionControl;

	
	return S_OK;
}

void CMonster_Body::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMonster_Body::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMonster_Body::Update_Late(_float fTimeDelta)
{
	if (m_pModel)
		m_pModel->Set_Animation(m_NextAnimKey, m_NextAnimLoop);


	__super::Update_Late(fTimeDelta);
	
}

void CMonster_Body::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CMonster_Body::Render()
{
	
	__super::Render();


	return S_OK;
}

CMonster_Body* CMonster_Body::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CMonster_Body* pInstance = new CMonster_Body(_pDevice, _pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster_Body::Clone(void* pArg)
{
	CMonster_Body* pInstance = new CMonster_Body(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Body::Free()
{
	__super::Free();
}

HRESULT CMonster_Body::Bind_ShaderResources()
{

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Float("b_Damage", m_pActionControl->m_bDamage)))
		return E_FAIL;

	//깜빡거림없음.
	if (FAILED(m_pShader->Bind_Float("g_Time", 0.f)))
		return E_FAIL;

	return S_OK;
}

void CMonster_Body::Register_AnimKey(_uint first, const _wstring& second)
{
	auto iter = m_AnimKeys.find(first);

	if(iter==m_AnimKeys.end())
		m_AnimKeys.emplace(first, second);

}

void CMonster_Body::Change_AnimKey(_uint first, const _wstring& second)
{
	auto iter = m_AnimKeys.find(first);

	if (iter != m_AnimKeys.end())
	{
		iter->second = second;

	}
}

_wstring CMonster_Body::Get_AnimKey(_uint first)
{
	return m_AnimKeys[first];

}
