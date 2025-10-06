#include "CFreeCamera.h"
#include "CPerspectiveCameraComponent.h"

CFreeCamera::CFreeCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CFreeCamera::CFreeCamera(const CFreeCamera& rhs)
    : CGameObject(rhs), m_pPerspectiveCameraCom(rhs.m_pPerspectiveCameraCom)
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFreeCamera::Initialize_Copytype(void* pArg)
{
    return S_OK;
}

void CFreeCamera::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CFreeCamera::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    Mouse_Move();

    if (GetKeyState(VK_RIGHT) & 0x8000)
        m_pTransformCom->Move(DIRECTION::RIGHT, fTimeDelta);

    if (GetKeyState(VK_LEFT) & 0x8000)
        m_pTransformCom->Move(DIRECTION::LEFT, fTimeDelta);

    if (GetKeyState(VK_UP) & 0x8000)
        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta);

    if (GetKeyState(VK_DOWN) & 0x8000)
        m_pTransformCom->Move(DIRECTION::BACKWARD, fTimeDelta);
}

void CFreeCamera::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    m_pPerspectiveCameraCom->Update_ViewMatrix(fTimeDelta);

}

void CFreeCamera::Update_Render(_float fTimeDelta)
{
}

HRESULT CFreeCamera::Render()
{
    return E_NOTIMPL;
}

void CFreeCamera::Mouse_Move()
{
}

CFreeCamera* CFreeCamera::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    return nullptr;
}

CGameObject* CFreeCamera::Clone(void* pArg)
{
    CFreeCamera* pInstance = new CFreeCamera(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CFreeCamera ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CFreeCamera::Free()
{
}
