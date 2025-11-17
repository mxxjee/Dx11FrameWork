#include "CNavigation.h"
#include "CCell.h"

const _float4x4* CNavigation::m_pParentMatrix = {};

CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)

{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent(Prototype)
{
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationData)
{
	//파일읽어서 셀생성해주시요
	_ulong dwByte = {};
	HANDLE   hFile=CreateFile(pNavigationData, GENERIC_READ,0,nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	//정점3개씩 읽으세요
	_float3		vPoints[ENUM_TO_UINT(POINTType::END)];
	while (true)
	{
		bool b= ReadFile(hFile, vPoints, sizeof(_float3) * ENUM_TO_UINT(POINTType::END), &dwByte, nullptr);
		if (0 == dwByte)
			break;


		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;


		m_Cells.push_back(pCell);
	}

	//각 삼각형의 네이버를 채운다.
	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;



	return S_OK;
}

HRESULT CNavigation::Initialize_Copytype(void* pArg)
{
	return S_OK;
}

_bool CNavigation::isMove(_fvector vResultPos)
{
	if (-1 == m_iCurrentCellIndex)
		return false;

	//Result를 parentmatrix의 역행렬을 곱해 cell space로맞춰준다.
	_matrix Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix));
	_vector InverseResultPos = XMVector3TransformCoord(vResultPos, Inverse);


	if (false == m_Cells[m_iCurrentCellIndex]->isIn(InverseResultPos))
		return false;

	return true;
}

HRESULT CNavigation::SetUp_Neighbors()
{
	//인접한 삼각형의 리스트를 채워준다.
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(POINTType::A), pSourCell->Get_Point(POINTType::B)))
				pSourCell->Set_Neighbor(LINE::AB, pDestCell);

			if (true == pDestCell->Compare(pSourCell->Get_Point(POINTType::B), pSourCell->Get_Point(POINTType::C)))
				pSourCell->Set_Neighbor(LINE::BC, pDestCell);

			if (true == pDestCell->Compare(pSourCell->Get_Point(POINTType::C), pSourCell->Get_Point(POINTType::A)))
				pSourCell->Set_Neighbor(LINE::CA, pDestCell);
				
		}
	}
	return S_OK;
}

CNavigation* CNavigation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pNavigationData)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationData)))
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

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

}
