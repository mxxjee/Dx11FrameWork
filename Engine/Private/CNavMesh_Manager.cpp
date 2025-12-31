#include "CNavMesh_Manager.h"
#include "CCell.h"
#include "CGameInstance.h"
#include "CCamera_Base.h"
#include "CShader.h"


CNavMesh_Manager::CNavMesh_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice{_pDevice},m_pDeviceContext{_pContext}, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CNavMesh_Manager::Initialize()
{
	XMStoreFloat4x4(&m_IdentityMatrix, XMMatrixIdentity());
	m_pParentMatrix = &m_IdentityMatrix;

	return S_OK;
}


HRESULT CNavMesh_Manager::Load_NavMesh(_uint iLevelIdx, const string& Filepath)
{
	vector<CCell*>* FindCells = Find_Cells(iLevelIdx);
	CheckTrueResult(FindCells != nullptr, E_FAIL);


	_ulong dwByte = {};
	HANDLE hFile = CreateFile(StringToWString(Filepath).c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DefaultCellInfo Info = {};
	DWORD fileSize = GetFileSize(hFile, nullptr);
	int cellCount = fileSize / 108;


	vector<CCell*>	m_pCells;
	m_pCells.resize(cellCount);

	for (int i = 0; i < cellCount; ++i)
	{
		DefaultCellInfo ReadInfo;
		bool bRead;

		bRead = ReadFile(hFile, ReadInfo.m_vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		bRead = ReadFile(hFile, ReadInfo.m_vNormals, sizeof(_float3) * 3, &dwByte, nullptr);
		bRead = ReadFile(hFile, ReadInfo.m_iNeighbors, sizeof(int) * 3, &dwByte, nullptr);
		bRead = ReadFile(hFile, &ReadInfo.m_iIndex, sizeof(UINT32), &dwByte, nullptr);
		bRead = ReadFile(hFile, &ReadInfo.m_Plane, sizeof(_float4), &dwByte, nullptr);
		bRead = ReadFile(hFile, &ReadInfo.CellType, sizeof(UINT32), &dwByte, nullptr);

		if (0 == dwByte)
			break;


		CCell* pInstance = CCell::Create(m_pDevice, m_pDeviceContext, ReadInfo);
		if (!pInstance)
			return E_FAIL;

		m_pCells[i]=pInstance;

	}

	m_LevelCells.emplace(iLevelIdx, m_pCells);
	CloseHandle(hFile);


	return S_OK;
}

vector<CCell*>* CNavMesh_Manager::Find_Cells(_uint iLevelIdx)
{
	auto iter = m_LevelCells.find(iLevelIdx);
	if (iter != m_LevelCells.end())
		return &(iter->second);

	return nullptr;
}

void CNavMesh_Manager::Reset_NaveMesh(_uint iLevelIdx)
{
	vector<CCell*>* FindCells = Find_Cells(iLevelIdx);
	CheckNull(FindCells);

	for (auto& pCell : *FindCells)
		Safe_Release(pCell);

	FindCells->clear();
	auto iter = m_LevelCells.find(iLevelIdx);
	m_LevelCells.erase(iter);



}

void CNavMesh_Manager::Set_MainCells(_uint LevelID)
{
	vector<CCell*>* Target = Find_Cells(LevelID);
	if (Target == nullptr)
		return;


	else
		m_MainCells = Target;

}

_vector CNavMesh_Manager::Get_CellPos_By_MainCells(_uint iIdx)
{
	CheckTrueResult(m_MainCells->empty(), _vector());
	CheckTrueResult(iIdx>m_MainCells->size(), _vector());

	return (*m_MainCells)[iIdx]->Get_CenterPos();
}

CNavMesh_Manager* CNavMesh_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CNavMesh_Manager* pInstance = new CNavMesh_Manager(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CNavMesh_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

#ifdef _DEBUG
HRESULT CNavMesh_Manager::Render()
{
	CheckFalseResult(CGameInstance::m_bDrawDebug,E_FAIL);
	CheckNullResult(m_MainCells, E_FAIL);
	CheckFalseResult(m_bDrawDebug,E_FAIL);
	CheckNullResult(m_pShader, E_FAIL);
	CheckTrueResult(m_MainCells->empty(),E_FAIL);

	if (m_pShader->Bind_Vector("g_Color", _float4(0.f, 1.f, 0.f, 1.f)))
		return E_FAIL;

	if (m_pParentMatrix == nullptr)
		m_pParentMatrix = &m_IdentityMatrix;

	if (m_pShader->Bind_Matrix("g_WorldMatrix",*m_pParentMatrix))
		return E_FAIL;

	m_pShader->Begin("Default");

	for (auto& pCell : *m_MainCells)
		pCell->Render();


	return S_OK;
}



void CNavMesh_Manager::Set_Shader(CShader* pShader)
{
	CheckTrue(m_pShader != nullptr);

	m_pShader = pShader;
	if (m_pShader)
		Safe_AddRef(m_pShader);
}
#endif
void CNavMesh_Manager::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pShader);


#endif // _DEBUG

	for(auto& pair: m_LevelCells)
	{

		for (auto& cell : pair.second)
			Safe_Release(cell);
		

		pair.second.clear();
	}
}
