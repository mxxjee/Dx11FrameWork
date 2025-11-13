#include "CModel.h"
#include "CMeshComponent.h"	
#include "CGameInstance.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CGameInstance.h"
#include "CBone.h"
#include "CAnimation.h"
#include "CChannel.h"




CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pDeviceContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CModel::CModel(const CModel& Prototype)
	: m_pOwner(Prototype.m_pOwner),
	m_ModelData(Prototype.m_ModelData),
	m_Meshs(Prototype.m_Meshs),
	//m_Bones(Prototype.m_Bones),
	m_eModelType(Prototype.m_eModelType),
	m_PreTransformMatrix(Prototype.m_PreTransformMatrix),
	m_PreAnimKey(Prototype.m_PreAnimKey),
	//m_Animations{Prototype.m_Animations},
	m_iNumAnimations{Prototype.m_iNumAnimations},
	m_pGameInstance(Prototype.m_pGameInstance),
	m_isTransition(Prototype.m_isTransition),
	m_fTranslationTime(Prototype.m_fTranslationTime),
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

	/*애니메이션을 다 따로 돌리기위해(Bone의 Transform이 다르게 하기위해 깊은복사)*/
	for (auto& PrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(PrototypeBone->Clone());
	

	/*각자 애니메이션을 일정하게 적용하기위해 깊은복사*/
	for (auto& Anim : Prototype.m_Animations)
	{
		m_Animations.emplace(Anim.first, Anim.second->Clone());
		//m_Animations.push_back();
	}
		

}
										//"C:\Users\kmj69\Documents\GitHub\ModelConverter\Output\Zelda\Zelda.json"
HRESULT CModel::Initialize_Prototype(_matrix PreTransformMatrix,const _char* pModelFilePath)
{

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	ifstream	file(pModelFilePath);
	json jModel = json::parse(file);
	bool bHasBone = jModel.contains("Bones");
	bool bHasAnim = jModel.contains("NumAnimations");


	m_eModelType=bHasBone ? MODEL::ANIM : MODEL::NONANIM;
	if (m_eModelType == MODEL::NONANIM)
		m_pShader = m_pGameInstance->Find_Shader(L"VtxMesh");

	else
		m_pShader = m_pGameInstance->Find_Shader(L"VtxAnimMesh");
	Safe_AddRef(m_pShader);
	

	if (bHasBone)
		if (FAILED(Load_BonesFromJson(jModel)))
			return E_FAIL;


	if (FAILED(LoadMaterialFromJSon(pModelFilePath, jModel)))	//Manager에 메쉬가 사용할 모든 Material들을 등록한다.
		return E_FAIL;



	///Bone여부에 따라서 내부적으로 애니메이션메쉬로 로드할건지, 아닌지 판단

	if (FAILED(LoadModelFromJson(bHasAnim,PreTransformMatrix,pModelFilePath, jModel)))	//파싱하면서 메테리얼이름을 읽어서 Manager에서 찾아서 meshcomp 한테 넘겨준다.
		return E_FAIL;


	if (m_iNumAnimations > 0)
	{
		if (FAILED(Load_Animation(pModelFilePath)))
			return E_FAIL;

	}

	fs::path	Tmp = pModelFilePath;

	wstring ModelName = Tmp.stem().wstring();
	CGameInstance::GetInstance()->Register_Model(ModelName, this);

	return S_OK;
}

HRESULT CModel::Initialize_Copytype(void* pArg)
{

	return S_OK;
}


									//"C:\Users\kmj69\Documents\GitHub\ModelConverter\Output\Zelda\Zelda.json"
