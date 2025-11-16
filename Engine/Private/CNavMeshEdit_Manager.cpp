#include "CNavMeshEdit_Manager.h"
#include "CNavEditPreview.h"
#include "CMapToolCell.h"
#include "CVIBuffer_Triangle.h"


IMPLEMENT_SINGLETON(CNavMeshEdit_Manager)

CNavMeshEdit_Manager::CNavMeshEdit_Manager()
{
}

CNavMeshEdit_Manager::~CNavMeshEdit_Manager()
{
}




void CNavMeshEdit_Manager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	m_pPreview = CNavEditPreview::Create(_pDevice, _pContext);
	Init_Points();
	m_PrePoints.resize(3);
}

vector<_uint> CNavMeshEdit_Manager::Get_Edge(const _float3& P, const _float3& A, const _float3& B, const _float3& C)
{
	vector<_uint> Result;

	map<_float,_uint, CompareKey> Distance;

	_vector vP = XMLoadFloat3(&P);

	_vector vA = XMLoadFloat3(&A);//0
	_vector vB = XMLoadFloat3(&B);//1
	_vector vC = XMLoadFloat3(&C);//2
	
	//마우스방향과 가장 가까운 선분을 살려둔다.
	_float PA= XMVectorGetX(XMVector3Length(vP - vA));
	_float PB = XMVectorGetX(XMVector3Length(vP - vB));
	_float PC = XMVectorGetX(XMVector3Length(vP - vC));

	Distance.emplace(PA,0);
	Distance.emplace(PB,1);
	Distance.emplace(PC,2);


	for (auto& pair : Distance)
		Result.push_back(pair.second);

	return Result;

}

void CNavMeshEdit_Manager::Free()
{
	Safe_Release(m_pPreview);
	for (auto& i : m_pMapToolCells)
		Safe_Release(i);
}



void CNavMeshEdit_Manager::UpdatePoints()
{
	CheckNull(m_pPreview);

	//Align_CW();
	m_pPreview->UpdatePoints(m_Points);
	
}

deque<PreviewPoint> CNavMeshEdit_Manager::Align_CW()
{
	CheckFalseResult(Check_FullPoints(m_Points), deque<PreviewPoint>());
	
	deque<PreviewPoint> CwQue=m_Points;

	vector< PreviewPoint> CW;
	for (int i = 0; i < 3; ++i)
	{
		CW.push_back(CwQue.front());
		CwQue.pop_front();

	}

	_float3 v0, v1;

	XMStoreFloat3(&v0, XMLoadFloat3(&CW[ENUM_TO_UINT(POINTType::B)].vPos) - XMLoadFloat3(&CW[ENUM_TO_UINT(POINTType::A)].vPos));
	XMStoreFloat3(&v1, XMLoadFloat3(&CW[ENUM_TO_UINT(POINTType::C)].vPos) - XMLoadFloat3(&CW[ENUM_TO_UINT(POINTType::A)].vPos));

	_float vCross = (v0.x * v1.z) - (v0.z * v1.x);
	
	//음수라면, BC를 스왑한다.
	if (vCross < 0.f)
	{
		PreviewPoint B = CW[ENUM_TO_UINT(POINTType::B)];
		PreviewPoint C = CW[ENUM_TO_UINT(POINTType::C)];

		swap(B, C);
		CW[ENUM_TO_UINT(POINTType::B)] = B;
		CW[ENUM_TO_UINT(POINTType::C)] = C;

		

	}

	//새로운 걸로 리턴
	for (int i = 0; i < 3; ++i)
		CwQue.push_back(CW[i]);

	return CwQue;
}



bool CNavMeshEdit_Manager::Check_EmptyPoints(const  deque<PreviewPoint>& Points)
{
	_vector vPos[3];
	for (int i = 0; i < 3; ++i)
	{
		vPos[i] = XMLoadFloat3(&Points[i].vPos);
	}

	bool bResult = XMVector3Equal(vPos[0], XMVectorSet(-999.f,-999.f,-999.f,1.f));
	bool bResult2 = XMVector3Equal(vPos[1], XMVectorSet(-999.f, -999.f, -999.f, 1.f));
	bool bResult3 = XMVector3Equal(vPos[2], XMVectorSet(-999.f, -999.f, -999.f, 1.f));

	return bResult && bResult2 && bResult3;

}

bool CNavMeshEdit_Manager::Check_FullPoints(const  deque<PreviewPoint>& Points)
{
	_vector vPos[3];
	for (int i = 0; i < 3; ++i)
	{
		vPos[i] = XMLoadFloat3(&Points[i].vPos);
	}

	bool bResult = XMVector3Equal(vPos[0], XMVectorSet(-999.f, -999.f, -999.f, 1.f));
	bool bResult2 = XMVector3Equal(vPos[1], XMVectorSet(-999.f, -999.f, -999.f, 1.f));
	bool bResult3 = XMVector3Equal(vPos[2], XMVectorSet(-999.f, -999.f, -999.f, 1.f));

	if (!bResult && !bResult2 && !bResult3)
		return true;

	else
		return false;
}

void CNavMeshEdit_Manager::Init_Points()
{
	m_Points.resize(3);
	for (int i = 0; i < 3; ++i)
		m_Points[i].vPos = _float3(-999.f, -999.f, -999.f);


}

