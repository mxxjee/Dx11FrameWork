#include "CPlayer.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CInput_Manager.h"
#include "CModel.h"
#include "CBody.h"
#include "CNavigation.h"
#include "PlayerStates.h"
#include "CPlayer_Body.h"

#include "CNPC.h"
#include "CCell.h"

#include "CInteraction_Manager.h"
#include "CGravity.h"
#include "CBoxColliderComponent.h"
#include "CBounding_AABB.h"
#include "CPlayer_Sword.h"
#include "CPlayer_Shield.h"

#include "CInteractionObject.h"
#include "CGameManager.h"
#include "CInventory_Manager.h"
#include "CMagicPowder.h"
#include "CInteraction_JackyBall.h"

#include "CMaterial.h"
#include "CTexture.h"





USING(Client)
CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CAnimModelObject(pDevice,pContext),
    m_pInputManager(CInput_Manager::GetInstance()),
    m_pInventoryManager(CInventory_Manager::GetInstance())
   
{
    Safe_AddRef(m_pInputManager);
}

CPlayer::CPlayer(const CPlayer& rhs)
    : CAnimModelObject(rhs),m_pInputManager(rhs.m_pInputManager),m_pInventoryManager(rhs.m_pInventoryManager)
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
    m_pGameManager=CGameManager::GetInstance();
    CModelObject::MODELOBJECT_DESC desc;
    CTransform::TRANSFORM_DESC TransDesc;

    m_fInitSpeed = 4.f;
    TransDesc.fSpeedPerSec = m_fInitSpeed;
    TransDesc.vLocalPosition = 
    TransDesc.vLocalRotation = { 0.f,180.f,0.f,0.f };                                                                                      


    desc.TransformDesc = &TransDesc;    
    desc.ObjTag = L"Player";

    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(&desc)))
        return E_FAIL;
 

    CPlayer_Body::PLAYER_BODY_DESC BodyDesc;
    BodyDesc.modelName = L"LinkAnim";
    BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pParentState = &m_iState;
    BodyDesc.ObjTag = desc.ObjTag + L"_body";
    BodyDesc.pDamgeRender = &m_ActionControl.m_fDamage;
    BodyDesc.pDamageTime = &m_fDamageTime;
    desc.BodyDesc = &BodyDesc;
    

    if (FAILED(Ready_Components(&desc)))
        return E_FAIL;


    if(FAILED(Ready_PartObjects(&desc)))
        return E_FAIL;

    if (FAILED(Ready_States()))
        return E_FAIL;
    
    if (FAILED(Ready_Expressions()))
        return E_FAIL;

    
    m_iPreState = CAnimModelObject::NONE;
    m_iState = CAnimModelObject::IDLE;

   
    if (m_pNavigationCom)
    {
        m_pTransformCom->Set_State(STATE::POSITION,m_pNavigationCom->Get_Cell(0)->Get_CenterPos());
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
    }
        
    
    if (m_pGameManager->Get_UseCutScene())
        StartCutScene();//컷씬상태에 맞는 메쉬,등등 설정
      

    else
        Change_State(IDLE);
    m_iSceneID = ENUM_TO_UINT(LEVEL_ID::STATIC);

    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    /*컷씬중일땐 움직임x*/
    //무조건 a키.
    m_Input.m_bInteract = CInteraction_Manager::GetInstance()->OnInteractKeyPresed();

    //돌아보기
    if (m_bRichardChapter)
    {
        _vector vPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
        m_pTransformCom->LookAtSmooth(vPos, 5.f, fTimeDelta);
    }


    if (!m_pGameManager->Get_UseCutScene() && !m_bRichardChapter)
    {
        Update_Input(fTimeDelta);
        UpdateFlash(fTimeDelta);
        Update_State(fTimeDelta);

    }
 
   //State_Change();     //애니메이션 완료 이후에 어떻게 바꿔줄것인지
 /*   if (m_pNextState != nullptr)
    {
        m_pCurState = m_pNextState;
        m_pNextState = nullptr;
      
    }*/


    if (m_pCurState)
        m_pCurState->Update(this, fTimeDelta);
  
    /*컨테이너 업데이트 - 바디가 플레이어 상태를 보고 set_Animation을 호출한다.*/
    __super::Update(fTimeDelta);
 }

