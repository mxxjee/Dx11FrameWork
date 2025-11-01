#include "CModel.h"
#include "CMeshComponent.h"	
#include "CGameInstance.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CGameInstance.h"


CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pDeviceContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CModel::CModel(const CModel& Prototype)
	: m_pOwner(Prototype.m_pOwner),
	m_ModelData(Prototype.m_ModelData),
	m_Meshs(Prototype.m_Meshs),
	m_pGameInstance(Prototype.m_pGameInstance),
	m_pShader(Prototype.m_pShader),
	m_pDevice(Prototype.m_pDevice),
	m_pDeviceContext(Prototype.m_pDeviceContext)
{
	Safe_AddRef(m_pShader);
	Safe_AddRef(m_pGameInstance);

	for (auto& pair : m_Meshs)
	{
		Safe_AddRef(pair.second);

	}
}
										//"C:\Users\kmj69\Documents\GitHub\ModelConverter\Output\Zelda\Zelda.json"
HRESULT CModel::Initialize_Prototype(_matrix PreTransformMatrix,const _char* pModelFilePath)
{
	m_pShader = m_pGameInstance->Find_Shader(L"VtxMesh");
	Safe_AddRef(m_pShader);

	if (FAILED(LoadMaterialFromJSon(pModelFilePath)))	//Manager에 메쉬가 사용할 모든 Material들을 등록한다.
		return E_FAIL;


	if (FAILED(LoadModelFromJson(PreTransformMatrix,pModelFilePath)))	//파싱하면서 메테리얼이름을 읽어서 Manager에서 찾아서 meshcomp 한테 넘겨준다.
		return E_FAIL;

	fs::path	Tmp = pModelFilePath;

	wstring ModelName = Tmp.stem().wstring();
	CGameInstance::GetInstance()->Register_Model(ModelName, this);

	return S_OK;
}

HRESULT CModel::Initialize_Copytype(void* pArg)
{


	MODEL_DSC* pDesc = static_cast<MODEL_DSC*>(pArg);
	if (pDesc)
	{
		if (m_pShader)
			Safe_Release(m_pShader);

		m_pShader = m_pGameInstance->Find_Shader(pDesc->ShaderName);
		Safe_AddRef(m_pShader);

		if (pDesc->pOwner)
			m_pOwner = pDesc->pOwner;
	}



	return S_OK;
}

									//"C:\Users\kmj69\Documents\GitHub\ModelConverter\Output\Zelda\Zelda.json"
HRESULT CModel::LoadModelFromJson(_matrix PreTransformMatrix,const _char* filepPath)
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


		meshData.m_MaterialData.m_MaterialName = StringToWString(jMesh["MaterialName"]);


		meshData.Transform= PreTransformMatrix;
	
		string vbPath = folderpath + string(jMesh["VBPath"]);
		string ibPath = folderpath + string(jMesh["IBPath"]);
		int MeshIdx = jMesh["MeshIdx"];

		//메쉬 생성, 필요한 정보들은 미리 세팅해서 던져준다.
		CMeshComponent* pMesh = nullptr;
		pMesh = CMeshComponent::Create(m_pDevice, m_pDeviceContext, meshData, folderpath.c_str(), MeshIdx);
		CheckNullResult(pMesh, E_FAIL);


		//pass이름을 따로정의했다면,(메쉬이름과 같이)
		m_ModelData.Meshes.push_back(pMesh->Get_MeshData());

		if (m_pShader->Check_PassName(MeshName))
			pMesh->Set_PassName(MeshName);
		
		//아니라면 DefaultPass이용.(기본값초기화)

			 
		m_Meshs.emplace(wstring(MeshName.begin(), MeshName.end()), pMesh);

	}

	return S_OK;
}

HRESULT CModel::LoadMaterialFromJSon(const _char* filePath)
{
	ModelData model;
	ifstream	file(filePath);

	json jModel = json::parse(file);
	for (auto& Material : jModel["Materials"])
	{
		map<aiTextureType, string>  m_TextureDatas;

		string MaterialName = Material["MaterialName"];
		wstring WMatName = wstring(MaterialName.begin(), MaterialName.end());

		m_TextureDatas.emplace(aiTextureType_DIFFUSE, Material["Diffuse"]);
		m_TextureDatas.emplace(aiTextureType_AMBIENT, Material["Ambient"]);
		m_TextureDatas.emplace(aiTextureType_NORMALS, Material["Normal"]);
		m_TextureDatas.emplace(aiTextureType_SPECULAR, Material["Specular"]);
		m_TextureDatas.emplace(aiTextureType_METALNESS, Material["METALNESS"]);

	
		fs::path Tmp = filePath;

		string BasePath = Tmp.parent_path().string() + "\\Materials\\";

		CMaterial* pMaterial = m_pGameInstance->Find_Material(WMatName);
		if (!pMaterial)
		{
			CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pDeviceContext, BasePath.c_str(), m_TextureDatas);
			if (FAILED(m_pGameInstance->Register_Material(WMatName, pMaterial)))
			{
				Safe_Release(pMaterial);
				continue;
			}
		}
	


		
	}

	return S_OK;
}

HRESULT CModel::Render(CMeshComponent* pMesh)
{
	
	return pMesh->Render();
}

CModel* CModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _matrix PreTransformMatrix,const _char* pFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(PreTransformMatrix,pFilePath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CModel* CModel::Clone(void* pArg)
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
	Safe_Release(m_pShader);
	Safe_Release(m_pGameInstance);
}

HRESULT CModel::Bind_Mateiral(CShader* pShader, const _char* pConstName, CMeshComponent* pMesh, aiTextureType eMaterialType, _uint Textureindex)
{
	CheckNullResult(pShader,E_FAIL);
	CheckNullResult(pMesh, E_FAIL);


	return pMesh->Bind_ShaderResource(pShader,pConstName,eMaterialType,Textureindex);
}


CMeshComponent* CModel::Get_Mesh(const wstring& Name)
{
	auto iter = m_Meshs.find(Name);
	if (iter != m_Meshs.end())
		return iter->second;

	return nullptr;
}
