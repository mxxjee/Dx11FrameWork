#include "CVIBuffer_Triangle.h"
#include "VertexData.h"

CVIBuffer_Triangle::CVIBuffer_Triangle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Triangle::CVIBuffer_Triangle(const CVIBuffer_Triangle& Prototype)
    : CVIBuffer{ Prototype }
{
}


HRESULT CVIBuffer_Triangle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CVIBuffer_Triangle::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;



#pragma region VertexBuffer
	//[1. 정점 버퍼를 정의하기 위한 정보]
	D3D11_BUFFER_DESC VertexDesc;
	m_iNumVertices = 3;
	m_iVertexStride = sizeof(VTXPOSCOR);

	//위치값 기록을 위한 동적배열(따로 멤버 보관)
	m_pVertexPositions.resize(m_iNumVertices);

	VertexDesc.ByteWidth = m_iVertexStride * m_iNumVertices;		//할당할 크기
	VertexDesc.Usage = D3D11_USAGE_DYNAMIC;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//바인딩 플래그(사용 용도)
	VertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	VertexDesc.MiscFlags = 0;
	VertexDesc.StructureByteStride = m_iVertexStride;

	//[2. 정점 버퍼를 초기화 하기 위한 정보]
	VTXPOSCOR* pVertices = new VTXPOSCOR[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSCOR) * m_iNumVertices);

	TRIANGLEBUFFER_DESC* pDesc = static_cast<TRIANGLEBUFFER_DESC*>(pArg);
	if (pDesc)
	{
		m_pVertexPositions[0] = pVertices[0].vPosition=pDesc->v0;
		m_pVertexPositions[1] = pVertices[1].vPosition = pDesc->v1;
		m_pVertexPositions[2] = pVertices[2].vPosition = pDesc->v2;
	}

	else
	{
		m_pVertexPositions[0] = pVertices[0].vPosition;
		m_pVertexPositions[1] = pVertices[1].vPosition;
		m_pVertexPositions[2] = pVertices[2].vPosition;
	
	}
	
	pVertices[0].vColor = _float4(1.f, 0.f, 0.f, 1.f);
	pVertices[1].vColor = _float4(0.f, 1.f, 0.f, 1.f);
	pVertices[2].vColor = _float4(0.f, 0.f, 1.f, 1.f);


	D3D11_SUBRESOURCE_DATA VertexData;
	VertexData.pSysMem = pVertices;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;

	//정점버퍼를 만들고, 초기화 시키는함수
	if (FAILED(m_pDevice->CreateBuffer(&VertexDesc, &VertexData, m_pVB.GetAddressOf())))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


	//[1.인덱스 버퍼를 만들기 위한 정보세팅]
	D3D11_BUFFER_DESC IndexDesc;
	m_iNumIndices = 3;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;


	IndexDesc.ByteWidth = m_iIndexStride * m_iNumIndices;		//할당할 크기
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;			//바인딩 플래그(사용 용도)
	IndexDesc.CPUAccessFlags = 0;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	IndexDesc.MiscFlags = 0;
	IndexDesc.StructureByteStride = m_iIndexStride;

	/*인덱스 정의*/
	_ushort* pIndices = new _ushort[m_iNumIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;


	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexDesc, &IndexData, m_pIB.GetAddressOf())))
		return E_FAIL;


	Safe_Delete_Array(pIndices);
	return S_OK;
}


void CVIBuffer_Triangle::UpdatePoints(_float3 p0, _float3 p1, _float3 p2)
{
	VTXPOSCOR vertices[3] = {};


	D3D11_MAPPED_SUBRESOURCE mapped{};
	m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	
	VTXPOSCOR* v = (VTXPOSCOR*)mapped.pData;
	v[0].vPosition = p0;
	v[1].vPosition = p1;
	v[2].vPosition = p2;

	for (int i = 0; i < 3; ++i)
		m_pVertexPositions[i] = vertices[i].vPosition;

	m_pContext->Unmap(m_pVB.Get(), 0);
}

CVIBuffer_Triangle* CVIBuffer_Triangle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CVIBuffer_Triangle* pInstance = new CVIBuffer_Triangle(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Triangle");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent* CVIBuffer_Triangle::Clone(void* pArg)
{
	CVIBuffer_Triangle* pInstance = new CVIBuffer_Triangle(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Triangle");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CVIBuffer_Triangle::Free()
{
	__super::Free(); 

}
