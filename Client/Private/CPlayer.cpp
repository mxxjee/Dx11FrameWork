#include "CPlayer.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CInput_Manager.h"
#include "CModel.h"
#include "CBody.h"
#include "CNavigation.h"
#include "PlayerStates.h"

#include "CNPC.h"
#include "CCell.h"

#include "CInteraction_Manager.h"
#include "CGravity.h"


USING(Client)
CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CModelObject(pDevice,pContext),m_pInputManager(CInput_Manager::GetInstance())
{
    Safe_AddRef(m_pInputManager);
}

CPlayer::CPlayer(const CPlayer& rhs)
    : CModelObject(rhs),m_pInputManager(rhs.m_pInputManager)
{
    Safe_AddRef(m_pInputManager);
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Initialize_Copytype(void* pArg)
{
    CModelObject::MODELOBJECT_DESC desc;
    CTransform::TRANSFORM_DESC TransDesc;

    m_fInitSpeed = 4.f;
    TransDesc.fSpeedPerSec = m_fInitSpeed;
    TransDesc.vLocalPosition = { 35.f,10.f,19.f,1.f };
    TransDesc.vLocalRotation = { 0.f,180.f,0.f,0.f };                                                                                      


    desc.TransformDesc = &TransDesc;    
    desc.ObjTag = L"Player";

    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(&desc)))
        return E_FAIL;
 

    CBody::BODY_DESC BodyDesc;
    BodyDesc.modelName = L"LinkAnim";
    BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pParentState = &m_iState;
    BodyDesc.ObjTag = desc.ObjTag + L"_body";

    desc.BodyDesc = &BodyDesc;

    if (FAILED(Ready_Components(&desc)))
        return E_FAIL;


    if(FAILED(Ready_PartObjects(&desc)))
        return E_FAIL;

    if (FAILED(Ready_States()))
        return E_FAIL;

    
    m_iPreState = CModelObject::NONE;
    m_iState = CModelObject::IDLE;


    if (m_pNavigationCom)
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
    
    Change_State(IDLE);

    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
   
    Update_Input(fTimeDelta);

   //State_Change();     //애니메이션 완료 이후에 어떻게 바꿔줄것인지
    if (m_pNextState != nullptr)
    {
        m_pCurState = m_pNextState;
        m_pNextState = nullptr;
      
    }


    if (m_pCurState)
        m_pCurState->Update(this, fTimeDelta);
  
    

    /*컨테이너 업데이트 - 바디가 플레이어 상태를 보고 set_Animation을 호출한다.*/
    __super::Update(fTimeDelta);
 }

void CPlayer::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);


    Update_Movement(fTimeDelta);


    if (m_pCurState)
        m_pCurState->Update_Late(this, fTimeDelta);

    //루트모션 값있으면 적용시키기.
    if (m_pBody)
    {   
        _float3 Delta = m_pBody->Get_RootDelta();
        m_pTransformCom->Set_State(STATE::POSITION,
            m_pTransformCom->Get_State(STATE::POSITION) + XMLoadFloat3(&Delta));


    }



    //Motion_Change();
  
}

void CPlayer::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);
}

HRESULT CPlayer::Render()
{
  /*이제 각 파츠들이 rendergroup으로 들어가서 렌더한다.*/

#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif

    return S_OK;
} 


void CPlayer::Update_Input(_float fTimeDelta)
{
    if (m_ActionControl.m_bCanAttack)
        m_Input.m_bisAttack = m_pInputManager->IsKeyHeld(KeyCode::B);
    else
        m_Input.m_bisAttack = false;




    if(m_ActionControl.m_bCanShield)
        m_Input.m_bisShield = m_pInputManager->IsKeyHeld(KeyCode::T);
    else
        m_Input.m_bisShield = false;

    m_Input.m_bisShieldRelease = m_pInputManager->IsKeyReleased(KeyCode::T);
    if (m_Input.m_bisJump = m_pInputManager->IsKeyPressed(KeyCode::X) && m_pGravity->IsJumping() == false)
    {
        m_pGravity->Jump(20);
        m_pGravity->SetOnGround(false);

    }
    /*등록한 홀드키에 대해서 모두 홀드키 시간, 여부 검사*/
    Update_HoldTime(fTimeDelta);
   
  
    /*떨어졌는지 셀타입에 따라 검사*/
    Update_Fall(fTimeDelta);

    m_Input.m_bisMove = m_pInputManager->IsKeyHeld(KeyCode::UpArrow) || m_pInputManager->IsKeyHeld(KeyCode::DownArrow)
        || m_pInputManager->IsKeyHeld(KeyCode::LeftArrow) || m_pInputManager->IsKeyHeld(KeyCode::RightArrow) && m_ActionControl.m_bCanMove;


    //무조건 a키.
    m_Input.m_bInteract = CInteraction_Manager::GetInstance()->OnInteractKeyPresed();

    //a키 눌려서 타겟이있을대만 갱신
    if(m_Input.m_bInteract)
        m_ActionControl.m_bTalk = CInteraction_Manager::GetInstance()->Check_InteractiveType(InteractionType::NPC);



    
}