HRESULT CModel::LoadModelFromJson(bool _bHasAnim, _matrix PreTransformMatrix,const _char* filepPath, json& jModel)
{
	ModelData model;
	fs::path fullpath = filepPath;
								//마지막파일명뺴고 추출
	string folderpath = fullpath.parent_path().string()+"\\";
	string ModelName = jModel["ModelName"];

	if (_bHasAnim)
		m_iNumAnimations = jModel["NumAnimations"].get<int>();

	else
		m_iNumAnimations = 0;

	vector<CMeshComponent*>	m_MeshList;

	model.name = wstring(ModelName.begin(), ModelName.end());
	for(auto& jMesh: jModel["Meshes"])
	{
		//MeshData정보 채워서 넘겨주기.
		MeshData meshData;
		
		string MeshName = jMesh["Name"];		//매쉬이름
		meshData.Name = wstring(MeshName.begin(), MeshName.end());
		meshData.m_MaterialData.m_MaterialName = StringToWString(jMesh["MaterialName"]);
		meshData.Transform= PreTransformMatrix;

		/*애니메이션 메쉬일 경우 본 정보 채ㅑ워서 넘기기*/
		if (m_eModelType == MODEL::ANIM)
		{
			meshData.m_iNumBones = jMesh["NumBones"] ;
			meshData.m_BoneIndices.resize(meshData.m_iNumBones);
			meshData.m_OffsetMatrices.resize(meshData.m_iNumBones);

			//기록한 거 하나씩 채워주기
			json BoneIdx = jMesh["BoneIdx"];

			if (BoneIdx.is_array())
			{
				for (int i = 0; i < meshData.m_iNumBones; ++i)
				{
					meshData.m_BoneIndices[i]= static_cast<int>(BoneIdx[i].get<int>());
				}
			}

			for (size_t i = 0; i < meshData.m_iNumBones; ++i)
			{
				int TargetIdx = meshData.m_BoneIndices[i];
				meshData.m_OffsetMatrices[i] = m_Bones[TargetIdx]->Get_OffSetMatrix();
			}
		}


		string vbPath = folderpath + string(jMesh["VBPath"]);
		string ibPath = folderpath + string(jMesh["IBPath"]);
		int MeshIdx = jMesh["MeshIdx"];

		

		//메쉬 생성, 필요한 정보들은 미리 세팅해서 던져준다.
		CMeshComponent* pMesh = nullptr;
		pMesh = CMeshComponent::Create(m_pDevice, m_pDeviceContext, meshData, folderpath.c_str(), MeshIdx, m_eModelType);
		CheckNullResult(pMesh, E_FAIL);
		m_MeshList.push_back(pMesh);

		//pass이름을 따로정의했다면,(메쉬이름과 같이)
		m_ModelData.Meshes.push_back(pMesh->Get_MeshData());

		string strPath = string(filepPath);
		m_ModelData.ResourcePath = wstring(strPath.begin(),strPath.end());

		bool hasPass = m_pShader->Check_PassName(MeshName);
		if (hasPass)
			pMesh->Set_PassName(MeshName);
		
		//아니라면 DefaultPass이용.(기본값초기화)

	
	}

	for (auto& pMesh : m_MeshList)
	{
		m_Meshs.emplace(pMesh->Get_MeshData().Name, pMesh);
	}
	return S_OK;
}



