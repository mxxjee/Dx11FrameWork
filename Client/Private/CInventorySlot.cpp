#include "CInventorySlot.h"
#include "CItem_Manager.h"

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
    m_TexKeys[ENUM_TO_UINT(State::HOVER)] = L"InvenSlot_Select";
    m_TexKeys[ENUM_TO_UINT(State::SELECT)] = L"InvenSlot_Select";

    m_eState = State::IDLE;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;


    return S_OK;
}

void CInventorySlot::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    CheckNull(m_pItemPanel);

    if(m_pItemPanel->Is_Active())
        m_pItemPanel->Update_Priority(fTimeDelta);

}

void CInventorySlot::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pItemPanel);


    if (m_pItemPanel->Is_Active())
        m_pItemPanel->Update(fTimeDelta);

}

void CInventorySlot::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    CheckNull(m_pItemPanel);

    if (m_pItemPanel->Is_Active())
        m_pItemPanel->Update_Late(fTimeDelta);
}

void CInventorySlot::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    CheckNull(m_pItemPanel);

    if (m_pItemPanel->Is_Active())
        m_pItemPanel->Update_Render(fTimeDelta);
}

HRESULT CInventorySlot::Render()
{
    __super::Render();
    CheckNullResult(m_pItemPanel, E_FAIL);

    if (m_pItemPanel->Is_Active())
        m_pItemPanel->Render();
    return S_OK;
}

HRESULT CInventorySlot::Ready_PartObject()
{

    CUI::tagUIDesc        Desc = {};

    _float fSizeX = 120 * 0.7f;
    _float fSizeY = 120 * 0.7f;

    Desc.ObjTag = tag+L"_ItemSlot";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    Desc.TextureKey = L"";

    Desc.iIdx = 0;

    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;
    Desc.fX = m_UIInitInfo.fX;
    Desc.fY = m_UIInitInfo.fY;

    Desc.Depth = m_UIInitInfo.Depth-0.01f;
    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    Desc.TransformDesc = &TransDesc;

    //AlphaAnimµî·Ï
    CUIComponent::UICOMP_DESC UIDesc = {};
    Desc.UICompDesc = &UIDesc;

    m_pItemPanel = dynamic_cast<CPanel*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Desc));
    if(m_pItemPanel)
        m_pItemPanel->Set_Active(false);

    return S_OK;
}

void CInventorySlot::Update_Item(ItemType eType)
{
    if (eType == ItemType::END)
    {
        m_pItemPanel->Set_Texture(L"");
        m_pItemPanel->Set_Active(false);
        
        m_eState = State::IDLE;
        Set_Texture(m_TexKeys[ENUM_TO_UINT(State::IDLE)]);
       
    }
    else
    {
        ITMINFO* pInfo = CItem_Manager::GetInstance()->Get_ItemInfo(eType);

        m_pItemPanel->Set_Texture(pInfo->TexKey);
        m_pItemPanel->Set_Active(true);

    }

    m_eType = eType;
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
        case Client::CSlot::SELECT:
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

void CInventorySlot::Set_QuickSlot(bool b)
{
   m_bQuickSlot = b;

   if (b)
   {
       if (m_eState == State::HOVER || m_eState == State::SELECT)
       {
           if (!m_bQuickSlot)
               Set_Texture(m_TexKeys[ENUM_TO_UINT(State::HOVER)]);

           else
               Set_Texture(L"ItemSlot_Quick_Select");
       }
   }

   else
   {
       Set_Texture(m_TexKeys[ENUM_TO_UINT(State::IDLE)]);
   }
  
}

void CInventorySlot::Free()
{
    Safe_Release(m_pItemPanel);
    __super::Free();
}