void CPlayer::Update_Fall(_float fTimeDelta)
{
     CheckNull(m_pNavigationCom);
    CheckTrue(m_ActionControl.m_bFall);


    _uint CellType = m_pNavigationCom->Get_CurrentCellType();
    if (ENUM_TO_UINT(CellType::FALL) == CellType)
        m_ActionControl.m_bFall = true;
}


void CPlayer::Update_Movement(_float fTimeDelta)
{
    CheckFalse(m_ActionControl.m_bCanMove);

    if (m_ActionControl.IsHold(HOLD_B))
        Hold_Movement(fTimeDelta);

    else if (m_ActionControl.m_bLadder)
        Ladder_Movement(fTimeDelta);

    else
        Normal_Movement(fTimeDelta);


    
    JumpMovement(fTimeDelta);


    
    if(m_pGravity->IsOnGround())
        m_pTransformCom->Set_State(STATE::POSITION,
        m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));


}

void CPlayer::Ladder_Movement(_float fTimeDelta)
{
    CheckFalse(m_ActionControl.m_bLadder);
    m_pTransformCom->Set_Speed(m_fInitSpeed / 1.5f);

   
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::DownArrow))
        m_pTransformCom->Move(DIRECTION::UP,fTimeDelta,Space::WORLD,nullptr);

    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::UpArrow))
        m_pTransformCom->Move(DIRECTION::DOWN, fTimeDelta, Space::WORLD, nullptr);





}

void CPlayer::Normal_Movement(_float fTimeDelta)
{
    CheckTrue(m_ActionControl.m_bFixDir);

    if(m_ActionControl.m_Holds[HoldKey::HOLD_T].m_bHeld)
        m_pTransformCom->Set_Speed(m_fInitSpeed/2.f);

    else if(m_ActionControl.m_bPush || m_ActionControl.m_bCarry)
        m_pTransformCom->Set_Speed(m_fInitSpeed / 4.f);


    else
        m_pTransformCom->Set_Speed(m_fInitSpeed);


   
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LeftArrow))
    {
        if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Rotation(_float3(0.f, -135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, -45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, -90.f, 0.f));

    }



    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::RightArrow))
    {
        if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))

            m_pTransformCom->Rotation(_float3(0.f, 135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, 45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, 90.f, 0.f));
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::DownArrow))
        m_pTransformCom->Rotation(_float3(0.f, 180.f, 0.f));



    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::UpArrow))
        m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));



    if (m_Input.m_bisMove)
        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta, Space::Local, m_pNavigationCom);






}
void CPlayer::Check_HoldTime(HoldKey Key, KeyCode KeyCode, _float fTimeDelta)
{

    //Pressed할 시 초기화
    if (m_pInputManager->IsKeyReleased(KeyCode))
    {
        m_ActionControl.m_Holds[Key].m_fTHoldTime = 0.f;
        m_ActionControl.m_Holds[Key].Reset();
    }

    if(m_pInputManager->IsKeyHeld(KeyCode))
        m_ActionControl.m_Holds[Key].m_fTHoldTime +=fTimeDelta;

    if (m_ActionControl.m_Holds[Key].m_fTHoldTime > m_ActionControl.m_Holds[Key].m_fThresHold)
        m_ActionControl.SetHold(Key, true);



}
void CPlayer::Hold_Movement(_float fTimeDelta)
{
    m_pTransformCom->Set_Speed(m_fInitSpeed / 2.f);

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LeftArrow))
    {
        if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Move(DIRECTION::LEFTDOWN, fTimeDelta, Space::WORLD, m_pNavigationCom);


        else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Move(DIRECTION::LEFTUP, fTimeDelta, Space::WORLD, m_pNavigationCom);


        else
            m_pTransformCom->Move(DIRECTION::LEFT, fTimeDelta, Space::WORLD, m_pNavigationCom);

    }

    

    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::RightArrow))
    {
        if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Move(DIRECTION::RIGHTDOWN,fTimeDelta, Space::WORLD, m_pNavigationCom);

        else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Move(DIRECTION::RIGHTUP, fTimeDelta, Space::WORLD, m_pNavigationCom);

        else
            m_pTransformCom->Move(DIRECTION::RIGHT, fTimeDelta, Space::WORLD, m_pNavigationCom);
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::DownArrow))
        m_pTransformCom->Move(DIRECTION::BACKWARD, fTimeDelta, Space::WORLD, m_pNavigationCom);


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::UpArrow))
        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta, Space::WORLD, m_pNavigationCom);







}

