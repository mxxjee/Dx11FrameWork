#include "CPlayer.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CInput_Manager.h"
#include "CModel.h"
#include "CBody.h"
#include "CNavigation.h"




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

    TransDesc.fSpeedPerSec = 5.f;
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
    
    m_iPreState = CModelObject::NONE;
    m_iState = CModelObject::IDLE;


    if (m_pNavigationCom)
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
   
  
    Update_Input(fTimeDelta);
    State_Change();

    /*컨테이너 업데이트*/
    __super::Update(fTimeDelta);
}

void CPlayer::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    Motion_Change();
  
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
    Move_Input(fTimeDelta);
    Event_Input(fTimeDelta);
}


void CPlayer::Move_Input(_float fTimeDelta)
{

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LeftArrow))
    {
        bPressed = true;

        if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Rotation(_float3(0.f, -135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, -45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, -90.f, 0.f));

    }



    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::RightArrow))
    {
        bPressed = true;

        if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Rotation(_float3(0.f, 135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, 45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, 90.f, 0.f));
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::DownArrow))
    {
        bPressed = true;
        m_pTransformCom->Rotation(_float3(0.f, 180.f, 0.f));
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::UpArrow))
    {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
        bPressed = true;
        m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));
    }

    if (m_pInputManager->IsKeyPressed(KeyCode::H))
    {
        iHp--;
        m_pGameInstance->BroadCastEvent(L"OnHeartDamaged",&iHp);

    }

    if (m_pInputManager->IsKeyReleased(KeyCode::UpArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::DownArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::LeftArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::RightArrow))
    {
        bPressed = false;

        Set_State(SET, IDLE);
    }
    if (bPressed)
    {
        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta,Space::Local,m_pNavigationCom);
        Set_State(SET, RUN);
    }


    if (m_pTarget)
        m_pTransformCom->Chase(m_pTarget->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD), fTimeDelta, 5);


    m_pTransformCom->Set_State(STATE::POSITION,
        m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));

}


void CPlayer::Event_Input(_float fTimeDelta)
{

    if (m_pInputManager->IsKeyPressed(KeyCode::B))
    {
        Set_State(SET, ATTACK);
    }


    /*if (m_pInputManager->IsKeyReleased(KeyCode::T))
    {

        if (m_iState & CModelObject::ATTACK)
            m_iState ^= ATTACK;

        m_iState |= IDLE;
    }*/
}



void CPlayer::Motion_Change()
{
    //이전과 현재상태가 다를떄 진입(애니메이션 진입)
    if (m_iPreState != m_iState)
    {
        if (m_iState & ATTACK)
        {
            //어택에진입했으요..

       }
           
        
    }


}

void CPlayer::State_Change()
{
    //모션이 끝난이후 상태바꾸기

    if (m_iState & ATTACK)
    {
        if (m_pBody->Get_IsAnimFinished())
            Set_State(SET, IDLE);

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


