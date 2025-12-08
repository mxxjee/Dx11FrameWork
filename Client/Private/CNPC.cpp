#include "CNPC.h"
#include "CPlayer.h"
#include "Client_Defines.h"
#include "CModel.h"
#include "CAnimBody.h"
#include "CCamera_Base.h"
#include "CInteraction_Manager.h"
#include "CNavigation.h"


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



    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player"));
    CInteraction_Manager::GetInstance()->RegisterInteractable(m_SceneName, dynamic_cast<CIInteractable*>(this));


    Ready_Events();
  
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


}

void CNPC::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CNPC::Render()
{
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

        return Distance <= m_NpcDesc.TalkRange;

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
    Payload.Floats["Float_Y"] = 3.f;
    Payload.Floats["Float_Z"] = -2.f;
    Payload.Ptrs["NPC"] = this;

    Enter_Interaction_Event.Payload = Payload;



    Exit_Interaction_Event.Name = "Exit_Interaction_NPC";
    EventPayload ExitPayload;

    //_float3(0.f, 3.f, -2.f)
    ExitPayload.Ptrs["Player"] = m_pPlayer;

    Exit_Interaction_Event.Payload = ExitPayload;
}



CGameObject* CNPC::Clone(void* pArg)
{
    return nullptr;
}

void CNPC::Free()
{
    Safe_Release(m_pNavigationCom);
    CInteraction_Manager::GetInstance()->UnRegisterInteractable(m_SceneName,this);
    __super::Free();
   
}