void CPlayer::Update_Late(_float fTimeDelta)
{

    Update_Movement(fTimeDelta);
    m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    __super::Update_Late(fTimeDelta);



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

#ifdef _DEBUG

    if (CGameInstance::m_bDrawDebug)
    {
        if(FAILED(m_pGameInstance->Add_DebugComponent(m_pCollider)))
            return;

        if (FAILED(m_pGameInstance->Add_DebugComponent(m_pNavigationCom)))
            return;
    }
  

#endif
}

HRESULT CPlayer::Render()
{
  /*이제 각 파츠들이 rendergroup으로 들어가서 렌더한다.*/


    return S_OK;
} 


void CPlayer::StartCutScene()
{
    //컷씬상태라면, 웨폰없는상태로 시작
    Change_State(ENUM_TO_UINT(PLAYER_STATE::PRATFALL));
    Set_HideWeapons();
}

void CPlayer::EndCutScene()
{
    //불변수 false만들고, 움직임가능하게 처리.
    CheckNull(m_pGameManager);
    m_pGameManager->Set_UseCutScene(false);
}

void CPlayer::Enter_State(int newState)
{
    switch (CPlayer::PLAYER_STATE(newState))
    {

    case CPlayer::PLAYER_STATE::DAMANGE:
        OnDamageBehavior();
        break;
    }
}

void CPlayer::Update_State(float fTimeDelta)
{

   

    switch ((PLAYER_STATE)m_iState)
    {
    case PLAYER_STATE::PUSH:
        Push_Interaction_Behavior(fTimeDelta);
        break;

    case PLAYER_STATE::CARRY:

        break;
    }

    
        

}

void CPlayer::Update_Input(_float fTimeDelta)
{
    CheckTrue(m_pGameInstance->Get_IsLoading());
    CheckFalse(m_pInputManager->Get_InputMode() == InputMode::GAME);


    CheckTrue(m_bRichardChapter);

    _uint iCurrentLevel = m_pGameInstance->Get_CurrentLevelID();


    //NPC방이 아닐때만 점프/어택/쉴드 가능
    if (iCurrentLevel != ENUM_TO_UINT(LEVEL_ID::ROOM) &&
        iCurrentLevel != ENUM_TO_UINT(LEVEL_ID::SPAWN))
    {
        if (m_ActionControl.m_bCanAttack)
            m_Input.m_bisAttack = m_pInputManager->IsKeyHeld(KeyCode::B);
        else
            m_Input.m_bisAttack = false;

        if (m_ActionControl.m_bCanShield)
            m_Input.m_bisShield = m_pInputManager->IsKeyHeld(KeyCode::T);
        else
            m_Input.m_bisShield = false;

        m_Input.m_bisShieldRelease = m_pInputManager->IsKeyReleased(KeyCode::T);
        if (m_Input.m_bisJump = m_pInputManager->IsKeyPressed(KeyCode::R) && m_iState != ENUM_TO_UINT(CPlayer::PLAYER_STATE::JUMP))
        {
            m_pGravity->Jump(20);
            m_pGravity->SetOnGround(false);

        }
    }


    if (m_pInputManager->IsKeyPressed(KeyCode::X))
        m_pInventoryManager->Use_QuickSlot_Item(KeyCode::X, 1);


   
    /*등록한 홀드키에 대해서 모두 홀드키 시간, 여부 검사*/
    Update_HoldTime(fTimeDelta);
   
  
    /*떨어졌는지 셀타입에 따라 검사*/
    Update_Fall(fTimeDelta);

    m_Input.m_bisMove = m_pInputManager->IsKeyHeld(KeyCode::UpArrow) || m_pInputManager->IsKeyHeld(KeyCode::DownArrow)
        || m_pInputManager->IsKeyHeld(KeyCode::LeftArrow) || m_pInputManager->IsKeyHeld(KeyCode::RightArrow) && m_ActionControl.m_bCanMove;


    
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

    JumpMovement(fTimeDelta);


    if (m_pGravity->IsOnGround())
        m_pTransformCom->Set_State(STATE::POSITION,
            m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));

    m_pTransformCom->UpdateImpulse(fTimeDelta,m_pNavigationCom);

    CheckFalse(m_ActionControl.m_bCanMove);
    CheckTrue(m_ActionControl.m_fDamage==1.f);
    CheckTrue(m_bRichardChapter);

    if (m_ActionControl.IsHold(HOLD_B))
        Hold_Movement(fTimeDelta);

    else if (m_ActionControl.m_bLadder)
        Ladder_Movement(fTimeDelta);

    else
        Normal_Movement(fTimeDelta);


 

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

    else if (m_ActionControl.m_bPush || m_ActionControl.m_bCarry)
    {
        float f = m_ActionControl.m_bCarry ? 2.f : 4.f;
        m_pTransformCom->Set_Speed(m_fInitSpeed / f);

    }

    else
        m_pTransformCom->Set_Speed(m_fInitSpeed);


   
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LeftArrow))
    {
        m_InputDir = DIRECTION::LEFT;

        if (!m_ActionControl.m_bPush)
        {
            if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
                m_pTransformCom->Rotation(_float3(0.f, -135.f, 0.f));

            else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
                m_pTransformCom->Rotation(_float3(0.f, -45.f, 0.f));
            else
                m_pTransformCom->Rotation(_float3(0.f, -90.f, 0.f));
        }

        else
        {

            m_pTransformCom->Rotation(_float3(0.f, -90.f, 0.f));
        }


    }



    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::RightArrow))
    {
        m_InputDir = DIRECTION::RIGHT;

        if (!m_ActionControl.m_bPush)
        {
            if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))

                m_pTransformCom->Rotation(_float3(0.f, 135.f, 0.f));

            else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
                m_pTransformCom->Rotation(_float3(0.f, 45.f, 0.f));
            else
                m_pTransformCom->Rotation(_float3(0.f, 90.f, 0.f));
        }

        else
        {

            m_pTransformCom->Rotation(_float3(0.f, 90.f, 0.f));

        }
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::DownArrow))
    {
        m_InputDir = DIRECTION::DOWN;
        m_pTransformCom->Rotation(_float3(0.f, 180.f, 0.f));

    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::UpArrow))
    {
        m_InputDir = DIRECTION::UP;
        m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));

    }



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


    m_pCurState = m_States[newState];
    Enter_State(m_iState);
    m_pCurState->Enter(this);
}

