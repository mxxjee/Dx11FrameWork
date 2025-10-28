#include "CMeshComponent.h"
#include "CVIBuffer_Model.h"


CMeshComponent::CMeshComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{
}

CMeshComponent::CMeshComponent(const CMeshComponent& Prototype)
    :CComponent(Prototype),
	m_MeshData(Prototype.m_MeshData),
	m_pVIBuffer(Prototype.m_pVIBuffer)
{
	Safe_AddRef(m_pVIBuffer);
}

HRESULT CMeshComponent::Initialize_Prototype(const MeshData& Data, const char* BasePath, _uint iIdx)
{
	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_MeshData = Data;

	string VBPath = string(BasePath) + "Mesh_" + to_string(iIdx) + ".vb";
	string IBPath = string(BasePath) + "Mesh_" + to_string(iIdx) + ".ib";

	if (!LoadBinaryVB(VBPath))
		return E_FAIL;

	if (!LoadBinaryIB(IBPath))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Model::Create(m_pDevice, m_pContext, m_MeshData.Transform,m_MeshData.Vertices, m_MeshData.Indices);
	CheckNullResult(m_pVIBuffer, E_FAIL);

    return S_OK;
}

HRESULT CMeshComponent::Initialize_Copytype(void* pArg)
{
    return S_OK;
}

bool CMeshComponent::LoadBinaryVB(const string& Path)
{
	ifstream file(Path, std::ios::binary);
	if (!file.is_open())
	{
		MSG_BOX("Failed to Open VBFile");
		return false;
	}

	//파일크기 구하기.
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	//정점개수 계산
	size_t vertexCount = size / sizeof(VTXMESH);
	m_MeshData.VertexCount = (_uint)vertexCount;
	m_MeshData.Vertices.resize(vertexCount);

	//읽기
	if (!file.read(reinterpret_cast<char*>(m_MeshData.Vertices.data()), size))
	{
		MSG_BOX("Failed to Read VBFile");
		return false;
	}
	return true;
}

bool CMeshComponent::LoadBinaryIB(const string& Path)
{
	ifstream file(Path, std::ios::binary);
	if (!file.is_open())
	{
		MSG_BOX("Failed to Open IBFile");
		return false;
	}

	//파일크기 구하기.
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	//정점개수 계산
	size_t IndexCount = size / sizeof(uint32_t);
	m_MeshData.IndexCount = (_uint)IndexCount;
	m_MeshData.Indices.resize(IndexCount);

	//읽기
	if (!file.read(reinterpret_cast<char*>(m_MeshData.Indices.data()), size))
	{
		MSG_BOX("Failed to Read IBFile");
		return false;
	}
	return true;
}

CMeshComponent* CMeshComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const MeshData& Data, const char* BasePath, _uint iIdx)
{
	CMeshComponent* pInstance = new CMeshComponent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Data,BasePath,iIdx)))
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

void CMeshComponent::Free()
{
    __super::Free();
	Safe_Release(m_pVIBuffer);
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
