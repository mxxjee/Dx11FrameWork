#include "CFontUI.h"
#include "Client_Defines.h"
#include "CFontComponent.h"

USING(Client)

CFontUI::CFontUI(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CUI(_pDevice,_pDeviceContext)
{
}

CFontUI::CFontUI(const CFontUI& rhs)
	:CUI(rhs)
{
}

HRESULT CFontUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CFontUI::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	if (FAILED(CFontUI::Ready_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

void CFontUI::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CFontUI::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CFontUI::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	CheckNull(m_pFontComp);


	m_pFontComp->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), m_vARGB.x,fTimeDelta);


}

void CFontUI::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);

}

HRESULT CFontUI::Render()
{
	if (m_pFontComp)
		m_pFontComp->Render();

	return S_OK;
}

void CFontUI::Set_FontStartFunction(function<void()> func)
{
	CheckNull(m_pFontComp);
	CheckNull(func);
	m_pFontComp->Set_StartFunction(func);

}

void CFontUI::Set_FontEndFunction(function<void()> func)
{
	CheckNull(m_pFontComp);
	CheckNull(func);
	m_pFontComp->Set_EndFunction(func);
}

void CFontUI::Set_Text(const wstring& Text)
{
	m_pFontComp->Set_Text(Text.c_str());

}

HRESULT CFontUI::Ready_Component(void* pArg)
{
	FONTUI_DESC* pDesc = static_cast<FONTUI_DESC*>(pArg);
	CheckNullResult(pDesc, E_FAIL);

	CFontComponent::Font_Desc pFontCompDesc;
	pFontCompDesc.FontName = pDesc->FontName;
	pFontCompDesc.pOwner = this;
	pFontCompDesc.vDefaultFontColor = pDesc->vDefaultFontColor;
	pFontCompDesc.fScale = pDesc->fSizeX;
	pFontCompDesc.fRotation = pDesc->fRotation;
	pFontCompDesc.vPosition = pDesc->vPosition;


	m_pFontComp = dynamic_cast<CFontComponent*>(m_pGameInstance->Clone_Prototype
	(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"Font"), &pFontCompDesc));

	if (FAILED(Add_Component(COMPONENT_TYPE::FONT, m_pFontComp, reinterpret_cast<CComponent**>(&m_pFontComp))))
		return E_FAIL;

	
	m_pFontComp->Set_TypingEffect(pDesc->m_bUseTypingEffect);
	m_pFontComp->Set_TypingTime(pDesc->m_fTypingTime);

	return S_OK;
}

CFontUI* CFontUI::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CFontUI* pInstance = new CFontUI(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CFontUI ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CFontUI::Clone(void* pArg)
{
	CFontUI* pInstance = new CFontUI(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CFontUI ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CFontUI::Free()
{
	__super::Free();
	Safe_Release(m_pFontComp);

}