wstring CPlayer::Get_CurrentAnimKey()
{
    return m_pAnimBody->Get_CurrentAnimKey();
}

void CPlayer::On_Heal(int Hp)
{
    //회복
    iHp = Hp;
    m_pGameInstance->BroadCastEvent(L"OnHeartHealed", &iHp);
  
}

void CPlayer::On_RichardChapterEvent(CGameObject* pObj)
{
    /*컷씬전용*/


    m_bRichardChapter = true;
    m_pTarget = pObj;

    m_Input.m_bisMove = false;
    Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));

    
}

void CPlayer::Exit_RichardChapterEvent()
{
    m_bRichardChapter = false;
    m_Input.m_bisMove = true;


}
void CPlayer::Create_PowderParticle()
{
    _uint iCurrentLevel = m_pGameInstance->Get_CurrentLevelID();

    CMagicPowder::MAGICPOWDER_DESC Desc;
    Desc.m_iLevelID = iCurrentLevel;
    Desc.ObjTag = L"MagicPowder";
    Desc.fLifeTime = 1.f;

    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vParticlePos = vPos + (XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 2.f);


    CTransform::TRANSFORM_DESC TransDesc;
    XMStoreFloat4(&TransDesc.vLocalPosition, vParticlePos);

    Desc.TransformDesc = &TransDesc;

    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT,
            ENUM_TO_UINT(LEVEL_ID::STATIC),
            PROTO_OBJ_NAME(L"MagicPowder"), &Desc);

    if (pObj)
    {
        CGameObject* ppObj = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(iCurrentLevel, L"Particle_Layer", ppObj)))
            return;



    }


}
HRESULT CPlayer::Ready_Expressions()
{
    CheckNullResult(m_pAnimBody, E_FAIL);
    CModel* pModel = m_pAnimBody->Get_Model();
    wstring ModelName = pModel->Get_ModelData().name;

    wstring FilePath = L"../../Resource/Model/Actor/LinkAnim/Materials/";

    //입 사진 추가바인드
    CMaterial* pMouthMat = m_pGameInstance->Find_Material(ModelName + L"_MI_mouth");
    if (pMouthMat)
    {
        auto pair = pMouthMat->Get_MaterialData()->m_Textures.find(aiTextureType::aiTextureType_DIFFUSE);
        if (pair == pMouthMat->Get_MaterialData()->m_Textures.end())
            return E_FAIL;

        m_pMouthTex = pair->second;

        for (int i = 0; i <= 8; ++i)
        {
            wstring MouthPath = FilePath + L"MI_mouth_alb." + to_wstring(i) + L".dds";
            m_pMouthTex->Load_Texture(MouthPath.c_str());

        }


    }

    return S_OK;
}
void CPlayer::Set_Expression(EXPRESSION expression)
{
    if (expression == EXPRESSION::IDLE)
        m_pMouthTex->Set_Texture(0);

    if (expression == EXPRESSION::HAPPY)
        m_pMouthTex->Set_Texture(2);

}
void CPlayer::Teleport(_uint iNewIdx)
{
    _vector vSpawnPos = m_pGameInstance->Get_CellPos_By_MainCells(iNewIdx);
    Get_Transform()->Set_State(STATE::POSITION, vSpawnPos);
    
    m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));

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

    _uint iCurrentLevelID = m_pGameInstance->Get_CurrentLevelID();

    //만약 npc룸 안이라면, 활성화하지 않음
    CheckTrue(iCurrentLevelID == ENUM_TO_UINT(LEVEL_ID::ROOM));
    CheckTrue(iCurrentLevelID == ENUM_TO_UINT(LEVEL_ID::SPAWN));



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

