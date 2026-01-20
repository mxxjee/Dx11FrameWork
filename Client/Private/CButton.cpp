#include "CButton.h"
#include "CInput_Manager.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"


USING(Client)
CButton::CButton(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CPanel(_pDevice, _pDeviceContext),
    m_pInput_Manager(CInput_Manager::GetInstance())
{
    Safe_AddRef(m_pInput_Manager);
}

CButton::CButton(const CButton& rhs)
    :CPanel(rhs),m_pInput_Manager(rhs.m_pInput_Manager)
{
    Safe_AddRef(m_pInput_Manager);
}

HRESULT CButton::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CButton::Initialize_Copytype(void* pArg)
{

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    if (pArg)
    {
        BUTTON_DESC* pDesc = static_cast<BUTTON_DESC*>(pArg);
        m_eKeyCode = pDesc->eKeyCode;
        m_OnSelectAction = pDesc->SelectActionFunc;

    }

    return S_OK;
}

void CButton::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CButton::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CButton::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    if (m_bHover)
    {
        if (m_pInput_Manager->IsKeyPressed(m_eKeyCode))
            if (m_OnSelectAction)
                m_OnSelectAction();

    }
}

void CButton::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CButton::Render()
{
    CheckFalseResult(m_bActive,E_FAIL);

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;          //IA´Ü°è

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

CButton* CButton::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CButton* pInstance = new CButton(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CButton ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CButton::Clone(void* pArg)
{
    CButton* pInstance = new CButton(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CButton ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CButton::Free()
{
    __super::Free();
    Safe_Release(m_pInput_Manager);
}
