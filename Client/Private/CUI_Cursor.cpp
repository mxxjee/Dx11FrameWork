#include "CUI_Cursor.h"
#include "CInventorySlot.h"


USING(Client)
CUI_Cursor::CUI_Cursor(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CPanel(_pDevice,_pDeviceContext)
{
}

CUI_Cursor::CUI_Cursor(const CUI_Cursor& rhs)
    : CPanel(rhs)
{
}

HRESULT CUI_Cursor::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}


HRESULT CUI_Cursor::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    
    m_pUICom->PlayAnim(UIAnimType::SCALE);


    return S_OK;
}



void CUI_Cursor::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CUI_Cursor::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_Cursor::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CUI_Cursor::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CUI_Cursor::Render()
{
    __super::Render();
    return S_OK;
}

CUI_Cursor* CUI_Cursor::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CUI_Cursor* pInstance = new CUI_Cursor(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CUI_Cursor ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CUI_Cursor::Clone(void* pArg)
{
    CUI_Cursor* pInstance = new CUI_Cursor(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CUI_Cursor ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CUI_Cursor::Free()
{
    __super::Free();
}

void CUI_Cursor::Set_SelectSlot(CGameObject* pObj)
{
    CInventorySlot* pSlot = dynamic_cast<CInventorySlot*>(pObj);
    CheckNull(pSlot);

    m_pTargetSlot = pSlot;
}