void CNavMeshEdit_Manager::Set_DrawPoint(_float3 v,bool bRegister)
{
	PreviewPoint Preview;

	Preview.vPos = v;
	Preview.vRegister = bRegister;

	
	


	//만약 아직클릭하지않았따면, 결정안한것이므로 그냥 기존꺼 변경

	if (!bRegister )
	{
		if (!m_bClear)
		{
			if (!XMVector3Equal(XMLoadFloat3(&v), XMLoadFloat3(&m_Points[2].vPos)))
			{
				if ((!m_bFixCell && !m_bFixEdge) || (!m_bFixCell))
				{
					if (!m_bFixEdge)
						pTargetCell = Find_NeareastCell(v);
				}


				CheckNull(pTargetCell);

				pTargetPreviewPoints = pTargetCell->Get_PreviewPoints();

				CheckNull(pTargetPreviewPoints);

				if (!m_bFixEdge)
					LastIdx = Get_Edge(v, pTargetPreviewPoints[0].vPos, pTargetPreviewPoints[1].vPos, pTargetPreviewPoints[2].vPos);

				CheckTrue(LastIdx.empty());

				//마지막 두점을 사용한다.
				int last = (LastIdx[1]) % 3;
				int prev = (LastIdx[0]) % 3;


				PreviewPoint lastP = pTargetPreviewPoints[last];
				PreviewPoint prevP = pTargetPreviewPoints[prev];

				//재구성
				m_Points[0] = prevP;
				m_Points[1] = lastP;

				m_Points[0].vRegister = true;
				m_Points[1].vRegister = true;

				m_Points[2] = Preview;

				//// 4) 다음 클릭은 index 2 부터 시작
				iDrawIdx = 2;
				m_bCheckNextEdge = false;
			}
		}
		

		else
		{
			m_Points[iDrawIdx] = Preview;
		}

	}

	else 
	{
		
		m_Points[iDrawIdx] = Preview;
		iDrawIdx = (iDrawIdx + 1) % 3;
	

	}
		

	
	UpdatePoints();

	
}

CMapToolCell* CNavMeshEdit_Manager::Find_NeareastCell(const _float3& vMousePos)
{
	CMapToolCell* pResult = nullptr;
	float minDist = FLT_MAX;

	//삼각형의 중점을 기준으로 가장가까운 삼각형색출
	for (auto& cell : m_pMapToolCells)
	{
		_float3* pPoints = cell->Get_vPoints();
		_float3 fCenter;
		XMStoreFloat3(&fCenter, (XMLoadFloat3(&pPoints[0]) + XMLoadFloat3(&pPoints[1]) + XMLoadFloat3(&pPoints[2])) / 3.f);


		
		
		float fDistance = XMVectorGetX(
			XMVector3Length(XMLoadFloat3(&fCenter)- XMLoadFloat3(&vMousePos))
		);

		if (fDistance < minDist)
		{
			pResult = cell;
			minDist = fDistance;
		}

	}

	return pResult;
}

HRESULT CNavMeshEdit_Manager::Create_MapToolCell(const deque<PreviewPoint>& New, const deque<PreviewPoint>& Origin)
{
	_float3 vPos[3];
	int Idx = 0;

	for (int i = 0; i < 3; ++i)
		vPos[i] = New[i].vPos;


	CMapToolCell::MAPTOOLCELL_DESC Desc;
	Desc.iIdx = m_pMapToolCells.size();
	
	CVIBuffer_Triangle::TRIANGLEBUFFER_DESC  TriangleDesc;
	TriangleDesc.v0 = vPos[0];
	TriangleDesc.v1 = vPos[1];
	TriangleDesc.v2 = vPos[2];

	Desc.TriangleCom = &TriangleDesc;

	CMapToolCell* pInstance = CMapToolCell::Create(m_pDevice, m_pContext, &Desc);
	if (!pInstance)
		return E_FAIL;

	pInstance->Set_PreviewPoints(Origin);
	m_pMapToolCells.push_back(pInstance);

	return S_OK;
}

void CNavMeshEdit_Manager::Update(_float fTimeDelta)
{
	//3개의 점이 다 채워졌는지 보고, 삼각형만들기.
	int RegisterCnt = count_if(m_Points.begin(), m_Points.end(), [](const PreviewPoint & A)
		{
			return A.vRegister == true;
		});


	if (RegisterCnt == 3)
	{
		m_PrePoints = m_Points;
		deque<PreviewPoint> New= Align_CW();
		//정렬한 정점으로 생성..
		if (FAILED(Create_MapToolCell(New, m_PrePoints)))
			return;


		//m_Points = New;
		m_bCheckNextEdge = true;
		if (m_bClear)
			m_bClear = false;
	}

}

void CNavMeshEdit_Manager::Render()
{
	CheckNull(m_pPreview);
	CheckTrue(Check_EmptyPoints(m_Points));

	m_pPreview->Render();
	for (auto& cell : m_pMapToolCells)
		cell->Render();
}

void CNavMeshEdit_Manager::Clear_Points()
{
	iDrawIdx = 0;
	m_Points.clear();
	Init_Points();
	m_PrePoints.resize(3);
	m_bClear = true;
}

