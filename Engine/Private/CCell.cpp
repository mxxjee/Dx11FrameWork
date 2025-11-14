#include "CCell.h"

CCell::CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),m_pContext(pContext)
{
}

bool CCell::Compare(_fvector vTargetPointA, _fvector vTargetPointB)
{
    //만약 인자로 들어온 점과 a가같으면 ,다른점이 B와 C랑 같은지비교
    if (true == XMVector3Equal(vTargetPointA, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::A)])))
    {
        if (true == XMVector3Equal(vTargetPointB, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::B)])))
            return true;


        if (true == XMVector3Equal(vTargetPointB, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::C)])))
            return true;

    }

    //만약 인자로 들어온 점과 B가같으면 ,다른점이 A와 C랑 같은지비교
    if (true == XMVector3Equal(vTargetPointA, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::B)])))
    {
        if (true == XMVector3Equal(vTargetPointB, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::A)])))
            return true;


        if (true == XMVector3Equal(vTargetPointB, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::C)])))
            return true;

    }

    //만약 인자로 들어온 점과 C가같으면 ,다른점이 A와 B랑 같은지비교
    if (true == XMVector3Equal(vTargetPointA, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::C)])))
    {
        if (true == XMVector3Equal(vTargetPointB, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::A)])))
            return true;


        if (true == XMVector3Equal(vTargetPointB, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::B)])))
            return true;

    }


    return false;
}

HRESULT CCell::Initialize_Prototype(const _float3* pPoints, _int iIndex)
{
    memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_TO_UINT(POINTType::END));

    m_iIndex = iIndex;
    
    _vector vLine = {};


    
    //각 세 변에 대한 법선벡터 만들기,XZ평면에 대해서만 수행하므로 y=0 으로 만든 후 법선을 구한다.
    //xz평면의 법선 -> (a,b) -> (-b,a)
    vLine= XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::B)]) - XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::A)]), 0.f));
    m_vNormals[ENUM_TO_UINT(LINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::C)]) - XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::B)]), 0.f));
    m_vNormals[ENUM_TO_UINT(LINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::A)]) - XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(POINTType::C)]), 0.f));
    m_vNormals[ENUM_TO_UINT(LINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    return S_OK;

}

CCell* CCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, _int iIndex)
{

    CCell* pInstance = new CCell(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(pPoints, iIndex)))
    {
        MSG_BOX("Failed to Create :CCell ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CCell::Free()
{
    __super::Free();
}
