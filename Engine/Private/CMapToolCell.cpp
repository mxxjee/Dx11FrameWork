#include "CMapToolCell.h"

CMapToolCell::CMapToolCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
}

HRESULT CMapToolCell::Initialize_Prototype(const _float3* pPoints, _int iIndex)
{
    return E_NOTIMPL;
}

CMapToolCell* CMapToolCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, _int iIndex)
{
    return nullptr;
}

void CMapToolCell::Free()
{
}
