#include "CNPC.h"
#include "CPlayer.h"
#include "Client_Defines.h"
#include "CModel.h"
#include "CAnimBody.h"
#include "CCamera_Base.h"
#include "CInteraction_Manager.h"
#include "CNavigation.h"
#include "CBoxColliderComponent.h"
#include "CBounding_AABB.h"


USING(Client)
CNPC::CNPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CAnimModelObject(pDevice, pContext)
{
}

CNPC::CNPC(const CNPC& rhs)
    : CAnimModelObject(rhs)
{
}

HRESULT CNPC::Initialize_Prototype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    NPC_DESC* pNpcDesc = static_cast<NPC_DESC*>(pArg);
    m_SceneName = pNpcDesc->SceneName;
    m_fTargetDistance = pNpcDesc->TalkRange;


    CAnimBody::ANIMBODY_DESC BodyDesc;
    BodyDesc.modelName = pNpcDesc->ModelName;
    BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pParentState = &m_iState;
    BodyDesc.ObjTag = pNpcDesc->ObjTag + L"_body";

    pNpcDesc->BodyDesc = &BodyDesc;

    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;


    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;



    m_pPlayer = CInteraction_Manager::GetInstance()->Get_MainPlayer();


    Ready_Events();
    if (m_pNavigationCom)
    {
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
    }
    return S_OK;
}

void CNPC::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CNPC::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CNPC::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
     m_pTransformCom->Set_State(STATE::POSITION,
        m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));

   
    m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


}

void CNPC::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

}

HRESULT CNPC::Render()
{
    if (m_pGameInstance->m_bDrawDebug)
        m_pCollider->Render();

    return S_OK;
}

bool CNPC::IsInteratable()
{
    CheckNullResult(m_pPlayer, false);

    CTransform* pPlayerTrans = m_pPlayer->Get_Transform();
    if (pPlayerTrans)
    {
        _vector PlayerPos = pPlayerTrans->Get_State(STATE::POSITION,TransformScope::WORLD);
        _vector ownPos = m_pTransformCom->Get_State(STATE::POSITION, TransformScope::WORLD);

       
        _float Distance = XMVectorGetX(XMVector3Length(PlayerPos - ownPos));

        return Distance <= m_fTargetDistance;

    }

    return false;
}

void CNPC::Enter_InteractRange()
{
    //말걸기 UI 활성화..

}

void CNPC::Stay_InteractRange(_float fTimeDelta)
{
    /*플레이어 쳐다보기*/
    _vector PlayerPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);
    _vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP));

    m_pTransformCom->LookAtSmooth(PlayerPos, 5.f, fTimeDelta);


}

void CNPC::Exit_InteractRange()
{
}

void CNPC::Enter_Interaction()
{
    m_bTalking = true;
    m_pAnimBody->Reserve_Animation(L"talk", true);

    m_pGameInstance->Emit(Enter_Interaction_Event);
    /*CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());


    pCameraBase->Set_Target(this);
    pCameraBase->Set_Offset(_float3(0.f, 3.f, -2.f));*/
}

void CNPC::Stay_Interaction(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    m_pPlayer->Get_Transform()->LookAtSmooth(m_pTransformCom->Get_State(STATE::POSITION), 5.f, fTimeDelta);

    if (m_fTime >= 3.f)
    {
        Exit_Interaction();
        m_fTime = 0.f;
        
        m_bPrevInteracting = false;
        m_bPrevRange = false;
    }

}

void CNPC::Exit_Interaction()
{
    m_bTalking = false;
    m_pPlayer->Get_ActionControl()->m_bTalk = false;
    m_pAnimBody->Reserve_Animation(L"wait", true);

    m_pGameInstance->Emit(Exit_Interaction_Event);

    //CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());


    //pCameraBase->Set_Target(m_pPlayer);
    //pCameraBase->Set_Offset(pCameraBase->Get_InitOffset());
}

HRESULT CNPC::Ready_Components(void* pArg)
{
    //생성 및 추가
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

   // //AABB콜라이더생성
   //////////////Boxcollider추가
    CCollider_Base::COLLIDER_DESC ColDesc;
    CBounding_AABB::BOUNDING_AABB_DESC AABBDesc;

    AABBDesc.Extents = _float3(0.5f, 0.5f, 0.5f);
    AABBDesc.vCenter = _float3(0.f, 1.f, 0.f);

    ColDesc.m_BoundingDesc = &AABBDesc;
    ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::INTERACTION);

    CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"BoxCollider"),
        &ColDesc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::BOX_COLLIDER,
        pCollider,
        reinterpret_cast<CComponent**>(&m_pCollider)
    )))
        return E_FAIL;


    m_pCollider->Set_Trigger(false);
    return S_OK;
}

HRESULT CNPC::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CBody::BODY_DESC* pBodyDesc = static_cast<CBody::BODY_DESC*>(pModelDesc->BodyDesc);
        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"NPC_Body"), L"Part_Body", pBodyDesc)))
            return E_FAIL;

        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));
        
        if (m_pBody)
            m_pAnimBody = dynamic_cast<CAnimBody*>(m_pBody);
    }
    return S_OK;
}

HRESULT CNPC::Ready_Resource(void* pArg)
{
    NPC_DESC    pTmpDesc;

    NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);
    if (!pDesc)
        pDesc = &pTmpDesc;

    m_NpcDesc.ScriptName = pDesc->ScriptName;
    m_NpcDesc.TalkRange = pDesc->TalkRange;
    m_NpcDesc.NPC_ID = pDesc->NPC_ID;


    return S_OK;
} 

void CNPC::Ready_Events()
{
    Enter_Interaction_Event.Name = "Enter_Interaction_NPC";
    EventPayload Payload;

    //_float3(0.f, 3.f, -2.f)
    Payload.Floats["Float_X"] = 0.f;
    Payload.Floats["Float_Y"] = 5.f;
    Payload.Floats["Float_Z"] = -4.f;
    Payload.Ptrs["NPC"] = this;

    Enter_Interaction_Event.Payload = Payload;



    Exit_Interaction_Event.Name = "Exit_Interaction_NPC";
    EventPayload ExitPayload;

    //_float3(0.f, 3.f, -2.f)
    ExitPayload.Ptrs["Player"] = m_pPlayer;

    Exit_Interaction_Event.Payload = ExitPayload;
}



CNPC* CNPC::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CNPC* pInstance = new CNPC(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CNPC ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CNPC::Clone(void* pArg)
{
    return nullptr;
}

void CNPC::Free()
{
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pCollider);

     __super::Free();
   
}