void CPlayer::OnAttackBegin()
{
    CPartObject* pPlayerSword = Find_PartObject(L"Player_Sword");
    if (pPlayerSword)
        pPlayerSword->Set_Active(true);

}

void CPlayer::OnAttackEnd()
{
    CPartObject* pPlayerSword = Find_PartObject(L"Player_Sword");
    if (pPlayerSword)
        pPlayerSword->Set_Active(false);

}

void CPlayer::Set_ShieldEnable(bool b)
{
    CPartObject* pPlayerShield = Find_PartObject(L"Player_Shield");
    if (pPlayerShield)
        pPlayerShield->Set_Active(b);
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

void CPlayer::Change_MainNavMesh()
{
    if (m_pNavigationCom)
    {
        m_pNavigationCom->Set_MainCell();
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
        m_pTransformCom->Set_State(STATE::POSITION,
            m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));

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
    Safe_Release(m_pCollider);


    for (auto& pair : m_States)
    {
        if (pair.second)
            Safe_Release(pair.second);
    }

    __super::Free();
}


void CPlayer::Damage_Behavior()
{
    _float3 vDir;
   
    XMStoreFloat3(&vDir, XMVectorSetW(m_pTransformCom->Get_State(STATE::LOOK), 0.f));

    if (m_bHitFront)//앞에서맞앙쓰니까 뒤로밀리기
    {
        vDir.x *= -1.f; vDir.y *= -1.f; vDir.z *= -1.f;
        m_pTransformCom->AddImpulse(0.15f, vDir);

    }
        

    else            //뒤에서맞앙쓰니까 앞으로밀리기
        m_pTransformCom->AddImpulse(0.15f, vDir);


    --iHp;
    m_pGameInstance->BroadCastEvent(L"OnHeartDamaged", &iHp);
    

}

void CPlayer::Push_Interaction_Behavior(_float fTimeDelta)
{

    
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



    //////////////Boxcollider추가

    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::PLAYER);
    pColliderDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

    CBounding_AABB::BOUNDING_AABB_DESC CollDesc;
    CollDesc.vCenter = { 0.f,0.5f,0.f };
    CollDesc.Extents = { 0.3f,0.8f,0.4f };
    pColliderDesc.m_BoundingDesc = &CollDesc;


    CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"BoxCollider"),
        &pColliderDesc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::BOX_COLLIDER,
        pCollider,
        reinterpret_cast<CComponent**>(&m_pCollider)
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
        pBodyDesc->pOwner = this;
        pBodyDesc->m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Player_Body"), L"Part_Body", pBodyDesc)))
            return E_FAIL;

        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));
        
        if (m_pBody)
            m_pAnimBody = dynamic_cast<CAnimBody*>(m_pBody);
    }


    /////////WEapon
    CWeapon::WEAPON_DESC SWordDesc{};
    SWordDesc.pOwner = this;
    SWordDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("itemA_L");
    SWordDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    SWordDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

    if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Player_Sword"), L"Player_Sword", &SWordDesc)))
        return E_FAIL;


    /////////Weapon-shield
    CWeapon::WEAPON_DESC ShieldDesc{};
    ShieldDesc.pOwner = this;
    ShieldDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("root");
    ShieldDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    ShieldDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

    if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Player_Shield"), L"Player_Shield", &ShieldDesc)))
        return E_FAIL;

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
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::DAMANGE), CPlayerDamageState::Create());

    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::PRATFALL), CPlayerPratFallState::Create());
    m_States.emplace(ENUM_TO_UINT(CPlayer::PLAYER_STATE::POWDER), CPlayerPowderState::Create());


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

