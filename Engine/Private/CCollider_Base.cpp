#include "CCollider_Base.h"

CCollider_Base::CCollider_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{

}

CCollider_Base::CCollider_Base(const CCollider_Base& Prototype)
    :CComponent(Prototype),
    m_WolrdMatrix(Prototype.m_WolrdMatrix),
    m_bActive(Prototype.m_bActive),
    m_bDebugDraw(Prototype.m_bDebugDraw),
    vOffset(Prototype.vOffset),
    vScaleOffSet(Prototype.vScaleOffSet),
    fRadius(Prototype.fRadius),
    vCenter(Prototype.vCenter)
{
    m_WolrdMatrix = XMMatrixIdentity();
}

HRESULT CCollider_Base::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCollider_Base::Initialize_Copytype(void* pArg)
{
    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);

    vOffset = pDesc->Offset;
    vScaleOffSet = pDesc->vScaleOffSet;

    return S_OK;
}



void CCollider_Base::Free()
{
    __super::Free();
}
