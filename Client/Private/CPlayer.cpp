#include "CPlayer.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CInput_Manager.h"
#include "CModel.h"
#include "CBody.h"
#include "CNavigation.h"
#include "PlayerStates.h"



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
        m_pCurState->Update(this);
  
    

    /*컨테이너 업데이트 - 바디가 플레이어 상태를 보고 set_Animation을 호출한다.*/
    __super::Update(fTimeDelta);
}

void CPlayer::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);


    Update_Movement(fTimeDelta);

    if (m_pCurState)
        m_pCurState->Update_Late(this);





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
    m_Input.m_bisJump = m_pInputManager->IsKeyPressed(KeyCode::X);

    /*등록한 홀드키에 대해서 모두 홀드키 시간, 여부 검사*/
    Update_HoldTime(fTimeDelta);
   
  
    m_Input.m_bisMove = m_pInputManager->IsKeyHeld(KeyCode::UpArrow) || m_pInputManager->IsKeyHeld(KeyCode::DownArrow)
        || m_pInputManager->IsKeyHeld(KeyCode::LeftArrow) || m_pInputManager->IsKeyHeld(KeyCode::RightArrow) && m_ActionControl.m_bCanMove;


}


void CPlayer::Update_Movement(_float fTimeDelta)
{
    CheckFalse(m_ActionControl.m_bCanMove);

    if (m_ActionControl.IsHold(HOLD_B))
        Hold_Movement(fTimeDelta);

        
    else
        Normal_Movement(fTimeDelta);


    if (m_pTarget)
        m_pTransformCom->Chase(m_pTarget->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD), fTimeDelta, 5);


    m_pTransformCom->Set_State(STATE::POSITION,
        m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));


}

void CPlayer::Normal_Movement(_float fTimeDelta)
{
    CheckTrue(m_ActionControl.m_bFixDir);

    if(m_ActionControl.m_Holds[HoldKey::HOLD_T].m_bHeld)
        m_pTransformCom->Set_Speed(m_fInitSpeed/1.5f);

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

    CComponent* pNavigation = dynamic_cast<CNavigation*>(m_pGameInstance->Clone_Prototype(
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

    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::JUMP, ), CPlayerJumpState::Create());



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
    }



    return StateDebugStr;
}