void CPlayer::Update_HoldTime(_float fTimeDelta)
{

    //홀드키에 대한 판단
    for (int i = 0; i < HoldKey_End; ++i)
    {
        KeyCode eKeyCode;
        switch (HoldKey(i))
        {
        case Client::CPlayer::HOLD_B:
            eKeyCode = KeyCode::B;
            break;
        case Client::CPlayer::HOLD_T:
            eKeyCode = KeyCode::T;
            break;
        case Client::CPlayer::HoldKey_End:
            break;
        default:
            break;
        }
        Check_HoldTime((HoldKey)i, eKeyCode, fTimeDelta);

    }
}

void CPlayer::Change_State(int newState)
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
    m_pNextState->Enter(this);
}

void CPlayer::Set_VisibleMesh(const wstring& MeshName, bool bVisible)
{
    if (m_pBody)
        m_pBody->Set_VisibleMesh(MeshName, bVisible);
}

void CPlayer::Set_HideWeapons()
{
    CheckNull(m_pBody);

	m_pBody->Set_VisibleMesh(L"linkHookShot_handl_low__linkHookShot_MI_hookShot", false);
	m_pBody->Set_VisibleMesh(L"linkShieldA_bis_low__linkShieldA_MI_shieldA", false);
	m_pBody->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldB", false);
	m_pBody->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldBMirror", false);

	m_pBody->Set_VisibleMesh(L"linkSwordA_blade_low__linkSwordA_MI_sowrdA", false);
	m_pBody->Set_VisibleMesh(L"linkSwordA_handle_low__linkSwordA_MI_sowrdAball", false);

	m_pBody->Set_VisibleMesh(L"linkSwordB_blade_low__linkSwordB_MI_sowrdB", false);
	m_pBody->Set_VisibleMesh(L"linkSwordB_handle_low__linkSwordB_MI_sowrdBball", false);

	m_pBody->Set_VisibleMesh(L"MagicRod_magicRodJem_low__MagicRod_MI_magicRod", false);
	m_pBody->Set_VisibleMesh(L"Ocarina_ocarina_low__Ocarina_MI_ocarina", false);
	m_pBody->Set_VisibleMesh(L"Shovel_handle_low__Shovel_MI_shovel", false);
    
}

void CPlayer::Show_Weapons()
{
    CheckNull(m_pBody);

	m_pBody->Set_VisibleMesh(L"linkShieldA_bis_low__linkShieldA_MI_shieldA", true);

	m_pBody->Set_VisibleMesh(L"linkSwordA_blade_low__linkSwordA_MI_sowrdA", true);
	m_pBody->Set_VisibleMesh(L"linkSwordA_handle_low__linkSwordA_MI_sowrdAball", true);

	m_pBody->Set_VisibleMesh(L"linkSwordB_blade_low__linkSwordB_MI_sowrdB", true);
	m_pBody->Set_VisibleMesh(L"linkSwordB_handle_low__linkSwordB_MI_sowrdBball", true);

}

