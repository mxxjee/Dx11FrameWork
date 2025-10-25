#include "CCollider_Base.h"

CCollider_Base::CCollider_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{

}

CCollider_Base::CCollider_Base(const CCollider_Base& Prototype)
    :CComponent(Prototype)
{
}

HRESULT CCollider_Base::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCollider_Base::Initialize_Copytype(void* pArg)
{
    return S_OK;
}



void CCollider_Base::Free()
{
    __super::Free();
}
