#include "CMonster.h"
#include "CModel.h"
#include "CGameInstance.h"
#include "CBody.h"
#include "Client_Defines.h"
#include "CMonsterState.h"
#include "CNavigation.h"
#include "CMonster_Body.h"
#include "CLayer.h"
#include "CImGui_Manager.h"
#include "CBoxColliderComponent.h"
#include "CPartObject.h"
#include "CCell.h"



USING(Client)
CMonster::CMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CModelObject(pDevice, pContext), iHp(0),iMaxHp(0)
{
}

CMonster::CMonster(const CMonster& rhs)
    : CModelObject(rhs),
    iHp(rhs.iHp),
    iMaxHp(rhs.iMaxHp)
{
}

HRESULT CMonster::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

   

    return S_OK;
  
}

HRESULT CMonster::Initialize_Copytype(void* pArg)
{
    CMonster::MONSTER_DESC* desc=static_cast<CMonster::MONSTER_DESC*>(pArg);
   
    
    CTransform::TRANSFORM_DESC* transdesc = static_cast<CTransform::TRANSFORM_DESC*>(desc->TransformDesc);
    m_fInitSpeed=transdesc->fSpeedPerSec;


    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    CMonster_Body::MONSTER_BODY_DESC* pBodyDesc = static_cast<CMonster_Body::MONSTER_BODY_DESC*>(desc->BodyDesc);

    pBodyDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    pBodyDesc->pParentState = &m_iState;
    pBodyDesc->pActionControl = &m_ActionControl;


    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;


    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

  
    if (m_pBody)
        m_pMonsterBody = dynamic_cast<CMonster_Body*>(m_pBody);

    if (m_pNavigationCom)
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));


  
    
    return S_OK;
}

void CMonster::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{

    __super::Update(fTimeDelta);


}

void CMonster::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);


    m_pTransformCom->UpdateImpulse(fTimeDelta, m_pNavigationCom);

    m_pTransformCom->Set_State(STATE::POSITION,
        m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));


    if (m_pCurState)
        m_pCurState->Update_Late(this, fTimeDelta);

    m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CMonster::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);
}

HRESULT CMonster::Render()
{
    __super::Render();

#ifdef _DEBUG
    if (m_pGameInstance->m_bDrawDebug)
        m_pCollider->Render();

#endif


    return S_OK;
}

void CMonster::Set_Active(bool _b)
{
    m_bActive = _b;
    for (auto& pair : m_PartObjects)
    {
        if (pair.second)
            pair.second->Set_Active(false);

    }

    m_pCollider->Set_Active(false);

}

void CMonster::Enter_State(int newState)
{
    //둘이다를때만 enter_state진입
 
}


_wstring CMonster::Get_AnimKey(_uint eType)
{
    if (m_pMonsterBody)
        return m_pMonsterBody->Get_AnimKey(eType);

    return L"";
}

void CMonster::Change_State(int newState)
{
    if (m_pCurState == m_States[newState])
        return;

    if (m_pCurState && !m_pCurState->CanExit())
        return;

    if (m_pCurState)
        m_pCurState->Exit(this);

    m_iPreState = m_iState;
    m_iState = newState;


    m_pNextState = m_States[newState];
    Enter_State(m_iState);
    m_pNextState->Enter(this);
}

string CMonster::Convert_String_To_Enum(_uint eState)
{
    string StateDebugStr = "";

    if (eState == 0)
        return "NONE";


    else
    {
        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO))
            StateDebugStr += "INTRO ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE))
            StateDebugStr += "IDLE ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK))
            StateDebugStr += "WALK";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::RUN))
            StateDebugStr += "RUN";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::JUMP))
            StateDebugStr += "JUMP ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK))
            StateDebugStr += "ATTACK ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE))
            StateDebugStr += "DAMAGE ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE))
            StateDebugStr += "DIE";


    }



    return StateDebugStr;
}

HRESULT CMonster::Ready_Resource(void* pArg)
{

    CheckNullResult(pArg, E_FAIL);
    MONSTER_DESC* pMonsterDesc = static_cast<MONSTER_DESC*>(pArg);

    iMaxHp = pMonsterDesc->MaxHp;
    iHp = iMaxHp;

    iAttack = pMonsterDesc->MaxHp;
    fActionRange = pMonsterDesc->fActionRange;
    m_iLevelID = pMonsterDesc->iLevelID;



    return S_OK;
}