HRESULT CModel::LoadMaterialFromJSon(const _char* filePath, json& jModel)
{

	string name = jModel["ModelName"];
	m_ModelData.name = wstring(name.begin(), name.end());

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

	
		

		string BasePath = "";
		fs::path Tmp = filePath;
		string Name = Tmp.parent_path().string();

		if (m_ModelData.name.find(L"Field")!=wstring::npos)
			BasePath = "..\\..\\Resource\\FieldTexture\\";

		else
			BasePath = Tmp.parent_path().string() + "\\Materials\\";

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

HRESULT CModel::Load_BonesFromJson(json& jModel)
{
	for (auto& Bone : jModel["Bones"])
	{
		BoneData boneData;

		string BoneName= Bone["BoneName"];
		boneData.BoneName = wstring(BoneName.begin(), BoneName.end());
		boneData.BoneIndex = Bone["ParentIndex"];

		json Matrix= Bone["Transformation"];
		auto flat = Matrix.get<vector<float>>();
		memcpy(&boneData.TransformationMatrix, flat.data(), sizeof(float) * 16);
		


		json OffsetMatrix = Bone["OffsetMatrix"];
		auto Tmp= OffsetMatrix.get<vector<float>>();
		memcpy(&boneData.m_OffsetMatrix, Tmp.data(), sizeof(float) * 16);
	

		CBone* pBone = CBone::Create(boneData);
		m_Bones.emplace_back(pBone);
		m_ModelData.Bones.push_back(boneData);

	}
	return S_OK;
}

HRESULT CModel::Load_Animation(const _char* filePath)
{


	fs::path path = filePath;
	string BasePath = path.parent_path().string();

	
	string AnimPath = BasePath + "\\" + WStringToUTF8(m_ModelData.name) + "_AnimData.json";

	ifstream	file(AnimPath);
	json jFile = json::parse(file);


	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(this, jFile,AnimPath.c_str(),i);
		if (nullptr == pAnimation)
			return E_FAIL;

		const char* AnimKey = pAnimation->Get_Name();
		string strKey = string(AnimKey);

		m_Animations.emplace(wstring(strKey.begin(),strKey.end()),pAnimation);
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

	for (auto& Bone : m_Bones)
	{
		Safe_Release(Bone);
	}

	for (auto& pair : m_Animations)
	{
		if (pair.second)
			Safe_Release(pair.second);
	}
	m_Animations.clear();


	Safe_Release(m_pShader);
	Safe_Release(m_pGameInstance);
}

HRESULT CModel::Bind_Mateiral(CShader* pShader, const _char* pConstName, CMeshComponent* pMesh, aiTextureType eMaterialType, _uint Textureindex)
{
	CheckNullResult(pShader,E_FAIL);
	CheckNullResult(pMesh, E_FAIL);


	return pMesh->Bind_ShaderResource(pShader,pConstName,eMaterialType,Textureindex);
}

HRESULT CModel::Bind_Bones(CShader* pShader, const char* pConstName, CMeshComponent* pMesh)
{
	CheckNullResult(pMesh, E_FAIL);

	return pMesh->Bind_Bones(pShader, pConstName, m_Bones);
}

void CModel::Play_Animation(_float fTimeDelta)
{


	//모든 메쉬들을 순회하며 bone정보를 업데이트한다.
	//m_iCurrentAnimationIndex에 해당하는 애니메이션 중, 현재 재생시간에 맞는 상태행렬을 실제 뼈에게 전달*/
	//갱신해준 뼈들의 TransformMatrix를 기반으로 하여 실제 뼈의 상태(CombinedMatrix)행렬을 만든다.

	

	//전이 수행..
	if (m_isTransition)
	{
		//전이시간, 선형보간 시간값
		Update_BlendAnim(fTimeDelta);
	}

	//아니라면 일반선형보간 수행
	else
	{
		CAnimation* CurAnim = Find_Animation(m_CurrentAnimKey);
		if (CurAnim)
			m_isAnimFinished=CurAnim->Update_TransformationMatrices(m_Bones, fTimeDelta);
	}
		
	for (auto& Bone : m_Bones)
	{
		Bone->Update_CombinedTransformMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}


	
}

void CModel::Update_BlendAnim(_float fTimeDelta)
{
	m_fTranslationTime += fTimeDelta;
	
	if (m_fTranslationTime < m_fTranslation)
	{
		CAnimation* pPreAnim = Find_Animation(m_PreAnimKey);
		CAnimation* pCurAnim = Find_Animation(m_CurrentAnimKey);

		//모든 본에 대해서 수행한다.
		//만약 전이하는애니메이션과 현재애니메이션에 공통적으로 존재하는 본은 서로 선형보간한다.
		//현재애니메이션 O, 전이하는 애니메이션 X -> 이전키프레임->다음키프레임
		for (int i = 0; i < m_Bones.size(); ++i)
		{
			CChannel* pPreExist = pPreAnim->Get_Channel_BoneIdx(i);
			CChannel* pbCurExist = pCurAnim->Get_Channel_BoneIdx(i);

			if (pPreExist && pbCurExist)
			{
				_matrix CurSRT = m_Bones[i]->Get_TransformMatrix_By_XMMatrix();
				pPreExist->UpdateTransformMatrix_To_Target(m_Bones, CurSRT,m_fTranslationTime);
				
			}
				

			else if (pPreExist && !pbCurExist)
			{
				CChannel* pTarget = pPreExist ? pPreExist : pbCurExist;
				if (pTarget == pPreExist)//이전애니메이션에만 존재할경우
					continue;

				else //현재 애니메이션에만 존재할경우
				{
					_matrix mat = pPreAnim->Get_CurrentKeyFrameBonSRT(i);
					pbCurExist->UpdateTransformMatrix_Blned_By_PrevToCurr(m_Bones, mat,m_fTranslationTime);

				}
			}

			else
				continue;

		}
	}

	else
	{
		m_isTransition = false;
		End_Transition();
	}


}

bool CModel::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
	bool bHit = false;
	float minDist = FLT_MAX;

	for (auto& pair : m_Meshs)
	{
		float TmpDist = 0.f;

		if (pair.second)
		{
			if (pair.second->Intersects_Ray(origin, rayDir, TmpDist))
			{
				if (TmpDist < minDist)
				{
					minDist = TmpDist;
					bHit = true;
				}
			}
		}
	}

	Dist = minDist;
	return bHit;

}

