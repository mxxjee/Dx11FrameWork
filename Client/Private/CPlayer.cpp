#include "CPlayer.h"
#include "CGameInstance.h"
#include "CTexture.h"
#include "CConstantBuffer.h"
#include "GeometryHelper.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CGeometry.h"
#include "CVertexShader.h"
#include "CPixelShader.h"
#include "CDefaultShader.h"
#include "CInputLayout.h"

#include "Client_Defines.h"



USING(Client)
CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CQuad(pDevice,pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
    : CQuad(rhs)
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
    m_pGameInstance->Add_RenderObject(RENDERGROUP::ALPHA, this);

}

HRESULT CPlayer::Render()
{
    __super::Render();



    return S_OK;
}


void CPlayer::Move_Input(_float fTimeDelta)
{
    if (m_pGameInstance->IsKeyHeld(KeyCode::RightArrow))
        m_pTransformCom->Move(DIRECTION::RIGHT, fTimeDelta);


    if (m_pGameInstance->IsKeyHeld(KeyCode::LeftArrow))
        m_pTransformCom->Move(DIRECTION::LEFT, fTimeDelta);

    if (m_pGameInstance->IsKeyHeld(KeyCode::UpArrow))
        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta);

    if (m_pGameInstance->IsKeyHeld(KeyCode::DownArrow))
        m_pTransformCom->Move(DIRECTION::BACKWARD, fTimeDelta);

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
    __super::Free();
}


