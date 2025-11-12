#include "CPlayer.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CInput_Manager.h"
#include "CModel.h"
#include "CBody.h"




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

    desc.BodyDesc = &BodyDesc;


    if(FAILED(Ready_PartObjects(&desc)))
        return E_FAIL;
    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
  
    Update_Input(fTimeDelta);

    /*컨테이너 업데이트*/
    __super::Update(fTimeDelta);
}

void CPlayer::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CPlayer::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
   
}

HRESULT CPlayer::Render()
{
  /*이제 각 파츠들이 rendergroup으로 들어가서 렌더한다.*/


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

        if (m_iState & CModelObject::RUN)
            m_iState ^= RUN;

        m_iState |= IDLE;
    }
    if (bPressed)
    {
        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta);
        if (m_iState & CModelObject::IDLE)
            m_iState ^= IDLE;

        m_iState |= RUN;
    }


    if (m_pTarget)
        m_pTransformCom->Chase(m_pTarget->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD), fTimeDelta, 5);


}

void CPlayer::Event_Input(_float fTimeDelta)
{

    if (m_pInputManager->IsKeyPressed(KeyCode::T))
    {
        if (m_iState & CModelObject::IDLE)
            m_iState ^= IDLE;

        m_iState |= ATTACK;
    }


    if (m_pInputManager->IsKeyReleased(KeyCode::T))
    {

        if (m_iState & CModelObject::ATTACK)
            m_iState ^= ATTACK;

        m_iState |= IDLE;
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
    __super::Free();
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

    }
    return S_OK;
}


