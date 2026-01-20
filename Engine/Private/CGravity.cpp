#include "CGravity.h"
#include "..\Public\CGravity.h"


CGravity::CGravity(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{
}

CGravity::CGravity(const CGravity& Prototype)
    :CComponent(Prototype),
    m_Gravity(Prototype.m_Gravity),
    m_bEnableGravity(Prototype.m_bEnableGravity),
    m_bIsJumping(Prototype.m_bIsJumping),
    m_bOnGround(Prototype.m_bOnGround),
    MaxFallSpeed(Prototype.MaxFallSpeed),
    m_VelocityY(Prototype.m_VelocityY)
{
}

HRESULT CGravity::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGravity::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGravity::Update(_float fTimeDelta)
{
    CheckFalse(m_bEnableGravity);


    //이게음수라면, 떨어지고있는중임.
    m_VelocityY += m_Gravity * fTimeDelta;


    if (m_VelocityY < MaxFallSpeed)
        m_VelocityY = MaxFallSpeed;
}

CGravity* CGravity::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CGravity* pInstance = new CGravity(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CGravity");
        Safe_Release(pInstance);
    }


    return pInstance;
}

CComponent* CGravity::Clone(void* pArg)
{
    CGravity* pInstance = new CGravity(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CGravity");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CGravity::Free()
{
    __super::Free();
}
