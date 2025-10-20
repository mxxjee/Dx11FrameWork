#include "CUI.h"
#include "CTransform.h"
#include "MathUtils.h"
#include "CGameInstance.h"
#include "CShader.h"


CUI::CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CQuad(pDevice,pContext)
{
}

CUI::CUI(const CUI& rhs)
    :CQuad(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto& vector = m_pGameInstance->Get_Viewports();

 
    return S_OK;
}

HRESULT CUI::Initialize_Copytype(void* pArg)
{
    UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    /*transform생성전에 ui좌표맞게 설정해주기*/
    if(FAILED(Ready_Resource(pArg)))
        return E_FAIL;

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    m_pUICom->Set_OwnerTransform(m_pTransformCom);


    return S_OK;
}

HRESULT CUI::Ready_Resource(void* pArg)
{
    UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
    CTransform::TRANSFORM_DESC* pTransDesc = static_cast<CTransform::TRANSFORM_DESC*>(pDesc->TransformDesc);
    _vector vScreenPos = XMVectorSet(pDesc->fX, pDesc->fY, pDesc->Depth, 1.f);

    auto& vector = m_pGameInstance->Get_Viewports();
    pTransDesc->vLocalPosition = MathUtils::ScreenToWorld_UI(vScreenPos, vector[0].Width, vector[0].Height);
    pTransDesc->vLocalScale = _float4(pDesc->fSizeX, pDesc->fSizeY, 1.f, 1.f);
    

    m_iIdx = pDesc->iIdx;

    return S_OK;
}

void CUI::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CUI::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);
    m_pUICom->Update_Component(fTimeDelta);
}

void CUI::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

void CUI::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
   
}

HRESULT CUI::Render()
{
 
    __super::Render();
    return S_OK;
}

_float CUI::Get_Depth()
{
    CheckNullResult(m_pTransformCom, 0.f);

    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION, TransformScope::WORLD);
    
    return XMVectorGetZ(vPos);


}
 

HRESULT CUI::Ready_Components(void* pArg)
{
    UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    CheckNullResult(pDesc, E_FAIL);

	CUIComponent::UICOMP_DESC* pUICompDesc = static_cast<CUIComponent::UICOMP_DESC*>(pDesc->UICompDesc);
	if (pUICompDesc)
		pUICompDesc->pOwner = this;

	CComponent* pUIComp = dynamic_cast<CUIComponent*>(m_pGameInstance->Clone_Prototype
	(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"UI"), pDesc->UICompDesc));

	if (FAILED(Add_Component(COMPONENT_TYPE::UI, pUIComp, reinterpret_cast<CComponent**>(&m_pUICom))))
		return E_FAIL;

    

    


    return S_OK;
}

void CUI::Free()
{
    __super::Free();
    Safe_Release(m_pUICom);
}

void CUI::OnActivated(bool isActive)
{
    if (isActive)
    {
        //바로 활성화.
        m_bActive = true;

        if (m_ActiveAnim[0])
            m_ActiveAnim[0]();

        

    }

    else
    {
      
        if (m_ActiveAnim[1]) //특정조건 이후 비활성화.
            m_ActiveAnim[1]();

        else
            m_bActive = false;
       
    }
}
