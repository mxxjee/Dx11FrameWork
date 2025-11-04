#include "CVIBuffer_Model.h"


CVIBuffer_Model::CVIBuffer_Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Model::CVIBuffer_Model(const CVIBuffer_Model& Prototype)
    :CVIBuffer(Prototype)
{
}

HRESULT CVIBuffer_Model::Initialize_Prototype(_matrix& Matrix, const string& VBPath, const string& IBPath, MODEL eModelType)
{

#pragma region VertexBuffer
	if (eModelType == MODEL::ANIM)
	{
		if (FAILED(Initialize_AnimMeshVertexBuffer(Matrix, VBPath)))
			return E_FAIL;

	}
	else
	{
		if (FAILED(Initialize_StaticMeshVertexBuffer(Matrix, VBPath)))
			return E_FAIL;
	}

#pragma endregion

#pragma region IndexBuffer
	ifstream file(IBPath, std::ios::binary);
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
	vector<_uint> Indices;
	size_t IndexCount = size / sizeof(uint32_t);
	int TotalSize = (_uint)IndexCount;
	Indices.resize(TotalSize);

	//읽기
	if (!file.read(reinterpret_cast<char*>(Indices.data()), size))
	{
		MSG_BOX("Failed to Read IBFile");
		return E_FAIL;
	}
	
	m_iIndexStride = 4;
	m_iNumIndices = (_uint)Indices.size();


	//[1.인덱스 버퍼를 만들기 위한 정보세팅]
	D3D11_BUFFER_DESC IndexDesc;

	IndexDesc.ByteWidth = m_iIndexStride * m_iNumIndices;		//할당할 크기
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;			//바인딩 플래그(사용 용도)
	IndexDesc.CPUAccessFlags = 0;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	IndexDesc.MiscFlags = 0;
	IndexDesc.StructureByteStride = m_iIndexStride;


	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem =Indices.data();
	m_pIndices = Indices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexDesc, &IndexData, m_pIB.GetAddressOf())))
		return E_FAIL;

#pragma endregion
	
	return S_OK;
}

HRESULT CVIBuffer_Model::Initialize_Copytype(void* pArg)
{ 
	return S_OK;
}