HRESULT CMonster::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CBody::BODY_DESC* pBodyDesc = static_cast<CBody::BODY_DESC*>(pModelDesc->BodyDesc);
        pBodyDesc->ObjTag = tag + L"_Body";

        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Monster_Body"), L"Part_Body", pBodyDesc)))
            return E_FAIL;

        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));

    }
    return S_OK;
}

HRESULT CMonster::Ready_Components(void* pArg)
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

void CMonster::Reserve_Animation_To_Body(_wstring AnimKey, bool bNextAnimLoop)
{
    CheckNull(m_pBody);
    m_pBody->Reserve_Animation(AnimKey, bNextAnimLoop);

}


bool CMonster::Is_InRange(_float fDistance)
{
    ///일단 플레이어가져와서 판단
    CGameObject* pPlayer = m_pGameInstance->Find_GameObject(0, L"Player_Layer", L"Player");
    CTransform* pPlayerTrans = pPlayer->Get_Transform();

    if (pPlayerTrans)
    {
        _vector PlayerPos = pPlayerTrans->Get_State(STATE::POSITION, TransformScope::WORLD);
        _vector ownPos = m_pTransformCom->Get_State(STATE::POSITION, TransformScope::WORLD);


        _float Distance = XMVectorGetX(XMVector3Length(PlayerPos - ownPos));
        if (Distance <= fDistance)
        {

            return true;
        }
    }


    return false;
}

CMonster* CMonster::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMonster* pInstance = new CMonster(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMonster ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
    CMonster* pInstance = new CMonster(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMonster ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMonster::Free()
{
    
    Safe_Release(m_pNavigationCom);
    m_pGameInstance->UnRegister_Collider(m_pCollider);
    for (auto& pair : m_States)
    {
        if (pair.second)
            Safe_Release(pair.second);
    }

    Safe_Release(m_pCollider);
    __super::Free();
}

void CMonster::Set_Dead()
{
    Set_Active(false);
    CLayer* pLayer = m_pGameInstance->Find_Layer(m_iLevelID, L"Monster_Layer");
    pLayer->RequestDestroy(this);
    
#if _DEBUG
    //selectobjec관련들 다 초기화
    CImGui_Manager::GetInstance()->Reset_Window("ObjectDebugWindow");
    CImGui_Manager::GetInstance()->Reset_Window("StateDebugWindow");


#endif

}

void CMonster::Set_CollisionEnable(bool _b)
{
    m_pCollider->Set_Active(_b);
}

void CMonster::Update_DeadState(_float fTimeDelta)
{
    if (iHp <= 0)
        m_ActionControl.m_bDead = true;

}

void CMonster::Patrol()
{
}



void CMonster::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CheckTrue(m_ActionControl.m_bDamage == 1.f);

    CGameObject* pOwner = pOther->Get_Owner();
    CheckNull(pOwner);

    switch (COLLISION_GROUP(iGroup))
    {
    case COLLISION_GROUP::PLAYER_WEAPON:
    {
        m_ActionControl.m_bDamage = 1.f;
        --iHp;

        CPartObject* pPart = dynamic_cast<CPartObject*>(pOwner);
        if (pPart)
        {
          
            //pOther을 바라보고,
            m_pTransformCom->LookAt(pPart->Get_Owner()->Get_Transform());
          
            _float3 vDir;
            XMStoreFloat3(&vDir, m_pTransformCom->Get_State(STATE::LOOK));
            m_pTransformCom->AddImpulse(-0.3f, vDir);
        }
            
    }
       
        break;
    }

   

}


void CMonster::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{

}


void CMonster::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{

}

void CMonster::Reset_RandomCell()
{
    CCell* pCell = m_pNavigationCom->Get_Cell(m_iHomeCell);
    CheckNull(pCell);

    float dist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(STATE::POSITION) - pCell->Get_CenterPos()));

    if (dist > m_fRoamRadius * 1.5f)
    {

        // 너무 멀리 벗어남, 다시 계산
        m_pNavigationCom->Get_RandomCells(m_pTransformCom->Get_State(STATE::POSITION),
            m_fRoamRadius, &m_RandomCells);
    }
}



void CMonster::Damage_Behavior(_float fTimeDelta)
{
    if (m_ActionControl.m_bDamage!=0.f)
    {
        //2초뒤 다시 원래색깔로..
        m_fDamageTime += fTimeDelta;

        if (m_fDamageTime >= 2.f)
        {
            m_ActionControl.m_bDamage = 0.f;
            m_fDamageTime = 0.f;
        }
   }



}
