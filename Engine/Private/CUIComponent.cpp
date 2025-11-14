#include "CUIComponent.h"
#include "CUI.h"
#include "MathUtils.h"
#include "CGameInstance.h"

CUIComponent::CUIComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext),m_ViewPortHeight(0.f),m_ViewPortWidth(0.f)
{
}

CUIComponent::CUIComponent(const CUIComponent& Prototype)
    :CComponent(Prototype), m_ViewPortWidth(Prototype.m_ViewPortWidth),m_ViewPortHeight(Prototype.m_ViewPortHeight)
{
}

HRESULT CUIComponent::Initialize_Prototype()
{
	m_ViewPortWidth = static_cast<float>(m_pGameInstance->Get_EngineDesc().iWinSizeX);
	m_ViewPortHeight = static_cast<float>(m_pGameInstance->Get_EngineDesc().iWinSizeY);

    return S_OK;
}

HRESULT CUIComponent::Initialize_Copytype(void* pArg)
{
	if (pArg)
	{
		UICOMP_DESC* pDesc = static_cast<UICOMP_DESC*>(pArg);
		for (int i = 0; i < ENUM_TO_UINT(UIAnimType::END); ++i)
		{
			/*값이 채워져있는경우에만 register한다.*/
			if (pDesc->_AnimInfo[i].m_fSpeed != 0)
				Register_Anim((UIAnimType)i,pDesc->_AnimInfo[i]);
		}
			
		if (FAILED(__super::Initialize_Copytype(pArg)))
			return E_FAIL;

	}

	if (m_pOwner)
	{
		m_pUIOwner = dynamic_cast<CUI*>(m_pOwner);
		m_pTransform = m_pOwner->Get_Transform();

	}




	

    return S_OK;
}

HRESULT CUIComponent::Update_Component(_float fTimeDelta)
{
	CheckNullResult(m_pUIOwner, E_FAIL);

	//오너의 애니메이션타입에 따라서 처리

	if (HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_SCALE))
		Update_UIAnim(UIAnimType::SCALE,fTimeDelta);

	if (HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_POSITION))
		Update_UIAnim(UIAnimType::POSITION,fTimeDelta);

	if (HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_ROTATION))
		Update_UIAnim(UIAnimType::ROTATION, fTimeDelta);

	if (HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_ALPHA))
		Update_UIAnim(UIAnimType::ALPHA, fTimeDelta);


	return S_OK;
}

void CUIComponent::Register_Anim(UIAnimType eType, const UIAnimInfo& Info)
{
	CheckTrue(eType == UIAnimType::END);

	m_AnimInfo[ENUM_TO_UINT(eType)] = Info;
	m_AnimInfo[ENUM_TO_UINT(eType)].m_bRegister = true;

	/*오너의 애니메이션타입 추가.*/
	Set_AnimFlag(eType);

}

void CUIComponent::Update_UIAnim(UIAnimType eType, _float fTimeDelta)
{
	auto& anim = m_AnimInfo[ENUM_TO_UINT(eType)];
	if (!anim.m_bPlay) return;


	switch (eType)
	{

	case Engine::UIAnimType::SCALE:
	{
		m_pTransform->ScaleLerp(XMLoadFloat4(&anim.fTarget), anim.m_fSpeed, fTimeDelta);
		_float3 vScale = m_pTransform->Get_Scale_ByFloat3();

		Check_LoopAnimation(eType, anim, _float4(vScale.x, vScale.y, vScale.z, 1.f), 0.01f);
	}
		
		break;

	case Engine::UIAnimType::POSITION:
	{
		{
			//스크린좌표->월드좌표로변환
			anim.fTarget = MathUtils::ScreenToWorld_UI(XMLoadFloat4(&anim.fTarget), m_ViewPortWidth, m_ViewPortHeight);
			m_pTransform->MoveLerp(XMLoadFloat4(&anim.fTarget), anim.m_fSpeed, fTimeDelta);

			_float4 Position;
			XMStoreFloat4(&Position, m_pTransform->Get_State(STATE::POSITION));
			Check_LoopAnimation(eType, anim, Position, 0.01f);
		}
		
		

	}
	break;
	case Engine::UIAnimType::ROTATION:
		m_pTransform->RotateLerp(XMLoadFloat4(&anim.fTarget), anim.m_fSpeed, fTimeDelta);
		break;

	case Engine::UIAnimType::ALPHA:
		//셰이더바인딩
	{
		
		LerpAlpha(anim.fTarget.x, anim.m_fSpeed, fTimeDelta);
		_float4 Color = m_pUIOwner->Get_ARGB();

		Check_LoopAnimation(eType, anim, Color, 0.01f);
	}
	
		break;

	default:
		break;
	}


}

void CUIComponent::LerpAlpha(_float vTarget, _float fLerpSpeed, _float fTimeDelta)
{
	float t = 1.0f - expf(-fTimeDelta * fLerpSpeed);
	_float4 CurrentColor = m_pUIOwner->Get_ARGB();

	float  newAlpha = MathUtils::Lerp(CurrentColor.x, vTarget, t);
	CurrentColor.x = newAlpha;

	m_pUIOwner->Set_ARGB(CurrentColor);
}

