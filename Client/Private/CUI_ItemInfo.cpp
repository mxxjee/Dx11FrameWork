#include "CUI_ItemInfo.h"
#include "CFontUI.h"
#include "ColorUtils.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CItem_Manager.h"



USING(Client)

CUI_ItemInfo::CUI_ItemInfo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CPanel(_pDevice, _pDeviceContext)
{
}

CUI_ItemInfo::CUI_ItemInfo(const CUI_ItemInfo& rhs)
    : CPanel(rhs)
{
}

HRESULT CUI_ItemInfo::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CUI_ItemInfo::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;


    return S_OK;
}

void CUI_ItemInfo::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CUI_ItemInfo::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_ItemInfo::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CUI_ItemInfo::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CUI_ItemInfo::Render()
{
    __super::Render();

    if (m_pFontUI->Is_Active())
        m_pFontUI->Render();
    return S_OK;;
}

HRESULT CUI_ItemInfo::Ready_PartObject()
{
    CFontUI::FONTUI_DESC FontUIDesc;
    FontUIDesc.FontName = L"Dialogue_Default";
    FontUIDesc.vDefaultFontColor = COLOR_GOLD;
    FontUIDesc.ObjTag = L"ItemDesc_Text";
    FontUIDesc.fSizeX = 0.5f;
    FontUIDesc.fSizeY = 0.5f;
    FontUIDesc.m_bUseTypingEffect = false;
    FontUIDesc.vPosition = _float2(m_UIInitInfo.fX, m_UIInitInfo.fY+100.f);

    FontUIDesc.fX = 0.f;
    FontUIDesc.fY = 0.f;
    FontUIDesc.Depth =m_UIInitInfo.Depth;

    FontUIDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    FontUIDesc.TransformDesc = &TransDesc;

    CUIComponent::UICOMP_DESC  UIDesc = {};
    FontUIDesc.UICompDesc = &UIDesc;

    m_pFontUI = dynamic_cast<CFontUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FontUI"), &FontUIDesc));
 
    if (m_pFontUI)
    {
        ITMINFO* pShield = CItem_Manager::GetInstance()->Get_ItemInfo(ItemType::SHIELD);
        m_pFontUI->Set_Text(pShield->ItemDesc);


    }
    
    /*if (m_pFontUI)
        m_pFontUI->Set_Active(false);*/

    return S_OK;
}

void CUI_ItemInfo::Update_ItemInfo(ItemType eType)
{
    ITMINFO* pInfo = CItem_Manager::GetInstance()->Get_ItemInfo(eType);
    if (pInfo)
    {
        Set_Texture(pInfo->TexKey);
        m_pFontUI->Set_Text(pInfo->InfoDesc);

    }

}

CUI_ItemInfo* CUI_ItemInfo::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CUI_ItemInfo* pInstance = new CUI_ItemInfo(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CUI_ItemInfo ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CUI_ItemInfo::Clone(void* pArg)
{
    CUI_ItemInfo* pInstance = new CUI_ItemInfo(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CUI_ItemInfo ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CUI_ItemInfo::Free()
{
    Safe_Release(m_pFontUI);
    __super::Free();
}
