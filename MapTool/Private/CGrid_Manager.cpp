#include "CGrid_Manager.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CVIBuffer.h"


USING(MapTool)
IMPLEMENT_SINGLETON(CGrid_Manager)

CGrid_Manager::CGrid_Manager()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}
HRESULT CGrid_Manager::Set_MainGrid(CGameObject* pGrid)
{
    CheckNullResult(pGrid, E_FAIL);

    CMapGrid* ppGrid = dynamic_cast<CMapGrid*>(pGrid);
    CheckNullResult(ppGrid, E_FAIL);

    m_pMainGrid = ppGrid;
    return S_OK;
}

void CGrid_Manager::Update_Priority(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pMainGrid->Update_Priority(fTimeDelta);

}

void CGrid_Manager::Update(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pMainGrid->Update(fTimeDelta);

}

void CGrid_Manager::Update_Late(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pMainGrid->Update_Late(fTimeDelta);
}

void CGrid_Manager::Update_Render(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::PRIORITY), m_pMainGrid);
}

void CGrid_Manager::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pMainGrid);

}

HRESULT CGrid_Manager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	m_pDevice = _pDevice;
	m_pDeviceContext = _pDeviceContext;



	m_EngineDesc = m_pGameInstance->Get_EngineDesc();

	return S_OK;
}


Triangle* CGrid_Manager::PickTerrain()
{
	
	CheckNullResult(m_pMainGrid,nullptr);

	//레이를 생성한다.
	//freecam의 view/proj가져오기
	_float4x4 Proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
	_float4x4 View = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));

	CheckTrueResult(MathUtils::IsZeroMatrix(Proj), nullptr);
	CheckTrueResult(MathUtils::IsZeroMatrix(View), nullptr);


	Ray ray = MathUtils::CreateRayLocal(m_EngineDesc.hWnd, m_pDeviceContext, m_pMainGrid, Proj, View);

	_uint TerrainX = m_pMainGrid->Get_VIBufferCom()->Get_NumVerticesX();
	_uint TerrainZ = m_pMainGrid->Get_VIBufferCom()->Get_NumVerticesZ();

	const _uint* m_pIndices = m_pMainGrid->Get_VIBufferCom()->Get_Indices();
	const _float3* m_pPositions = m_pMainGrid->Get_VIBufferCom()->Get_VertexPositions();

	_uint IndicesNum = m_pMainGrid->Get_VIBufferCom()->Get_NumIndices();

	for (_uint i = 0; i < IndicesNum; i += 3)
	{


		_vector p0 = XMLoadFloat3(&m_pPositions[m_pIndices[i]]);
		_vector p1 = XMLoadFloat3(&m_pPositions[m_pIndices[i + 1]]);
		_vector p2 = XMLoadFloat3(&m_pPositions[m_pIndices[i + 2]]);

		float dist = 0.f;
		if (TriangleTests::Intersects(ray.Origin, ray.Dir, p0, p1, p2, dist))
		{
			if (isnan(dist))
				return nullptr;



			PickLocalTriangle.v0 = m_pPositions[m_pIndices[i]];
			PickLocalTriangle.v1 = m_pPositions[m_pIndices[i + 1]];
			PickLocalTriangle.v2 = m_pPositions[m_pIndices[i + 2]];

			XMStoreFloat3(&PickingWolrdPos, ray.Origin + ray.Dir * dist);

			return &PickLocalTriangle;

		}



	}



	return nullptr;
}
