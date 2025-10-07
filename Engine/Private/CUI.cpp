#include "CUI.h"
#include "CTransform.h"
#include "MathUtils.h"
#include "CGameInstance.h"
#include "CConstantBuffer.h"

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

    m_eRenderGroup = RENDERGROUP::UI;
       
	CTransform::TRANSFORM_DESC* pTransDesc = static_cast<CTransform::TRANSFORM_DESC*>(pDesc->TransformDesc);
	_vector vScreenPos = XMVectorSet(pDesc->fX, pDesc->fY, pDesc->Depth, 1.f);
    
    auto& vector = m_pGameInstance->Get_Viewports();
    pTransDesc->vLocalPosition=MathUtils::ScreenToWorld_UI(vScreenPos, vector[0].Width,vector[0].Height);
    pTransDesc->vLocalScale = _float4(pDesc->fSizeX, pDesc->fSizeY, 1.f, 1.f);


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
    CGameObject::Update(fTimeDelta);
    CheckNull(m_pTransformCom);

    m_transformData.matworld = m_pTransformCom->Get_World();
    m_transformData.view = m_pGameInstance->GetViewMatrix(true);
    m_transformData.proj = m_pGameInstance->GetProjMatrix(true);

    m_Pipeline.constantBuffer->CopyData(m_transformData);
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


void CUI::Free()
{
    __super::Free();
}
