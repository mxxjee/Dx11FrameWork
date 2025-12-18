#include "CMapToolCell.h"
#include "CVIBuffer_Triangle.h"
#include "CGameInstance.h"
#include "CShader.h"
#include "CInput_Manager.h"
#include "CNavMeshEdit_Manager.h"
#include "CMapObject_Manager.h"



CMapToolCell::CMapToolCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    g_Color = _float4(0.f, 1.f, 0.f, 1.f);
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


    m_pShader = m_pGameInstance->Find_Shader(L"VtxPos");
    Safe_AddRef(m_pShader);

    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    if (FAILED(Create_WireFrameRS()))
        return E_FAIL;

    if (FAILED(Create_SolidRS()))
        return E_FAIL;


    return S_OK;
}

void CMapToolCell::UpdatePoints(deque<PreviewPoint> Points)
{
    _float3 fPoints[3] = {
       _float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f) };

    int iIdx = 0;

    while (!Points.empty())
    {
        _float3 vPos = Points.front().vPos;
        fPoints[iIdx] = vPos;

        Points.pop_front();


        if (!XMVector3Equal(XMLoadFloat3(&vPos), XMVectorSet(-999.f, -999.f, -999.f, 1.f)))
            ++iIdx;
    }



    if (iIdx == 1)
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    else if (iIdx == 2)
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    else if (iIdx == 3)
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    else
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_pVIBufferCom->UpdatePoints(fPoints[0], fPoints[1], fPoints[2]);


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

    m_pContext->RSSetState(m_pCurrentRS.Get());

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
        return E_FAIL;

    _float4x4 ViewProjMatrix;
    XMStoreFloat4x4(&ViewProjMatrix, m_pGameInstance->Get_ViewProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE)));

    if (m_pShader->Bind_Matrix("g_ViewProjMatrix", ViewProjMatrix))
        return E_FAIL;

    if (m_pShader->Bind_Vector("g_Color", g_Color))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;          //IA단계

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pContext->RSSetState(pOldRS.Get());


    return S_OK;
}

void CMapToolCell::Set_Plane()
{
   
    _vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::A)]),
        XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::B)]),
        XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(POINTType::C)]));


    XMStoreFloat4(&m_CellInfo.m_Plane, vPlane);
 
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

    m_pWireframeRS.Reset();

    Safe_Release(m_pShader);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pVIBufferCom);

}

HRESULT CMapToolCell::Create_WireFrameRS()
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

    m_pCurrentRS = m_pWireframeRS;

    return S_OK;
}

HRESULT CMapToolCell::Create_SolidRS()
{
    D3D11_RASTERIZER_DESC desc{};
    desc.FillMode = D3D11_FILL_SOLID;  // 선 모드
    desc.CullMode = D3D11_CULL_BACK;       // 뒷면도 그리게
    desc.FrontCounterClockwise = FALSE;
    desc.DepthClipEnable = TRUE;           // 보통 TRUE

    if (FAILED(m_pDevice->CreateRasterizerState(&desc, m_pSolidRS.GetAddressOf())))
    {
        MSG_BOX("Failed to Create Wireframe RasterizerState");
        return E_FAIL;
    }

    return S_OK;
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

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    ImGui::BulletText("Neighbor= AB:%d, BC:%d, CA:%d", m_CellInfo.m_iNeighbors[ENUM_TO_UINT(LINE::AB)], 
                                                        m_CellInfo.m_iNeighbors[ENUM_TO_UINT(LINE::BC)],
                                                        m_CellInfo.m_iNeighbors[ENUM_TO_UINT(LINE::CA)]);
    ImGui::PopStyleColor();


    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    auto A = m_CellInfo.m_Plane.x;
    auto B = m_CellInfo.m_Plane.y;
    auto C = m_CellInfo.m_Plane.z;
    auto D = m_CellInfo.m_Plane.w;

    ImGui::Text("Plane: %.3f x %+.3f y %+.3f z %+.3f = 0", A, B, C, D);
    ImGui::PopStyleColor();


    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
    ImGui::BulletText("Type:%s",
        Convert_String_To_Type(static_cast<CellType>(m_CellInfo.CellType)).c_str());
    ImGui::PopStyleColor();

    ///라디오버튼..?
   
    ImGui::RadioButton("Default", (int*)&m_CellInfo.CellType, (int)CellType::DEFAULT);
    ImGui::RadioButton("FALL", (int*)&m_CellInfo.CellType, (int)CellType::FALL);


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
    //초기화때려주고..
    
    m_CellInfo.m_iNeighbors[ENUM_TO_UINT(eLine)] =-1;

    CheckNull(pCell);
    m_CellInfo.m_iNeighbors[ENUM_TO_UINT(eLine)] = pCell->Get_CellInfo().m_iIndex;
}

void CMapToolCell::Update_SelectMode(float _fTimeDelta)
{
  
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Delete))
    {

        CNavMeshEdit_Manager::GetInstance()->RequestDestory(this);
        CMapObject_Manager::GetInstance()->Set_SelectObject(nullptr);
    }


  
 
}

string CMapToolCell::Convert_String_To_Type(CellType eType)
{

    string StateDebugStr = "";

    switch (eType)
    {
    case CellType::DEFAULT:
        StateDebugStr = "DEFAULT";
        break;

    case CellType::FALL:
        StateDebugStr = "FALL";
        break;
    }
   


    return StateDebugStr;
}

HRESULT CMapToolCell::Set_WireFrameMode()
{
    m_passName = "WireFrame";


    return S_OK;
}

HRESULT CMapToolCell::Set_SolidMode()
{
    m_passName = "Default";


    return S_OK;
}


void CMapToolCell::OnSeletected(bool bSelected)
{
    D3D11_RASTERIZER_DESC desc{};
    if (bSelected)
    {

        Set_SolidMode();

        //이웃에 대해서도..
        for (int i = 0; i < ENUM_TO_UINT(LINE::END); ++i)
        {
            int TargetIdx = m_CellInfo.m_iNeighbors[ENUM_TO_UINT(i)];
            if (TargetIdx == -1)
                continue;

            CMapToolCell* pCell = CNavMeshEdit_Manager::GetInstance()->Get_MapToolCell(TargetIdx);
            if (pCell)
            {
                pCell->g_Color = _float4(0.f, 0.f, 1.f, 0.5f);
                pCell->Set_SolidMode();
            }
                
        }

    }

    else
    {
        Set_WireFrameMode();
        //이웃에 대해서도..
        for (int i = 0; i < ENUM_TO_UINT(LINE::END); ++i)
        {
            int TargetIdx = m_CellInfo.m_iNeighbors[ENUM_TO_UINT(i)];
            if (TargetIdx == -1)
                continue;

            CMapToolCell* pCell = CNavMeshEdit_Manager::GetInstance()->Get_MapToolCell(TargetIdx);
            if (pCell)
            {
                pCell->g_Color = _float4(0.f, 1.f, 0.f, 1.f);
                pCell->Set_WireFrameMode();
            }
        }
    }
       


  
}

void CMapToolCell::Save_To_Json(json& Json)
{
}

void CMapToolCell::Show_Gizmo()
{
}
