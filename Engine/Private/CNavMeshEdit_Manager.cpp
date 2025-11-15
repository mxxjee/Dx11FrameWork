#include "CNavMeshEdit_Manager.h"
#include "CNavEditPreview.h"
#include "CMapToolCell.h"


IMPLEMENT_SINGLETON(CNavMeshEdit_Manager)

CNavMeshEdit_Manager::CNavMeshEdit_Manager()
{
}

CNavMeshEdit_Manager::~CNavMeshEdit_Manager()
{
}


void CNavMeshEdit_Manager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	m_pPreview = CNavEditPreview::Create(_pDevice, _pContext);
	Init_Points();
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
	CheckFalseResult(Check_FullPoints(), deque<PreviewPoint>());
	
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

	_float vCross = v0.x * v1.z - v0.z - v1.x;
	
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

void CNavMeshEdit_Manager::Set_DrawIdx()
{
	deque<PreviewPoint> vNew = m_Points;
	vector<PreviewPoint> vNewPoint;

	//drawidx를 기준으로 다시정렬.
	for (int i = 0; i < 3; ++i)
	{
		vNewPoint.push_back(vNew.front());
		vNew.pop_front();
	}

	sort(vNewPoint.begin(), vNewPoint.end(),[](const PreviewPoint& A, const PreviewPoint& B)
		{
			return A.m_iDrawIdx < B.m_iDrawIdx;
		});


	for (int i = 0; i < 3; ++i)
		vNew.push_back(vNewPoint[i]);

	m_Points.swap(vNew);


}

bool CNavMeshEdit_Manager::Check_EmptyPoints()
{
	_vector vPos[3];
	for (int i = 0; i < 3; ++i)
	{
		vPos[i] = XMLoadFloat3(&m_Points[i].vPos);
	}

	bool bResult = XMVector3Equal(vPos[0], XMVectorSet(-999.f,-999.f,-999.f,1.f));
	bool bResult2 = XMVector3Equal(vPos[1], XMVectorSet(-999.f, -999.f, -999.f, 1.f));
	bool bResult3 = XMVector3Equal(vPos[2], XMVectorSet(-999.f, -999.f, -999.f, 1.f));

	return bResult && bResult2 && bResult3;

}

bool CNavMeshEdit_Manager::Check_FullPoints()
{
	_vector vPos[3];
	for (int i = 0; i < 3; ++i)
	{
		vPos[i] = XMLoadFloat3(&m_Points[i].vPos);
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
	Preview.m_iDrawIdx = iDrawIdx;

	//만약 아직클릭하지않았따면, 결정안한것이므로 그냥 기존꺼 변경
	if (!bRegister)
	{
		m_Points[iDrawIdx] = Preview;
	}

	else
	{
		m_Points[iDrawIdx]=Preview;
		++iDrawIdx;

	}

	
	UpdatePoints();

	
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
		int Full = true;

		deque<PreviewPoint> New= Align_CW();
		//생성..

		//나머지 클릭한순서 수정
		//Set_DrawIdx();
		
		if (iDrawIdx == 3)
			iDrawIdx = 0;

		else
			iDrawIdx = 2;
		PreviewPoint Preview;
		m_Points[iDrawIdx]= Preview;
		
		//m_Points.resize(3);
	}

}

void CNavMeshEdit_Manager::Render()
{
	CheckNull(m_pPreview);
	CheckTrue(Check_EmptyPoints());

	m_pPreview->Render();
	/*for (auto& cell : m_pMapToolCells)
		cell->Render();*/
}

void CNavMeshEdit_Manager::Clear_Points()
{
	iDrawIdx = 0;
	m_Points.clear();
	Init_Points();
	
}

