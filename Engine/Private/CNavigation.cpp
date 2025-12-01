#include "CNavigation.h"
#include "CCell.h"
#include "CGameInstance.h"
#include "CShader.h"



CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)

{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent(Prototype)
{
}

void CNavigation::Set_CurrentIdx(_vector vWorldPos)
{
	int size = (int)m_Cells->size();

	_matrix Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix));
	_vector vCellResultPos = XMVector3TransformCoord(vWorldPos, Inverse);
	_int NeighborIdx;
	
	bool bQuit = false;
	for (int i = 0; i < size; ++i)
	{
		if (false == (*m_Cells)[i]->isIn(vCellResultPos, &NeighborIdx))
		{
			//curIndex를 갱신하기위해
			while (true)
			{
				//못찾으면..while문탈출 후 다른 셀찾기
				if (-1 == NeighborIdx)
					break;

				if (true == (*m_Cells)[NeighborIdx]->isIn(vCellResultPos, &NeighborIdx))
				{
					m_iPreCellIdx = m_iCurrentCellIndex;
					m_iCurrentCellIndex = NeighborIdx;
					return;
				}
				
			}
			
		}

		else
		{
			m_iPreCellIdx = m_iCurrentCellIndex;
			m_iCurrentCellIndex = i;
			return;
		}
	}

}

HRESULT CNavigation::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CNavigation::Initialize_Copytype(void* pArg)
{
	if(FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	//NavMEshManager에게 요청한다.(셀들과 parentmatrix의 포인터 )
	m_Cells=m_pGameInstance->Get_MainCells();
	m_pParentMatrix = m_pGameInstance->Get_ParentMatrix();

#ifdef _DEBUG
	m_pShader = m_pGameInstance->Find_Shader(L"VtxPos");
	if (m_pShader) 
		Safe_AddRef(m_pShader);
#endif

	return S_OK;
}

_bool CNavigation::isMove(_fvector vResultPos)
{
	if (-1 == m_iCurrentCellIndex)
		return false;

	//Result를 parentmatrix의 역행렬을 곱해 cell space로맞춰준다.
	_matrix Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix));
	_vector vCellResultPos = XMVector3TransformCoord(vResultPos, Inverse);

	_int		iNeighborIndex = { -1 };


	if (false == (*m_Cells)[m_iCurrentCellIndex]->isIn(vCellResultPos, &iNeighborIndex))
	{
		//이웃이없다면 , 이 결과좌표로 갱신하지말것(이동불가)
		if(-1==iNeighborIndex)
			return false;

		else
		{
			//curIndex를 갱신하기위해
			while (true)
			{
				if (true == (*m_Cells)[iNeighborIndex]->isIn(vCellResultPos, &iNeighborIndex))
					break;

				if (-1 == iNeighborIndex)
					return false;
			}

			m_iPreCellIdx = m_iCurrentCellIndex;
			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
	}

	return true;
}

_bool CNavigation::CheckGround(_fvector& vResultPos, _float& vOutGroundPosY)
{
	CheckTrueResult(m_iCurrentCellIndex == -1, false);

	///vResultpos를 cell과 같은 공간으로 일치시킴.
	_vector vCellSpace_ResultPos = XMVector3TransformCoord(vResultPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

	//평면위에 존재하고, 삼각형 내부에 있으면 현재 셀위에 있음 판정

	//1.평면과 현재 점 사이의 길이
	CCell* pCurrentCell = (*m_Cells)[m_iCurrentCellIndex];
	DefaultCellInfo cellInfo = pCurrentCell->Get_CellInfo();

	_vector NormalPlane = XMVector3Normalize(XMVectorSet(cellInfo.m_Plane.x, cellInfo.m_Plane.y, cellInfo.m_Plane.z, 1.f));

	//AX+BY+CZ+D
	float dist = (XMVectorGetX(NormalPlane) * XMVectorGetX(vCellSpace_ResultPos) +
		XMVectorGetY(NormalPlane) * XMVectorGetY(vCellSpace_ResultPos) +
		XMVectorGetZ(NormalPlane) * XMVectorGetZ(vCellSpace_ResultPos) +
		cellInfo.m_Plane.w);

	_int neighbor = 0;

	bool bIsIn = pCurrentCell->isIn(vCellSpace_ResultPos, &neighbor);


	if (bIsIn && dist >= -0.1f && dist <= 0.1f)
	{
		vOutGroundPosY = pCurrentCell->Compute_Height(vCellSpace_ResultPos);
		return true;
	}

	else
		return false;
}  

_vector CNavigation::SetUp_OnNavigation(_fvector vWorldPos)
{
	_vector vCellPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

	vCellPos = XMVectorSetY(vCellPos, (*m_Cells)[m_iCurrentCellIndex]->Compute_Height(vCellPos));

	return XMVector3TransformCoord(vCellPos, XMLoadFloat4x4(m_pParentMatrix));
}

_uint CNavigation::Get_CurrentCellType()
{

	return ENUM_TO_UINT((*m_Cells)[m_iCurrentCellIndex]->Get_CurrentCellType());
}

const list<_vector>* CNavigation::Make_Route(_int iGoalIndex)
{
	if (m_iOldGoalIndex == iGoalIndex)
		return nullptr;

	_int iStartIndex = m_iCurrentCellIndex;
	
	
	//시작점 설정-현재 나의 위치
	m_OpenList.push_back(m_iCurrentCellIndex);


	//탐색단계
	while (true)
	{
		//휴리스틱비용에 맞는 정렬
		m_OpenList.sort([&](_int iSourIndex, _int iDestIndex) {

			return (*m_Cells)[iSourIndex]->Compute_Cost(*m_Cells, iGoalIndex) < (*m_Cells)[iDestIndex]->Compute_Cost(*m_Cells, iGoalIndex);
			});


		//가장 앞 셀 꺼내기
		_int Min = m_OpenList.front();
		CCell* pMinCell = (*m_Cells)[Min];


		_int        iNeighbors[3] = {};
		pMinCell->Get_Neighbors(iNeighbors);

		//셀과 인접한 노드들 openList에 추가하기
		for (int i = 0; i < 3; ++i)
		{

			(*m_Cells)[iNeighbors[i]]->Set_ParentIndex(Min);

			if(CanPush(iNeighbors[i]))
				m_OpenList.push_back(iNeighbors[i]);
		}

		m_OpenList.pop_front();
		m_CloseList.push_back(Min);
	}


	m_iOldGoalIndex = iGoalIndex;
	
}

_bool CNavigation::CanPush(_int iCellIndex)
{
	/* 이 셀이 이동가능하지 않은 셀이라면 */
  // return false;

	//중복검사
	auto    iter = find_if(m_OpenList.begin(), m_OpenList.end(), [&](_int iCurrentCellIndex)
		{
			if (iCurrentCellIndex == iCellIndex)
				return true;

			return false;
		});

	if (iter != m_OpenList.end())
		return false;

	iter = find_if(m_CloseList.begin(), m_CloseList.end(), [&](_int iCurrentCellIndex)
		{
			if (iCurrentCellIndex == iCellIndex)
				return true;

			return false;
		});

	if (iter != m_CloseList.end())
		return false;

	return true;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	CheckNullResult(m_pShader, E_FAIL);

	_float4x4       WorldMatrix = *m_pParentMatrix;
	WorldMatrix._42 += 0.1f;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Vector("g_Color", g_Color)))
		return E_FAIL;

	m_pShader->Begin("Default");

	(*m_Cells)[m_iCurrentCellIndex]->Render();

	return E_NOTIMPL;
}
#endif

CNavigation* CNavigation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();
#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