void CPlayer::Reserve_Animation_To_Body(_wstring AnimKey, bool bNextAnimLoop, bool immediately)
{
    CheckNull(m_pAnimBody);
    m_pAnimBody->Reserve_Animation(AnimKey, bNextAnimLoop, immediately);
}





void CPlayer::JumpMovement(_float fTimeDelta)
{
    m_pGravity->Update(0.016f);

    // 이번 프레임 Y 이동량
    float fDT = m_pGravity->GetFallDistance(0.016f);

    _vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vNewPos = vCurPos + XMVectorSet(0.f, fDT, 0.f, 0.f);
   
    //바닥 체크
    _float vOutY = 0.f;
    bool bOnGround = m_pNavigationCom->CheckGround(vNewPos, vOutY);

    //------------------------------------------------
    //점프 중 (위로 뜨거나, 아직 공중일 때)
    //------------------------------------------------
    if (m_pGravity->IsJumping())
    {
        //점프 시 위치설정
        m_pTransformCom->Set_State(STATE::POSITION, vNewPos);

        //낙하 상태체크
        if (m_pGravity->GetVelocityY() <= 0.f)
        {
            m_pGravity->SetJumping(false);
        }

        return;
    }

    //------------------------------------------------
    // 점프 중은 아닌데, 아직 공중 (떨어지는 중)
    //------------------------------------------------
    if (!bOnGround)
    { 
        m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
        m_pGravity->SetOnGround(false);
        return;
    }

    //------------------------------------------------
    // 바닥 감지 && 떨어지는 중-> 착지 처리
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
    //  그냥 평지 위에 서 있는 상태
    //------------------------------------------------
    m_pGravity->SetOnGround(true);
}

#ifdef  _DEBUG
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

        if (eState == ENUM_TO_UINT(CPlayer::PLAYER_STATE::DAMANGE))
            StateDebugStr += "DAMAGE";
    }



    return StateDebugStr;
}

void CPlayer::Render_Transform_Imgui()
{
    __super::Render_Transform_Imgui();

    string result;
    if (m_pGravity->IsOnGround())
        result = "true";
    else
        result = "false";

    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
    ImGui::BulletText("isOnGround:%s",
        result.c_str());
    ImGui::BulletText("CurIdex:%d", m_pNavigationCom->Get_CurrentCell()->Get_Idx());
    ImGui::PopStyleColor();


}
void CPlayer::Render_StateDebug(int* pArg)
{
    /*if (ImGui::RadioButton("Ladder", (int*)(pArg), 0))
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
    }*/

}

#endif

void CPlayer::OnDamageBehavior()
{
    //2초후 다시 충돌 키기
    m_pGameInstance->Invoke(2.f, 0.f, false, false, [this]()
        {
            m_bCanCollision = true;
            }, this);



    //Damage Animnotify...시간이후 깜빡거림
    
}

void CPlayer::Shield_Hit_Behavior()
{
    CheckFalse(Get_Hold(CPlayer::HoldKey::HOLD_T));

    //현재스테이트의 shield함수 실행
    CPlayerHoldShield* pShieldState = dynamic_cast<CPlayerHoldShield*>(m_pCurState);
    if (pShieldState)
    {
        
        pShieldState->Hit_Shield(this);

    }
}

void CPlayer::Push_Behavior()
{
    _float3 vDir;
    XMStoreFloat3(&vDir, m_pTransformCom->Get_State(STATE::LOOK));
    m_pTransformCom->AddImpulse(-0.1f, vDir);
    m_bCanCollision = false;
    m_pGameInstance->Invoke(2.f, 0.f, false, false, [this]()
        {
            m_bCanCollision = true;
        }, this);


    //ShieldHit애니메이션
    Shield_Hit_Behavior();
}

bool CPlayer::Set_CarryAndThrowState(CInteractionObject* pObj)
{
    CheckNullResult(pObj, false);

    //들고있는경우에는 carry모드 , 아니라면 throwmode
    if (m_ActionControl.m_bCarry)
    {
        CInteraction_JackyBall* pBall = dynamic_cast<CInteraction_JackyBall*>(pObj);
        if (pBall)
        {
            if (pBall->Get_Owner() != nullptr)
                return false;
            pBall->Set_Owner(this);
        }
 


        Change_State(ENUM_TO_UINT(PLAYER_STATE::CARRY));
        m_CarryObject = pObj;


        pObj->Set_InteractionMode(true);
        pObj->Set_ParentMatrix(m_pTransformCom->Get_WorldMatrixPtr());
        pObj->Set_SocketMatrix(m_pBody->Get_SocketMatrix("itemA_L"));
        
       


        return true;

    }

    else
    { 
        CheckNullResult(m_CarryObject,false);
        m_CarryObject->Throw();
        m_CarryObject = nullptr;

        return false;
    }
   
    

    return true;
}

