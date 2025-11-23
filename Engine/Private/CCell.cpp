#include "CCell.h"
#include "CVIBuffer_Triangle.h"
#include "CGameInstance.h"


CCell::CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}


_float CCell::Compute_Height(_vector vCellTargetPos)
{

    return (-(m_CellInfo.m_Plane.x) * XMVectorGetX(vCellTargetPos) - (m_CellInfo.m_Plane.z) * XMVectorGetZ(vCellTargetPos) - (m_CellInfo.m_Plane.w)) / (m_CellInfo.m_Plane.y);

}


HRESULT CCell::Initialize_Prototype(const DefaultCellInfo& Info)
{
    memcpy(&m_CellInfo, &Info, sizeof(DefaultCellInfo));


    if (FAILED(Ready_Components()))
        return E_FAIL;


    if (FAILED(Create_WireFrameRS()))
        return E_FAIL;

    return S_OK;

}

HRESULT CCell::Ready_Components()
{
    CVIBuffer_Triangle::TRIANGLEBUFFER_DESC TriangleDesc;

    TriangleDesc.v0 = m_CellInfo.m_vPoints[0];
    TriangleDesc.v1 = m_CellInfo.m_vPoints[1];
    TriangleDesc.v2 = m_CellInfo.m_vPoints[2];


    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Triangle"), &TriangleDesc));
    m_pVIBufferCom = dynamic_cast<CVIBuffer_Triangle*>(pBuffer);


    return S_OK;
}

_bool CCell::isIn(_fvector vResultPos, _int* pNeighborIndex)
{
   
    for (int i = 0; i < ENUM_TO_UINT(LINE::END); ++i)
    {
        //시작지점에서부터 현재지점까지의 방향과
        _vector		vDir = XMVector3Normalize(vResultPos - XMLoadFloat3(&m_CellInfo.m_vPoints[i]));

        //시작지점의 방향벡터를 내적했을때 양수이면 밖으로나간것임.
        if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_CellInfo.m_vNormals[i]))))
        {
            *pNeighborIndex = m_CellInfo.m_iNeighbors[i];
            return false;
        }
    }
    return true;
}



_float CCell::Compute_Cost(const vector<CCell*>& Cells, _int iGoalIndx)
{
    _float fPCost = {};
    if (-1 != m_iParentIndex)
        fPCost= XMVectorGetX(XMVector3Length(Cells[m_iParentIndex]->Get_CenterPos() - Get_CenterPos()));


    _float		fGCost = XMVectorGetX(XMVector3Length(Cells[iGoalIndx]->Get_CenterPos() - Get_CenterPos()));

    m_fTotalCost = fPCost + fGCost;

    return m_fTotalCost;
}

_vector CCell::Get_CenterPos()
{
    _vector		vPosition = {};

    for (size_t i = 0; i < 3; i++)
    {
        vPosition += XMLoadFloat3(&m_CellInfo.m_vPoints[i]);
    }

    return vPosition / 3.f;
}


void CCell::Update_Render()
{
}

HRESULT CCell::Render()
{
    ComPtr<ID3D11RasterizerState> pOldRS = nullptr;
    m_pContext->RSGetState(pOldRS.GetAddressOf());
    m_pContext->RSSetState(m_pWireframeRS.Get());

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;          //IA단계

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    m_pContext->RSSetState(pOldRS.Get());


    return S_OK;

}

HRESULT CCell::Create_WireFrameRS()
{
    D3D11_RASTERIZER_DESC desc{};
    desc.FillMode = D3D11_FILL_WIREFRAME;  // 선 모드
    desc.CullMode = D3D11_CULL_BACK;       // 뒷면도 그리게
    desc.FrontCounterClockwise = FALSE;
    desc.DepthClipEnable = TRUE;           // 보통 TRUE

    if (FAILED(m_pDevice->CreateRasterizerState(&desc, m_pWireframeRS.GetAddressOf())))
    {
        MSG_BOX("Failed to Create Wireframe RasterizerState");
        return E_FAIL;
    }


    return S_OK;
}

CCell* CCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const DefaultCellInfo& Info)
{
    CCell* pInstance = new CCell(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(Info)))
    {
        MSG_BOX("Failed to Create :CCell");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CCell::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pGameInstance);
}
