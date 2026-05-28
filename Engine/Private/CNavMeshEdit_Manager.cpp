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

	m_pMapToolCells.reserve(100);
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

HRESULT CNavMeshEdit_Manager::Save_NavigationData(const string& filePath,_uint iNum)
{
	/*파일 입출력을 통해서 네비게이션 데이터를 저장하는 함수*/
	_ulong dwByte = {};
	string NewFilePath = "";
	if (iNum == 0)
		NewFilePath = filePath;
	else
		NewFilePath = filePath + to_string(iNum);


	HANDLE		hFile = CreateFile(StringToWString(NewFilePath).c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	CheckTrueResult(m_pMapToolCells.empty(), E_FAIL);


	/*Cell들의 정보를 저장한다.*/
	for (auto& cell : m_pMapToolCells)
	{
		DefaultCellInfo info = cell->Get_CellInfo();

		WriteFile(hFile, info.m_vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		WriteFile(hFile, info.m_vNormals, sizeof(_float3) * 3, &dwByte, nullptr);
		WriteFile(hFile, info.m_iNeighbors, sizeof(int) * 3, &dwByte, nullptr);
		WriteFile(hFile, &info.m_iIndex, sizeof(UINT32), &dwByte, nullptr);
		WriteFile(hFile, &info.m_Plane, sizeof(_float4), &dwByte, nullptr);
		WriteFile(hFile, &info.CellType, sizeof(UINT32), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CNavMeshEdit_Manager::Load_NavigationData(const string& filePath)
{
	for (auto& pCell : m_pMapToolCells)
		Safe_Release(pCell);

	m_pMapToolCells.clear();


	_ulong dwByte = {};
	HANDLE hFile = CreateFile(StringToWString(filePath).c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	
	DWORD fileSize = GetFileSize(hFile, nullptr);
	int cellCount = fileSize / 108;


	for (int i = 0; i < cellCount; ++i)
	{
		DefaultCellInfo Info = {};

		CMapToolCell::MAPTOOLCELL_DESC Desc;
		bool bRead;

		bRead = ReadFile(hFile, Info.m_vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		bRead = ReadFile(hFile, Info.m_vNormals, sizeof(_float3) * 3, &dwByte, nullptr);
		bRead = ReadFile(hFile, Info.m_iNeighbors, sizeof(int) * 3, &dwByte, nullptr);
		bRead = ReadFile(hFile, &Info.m_iIndex, sizeof(UINT32), &dwByte, nullptr);
		bRead = ReadFile(hFile, &Info.m_Plane, sizeof(_float4), &dwByte, nullptr);
		bRead = ReadFile(hFile, &Info.CellType, sizeof(UINT32), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		
		Desc.iIdx = Info.m_iIndex;

		CVIBuffer_Triangle::TRIANGLEBUFFER_DESC  TriangleDesc;
		TriangleDesc.v0 = Info.m_vPoints[0];
		TriangleDesc.v1 = Info.m_vPoints[1];
		TriangleDesc.v2 = Info.m_vPoints[2];

		Desc.TriangleCom = &TriangleDesc;

		CMapToolCell* pInstance = CMapToolCell::Create(m_pDevice, m_pContext, &Desc);
		if (!pInstance)
			return E_FAIL;

		pInstance->Set_Index(Desc.iIdx);
		pInstance->Set_Info(Info);

		m_pMapToolCells.push_back(pInstance);


	}

	CloseHandle(hFile);

	return S_OK;
}

void CNavMeshEdit_Manager::Show_Solo_Cell(bool bToggle)
{

	if (bToggle)
	{
		for (auto& pDestCell : m_pMapToolCells)
		{
			DefaultCellInfo info = pDestCell->Get_CellInfo();

			bool bExist = false;

			for (int i = 0; i < 3; ++i)
			{
				bExist |= info.m_iNeighbors[i] == -1 ? false : true;
			}

			if (!bExist)
			{
				pDestCell->Set_SolidMode();
				pDestCell->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
			}



		}
	}

	else
	{
		for (auto& pDestCell : m_pMapToolCells)
		{
			pDestCell->Set_WireFrameMode();
			pDestCell->Set_Color(_float4(0.f, 1.f, 0.f, 1.f));


		}
	}
	
}

HRESULT CNavMeshEdit_Manager::SetUp_Neighbors()
{
	for (auto& pSourCell : m_pMapToolCells)
	{
		pSourCell->Set_Neighbor(LINE::AB, nullptr);
		pSourCell->Set_Neighbor(LINE::BC, nullptr);
		pSourCell->Set_Neighbor(LINE::CA, nullptr);

		for (auto& pDestCell : m_pMapToolCells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_vPoint(POINTType::A), pSourCell->Get_vPoint(POINTType::B)))
				pSourCell->Set_Neighbor(LINE::AB, pDestCell);

			if (true == pDestCell->Compare(pSourCell->Get_vPoint(POINTType::B), pSourCell->Get_vPoint(POINTType::C)))
				pSourCell->Set_Neighbor(LINE::BC, pDestCell);

			if (true == pDestCell->Compare(pSourCell->Get_vPoint(POINTType::C), pSourCell->Get_vPoint(POINTType::A)))
				pSourCell->Set_Neighbor(LINE::CA, pDestCell);

		}
	}
	return S_OK;
}

void CNavMeshEdit_Manager::SetUp_Planes()
{
	for (auto& pCell : m_pMapToolCells)
	{
		pCell->Set_Plane();
	}
}

void CNavMeshEdit_Manager::RequestDestory(CMapToolCell* pObj)
{
	pObj->Set_Active(false);
	m_DestroyQueue.push(pObj);

	

}

void CNavMeshEdit_Manager::ProcessDestroy()
{
	
	bool bNeedToUpdate = false;

	while (!m_DestroyQueue.empty())
	{
		bNeedToUpdate = true;

		CMapToolCell* pObj = m_DestroyQueue.front();

		m_DestroyQueue.pop();
		
		m_pMapToolCells.erase(
			remove_if(m_pMapToolCells.begin(), m_pMapToolCells.end(), 
				[&](auto* p) {return p == pObj; }),
			m_pMapToolCells.end());

		Safe_Release(pObj);
	}


	//인덱스 재정렬
	if (bNeedToUpdate)
	{
		for (int i = 0; i < m_pMapToolCells.size(); ++i)
		{
			m_pMapToolCells[i]->Set_Index(i);

		}

	}




}

HRESULT CNavMeshEdit_Manager::Render_Preview_Imgui_Render()
{
	CheckTrueResult(m_Points.empty(), S_OK);


	//현재 그리기위한 인덱스출력
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
	ImGui::BulletText("Current Draw Index:%d\n", iDrawIdx);
	ImGui::PopStyleColor();


	//점3개출력
	
	for (int i = 0; i < 3; ++i)
	{


		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		string str = "";
		if (m_Points[i].vRegister)
			str = "true";

		else
			str = "false";

		ImGui::BulletText("Position X:%f, Y:%f, Z:%f / Register : %s", m_Points[i].vPos.x,
			m_Points[i].vPos.y,
			m_Points[i].vPos.z,
			str.c_str());
		ImGui::PopStyleColor();
	}
		



	return S_OK;
}



void CNavMeshEdit_Manager::UpdatePoints()
{
	CheckNull(m_pPreview);

	//Align_CW();
	m_pPreview->UpdatePoints(m_Points);
	
}

deque<PreviewPoint> CNavMeshEdit_Manager::Align_CW(const deque<PreviewPoint>& Origin)
{
	//CheckFalseResult(Check_FullPoints(Origin), deque<PreviewPoint>());
	
	deque<PreviewPoint> CwQue= Origin;

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
	if (vCross> 0.f)
	{
		PreviewPoint B = CW[ENUM_TO_UINT(POINTType::B)];
		PreviewPoint C = CW[ENUM_TO_UINT(POINTType::C)];

		CW[ENUM_TO_UINT(POINTType::B)] = C;          
		CW[ENUM_TO_UINT(POINTType::C)] = B;

		

	}

	//새로운 걸로 리턴
	for (int i = 0; i < 3; ++i)
		CwQue.push_back(CW[i]);

	return CwQue;
}



bool CNavMeshEdit_Manager::Check_EmptyPoints(const  deque<PreviewPoint>& Points)
{
	CheckTrueResult(Points.empty(), true);

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

bool CNavMeshEdit_Manager::Check_EmptyPoints(PreviewPoint* Points)
{
	CheckNullResult(Points, false);
	_vector vPos[3];
	for (int i = 0; i < 3; ++i)
	{
		vPos[i] = XMLoadFloat3(&Points[i].vPos);
	}

	bool bResult = XMVector3Equal(vPos[0], XMVectorSet(-999.f, -999.f, -999.f, 1.f));
	bool bResult2 = XMVector3Equal(vPos[1], XMVectorSet(-999.f, -999.f, -999.f, 1.f));
	bool bResult3 = XMVector3Equal(vPos[2], XMVectorSet(-999.f, -999.f, -999.f, 1.f));

	return bResult && bResult2 && bResult3;

	return false;
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
		//클리어모드도 아니고, ctrl+z모드도 아닐떄(인덱스 고정하면안됨)
		if (!m_bClear&&!m_bRestore)
		{
			//이어그릴떄 알아서이어그려지게해줌
			if (!XMVector3Equal(XMLoadFloat3(&v), XMLoadFloat3(&m_Points[2].vPos)))
			{
				if ((!m_bFixCell && !m_bFixEdge) || (!m_bFixCell))
				{
					if (!m_bFixEdge)
						pTargetCell = Find_NeareastCell(v);
				}


				CheckNull(pTargetCell);

				pTargetPreviewPoints = pTargetCell->Get_PreviewPoints();

				CheckTrue(Check_EmptyPoints(pTargetPreviewPoints));

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
			if (!m_bRestore&&!m_bClear)
			{
				//제일가까운점에 알아서 스냅
				CMapToolCell* pNearest = Find_NeareastCell(v);
				if (pNearest)
				{
					DefaultCellInfo info = pNearest->Get_CellInfo();

					_float3 vPos = Find_NeareastPos(v, 2.f, info.m_vPoints[ENUM_TO_UINT(POINTType::A)],
						info.m_vPoints[ENUM_TO_UINT(POINTType::B)], info.m_vPoints[ENUM_TO_UINT(POINTType::C)]);


					if (!XMVector3Equal(XMLoadFloat3(&vPos), XMVectorSet(-999.f, -999.f, -999.f, 1.f)))
						Preview.vPos = vPos;
				}

			}

			//클리어상태나 복구모드일떈 그냥 따른처리없이 그대로붙게ㅏㅎ자.
			m_Points[iDrawIdx] = Preview;
		}
		
	}

	else 
	{
		//제일가까운 꼭짓점가져오기

		if (!m_bRestore)
		{
			CMapToolCell* pCell = Find_NeareastCell(v);
			if (pCell && m_pMapToolCells.size() > 1)
			{
				DefaultCellInfo info = pCell->Get_CellInfo();

				_float3 vPos = Find_NeareastPos(v, 0.8f, info.m_vPoints[ENUM_TO_UINT(POINTType::A)],
					info.m_vPoints[ENUM_TO_UINT(POINTType::B)], info.m_vPoints[ENUM_TO_UINT(POINTType::C)]);

				if (!XMVector3Equal(XMLoadFloat3(&vPos), XMVectorSet(-999.f, -999.f, -999.f, 1.f)))
					Preview.vPos = vPos;

			}
		}

		if (m_bRestore)
			m_bRestore = false;


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

_float3 CNavMeshEdit_Manager::Find_NeareastPos(const _float3 vPos, const _float fRadius,const _float3& A, const _float3& B, const _float3& C)
{
	float minDist = FLT_MAX;                                                                                                                                                                                                                                                                                                                                                                                                                                                              

	_vector Pos, vA, vB, vC;
	Pos = XMLoadFloat3(&vPos);
	vA = XMLoadFloat3(&A);
	vB = XMLoadFloat3(&B);
	vC = XMLoadFloat3(&C);


	_float PA = XMVectorGetX(XMVector3LengthSq(Pos -vA));
	_float PB = XMVectorGetX(XMVector3LengthSq(Pos - vB));
	_float PC = XMVectorGetX(XMVector3LengthSq(Pos - vC));

	_float m = min(PA, PB);
	m = min(m, PC);

	_float TargetDist = 0;

	if (m == PA)
	{
		if (PA < fRadius)
			return A;
		else
			return _float3(-999.f, -999.f, -999.f);
	}
	
	if (m == PB)
	{
		if (PB < fRadius)
			return B;
		else
			return _float3(-999.f, -999.f, -999.f);
	}


	if (m == PC)
	{
		if (PC < fRadius)
			return C;
		else
			return _float3(-999.f, -999.f, -999.f);
	}

	return _float3(-999.f, -999.f, -999.f);
	
}

HRESULT CNavMeshEdit_Manager::Create_MapToolCell(const deque<PreviewPoint>& New, const deque<PreviewPoint>& Origin)
{
	_float3 vPos[3];
	int Idx = 0;

	for (int i = 0; i < 3; ++i)
		vPos[i] = New[i].vPos;


	CMapToolCell::MAPTOOLCELL_DESC Desc;
	Desc.iIdx = (_uint)m_pMapToolCells.size();
	
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

void CNavMeshEdit_Manager::Ctrl_Z()
{
	

	//마지막으로 추가된 점 없애기
	CheckTrue(iDrawIdx == 0);


	auto iter = m_Points.begin() + iDrawIdx;
	(*iter) = PreviewPoint();



	/*셀 제거 시작*/
	if (iDrawIdx == 1)
	{
		m_Points[1] = PreviewPoint();
	}

	else if (iDrawIdx != 1)
		--iDrawIdx;


	
#pragma region 원래있던 삼각형 수정하는거..포기
	////포인트가 다 비워졌을때는 삼각형점에서 꺼내온다.
	//if(m_bRestoreCell)
	//{
	//	CheckTrue(m_pMapToolCells.empty());
	//	//마지막 삼각형의 점을 수정하자..

	//	

	//	auto iter = m_pMapToolCells.end() - 1;


	//	//마지막 삼각형 점 가져오기.
	//	PreviewPoint* prePoints = (*iter)->Get_PreviewPoints();

	//	m_Points[iDrawIdx].vPos = prePoints[iRestoreIdx].vPos;


	//	//만약 마지막점을 꺼냈다면, 삭제해라.
	//	if (iRestoreIdx == 0)
	//	{
	//		m_pMapToolCells.erase(iter);
	//		Safe_Release(*iter);
	//		m_bRestoreCell = false;
	//		iRestoreIdx = 1;
	//	}

	//	//아ㅣ니라면, 점계속다시업데이트해줘라.
	//	else
	//	{
	//		--iRestoreIdx;
	//		++iDrawIdx;
	//		deque<PreviewPoint> NewPoints;
	//		NewPoints.resize(3);
	//		for (int i = 0; i <= iRestoreIdx+1; ++i)
	//			NewPoints[i]=prePoints[i];

	//		Rebuild_Cell((*iter),NewPoints);
	//	}
	//	
	//}

#pragma endregion

	m_bRestore = true;
	UpdatePoints();
	
	

}

void CNavMeshEdit_Manager::Modify_Triangle()
{
	/*auto iter = m_pMapToolCells.end() - 1;


	Rebuild_Cell((*iter), NewPoints);*/
}

void CNavMeshEdit_Manager::Rebuild_Cell(CMapToolCell* pCell, deque<PreviewPoint>& NewPoints)
{
	//시계방향정렬하고, 정점재구성
	deque<PreviewPoint> Allign=Align_CW(NewPoints);
	pCell->UpdatePoints(Allign);


}


void CNavMeshEdit_Manager::Update(_float fTimeDelta)
{
	//3개의 점이 다 채워졌는지 보고, 삼각형만들기.
	int RegisterCnt = (int)count_if(m_Points.begin(), m_Points.end(), [](const PreviewPoint & A)
		{
			return A.vRegister == true;
		});


	if (RegisterCnt == 3)
	{
		m_PrePoints = m_Points;
		deque<PreviewPoint> New= Align_CW(m_Points);
		//정렬한 정점으로 생성..
		if (FAILED(Create_MapToolCell(New, m_PrePoints)))
			return;


		//m_Points = New;
		m_bCheckNextEdge = true;
		if (m_bClear)
			m_bClear = false;
		
		m_Points[2].vRegister = false;

	}

}

void CNavMeshEdit_Manager::Update_Late(_float fTimeDelta)
{
	ProcessDestroy();
}

void CNavMeshEdit_Manager::Render()
{
	CheckNull(m_pPreview);

	if(!Check_EmptyPoints(m_Points))
		m_pPreview->Render();

	for (auto& cell : m_pMapToolCells)
	{
		if(cell->Is_Active())
			cell->Render();
	}
		
}

void CNavMeshEdit_Manager::Clear_Points()
{
	iDrawIdx = 0;
	m_Points.clear();
	Init_Points();
	m_PrePoints.resize(3);
	m_bClear = true;
}


