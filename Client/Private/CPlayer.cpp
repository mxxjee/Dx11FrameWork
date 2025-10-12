#include "CPlayer.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CInput_Manager.h"



USING(Client)
CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CQuad(pDevice,pContext),m_pInputManager(CInput_Manager::GetInstance())
{
    Safe_AddRef(m_pInputManager);
}

CPlayer::CPlayer(const CPlayer& rhs)
    : CQuad(rhs),m_pInputManager(rhs.m_pInputManager)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;
 
    m_eRenderGroup = RENDERGROUP::ALPHA;
    /*m_pTarget = m_pGameInstance->Find_Camera(L"FreeCam");*/


    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);

    Move_Input(fTimeDelta);


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
    __super::Render();



    return S_OK;
}


void CPlayer::Move_Input(_float fTimeDelta)
{
    /*if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::RightArrow))
    {
        bPressed = true;

        if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, -135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Rotation(_float3(0.f, -45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, -90.f, 0.f));
    }



    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LeftArrow))
    {
        bPressed = true;

        if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, 135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Rotation(_float3(0.f, 45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, 90.f, 0.f));
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::UpArrow))
    {
        bPressed = true;
        m_pTransformCom->Rotation(_float3(0.f, 180.f, 0.f));
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::DownArrow))
    {
        bPressed = true;
        m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));
    }


    if (m_pInputManager->IsKeyReleased(KeyCode::UpArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::DownArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::LeftArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::RightArrow))
        bPressed = false;

      if (bPressed)
        m_pTransformCom->Move(DIRECTION::FORWARD, (-1)*fTimeDelta);
*/

    if (m_pTarget)
        m_pTransformCom->Chase(m_pTarget->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD), fTimeDelta, 5);

   
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


