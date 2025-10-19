#include "CUIComponent.h"
#include "CUI.h"
#include "MathUtils.h"


CUIComponent::CUIComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{
}

CUIComponent::CUIComponent(const CUIComponent& Prototype)
    :CComponent(Prototype)
{
}

HRESULT CUIComponent::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CUIComponent::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	if (m_pOwner)
	{
		CUI* m_pUIOwner = dynamic_cast<CUI*>(m_pOwner);
		m_pTransform = m_pOwner->Get_Transform();

	}

	UICOMP_DESC* pDesc = static_cast<UICOMP_DESC*>(pArg);

	for (int i = 0; i < ENUM_TO_UINT(UIAnimType::COUNT); ++i)
		m_AnimInfo[i] = pDesc->_AnimInfo[i];


	

    return S_OK;
}

HRESULT CUIComponent::Update_Component(_float fTimeDelta)
{
	CheckNullResult(m_pUIOwner, E_FAIL);

	//오너의 애니메이션타입에 따라서 처리
	auto type = m_pUIOwner->Get_UIAnimType();

	if (HasFlag(type, UIAnimType::SCALE))
		Update_UIAnim(UIAnimType::SCALE,fTimeDelta);

	if (HasFlag(type, UIAnimType::POSITION))
		Update_UIAnim(UIAnimType::POSITION,fTimeDelta);

	if (HasFlag(type, UIAnimType::ROTATION))
		Update_UIAnim(UIAnimType::ROTATION, fTimeDelta);

	if (HasFlag(type, UIAnimType::ALPHA))
		Update_UIAnim(UIAnimType::ALPHA, fTimeDelta);


	return S_OK;
}

void CUIComponent::Update_UIAnim(UIAnimType eType, _float fTimeDelta)
{
	auto& anim = m_AnimInfo[ENUM_TO_UINT(eType)];
	if (!anim.m_bPlay) return;


	switch (eType)
	{
	case Engine::UIAnimType::NONE:
		break;
	case Engine::UIAnimType::SCALE:
		m_pTransform->ScaleLerp(XMLoadFloat4(&anim.fTarget), anim.m_fSpeed, fTimeDelta);
		break;

	case Engine::UIAnimType::POSITION:
		m_pTransform->MoveLerp(XMLoadFloat4(&anim.fTarget), anim.m_fSpeed, fTimeDelta);
		break;

	case Engine::UIAnimType::ROTATION:
		m_pTransform->RotateLerp(XMLoadFloat4(&anim.fTarget), anim.m_fSpeed, fTimeDelta);
		break;

	case Engine::UIAnimType::ALPHA:
		//셰이더바인딩
		LerpAlpha(anim.fTarget.x, anim.m_fSpeed, fTimeDelta);
		break;

	default:
		break;
	}

}

void CUIComponent::LerpAlpha(_float vTarget, _float fLerpSpeed, _float fTimeDelta)
{
	float t = 1.0f - expf(-fTimeDelta * fLerpSpeed);
	//float 
}



CUIComponent* CUIComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CUIComponent* pInstance = new CUIComponent(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIComponent");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent* CUIComponent::Clone(void* pArg)
{
	CUIComponent* pInstance = new CUIComponent(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Create : CUIComponent");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CUIComponent::Free()
{
	__super::Free();
}
