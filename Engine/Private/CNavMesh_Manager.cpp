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

	vector<CCell*>	m_pCells;

	_ulong dwByte = {};
	HANDLE hFile = CreateFile(StringToWString(Filepath).c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DefaultCellInfo Info = {};
	while (true)
	{
		bool bRead=ReadFile(hFile, &Info, sizeof(DefaultCellInfo), &dwByte, nullptr);
		if (0 == dwByte)
			break;


		CCell* pInstance = CCell::Create(m_pDevice, m_pDeviceContext, Info);
		if (!pInstance)
			return E_FAIL;

		m_pCells.push_back(pInstance);


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

void CNavMesh_Manager::Set_MainCells(_uint LevelID)
{
	vector<CCell*>* Target = Find_Cells(LevelID);
	if (Target == nullptr)
		return;


	else
		m_MainCells = Target;

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

#endif

void CNavMesh_Manager::Set_Shader(CShader* pShader)
{
	m_pShader = pShader;
	if (m_pShader)
		Safe_AddRef(m_pShader);
}
void CNavMesh_Manager::Free()
{
	Safe_Release(m_pShader);
	for(auto& pair: m_LevelCells)
	{

		for (auto& cell : pair.second)
			Safe_Release(cell);
		

		pair.second.clear();
	}
}
