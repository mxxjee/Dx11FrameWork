#include "CNPC.h"
#include "CPlayer.h"
#include "Client_Defines.h"
#include "CModel.h"
#include "CBody.h"
#include "CCamera_Base.h"
#include "CInteraction_Manager.h"


USING(Client)
CNPC::CNPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CModelObject(pDevice, pContext)
{
}

CNPC::CNPC(const CNPC& rhs)
    : CModelObject(rhs)
{
}

HRESULT CNPC::Initialize_Prototype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    NPC_DESC* pNpcDesc = static_cast<NPC_DESC*>(pArg);


    CBody::BODY_DESC BodyDesc;
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

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player"));

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
    m_pBody->Reserve_Animation(L"talk", true);

    CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());


    pCameraBase->Set_Target(this);
    pCameraBase->Set_Offset(_float3(0.f, 3.f, -2.f));
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
    m_pBody->Reserve_Animation(L"wait", true);
    CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());


    pCameraBase->Set_Target(m_pPlayer);
    pCameraBase->Set_Offset(pCameraBase->Get_InitOffset());
}

HRESULT CNPC::Ready_Components(void* pArg)
{
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



CGameObject* CNPC::Clone(void* pArg)
{
    return nullptr;
}

void CNPC::Free()
{
    __super::Free();
    CInteraction_Manager::GetInstance()->UnRegisterInteractable(this);
}