void CModel::Set_Animation(const wstring& AnimKey, _bool isLoop)
{
	CAnimation* pAnim = Find_Animation(AnimKey);
	CheckNull(pAnim);

	m_PreAnimKey = m_CurrentAnimKey;
	m_CurrentAnimKey = AnimKey;

	if (m_PreAnimKey!=L"" && m_PreAnimKey != m_CurrentAnimKey)
	{
		m_isTransition = true;

		////이전 애니메이션 초기화(다음에 실행시 처음부터 시작)
		CAnimation* pPreAnim = Find_Animation(m_PreAnimKey);
		if (pPreAnim)
			pPreAnim->Reset_Animtion();
	}

	pAnim->Set_Loop(isLoop);
}

void CModel::Set_Animation_Speed(const wstring& AnimKey, _float TickPerSecond)
{
	CAnimation* pAnim = Find_Animation(AnimKey);
	CheckNull(pAnim);

	return pAnim->Set_TickPerSecond(TickPerSecond);

}

int CModel::Get_BoneIndex(const char* pBoneName)
{

	_int       iIndex = { 0 };

	auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (pBone->Compare_Name(pBoneName))
				return true;

			++iIndex;

			return false;
		});

	if (iter == m_Bones.end())
		return -1;

	return iIndex;
}

void CModel::Set_Loop(const wstring& AnimKey, bool bLoop)
{
	CAnimation* pAnim = Find_Animation(AnimKey);

	if (pAnim)
		pAnim->Set_Loop(bLoop);


}


CMeshComponent* CModel::Get_Mesh(const wstring& Name)
{
	auto iter = m_Meshs.find(Name);
	if (iter != m_Meshs.end())
		return iter->second;

	return nullptr;
}

CAnimation* CModel::Find_Animation(const _wstring& Key)
{
	auto iter = m_Animations.find(Key);
	if (iter == m_Animations.end())
		return nullptr;


	return iter->second;
}

void CModel::Set_VisibleMesh(const _wstring& MeshName, bool bVisible)
{
	auto iter = m_Meshs.find(MeshName);
	if (iter != m_Meshs.end())
		iter->second->Set_Visible(bVisible);

	else
		return;

}



void CModel::End_Transition()
{
	
	m_fTranslationTime = 0.f;

	//CAnimation* pPreAnim = Find_Animation(m_PreAnimKey);
	//if(pPreAnim)
	//	pPreAnim->Set_CurrentTrackPosition(0.f);//다음에 실행됐을떄 처음부터 실행하라고..

}