void CPlayer::Set_Default()
{

    m_pBody->Set_VisibleMesh(L"linkShieldA_bis_low__linkShieldA_MI_shieldA", true);

    m_pBody->Set_VisibleMesh(L"linkSwordA_blade_low__linkSwordA_MI_sowrdA", true);
    m_pBody->Set_VisibleMesh(L"linkSwordA_handle_low__linkSwordA_MI_sowrdAball", true);

    m_pBody->Set_VisibleMesh(L"linkSwordB_blade_low__linkSwordB_MI_sowrdB", true);
    m_pBody->Set_VisibleMesh(L"linkSwordB_handle_low__linkSwordB_MI_sowrdBball", true);


    m_pBody->Set_VisibleMesh(L"flipperL_low__MI_flippers", false);
    m_pBody->Set_VisibleMesh(L"MagicRod_magicRodJem_low__MagicRod_MI_magicRod", false);
    m_pBody->Set_VisibleMesh(L"Shovel_handle_low__Shovel_MI_shovel", false);
    m_pBody->Set_VisibleMesh(L"linkHookShot_handl_low__linkHookShot_MI_hookShot", false);
    m_pBody->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldBMirror", false);
    m_pBody->Set_VisibleMesh(L"linkShieldB_bis_low__linkShieldB_MI_shieldB", false);
    m_pBody->Set_VisibleMesh(L"Ocarina_ocarina_low__Ocarina_MI_ocarina", false);
}

void CPlayer::Respawn()
{
    CheckNull(m_pNavigationCom);

    /*이전 셀에서 리스폰*/ 

    vector<CCell*>* MainCell = m_pGameInstance->Get_MainCells();
    if (MainCell)
    {
        _uint PreCellIdx = m_pNavigationCom->Get_PreCellIdx();
        CCell* pCell = (*MainCell)[PreCellIdx];

        while (pCell->Get_CurrentCellType() == ENUM_TO_UINT(CellType::FALL))
        {
            //만약 고른게 또 fall 타입이라면,, 
            int Neighbors[3];
            pCell->Get_Neighbors(Neighbors);
            for (int i = 0; i < 3; ++i)
            {
                PreCellIdx = Neighbors[i];
                pCell = (*MainCell)[PreCellIdx];

                if (pCell->Get_CurrentCellType() != ENUM_TO_UINT(CellType::FALL))
                    break;
            }

             
        }
        m_pTransformCom->Set_State(STATE::POSITION, (*MainCell)[PreCellIdx]->Get_CenterPos());
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));

    }
}

CPlayer* CPlayer::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CPlayer* pInstance = new CPlayer(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CPlayer ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CPlayer ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CPlayer::Free()
{
    Safe_Release(m_pInputManager);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pGravity);

    for (auto& pair : m_States)
    {
        if (pair.second)
            Safe_Release(pair.second);
    }

    __super::Free();
}


HRESULT CPlayer::Ready_Components(void* pArg)
{
    // Transform 생성 및 추가
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

    /////////////////Gravity추가
    CComponent* pGravity = dynamic_cast<CGravity*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"Gravity"),
        &Desc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::GRAVITYCOM,
        pGravity,
        reinterpret_cast<CComponent**>(&m_pGravity)
    )))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CBody::BODY_DESC* pBodyDesc = static_cast<CBody::BODY_DESC*>(pModelDesc->BodyDesc);
        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Player_Body"), L"Part_Body", pBodyDesc)))
            return E_FAIL;

        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));

    }
    return S_OK;
}

HRESULT CPlayer::Ready_States()
{
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE), CPlayerIdleState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::RUN), CPlayerRunState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ATTACK), CPlayerAttackState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_ATTACK), CPlayerHoldAttackState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_SHIELD), CPlayerHoldShield::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::SLASH_SHIELD), CPlayerSlashShieldState::Create());

    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::JUMP), CPlayerJumpState::Create());

    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::LADDER), CPlayerLadderState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::PUSH), CPlayerPushState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ITEMGET), CPlayerStateGetItem::Create());

    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::CARRY), CPlayerCarryState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::TALK), CPlayerTalkState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::FALL), CPlayerFallState::Create());


    /// <summary>
    /// 키 설정
    /// 
    HOLDKEY_DATA    HoldKeyB;
    HoldKeyB.Code = KeyCode::B;
    HoldKeyB.m_fThresHold = 0.2f;


    HOLDKEY_DATA    HoldKeyT;
    HoldKeyT.Code = KeyCode::T;
    HoldKeyT.m_fThresHold = 0.1f;


    m_ActionControl.m_Holds.push_back(HoldKeyB);
    m_ActionControl.m_Holds.push_back(HoldKeyT);

    return S_OK;
}

void CPlayer::Reserve_Animation_To_Body(_wstring AnimKey, bool bNextAnimLoop)
{
    CheckNull(m_pBody);
    m_pBody->Reserve_Animation(AnimKey, bNextAnimLoop);
}



