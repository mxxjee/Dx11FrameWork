#include "CInteractionObject.h"
#include "CGameInstance.h"
#include "CBody.h"
#include "CPlayer.h"


#include "CAnimBody.h"

#include "CInteraction_Manager.h"
#include "MathUtils.h"
#include "CInteraction_TriggerBox.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"

#include "CGameManager.h"

#include "CNavigation.h"
#include "CCell.h"



USING(Client)
CInteractionObject::CInteractionObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CContainerObject(pDevice, pContext)
{
}

CInteractionObject::CInteractionObject(const CInteractionObject& rhs)
    : CContainerObject(rhs)
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
    Interaction_DESC* pDesc = static_cast<Interaction_DESC*>(pArg);


    m_eRenderGroup = pDesc->eRenderGroup;
    m_eInteractionType = pDesc->eInteractionType;
    m_eInteractionType = pDesc->eInteract_Object_Type;
    m_SceneName = pDesc->SceneName;
    m_fTargetDistance = pDesc->fTargetDistance;


    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;


    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;

    m_pPlayer = CGameManager::GetInstance()->Get_MainPlayer();

    //vPos.m128_f32 0x00000070d30fd1a0 {21.9783173, 10.5500002, 28.0775661, 0.00000000}
    m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));



    return S_OK;
}

void CInteractionObject::Update_Priority(_float fTimeDelta)
{
   

    __super::Update_Priority(fTimeDelta);
}

void CInteractionObject::Update(_float fTimeDelta)
{
    
    __super::Update(fTimeDelta);
	if (m_bPhysics)
	{
		m_pTransformCom->UpdateImpulse(fTimeDelta, nullptr,true);
         
         /*XMStoreFloat3(&Pos, m_pTransformCom->Get_State(STATE::POSITION));
         Pos.y -= 9.8f * 0.016f;*/
        
         ////현재셀의 지면높이가져오기 
        
        XMStoreFloat3(&Pos, m_pTransformCom->Get_State(STATE::POSITION));

         m_pNavigationCom->Set_CurrentIdx(XMLoadFloat3(&Pos));
        
         _float CellY = m_pNavigationCom->GetHeight(XMLoadFloat3(&Pos));
         if (Pos.y <= CellY)
         {
             m_pCollider->Set_Trigger(false);
             m_bPhysics = false;
             Pos.y = CellY;
             m_pTransformCom->Set_State(STATE::POSITION,
                 m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));
        
             return;
         }
        
        
	}
}

void CInteractionObject::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

  
    m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CInteractionObject::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(m_eRenderGroup), this);

#ifdef _DEBUG
    if (CGameInstance::m_bDrawDebug)
    {
        if (FAILED(m_pGameInstance->Add_DebugComponent(m_pCollider)))
            return;
    }
#endif
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
    __super::Free();

    Safe_Release(m_pCollider);
    Safe_Release(m_pBody);
    Safe_Release(m_pNavigationCom);

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


    ///Navigation 생성
    CComponent::COMPONENT_DESC Desc;
    Desc.pOwner = this;

    CComponent* pNavigation = dynamic_cast<Engine::CNavigation*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"Navigation"),
        &Desc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::NAVIGATION,
        pNavigation,
        reinterpret_cast<CComponent**>(&m_pNavigationCom)
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

        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));
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
        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));
    }



    return S_OK;
}

bool CInteractionObject::IsInteratable()
{
    return false;
}

void CInteractionObject::Enter_InteractRange()
{



}

void CInteractionObject::Stay_InteractRange(_float fTimeDelta)
{
}

void CInteractionObject::Exit_InteractRange()
{

}

void CInteractionObject::Enter_Interaction()
{
    m_bCall_Exit_Interaction = false;

}

void CInteractionObject::Stay_Interaction(_float fTimeDelta)
{
}

void CInteractionObject::Exit_Interaction()
{
    CheckTrue(m_bCall_Exit_Interaction);
    m_bCall_Exit_Interaction = true;
}

_int CInteractionObject::Get_Interaction_Priority()
{
    return InteractionType::OBJECT;
}

void CInteractionObject::Set_InteractionMode(bool b)
{
    m_bInteraction = b;


    if (b)
    {
        m_pCollider->Set_Trigger(b);

    }


}

void CInteractionObject::Throw()
{
    CheckNull(m_pSocketMatrix);

    _float3 ThrowDir;
    CTransform* pTransform = m_pPlayer->Get_Transform();
    CheckNull(pTransform);


    _vector vDir = (pTransform->Get_State(STATE::LOOK) + m_pTransformCom->Get_State(STATE::UP));

    XMStoreFloat3(&ThrowDir, vDir);

    m_pSocketMatrix = nullptr;

    m_pTransformCom->AddImpulse(0.4f, ThrowDir, nullptr,true);
    m_pNavigationCom->Set_CurrentIdx(pTransform->Get_State(STATE::POSITION));
    m_bPhysics = true;
    XMStoreFloat3(&Pos, m_pTransformCom->Get_State(STATE::POSITION));
}
