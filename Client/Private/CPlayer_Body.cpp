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
	m_pModel->Set_Loop(L"slash", false);

	m_pModel->Set_Animation_Speed(L"Idle", 60.f);
	m_pModel->Set_Animation_Speed(L"run", 60.f);
	m_pModel->Set_Animation_Speed(L"slash",90.f);

	m_pModel->Set_Animation_Speed(L"slash_hold_st", 80.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_lp", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_ed", 80.f);


	m_pModel->Set_Animation_Speed(L"slash_hold_l", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_r", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_b", 60.f);
	m_pModel->Set_Animation_Speed(L"slash_hold_f", 60.f);


	m_pModel->Set_Animation_Speed(L"shield_st", 100.f);
	m_pModel->Set_Animation_Speed(L"shield_lp",100.f);
	m_pModel->Set_Animation_Speed(L"shield_ed", 150.f);

	m_pModel->Set_Animation_Speed(L"shield_hold_f", 60.f);
	
	
	m_pModel->Set_TransitionTime(0.2f);

#pragma region Set Visible Mesh
	m_pModel->Set_VisibleMesh(L"flipperL_low__MI_flippers", false);
	m_pModel->Set_VisibleMesh(L"MagicRod_magicRodJem_low__MagicRod_MI_magicRod", false);
	m_pModel->Set_VisibleMesh(L"Shovel_handle_low__Shovel_MI_shovel", false);
	m_pModel->Set_VisibleMesh(L"linkHookShot_handl_low__linkHookShot_MI_hookShot", false);
	m_pModel->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldBMirror", false);
	m_pModel->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldB", false);
	m_pModel->Set_VisibleMesh(L"Ocarina_ocarina_low__Ocarina_MI_ocarina", false);
#pragma endregion


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

void CPlayer_Body::Motion_Change()
{

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