void CPlayer::UpdateFlash(_float fTimeDelta)
{
    CheckFalse(m_bFlash);
    m_fDamageTime += fTimeDelta;
    //.task.로 2초뒤에 m_fTime=0, m_bFlash=false

}

void CPlayer::Set_Flash(bool b)
{
     m_bFlash = b; 

     if (b) //5초뒤에 다시꺼주기에약
     {
         m_pGameInstance->Invoke(1.5f, 0.f, false, false,
             [this]()
             {
                 Reset_Flash();
             },
             this);
     }
        
}

void CPlayer::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CheckFalse(m_bCanCollision);
    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {
    case Client::COLLISION_GROUP::MONSTER:
    case Client::COLLISION_GROUP::MONSTER_WEAPON:
    {   
       
        if(!m_ActionControl.m_bCarry)
            m_ActionControl.m_fDamage =1.f;

    _vector vDir = (pOwner->Get_Transform()->Get_State(STATE::POSITION))
        - (m_pTransformCom->Get_State(STATE::POSITION));

        m_bHitFront = m_pTransformCom->IsFront(vDir);
        m_bCanCollision = false;
    }
        break;

    case Client::COLLISION_GROUP::TRIGGER:
     
        break;
    case Client::COLLISION_GROUP::END:
        break;


    case Client::COLLISION_GROUP::INTERACTION:
        Check_Interaction_Collision(pOther);
        break;
    default:
        break;


    }
    

}

void CPlayer::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
    CheckFalse(m_bCanCollision);
    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {


    case Client::COLLISION_GROUP::INTERACTION:
        Check_Interaction_Stay_Collision(pOther);
        break;
    default:
        break;


    }



  
}

void CPlayer::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{

    CheckFalse(m_bCanCollision);
    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {


    case Client::COLLISION_GROUP::INTERACTION:
        Check_Interaction_ExitCollision(pOther);
        break;
    default:
        break;


    }



}

void CPlayer::Check_Interaction_Collision(CCollider_Base* pOther)
{
    //들고있는상태에선 밀어내는동작X
    CheckTrue(m_ActionControl.m_bCarry || m_ActionControl.m_bPush);


    CGameObject* pOtherOwner = pOther->Get_Owner();
    CheckNull(pOtherOwner);

    if (pOtherOwner->Get_Tag() == L"JackyIronBall0")
    {
        CInteraction_JackyBall* pJackyBall = dynamic_cast<CInteraction_JackyBall*>(pOtherOwner);
        CheckNull(pJackyBall);

        CGameObject* pBallOwner = pJackyBall->Get_Owner();
        CheckTrue(pBallOwner == nullptr);
        
        if (pBallOwner != this)
        {
            m_ActionControl.m_fDamage = 1.f;

            _vector vDir = (pBallOwner->Get_Transform()->Get_State(STATE::POSITION))
                - (m_pTransformCom->Get_State(STATE::POSITION));

            m_bHitFront = m_pTransformCom->IsFront(vDir);
            m_bCanCollision = false;
        }
    }

    CIInteractable* pInteratable = dynamic_cast<CIInteractable*>(pOtherOwner);
    CheckNull(pInteratable);

    Interact_Behavior_Type BehaviorType = pInteratable->Get_BehaviorType();
    switch (BehaviorType)
    {
    case Interact_Behavior_Type::PUSHABLE:
    case Interact_Behavior_Type::CARRYABLE:
        //여기서 방향벡터가 대각선이라면, push불가
  
    {

        _float3 vLook;

        XMStoreFloat3(&vLook, XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)));
        bool isDiagonal = (fabs(vLook.x) > 0.1f && fabs(vLook.z) > 0.1f);

        if (!isDiagonal)
        {
            if (!m_ActionControl.m_bPush)
            {
                m_ActionControl.m_bPush = true;
            }
        }
    }
        
        
        break;
    case END:
        break;
    default:
        break;
    }
}

void CPlayer::Check_Interaction_Stay_Collision(CCollider_Base* pOther)
{
 
}

void CPlayer::Check_Interaction_ExitCollision(CCollider_Base* pOther)
{
    if (m_ActionControl.m_bPush)
        m_ActionControl.m_bPush = false;

}
