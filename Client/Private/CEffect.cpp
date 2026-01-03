#include "CEffect.h"
#include "CEffectData_Manager.h"
#include "CShader.h"

USING(Client)

CEffect::CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
{
}


HRESULT CEffect::Render()
{
	__super::Render();

	return S_OK;
}

void CEffect::Spawn(const _float4x4* pSocketMatrix, const _float4x4* pParentMatrix)
{
}

void CEffect::Play()
{
}

void CEffect::Stop()
{
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
}


HRESULT CEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::Initialize_Copytype(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	m_pEffectData_Manager = CEffectData_Manager::GetInstance();

	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;
	m_eRenderGroup = pDesc->eRenderGroup;
	m_pShader = m_pGameInstance->Find_Shader(pDesc->ShaderName);

	return S_OK;
}

void CEffect::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CEffect::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}


void CEffect::Render_DebugImgui()
{
	__super::Render_DebugImgui();

}
void CEffect::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);
}
