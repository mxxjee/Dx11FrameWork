#include "CInteractionObject.h"
#include "CGameInstance.h"
#include "CBody.h"

#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"
#include "CAnimBody.h"

USING(Client)
CInteractionObject::CInteractionObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CContainerObject(pDevice,pContext)
{
}

CInteractionObject::CInteractionObject(const CInteractionObject& rhs)
    :CContainerObject(rhs)
{
}

HRESULT CInteractionObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInteractionObject::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;


    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;

    Interaction_DESC* pDesc = static_cast<Interaction_DESC*>(pArg);
    
    m_eInteractionType = pDesc->eInteractionType;
    m_eInteractionType = pDesc->eInteract_Object_Type;


    return S_OK;
}

void CInteractionObject::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CInteractionObject::Update(_float fTimeDelta)
{

    __super::Update(fTimeDelta);
 
}

void CInteractionObject::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CInteractionObject::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(m_eRenderGroup), this);

}

HRESULT CInteractionObject::Render()
{
    __super::Render();

    return S_OK;
}

CInteractionObject* CInteractionObject::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInteractionObject* pInstance = new CInteractionObject(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInteractionObject ");
        Safe_Release(pInstance);

    }



    return pInstance;

}

CGameObject* CInteractionObject::Clone(void* pArg)
{
    CInteractionObject* pInstance = new CInteractionObject(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CInteractionObject ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CInteractionObject::Free()
{
}

HRESULT CInteractionObject::Ready_Components(void* pArg)
{
    Interaction_DESC* Interaction_Desc = static_cast<Interaction_DESC*>(pArg);

    //AABB콜라이더생성
    //////////////Boxcollider추가
    CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"BoxCollider"),
        Interaction_Desc->pColliderComp)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::BOX_COLLIDER,
        pCollider,
        reinterpret_cast<CComponent**>(&m_pCollider)
    )))
        return E_FAIL;


    return S_OK;
}

HRESULT CInteractionObject::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);

    Interaction_DESC* pDesc = static_cast<Interaction_DESC*>(pArg);
    //애니메이션 사용하는 인터렉션 오브젝트라면, 애님바디 할당
    if (pDesc->bAnimated)
    {
        CAnimBody::ANIMBODY_DESC pAnimBodyDesc;
        pAnimBodyDesc.eRenderGroup = m_eRenderGroup;
        pAnimBodyDesc.pOwner = this;
        pAnimBodyDesc.modelName = pDesc->ModelName;
        pAnimBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
        pAnimBodyDesc.pParentState = nullptr;
        pAnimBodyDesc.ObjTag = pDesc->ObjTag + L"_body";

        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"AnimBody"), L"Part_Body", &pAnimBodyDesc)))
            return E_FAIL;

    }

    //아니라면 .static body할당
    {
        CBody::BODY_DESC pBodyDesc;
        pBodyDesc.eRenderGroup = m_eRenderGroup;
        pBodyDesc.pOwner = this;
        pBodyDesc.modelName = pDesc->ModelName;
        pBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
        pBodyDesc.ObjTag = pDesc->ObjTag + L"_body";

        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"StaticBody"), L"Part_Body", &pBodyDesc)))
            return E_FAIL;
    }
    
    return S_OK;
}
