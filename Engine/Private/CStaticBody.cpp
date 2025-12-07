#include "CStaticBody.h"


CStaticBody::CStaticBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBody(pDevice,pContext)
{
}

CStaticBody::CStaticBody(const CStaticBody& rhs)
	:CBody(rhs)
{
}

CStaticBody* CStaticBody::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CStaticBody* pInstance = new CStaticBody(_pDevice, _pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CStaticBody");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CStaticBody::Clone(void* pArg)
{
    CStaticBody* pInstance = new CStaticBody(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CStaticBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStaticBody::Free()
{
    __super::Free();

}
