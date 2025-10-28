#include "CModelObject.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CShader.h"


CModelObject::CModelObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CModelObject::CModelObject(const CModelObject& rhs)
    :CGameObject(rhs)
{
}

HRESULT CModelObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelObject::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

    return S_OK;

}

void CModelObject::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CModelObject::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);
}

void CModelObject::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CModelObject::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
   m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CModelObject::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    if (FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;

    if (FAILED(m_pModelComp->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelObject::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;



    return S_OK;
}

CModelObject* CModelObject::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CModelObject* pInstance = new CModelObject(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CModelObject ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CModelObject::Clone(void* pArg)
{
    CModelObject* pInstance = new CModelObject(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CModelObject ");
        Safe_Release(pInstance);

    }
    return pInstance;
}


HRESULT CModelObject::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);

    CComponent* pModel = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(pModelDesc->modelName + L"_Model"), pArg));
    if (FAILED(__super::Add_Component(COMPONENT_TYPE::MODEL, pModel, (CComponent**)&m_pModelComp)))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelObject::Ready_Resource(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModel_dsc = static_cast<MODELOBJECT_DESC*>(pArg);

    m_eRenderGroup = pModel_dsc->eRenderGroup;
    m_ShaderName = pModel_dsc->ShaderName;
    m_passName = pModel_dsc->passName;


    m_pShader = m_pGameInstance->Find_Shader(pModel_dsc->ShaderName);
    Safe_AddRef(m_pShader);



    return S_OK;
}

void CModelObject::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
    Safe_Release(m_pModelComp);

}
