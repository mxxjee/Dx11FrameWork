#include "CUI.h"
#include "CTransform.h"
#include "MathUtils.h"
#include "CGameInstance.h"


CUI::CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CUI::CUI(const CUI& rhs)
    :CGameObject(rhs),
    m_ViewMatrix{rhs.m_ViewMatrix},
    m_ProjMatrix{rhs.m_ProjMatrix}
{
}

HRESULT CUI::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto& vector = m_pGameInstance->Get_Viewports();

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(
        vector[0].Width,
        vector[0].Height,
        0.1f,1.f));

    return S_OK;
}

HRESULT CUI::Initialize_Copytype(void* pArg)
{
    UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

       
	CTransform::TRANSFORM_DESC* pTransDesc = static_cast<CTransform::TRANSFORM_DESC*>(pDesc->TransformDesc);
	_vector vScreenPos = XMVectorSet(pDesc->fX, pDesc->fY, 0.1f, 1.f);
    
    auto& vector = m_pGameInstance->Get_Viewports();
    pTransDesc->vLocalPosition=MathUtils::ScreenToWorld_UI(vScreenPos, vector[0].Width,vector[0].Height);


    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    return S_OK;
}

void CUI::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CUI::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

HRESULT CUI::Render()
{
    return S_OK;
}


void CUI::Free()
{
    __super::Free();
}
