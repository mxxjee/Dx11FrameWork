#include "CUI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice,pContext)
{
}

CUI::CUI(const CUI* rhs)
    :CGameObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CUI::Initialize_Copytype(void* pArg)
{
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