void CUIComponent::Set_OwnerTransform(CTransform* ptransform)
{
	CheckNull(ptransform);
	m_pTransform = ptransform;
}



void CUIComponent::PlayAnim(UIAnimType _AnimType, _float4 vStart, _float4 vTarget, float fSpeed, bool bLoop)
{
	CheckTrue(_AnimType == UIAnimType::END);

	m_AnimInfo[ENUM_TO_UINT(_AnimType)].m_bRegister = true;

	m_AnimInfo[ENUM_TO_UINT(_AnimType)].m_bPlay = true;
	m_AnimInfo[ENUM_TO_UINT(_AnimType)].m_bEnd = false;

	m_AnimInfo[ENUM_TO_UINT(_AnimType)].fStart = vStart;
	m_AnimInfo[ENUM_TO_UINT(_AnimType)].fTarget = vTarget;

	m_AnimInfo[ENUM_TO_UINT(_AnimType)].m_fSpeed = fSpeed;
	m_AnimInfo[ENUM_TO_UINT(_AnimType)].bLoop = bLoop;

	/*오너의 애니메이션타입 추가.*/
	Set_AnimFlag(_AnimType);

	Apply_StartValue(_AnimType, m_AnimInfo[ENUM_TO_UINT(_AnimType)]);

}

void CUIComponent::PlayAnim(UIAnimType _AnimType)
{
	CheckTrue(_AnimType == UIAnimType::END);


	//해당타입에 애니메이션 클립등록안했으면 재생X
	CheckFalse(m_AnimInfo[ENUM_TO_UINT(_AnimType)].m_bRegister);
	

	/*오너의 애니메이션타입 추가.*/
	Set_AnimFlag(_AnimType);

	Apply_StartValue(_AnimType, m_AnimInfo[ENUM_TO_UINT(_AnimType)]);


}

void CUIComponent::Apply_StartValue(UIAnimType eType, UIAnimInfo& Info)
{
	switch (eType)
	{
	case Engine::UIAnimType::SCALE:
		m_pTransform->Set_Scale(Info.fStart);
		break;
	case Engine::UIAnimType::POSITION:
	{
		Info.fStart = MathUtils::ScreenToWorld_UI(XMLoadFloat4(&Info.fStart), m_ViewPortWidth, m_ViewPortHeight);
		m_pTransform->Set_State(STATE::POSITION, Info.fStart);
	}
		break;
	case Engine::UIAnimType::ROTATION:
		m_pTransform->Rotation(_float3(Info.fStart.x, Info.fStart.y, Info.fStart.z));
		break;

	case Engine::UIAnimType::ALPHA:
	{
		_float4 Color = m_pUIOwner->Get_ARGB();
		m_pUIOwner->Set_ARGB(_float4(Info.fStart.x, Color.y, Color.z, Color.w));
	}

	default:
		break;
	}

	m_AnimInfo[ENUM_TO_UINT(eType)].m_bPlay = true;
}



void CUIComponent::Check_LoopAnimation(UIAnimType eType, UIAnimInfo& Info, _float4 vCurrent, _float Epslion)
{

	//오차범위내면 종료.
	if (eType != UIAnimType::ALPHA)
	{
		if (fabs(vCurrent.x - Info.fTarget.x) < Epslion &&
			fabs(vCurrent.y - Info.fTarget.y) < Epslion &&
			fabs(vCurrent.z - Info.fTarget.z) < Epslion)
		{
			if (Info.bLoop)
				Apply_StartValue(eType, Info);

			else
			{
				Info.m_bPlay = false;
				Info.m_bEnd = true;
			}

		}

	}

	//알파값은 x처리..
	else
	{
		if (fabs(vCurrent.x - Info.fTarget.x) < Epslion)
		{
			if (Info.bLoop)
				Apply_StartValue(eType, Info);

			else
			{
				Info.m_bPlay = false;
				Info.m_bEnd = true;
			}

		}
	}
}

void CUIComponent::Set_AnimFlag(UIAnimType eType)
{
	switch (eType)
	{
	case UIAnimType::SCALE:
		if(!HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_SCALE))
			m_UIAnimType |= UIAnimFlag::UI_ANIM_SCALE;
		break;

	case UIAnimType::POSITION:
		if (!HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_POSITION))
			m_UIAnimType |= UIAnimFlag::UI_ANIM_POSITION;
		break;


	case UIAnimType::ROTATION:
		if (!HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_ROTATION))
			m_UIAnimType |= UIAnimFlag::UI_ANIM_ROTATION;
		break;

	case UIAnimType::ALPHA:
		if (!HasFlag(m_UIAnimType, UIAnimFlag::UI_ANIM_ALPHA))
			m_UIAnimType |= UIAnimFlag::UI_ANIM_ALPHA;
		break;

	}

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
