#include "CMeshComponent.h"
#include "CVIBuffer_Model.h"
#include "CMaterial.h"
#include "CGameInstance.h"
#include "CShader.h"
#include "CBone.h"




CMeshComponent::CMeshComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext),m_pGameInstance(CGameInstance::GetInstance()),m_pVIBuffer{nullptr}
{
}

CMeshComponent::CMeshComponent(const CMeshComponent& Prototype)
    :CComponent(Prototype),
	m_MeshData(Prototype.m_MeshData),
	m_pVIBuffer(Prototype.m_pVIBuffer),
	passName(Prototype.passName),
	m_pGameInstance(Prototype.m_pGameInstance),
	m_pIndices(Prototype.m_pIndices),
	m_pPositions(Prototype.m_pPositions)
{
	Safe_AddRef(m_pVIBuffer);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMeshComponent::Initialize_Prototype(const MeshData& Data, const char* BasePath, _uint iIdx,MODEL eModelType)
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_MeshData = Data;

	string VBPath = string(BasePath) + "Mesh_" + to_string(iIdx) + ".vb";
	string IBPath = string(BasePath) + "Mesh_" + to_string(iIdx) + ".ib";




	if (FAILED(Set_Material()))
		return E_FAIL;

	_uint iSize = eModelType == MODEL::ANIM ? sizeof(VTXANIMMESH) : sizeof(VTXMESH);
	m_pVIBuffer = CVIBuffer_Model::Create(m_pDevice, m_pContext, m_MeshData.Transform,VBPath,IBPath,eModelType);
	CheckNullResult(m_pVIBuffer, E_FAIL);


	m_pIndices = m_pVIBuffer->Get_Indices();
	m_pPositions = m_pVIBuffer->Get_VertexPositions();

    return S_OK;
}

HRESULT CMeshComponent::Initialize_Copytype(void* pArg)
{
    return S_OK;
}



HRESULT CMeshComponent::Set_Material()
{
	m_pMaterial = m_pGameInstance->Find_Material(m_MeshData.m_MaterialData.m_MaterialName);
	if (!m_pMaterial)
		return E_FAIL;

	Safe_AddRef(m_pMaterial);
	return S_OK;
}


CMeshComponent* CMeshComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const MeshData& Data, const char* BasePath, _uint iIdx,MODEL eModelType)
{
	CMeshComponent* pInstance = new CMeshComponent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Data,BasePath,iIdx, eModelType)))
	{
		MSG_BOX("Failed to Created : CMeshComponent");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMeshComponent::Clone(void* pArg)
{
	CMeshComponent* pInstance = new CMeshComponent(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeshComponent");
		Safe_Release(pInstance);
	}

	return pInstance;
}


HRESULT CMeshComponent::Render()
{
	if (m_pVIBuffer)
	{
		m_pVIBuffer->Bind_Resource();
		m_pVIBuffer->Render();
	}
	else
		return E_FAIL;

	return S_OK;
}

bool CMeshComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
	for (_uint i = 0; i < m_pIndices.size(); i += 3)
	{


		_vector p0 = XMLoadFloat3(&m_pPositions[m_pIndices[i]]);
		_vector p1 = XMLoadFloat3(&m_pPositions[m_pIndices[i + 1]]);
		_vector p2 = XMLoadFloat3(&m_pPositions[m_pIndices[i + 2]]);

		float dist = 0.f;
		if (TriangleTests::Intersects(origin, rayDir, p0, p1, p2, dist))
		{
			Dist = dist;
			if (isnan(dist))
				return false;

			return true;

		}



	}

	return false;



}

HRESULT CMeshComponent::Bind_ShaderResource(CShader* pShader, const _char* pConstName, aiTextureType eMaterialType, _uint Textureindex)
{
	CheckNullResult(m_pMaterial,E_FAIL);

	return m_pMaterial->Bind_ShaderResource(pShader,pConstName,eMaterialType,Textureindex);
}

HRESULT CMeshComponent::Bind_Bones(CShader* pShader, const _char* pConstatName, const vector<CBone*>& Bones)
{
	for (size_t i = 0; i < m_MeshData.m_iNumBones; ++i)
	{
		
		/*연산한 뼈의 combined행렬을 벡터에 담는다*/
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_MeshData.m_OffsetMatrices[i])
			* Bones[m_MeshData.m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
    
		
	}

	/*한번에 떤진다.*/
	return pShader->Bind_Matrices(pConstatName, m_BoneMatrices, g_iMaxNumBones);
}

void CMeshComponent::Free()
{
	__super::Free();

	Safe_Release(m_pMaterial);
	Safe_Release(m_pVIBuffer);
}