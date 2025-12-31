#include "CInventorySlot.h"

USING(Client)
CInventorySlot::CInventorySlot(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CSlot(_pDevice,_pDeviceContext)
{
}

CInventorySlot::CInventorySlot(const CInventorySlot& rhs)
    : CSlot(rhs)
{
}

HRESULT CInventorySlot::Initialize_Prototype()
{

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;



    return S_OK;
}

HRESULT CInventorySlot::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_TexKeys.resize(ENUM_TO_UINT(State::END));
    m_TexKeys[ENUM_TO_UINT(State::IDLE)] = L"InvenSlot_NonSelect";
    m_TexKeys[ENUM_TO_UINT(State::HOVER)] = L"ItemSlot_Quick_NonSelect";
    m_TexKeys[ENUM_TO_UINT(State::SELECT)] = L"ItemSlot_Quick_NonSelect";

    m_eState = State::IDLE;

    return S_OK;
}

void CInventorySlot::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CInventorySlot::Update(_float fTimeDelta)
{
    //매프레임마다 인벤토리매니저 정보세팅

    __super::Update(fTimeDelta);

}

void CInventorySlot::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

void CInventorySlot::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CInventorySlot::Render()
{
    __super::Render();
    return S_OK;
}

CInventorySlot* CInventorySlot::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInventorySlot* pInstance = new CInventorySlot(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInventorySlot ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CInventorySlot::Clone(void* pArg)
{
    CInventorySlot* pInstance = new CInventorySlot(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CFadeScreen ");
        Safe_Release(pInstance);

    }
    return pInstance;
}


void CInventorySlot::Enter_State()
{
    if (m_ePreState != m_eState)
    {
        switch (m_eState)
        {
        case Client::CSlot::IDLE:
        {
            if (!m_bQuickSlot)
                Set_Texture(m_TexKeys[ENUM_TO_UINT(State::IDLE)]);

            else
                Set_Texture(L"ItemSlot_Quick_NonSelect");

        }

        break;
        case Client::CSlot::HOVER:
        {
            if (!m_bQuickSlot)
                Set_Texture(m_TexKeys[ENUM_TO_UINT(State::HOVER)]);

            else
                Set_Texture(L"ItemSlot_Quick_Select");
        }
        break;

        default:
            break;
        }

        m_ePreState = m_eState;
    }
    

}

void CInventorySlot::Update_State()
{
    switch (m_eState)
    {
    case Client::CSlot::IDLE:
        break;
    case Client::CSlot::HOVER:
        break;
    case Client::CSlot::SELECT:
        break;
    case Client::CSlot::END:
        break;
    default:
        break;
    }
}

void CInventorySlot::Free()
{
    __super::Free();
}