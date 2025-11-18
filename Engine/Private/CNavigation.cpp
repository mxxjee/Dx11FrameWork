#include "CNavigation.h"
#include "CCell.h"
#include "CGameInstance.h"
#include "CShader.h"



CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)

{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent(Prototype), g_Color(Prototype.g_Color)
{
}

void CNavigation::Set_CurrentIdx(_vector vWorldPos)
{
	int size = (int)m_Cells->size();

	_matrix Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix));
	_vector vCellResultPos = XMVector3TransformCoord(vWorldPos, Inverse);
	_int NeighborIdx;

	for (int i = 0; i < size; ++i)
	{
		if ((*m_Cells)[i]->isIn(vCellResultPos,&NeighborIdx))
		{
			//curIndex를 갱신하기위해
			while (true)
			{
				if (true == (*m_Cells)[NeighborIdx]->isIn(vCellResultPos, &NeighborIdx))
					break;

				if (-1 == NeighborIdx)
					break;
			}
			m_iCurrentCellIndex = NeighborIdx;
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

	m_pShader = m_pGameInstance->Find_Shader(L"VtxPos");
	if (m_pShader) 
		Safe_AddRef(m_pShader);
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

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
	}

	return true;
}

_vector CNavigation::SetUp_OnNavigation(_fvector vWorldPos)
{
	_vector vCellPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

	vCellPos = XMVectorSetY(vCellPos, (*m_Cells)[m_iCurrentCellIndex]->Compute_Height(vCellPos));

	return XMVector3TransformCoord(vCellPos, XMLoadFloat4x4(m_pParentMatrix));
}

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
	Safe_Release(m_pShader);

}
