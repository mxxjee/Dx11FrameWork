#include "CBounding.h"

CBounding::CBounding(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT CBounding::Update(CTransform* pTransform)
{
    return E_NOTIMPL;
}

HRESULT CBounding::Initialize()
{
    return S_OK;
}

void CBounding::Free()
{
    __super::Free();
}