void CPlayer::JumpMovement(_float fTimeDelta)
{
    // 1) 중력 갱신
    m_pGravity->Update(0.016);

    // 2) 이번 프레임 Y 이동량
    float fDT = m_pGravity->GetFallDistance(0.016);

    _vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vNewPos = vCurPos + XMVectorSet(0.f, fDT, 0.f, 0.f);

    // 3) 바닥 체크
    _float vOutY = 0.f;
    bool bOnGround = m_pNavigationCom->CheckGround(vNewPos, vOutY);

    //------------------------------------------------
    // A. 점프 중 (위로 뜨거나, 아직 공중일 때)
    //------------------------------------------------
    if (m_pGravity->IsJumping())
    {
        // 일단 위치는 무조건 업데이트 (이륙/상승 허용)
        m_pTransformCom->Set_State(STATE::POSITION, vNewPos);

        if (m_pGravity->GetVelocityY() <= 0.f)
        {
            m_pGravity->SetJumping(false);   // 점프 상승 종료
            // 낙하 상태로 넘어가고, 아래 분기(B)에서 떨어짐 처리됨
        }

        return;
    }

    //------------------------------------------------
    // B. 점프 중은 아닌데, 아직 공중 (떨어지는 중)
    //------------------------------------------------
    if (!bOnGround)
    { 
        m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
        m_pGravity->SetOnGround(false);
        return;
    }

    //------------------------------------------------
    // C. 바닥 감지 && 떨어지는 중 → 착지 처리
    //------------------------------------------------
    if (m_pGravity->GetVelocityY() <= 0.f)
    {
        // 바닥 높이로 스냅
        vNewPos = XMVectorSetY(vNewPos, vOutY);
        m_pTransformCom->Set_State(STATE::POSITION, vNewPos);

        // 착지
        m_pGravity->Land();      // 여기서 Velocity 0, OnGround=true, Jumping=false
        return;
    }

    //------------------------------------------------
    // D. 그냥 평지 위에 서 있는 상태
    //------------------------------------------------
    m_pGravity->SetOnGround(true);
}


string CPlayer::Convert_String_To_Enum(_uint eState)
{
    string StateDebugStr = "";

    if (eState == 0)
        return "NONE";


    else
    {
        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE))
            StateDebugStr += "IDLE ";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::RUN))
            StateDebugStr += "Run ";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::ATTACK))
            StateDebugStr += "ATTACK ";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_ATTACK))
            StateDebugStr += "HOLD_ATTACK ";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_SHIELD))
            StateDebugStr += "HOLD_SHIELD ";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::SLASH_SHIELD))
            StateDebugStr += "SLASH_SHIELD ";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::LADDER))
            StateDebugStr += "LADDER";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::JUMP))
            StateDebugStr += "JUMP";


        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::PUSH))
            StateDebugStr += "PUSH";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::ITEMGET))
            StateDebugStr += "ITEMGET";

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::TALK))
            StateDebugStr += "TALK";
    }



    return StateDebugStr;
}

void CPlayer::Render_StateDebug(int* pArg)
{
    if (ImGui::RadioButton("Ladder", (int*)(pArg), 0))
    {
        m_ActionControl.m_bLadder = true;
        m_ActionControl.m_bPush = false;
        m_ActionControl.m_bCarry = false;
        m_ActionControl.m_bItemGet = false;
    }
    if (ImGui::RadioButton("Push", (int*)(pArg), 1))
    {
        m_ActionControl.m_bLadder = false;
        m_ActionControl.m_bPush = true;
        m_ActionControl.m_bCarry = false;
        m_ActionControl.m_bItemGet = false;
    }


    if (ImGui::RadioButton("Carry", (int*)(pArg), 2))
    {
        m_ActionControl.m_bLadder = false;
        m_ActionControl.m_bPush = false;
        m_ActionControl.m_bCarry = true;
        m_ActionControl.m_bItemGet = false;
    }


    if (ImGui::RadioButton("ItemGet", (int*)(pArg), 3))
    {
        m_ActionControl.m_bLadder = false;
        m_ActionControl.m_bPush = false;
        m_ActionControl.m_bCarry = false;
        m_ActionControl.m_bItemGet = true;
    }

    if (ImGui::RadioButton("None", (int*)(pArg), 4))
    {
        m_ActionControl.m_bLadder = false;
        m_ActionControl.m_bPush = false;
        m_ActionControl.m_bCarry = false;
        m_ActionControl.m_bItemGet = false;
    }

}


