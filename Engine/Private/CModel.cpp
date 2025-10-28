#include "CModel.h"
#include "CMeshComponent.h"	

CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent(Prototype),
	m_ModelData(Prototype.m_ModelData),
	m_Meshs(Prototype.m_Meshs)
{
	for (auto& pair : m_Meshs)
	{
		Safe_AddRef(pair.second);

	}
}
										//"C:\Users\kmj69\Documents\GitHub\ModelConverter\Output\Zelda\Zelda.json"
HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	if (FAILED(LoadModelFromJson(pModelFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	return S_OK;
}

									//"C:\Users\kmj69\Documents\GitHub\ModelConverter\Output\Zelda\Zelda.json"
HRESULT CModel::LoadModelFromJson(const _char* filepPath)
{
	ModelData model;
	ifstream	file(filepPath);
	
	json jModel = json::parse(file);

	fs::path fullpath = filepPath;
								//마지막파일명뺴고 추출
	string folderpath = fullpath.parent_path().string()+"\\";
	string ModelName = jModel["ModelName"];


	model.name = wstring(ModelName.begin(), ModelName.end());
	for(auto& jMesh: jModel["Meshes"])
	{
		//MeshData정보 채워서 넘겨주기.
		MeshData meshData;

		string MeshName = jMesh["Name"];		//매쉬이름
		meshData.Name = wstring(MeshName.begin(), MeshName.end());

		json Tmp = jMesh["Transform"];
		for (int r = 0; r < 4; ++r)
		{
			if (Tmp[r].is_array() && Tmp[r].size() == 4)
			{
				for (int c = 0; c < 4; ++c)
				{
					meshData.Transform.m[r][c] = Tmp[c][r].get<float>();
				}
					
			}
		
		}
		
		
		string vbPath = folderpath + string(jMesh["VBPath"]);
		string ibPath = folderpath + string(jMesh["IBPath"]);
		int MeshIdx = jMesh["MeshIdx"];

		CMeshComponent* pMesh = nullptr;
		pMesh = CMeshComponent::Create(m_pDevice, m_pContext, meshData,folderpath.c_str(), MeshIdx);
		CheckNullResult(pMesh, E_FAIL);



		m_Meshs.emplace(wstring(MeshName.begin(), MeshName.end()), pMesh);

	}

	return S_OK;
}

CModel* CModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();
	for (auto& pair : m_Meshs)
	{
		if (pair.second)
			Safe_Release(pair.second);

	}
}

HRESULT CModel::Render()
{
	for (auto& pair : m_Meshs)
	{
		if (pair.second)
			pair.second->Render();
	}

	return S_OK;
}
