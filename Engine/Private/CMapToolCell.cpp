#include "CMapToolCell.h"
#include "CVIBuffer_Triangle.h"
#include "CGameInstance.h"
#include "CShader.h"



CMapToolCell::CMapToolCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

void CMapToolCell::Set_PreviewPoints(const deque<PreviewPoint>& New)
{
    for (int i = 0; i < New.size(); ++i)
    {
        m_PreviewPoints[i] = New[i];
    }
}

HRESULT CMapToolCell::Initialize_Prototype(void* pArg)
{
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;


    m_pShader = m_pGameInstance->Find_Shader(L"VtxPosCor");
    Safe_AddRef(m_pShader);

    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

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

HRESULT CMapToolCell::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MAPTOOLCELL_DESC* pDesc = static_cast<MAPTOOLCELL_DESC*>(pArg);

    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Triangle"), pDesc->TriangleCom));
    m_pVIBufferCom = dynamic_cast<CVIBuffer_Triangle*>(pBuffer);
   

    return S_OK;
}

HRESULT CMapToolCell::Ready_Resource(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MAPTOOLCELL_DESC* pDesc = static_cast<MAPTOOLCELL_DESC*>(pArg);


    if (pDesc->TriangleCom)
    {
        CVIBuffer_Triangle::TRIANGLEBUFFER_DESC* pTriangleDesc = static_cast<CVIBuffer_Triangle::TRIANGLEBUFFER_DESC*>(pDesc->TriangleCom);

        m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::A)] = pTriangleDesc->v0;
        m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::B)] = pTriangleDesc->v1;
        m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::C)] = pTriangleDesc->v2;


    }

    m_CellInfo.m_iIndex = pDesc->iIdx;

    _vector vLine = {};


    vLine= XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::B)]) - XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::A)]), 0.f));
    m_CellInfo.m_vNormals[ENUM_TO_UINT(LINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::C)]) - XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::B)]), 0.f));
    m_CellInfo.m_vNormals[ENUM_TO_UINT(LINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::A)]) - XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::C)]), 0.f));
    m_CellInfo.m_vNormals[ENUM_TO_UINT(LINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));


    return S_OK;

}

HRESULT CMapToolCell::Render()
{
    CheckNullResult(m_pShader, E_FAIL);

    ComPtr<ID3D11RasterizerState> pOldRS = nullptr;
    m_pContext->RSGetState(pOldRS.GetAddressOf());

    m_pContext->RSSetState(m_pWireframeRS.Get());

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
        return E_FAIL;

    _float4x4 ViewProjMatrix;
    XMStoreFloat4x4(&ViewProjMatrix, m_pGameInstance->Get_ViewProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE)));

    if (m_pShader->Bind_Matrix("g_ViewProjMatrix", ViewProjMatrix))
        return E_FAIL;

    if (m_pShader->Bind_Vector("g_Color", _float4(0.f, 1.f, 0.f, 1.f)))
        return E_FAIL;

    if (FAILED(m_pShader->Begin("Default")))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;          //IA단계

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pContext->RSSetState(pOldRS.Get());


    return S_OK;
}

CMapToolCell* CMapToolCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CMapToolCell* pInstnace = new CMapToolCell(pDevice, pContext);
    if (FAILED(pInstnace->Initialize_Prototype(pArg)))
    {
        MSG_BOX("FAiled To Create:CMAptool");
        Safe_Release(pInstnace);

    }
    return pInstnace;
}

void CMapToolCell::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pVIBufferCom);

}

void CMapToolCell::Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation)
{
    string Tag = to_string(m_CellInfo.m_iIndex) + "Cell";

    ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Name : %s", Tag.c_str());

    string Type = "NavMesh";

    ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Type: %s", Type.c_str());

    //자신인덱스출력
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    ImGui::BulletText("Index:%d", m_CellInfo.m_iIndex);
    ImGui::PopStyleColor();


    //점3개출력
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    for(int i=0;i<3;++i)
        ImGui::BulletText("Position X:%f, Y:%f, Z:%f", m_CellInfo.m_vPoints[i].x, 
            m_CellInfo.m_vPoints[i].y,
            m_CellInfo.m_vPoints[i].z);


    ImGui::PopStyleColor();

    //자신 네이버 출력
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    ImGui::BulletText("Neighbor= AB:%d, BC:%d, CA:%d", m_CellInfo.m_iNeighbors[ENUM_TO_UINT(LINE::AB)], 
                                                        m_CellInfo.m_iNeighbors[ENUM_TO_UINT(LINE::BC)],
                                                        m_CellInfo.m_iNeighbors[ENUM_TO_UINT(LINE::CA)]);
    ImGui::PopStyleColor();

    //ImGui::Separator();
    //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
    //ImGui::BulletText("Rotation X:%f, Y:%f, Z:%f",
    //    rResult.x,
    //    rResult.y,
    //    rResult.z);
    //ImGui::PopStyleColor();



}

bool CMapToolCell::Compare(_vector PointA, _vector PointB)
{
  
    if (true == XMVector3NearEqual(PointA, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::A)]), XMVectorSet(EPS, EPS, EPS, 0.f)))
    {
        if (true == XMVector3Equal(PointB, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::B)])))
            return true;

        if (true == XMVector3Equal(PointB, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::C)])))
            return true;
    }

    if (true == XMVector3NearEqual(PointA, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::B)]), XMVectorSet(EPS, EPS, EPS, 0.f)))
    {
        if (true == XMVector3Equal(PointB, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::C)])))
            return true;

        if (true == XMVector3Equal(PointB, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::A)])))
            return true;
    }

    if (true == XMVector3NearEqual(PointA, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::C)]), XMVectorSet(EPS, EPS, EPS, 0.f)))
    {
        if (true == XMVector3Equal(PointB, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::A)])))
            return true;

        if (true == XMVector3Equal(PointB, XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::B)])))
            return true;
    }

    return false;
}

void CMapToolCell::Set_Neighbor(LINE eLine, CMapToolCell* pCell)
{
    m_CellInfo.m_iNeighbors[ENUM_TO_UINT(eLine)] = pCell->Get_CellInfo().m_iIndex;
}

void CMapToolCell::OnSeletected(bool bSelected)
{
}

void CMapToolCell::Save_To_Json(json& Json)
{
}

void CMapToolCell::Show_Gizmo()
{
}
