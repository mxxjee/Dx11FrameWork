#include "CUI_Credit.h"
#include "CGameInstance.h"

#include "Client_Defines.h"
USING(Client)

CUI_Credit::CUI_Credit(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CPanel(_pDevice, _pDeviceContext)
{
}

CUI_Credit::CUI_Credit(const CUI_Credit& rhs)
    : CPanel(rhs)
{
}

HRESULT CUI_Credit::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Credit::Initialize_Copytype(void* pArg)
{
    /*°ª¼¼ÆÃ */

    CREDIT_DSEC* pDesc = static_cast<CREDIT_DSEC*>(pArg);
    m_fSpeed = pDesc->fSpeed;

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;



    return S_OK;
}

void CUI_Credit::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CUI_Credit::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);



}

void CUI_Credit::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    _vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);
    _float Y = XMVectorGetY(vCurPos) + (fTimeDelta * m_fSpeed);
    vCurPos=XMVectorSetY(vCurPos, Y);

    m_pTransformCom->Set_State(STATE::POSITION, vCurPos);

}

void CUI_Credit::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);

}

HRESULT CUI_Credit::Render()
{
    __super::Render();

    return S_OK;
}

CUI_Credit* CUI_Credit::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CUI_Credit* pInstance = new CUI_Credit(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CUI_Credit ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CUI_Credit::Clone(void* pArg)
{
    CUI_Credit* pInstance = new CUI_Credit(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CUI_Credit ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CUI_Credit::Free()
{
    __super::Free();
}