HRESULT CVIBuffer_Model::Initialize_AnimMeshVertexBuffer(_matrix& Matrix, const string& Path)
{
	/*Read VertexBufferFile*/
	ifstream file(Path, std::ios::binary);
	if (!file.is_open())
	{
		MSG_BOX("Failed to Open VBFile");
		return E_FAIL;
	}

	//파일크기 구하기.
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	//정점개수 계산
	size_t vertexCount = size / sizeof(VTXANIMMESH);


	//읽기
	vector<VTXANIMMESH>   Vertices;
	Vertices.resize(vertexCount);
	if (!file.read(reinterpret_cast<char*>(Vertices.data()), size))
	{
		MSG_BOX("Failed to Read VBFile");
		return E_FAIL;

	}

	m_iVertexStride = sizeof(VTXANIMMESH);
	m_iNumVertices = (_uint)Vertices.size();

	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVertexBuffers = 1;

	//[1. 정점 버퍼를 정의하기 위한 정보]
	D3D11_BUFFER_DESC VertexDesc;

	VertexDesc.ByteWidth = m_iVertexStride * m_iNumVertices;		//할당할 크기
	VertexDesc.Usage = D3D11_USAGE_DEFAULT;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//바인딩 플래그(사용 용도)
	VertexDesc.CPUAccessFlags = 0;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	VertexDesc.MiscFlags = 0;
	VertexDesc.StructureByteStride = m_iVertexStride;

	//[2. 정점 버퍼를 초기화 하기 위한 정보]
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	//위치값 기록을 위한 동적배열(따로 멤버 보관)

	m_pVertexPositions.resize(m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		_vector vPosition = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].vPosition), Matrix);
		XMStoreFloat3(&Vertices[i].vPosition, vPosition);
		m_pVertexPositions[i] = Vertices[i].vPosition;

		//노말도 돌려줘야함.
		_vector vNormal = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].vNormal), Matrix);
		XMStoreFloat3(&Vertices[i].vNormal, vNormal);

		//tangent
		_vector vTangent = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].Tangent), Matrix);
		XMStoreFloat3(&Vertices[i].Tangent, vTangent);

		//bitangent까지 돌려줘야한다.
		_vector vBiTangent = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].BiNormal), Matrix);
		XMStoreFloat3(&Vertices[i].BiNormal, vBiTangent);
	}

	D3D11_SUBRESOURCE_DATA VertexData;
	VertexData.pSysMem = Vertices.data();
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;


	//정점버퍼를 만들고, 초기화 시키는함수
	if (FAILED(m_pDevice->CreateBuffer(&VertexDesc, &VertexData, m_pVB.GetAddressOf())))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Model::Initialize_StaticMeshVertexBuffer(_matrix& Matrix, const string& Path)
{
	/*Read VertexBufferFile*/
	ifstream file(Path, std::ios::binary);
	if (!file.is_open())
	{
		MSG_BOX("Failed to Open VBFile");
		return E_FAIL;
	}

	//파일크기 구하기.
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	//정점개수 계산
	size_t vertexCount = size / sizeof(VTXMESH);


	//읽기
	vector<VTXMESH>   Vertices;
	Vertices.resize(vertexCount);
	if (!file.read(reinterpret_cast<char*>(Vertices.data()), size))
	{
		MSG_BOX("Failed to Read VBFile");
		return E_FAIL;

	}

	m_iVertexStride = sizeof(VTXMESH);
	m_iNumVertices = (_uint)Vertices.size();

	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVertexBuffers = 1;

	//[1. 정점 버퍼를 정의하기 위한 정보]
	D3D11_BUFFER_DESC VertexDesc;

	VertexDesc.ByteWidth = m_iVertexStride * m_iNumVertices;		//할당할 크기
	VertexDesc.Usage = D3D11_USAGE_DEFAULT;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//바인딩 플래그(사용 용도)
	VertexDesc.CPUAccessFlags = 0;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	VertexDesc.MiscFlags = 0;
	VertexDesc.StructureByteStride = m_iVertexStride;

	//[2. 정점 버퍼를 초기화 하기 위한 정보]
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//위치값 기록을 위한 동적배열(따로 멤버 보관)

	m_pVertexPositions.resize(m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		_vector vPosition = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].vPosition), Matrix);
		XMStoreFloat3(&Vertices[i].vPosition, vPosition);
		m_pVertexPositions[i] = Vertices[i].vPosition;

		//노말도 돌려줘야함.
		_vector vNormal = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].vNormal), Matrix);
		XMStoreFloat3(&Vertices[i].vNormal, vNormal);

		//tangent
		_vector vTangent = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].Tangent), Matrix);
		XMStoreFloat3(&Vertices[i].Tangent, vTangent);

		//bitangent까지 돌려줘야한다.
		_vector vBiTangent = XMVector3TransformCoord(XMLoadFloat3(&Vertices[i].BiNormal), Matrix);
		XMStoreFloat3(&Vertices[i].BiNormal, vBiTangent);



	}

	D3D11_SUBRESOURCE_DATA VertexData;
	VertexData.pSysMem = Vertices.data();
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;


	//정점버퍼를 만들고, 초기화 시키는함수
	if (FAILED(m_pDevice->CreateBuffer(&VertexDesc, &VertexData, m_pVB.GetAddressOf())))
		return E_FAIL;


	Safe_Delete_Array(pVertices);

	return S_OK;
}



CVIBuffer_Model* CVIBuffer_Model::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _matrix& Matrix, const string& VBPath, const string& IBPath, MODEL eModelType)
{
	CVIBuffer_Model* pInstance = new CVIBuffer_Model(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(Matrix, VBPath,IBPath,eModelType)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Model");
		Safe_Release(pInstance);
	}


	return pInstance;
}



CComponent* CVIBuffer_Model::Clone(void* pArg)
{

	CVIBuffer_Model* pInstance = new CVIBuffer_Model(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CVIBuffer_Model::Free()
{
	__super::Free();
}
