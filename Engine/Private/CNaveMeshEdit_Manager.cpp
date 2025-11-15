#include "CNaveMeshEdit_Manager.h"
#include "CNavEditPreview.h"
#include "CMapToolCell.h"


IMPLEMENT_SINGLETON(CNaveMeshEdit_Manager)

CNaveMeshEdit_Manager::CNaveMeshEdit_Manager()
{
}

CNaveMeshEdit_Manager::~CNaveMeshEdit_Manager()
{
}


void CNaveMeshEdit_Manager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	m_pPreview = CNavEditPreview::Create(_pDevice, _pContext);


}



void CNaveMeshEdit_Manager::UpdatePoints()
{
	CheckNull(m_pPreview);

	m_pPreview->UpdatePoints(m_Points[ENUM_TO_UINT(POINTType::A)], 
		m_Points[ENUM_TO_UINT(POINTType::B)], 
		m_Points[ENUM_TO_UINT(POINTType::C)]);
	
}

void CNaveMeshEdit_Manager::Set_DrawPoint(_float3 v, int idx)
{
	m_Points[idx] = v;

}

void CNaveMeshEdit_Manager::Update(_float fTimeDelta)
{
	//3개의 점이 다 채워졌는지 보고, 삼각형만들기.
	if (m_Points.size() == 3)
	{
		int Full = true;
	}

}

void CNaveMeshEdit_Manager::Render()
{
	m_pPreview->Render();
	/*for (auto& cell : m_pMapToolCells)
		cell->Render();*/
}

