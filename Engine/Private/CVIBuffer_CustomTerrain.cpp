#include "CVIBuffer_CustomTerrain.h"


CVIBuffer_CustomTerrain::CVIBuffer_CustomTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_CustomTerrain::CVIBuffer_CustomTerrain(const CVIBuffer_CustomTerrain& Prototype)
	: CVIBuffer( Prototype )
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	, m_iOffSet{Prototype.m_iOffSet}
	
{
	
}

HRESULT CVIBuffer_CustomTerrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ, _uint Offset)
{	

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	Offset = Offset;

	D3D11_BUFFER_DESC VertexDesc{};
	VTXNORTEX* pVertice = nullptr;

	D3D11_BUFFER_DESC IndexDesc{};
	_uint* pIntIndices = nullptr;

	if (FAILED(CreateVertexBuffer_Begin(m_iNumVerticesX, m_iNumVerticesZ, &pVertice, &VertexDesc)))
		return E_FAIL;


	if (FAILED(CreateIndexBuffer_Begin(m_iNumVerticesX, m_iNumVerticesZ, &pVertice, &pIntIndices, &IndexDesc)))
		return E_FAIL;


	if (FAILED(CreateVertexBuffer_End(VertexDesc, pVertice)))
		return E_FAIL;


	if (FAILED(CreateIndexBuffer_End(IndexDesc, pIntIndices)))
		return E_FAIL;



	return S_OK;
}

HRESULT CVIBuffer_CustomTerrain::Initialize_Copytype(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_CustomTerrain::ResizeBuffer(_uint fNewVertexCountX, _uint fNewVertexCountZ)
{
	//원래 할당했떤 정점보다 클때만 다시 Buffer(정점 + 인덱스 둘다)을 재생성한다.
	//작을 경우는 인덱스버퍼만 수정,
	
	_uint NewNumVertices = fNewVertexCountX * fNewVertexCountZ;

	//정점의 개수가 늘어난경우
	if (NewNumVertices > m_iNumVerticesX * m_iNumVerticesZ)
	{
		m_pVB.Reset();
		m_pIB.Reset();

		m_iNumVerticesX = fNewVertexCountX;
		m_iNumVerticesZ = fNewVertexCountZ;

		D3D11_BUFFER_DESC VertexDesc{};
		VTXNORTEX* pVertice = nullptr;

		D3D11_BUFFER_DESC IndexDesc{};
		_uint* pIntIndices = nullptr;

		if (FAILED(CreateVertexBuffer_Begin(m_iNumVerticesX, m_iNumVerticesZ, &pVertice, &VertexDesc)))
			return E_FAIL;
		
		if (FAILED(CreateIndexBuffer_Begin(m_iNumVerticesX, m_iNumVerticesZ, &pVertice, &pIntIndices, &IndexDesc)))
				return E_FAIL;
		

		if (FAILED(CreateVertexBuffer_End(VertexDesc, pVertice)))
			return E_FAIL;


		if (FAILED(CreateIndexBuffer_End(IndexDesc, pIntIndices)))
			return E_FAIL;

		

	}

	else
	{
		m_iNumVerticesX = fNewVertexCountX;
		m_iNumVerticesZ = fNewVertexCountZ;

		D3D11_MAPPED_SUBRESOURCE mapped;

		if (FAILED(Modify_VertexBuffer(&mapped)))
			return E_FAIL;

		if (FAILED(Modify_IndexBuffer(&mapped)))
			return E_FAIL;
	}



	return S_OK;
}




HRESULT CVIBuffer_CustomTerrain::CreateVertexBuffer_Begin(_uint VertexCountX, _uint VertexCountZ, VTXNORTEX** pVertices, D3D11_BUFFER_DESC* pDesc)
{
	//[1. 정점 버퍼를 정의하기 위한 정보]

	m_iNumVertices = VertexCountX * VertexCountZ;
	m_iVertexStride = sizeof(VTXNORTEX);

	//위치값 기록을 위한 동적배열(따로 멤버 보관)
	m_pVertexPositions = new _float3[m_iNumVertices];

	
	pDesc->ByteWidth = m_iVertexStride * m_iNumVertices;		//할당할 크기
	pDesc->Usage = D3D11_USAGE_DYNAMIC;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	pDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;			//바인딩 플래그(사용 용도)
	pDesc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	pDesc->MiscFlags = 0;
	pDesc->StructureByteStride = 0;

	//[2. 정점 버퍼를 초기화 하기 위한 정보]
	*pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(*pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIdx = i * m_iNumVerticesX + j;


			(*pVertices)[iIdx].vPosition = _float3(j * (float)m_iOffSet, 0.f, i * (float)m_iOffSet);
			(*pVertices)[iIdx].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			(*pVertices)[iIdx].vNormal = _float3(0.f, 0.f, 0.f);
		
			m_pVertexPositions[iIdx] = (*pVertices)[iIdx].vPosition;

		}
	}


	return S_OK;
}

HRESULT CVIBuffer_CustomTerrain::CreateVertexBuffer_End(D3D11_BUFFER_DESC& VertexDesc, VTXNORTEX* pVertices)
{
	//법선벡터 정규화
	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	D3D11_SUBRESOURCE_DATA VertexData;
	VertexData.pSysMem = pVertices;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&VertexDesc, &VertexData, m_pVB.GetAddressOf())))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_CustomTerrain::CreateIndexBuffer_End(D3D11_BUFFER_DESC& IndexDesc, _uint* pIndices)
{
	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexDesc, &IndexData, m_pIB.GetAddressOf())))
		return E_FAIL;


	Safe_Delete_Array(pIndices);

	return S_OK;
}





HRESULT CVIBuffer_CustomTerrain::Modify_VertexBuffer(D3D11_MAPPED_SUBRESOURCE* mapped)
{
	
	//이전내용없애고 다시쓰자.
	if (FAILED(m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, mapped)))
		return E_FAIL;


	VTXNORTEX* pVertices = reinterpret_cast<VTXNORTEX*>(mapped->pData);
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIdx = i * m_iNumVerticesX + j;


			pVertices[iIdx].vPosition = _float3(j * (float)m_iOffSet, 0.f, i * (float)m_iOffSet);
			pVertices[iIdx].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			pVertices[iIdx].vNormal = _float3(0.f, 0.f, 0.f);

			m_pVertexPositions[iIdx] = (pVertices)[iIdx].vPosition;
		}
	}


	return S_OK;
}

HRESULT CVIBuffer_CustomTerrain::Modify_IndexBuffer(D3D11_MAPPED_SUBRESOURCE* mapped)
{
	D3D11_MAPPED_SUBRESOURCE Indexmapped;
	if (FAILED(m_pContext->Map(m_pIB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Indexmapped)))
		return E_FAIL;


	VTXNORTEX* pVertices = reinterpret_cast<VTXNORTEX*>(mapped->pData);
	_uint* pIndices = reinterpret_cast<_uint*>(Indexmapped.pData);
	_uint iNumIndices{};

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint       iIndex = i * m_iNumVerticesX + j;

			//좌하단 = iIndx
			_uint iIndices[4] =
			{
				iIndex + m_iNumVerticesX,		//좌상단
				iIndex + m_iNumVerticesX + 1,	//우상단
				iIndex + 1,		//우하단
				iIndex,			//좌상단
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

		_vector vSrv, vTmp, vNormal;


			vSrv = XMLoadFloat3(&((pVertices)[iIndices[1]].vPosition))
				- XMLoadFloat3(&((pVertices)[iIndices[0]].vPosition));

			vTmp = XMLoadFloat3(&((pVertices)[iIndices[2]].vPosition))
				- XMLoadFloat3(&((pVertices)[iIndices[1]].vPosition));

			vNormal = XMVector3Normalize(XMVector3Cross(vSrv, vTmp));

			//Normal값 누적
			XMStoreFloat3(&(pVertices)[iIndices[0]].vNormal,
				XMLoadFloat3(&(pVertices)[iIndices[0]].vNormal) + vNormal);

			XMStoreFloat3(&(pVertices)[iIndices[1]].vNormal,
				XMLoadFloat3(&(pVertices)[iIndices[1]].vNormal) + vNormal);

			XMStoreFloat3(&(pVertices)[iIndices[2]].vNormal,
				XMLoadFloat3(&(pVertices)[iIndices[2]].vNormal) + vNormal);


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];


			vSrv = XMLoadFloat3(&((pVertices)[iIndices[2]].vPosition))
				- XMLoadFloat3(&((pVertices)[iIndices[0]].vPosition));

			vTmp = XMLoadFloat3(&((pVertices)[iIndices[3]].vPosition))
				- XMLoadFloat3(&((pVertices)[iIndices[2]].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSrv, vTmp));

			//Normal값 누적
			XMStoreFloat3(&(pVertices)[iIndices[0]].vNormal,
				XMLoadFloat3(&(pVertices)[iIndices[0]].vNormal) + vNormal);

			XMStoreFloat3(&(pVertices)[iIndices[2]].vNormal,
				XMLoadFloat3(&(pVertices)[iIndices[2]].vNormal) + vNormal);

			XMStoreFloat3(&(pVertices)[iIndices[3]].vNormal,
				XMLoadFloat3(&(pVertices)[iIndices[3]].vNormal) + vNormal);


		}
	}

	m_iNumIndices = iNumIndices;

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		m_pIndices[i] = pIndices[i];
	}

	for (_uint i = 0; i < m_iNumVerticesX * m_iNumVerticesZ; ++i)
	{
		XMVECTOR n = XMLoadFloat3(&pVertices[i].vNormal);
		n = XMVector3Normalize(n);
		XMStoreFloat3(&pVertices[i].vNormal, n);
	}

	m_pContext->Unmap(m_pIB.Get(), 0);
	m_pContext->Unmap(m_pVB.Get(), 0);
	return S_OK;
}


CVIBuffer_CustomTerrain* CVIBuffer_CustomTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumVerticesX, _uint iNumVerticesZ, _uint Offset)
{
	CVIBuffer_CustomTerrain* pInstance = new CVIBuffer_CustomTerrain(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ, Offset)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}


	return pInstance;
}



CComponent* CVIBuffer_CustomTerrain::Clone(void* pArg)
{
	CVIBuffer_CustomTerrain* pInstance = new CVIBuffer_CustomTerrain(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_CustomTerrain");
		Safe_Release(pInstance);
	}


	return pInstance;
}



void CVIBuffer_CustomTerrain::Free()
{
	__super::Free();
}

HRESULT CVIBuffer_CustomTerrain::CreateIndexBuffer_Begin(_uint VertexCountX, _uint VertexCountZ, VTXNORTEX** pVertices, _uint** pIndices, D3D11_BUFFER_DESC* pDesc)
{

	//[1.인덱스 버퍼를 만들기 위한 정보세팅]
	m_iNumIndices = (VertexCountX - 1) * (VertexCountZ - 1) * 2 * 3;
	m_iNumVertexBuffers = 1;
	m_iIndexStride = 4;



	pDesc->ByteWidth = m_iIndexStride * m_iNumIndices;		//할당할 크기
	pDesc->Usage = D3D11_USAGE_DYNAMIC;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	pDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;			//바인딩 플래그(사용 용도)
	pDesc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	pDesc->MiscFlags = 0;
	pDesc->StructureByteStride = 0;

	/*인덱스 정의*/


	*pIndices = new _uint[m_iNumIndices];
	m_pIndices = new _uint[m_iNumIndices];		//따로멤버에 정의

	_uint       iNumIndices = {};

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint       iIndex = i * m_iNumVerticesX + j;

			//좌하단 = iIndx
			_uint iIndices[4] =
			{
				iIndex + m_iNumVerticesX,		//좌상단
				iIndex + m_iNumVerticesX + 1,	//우상단
				iIndex + 1,		//우하단
				iIndex,			//좌상단
			};

			(*pIndices)[iNumIndices++] = iIndices[0];
			(*pIndices)[iNumIndices++] = iIndices[1];
			(*pIndices)[iNumIndices++] = iIndices[2];

			_vector vSrv, vTmp, vNormal;


			vSrv = XMLoadFloat3(&((*pVertices)[iIndices[1]].vPosition))
				- XMLoadFloat3(&((*pVertices)[iIndices[0]].vPosition));

			vTmp = XMLoadFloat3(&((*pVertices)[iIndices[2]].vPosition))
				- XMLoadFloat3(&((*pVertices)[iIndices[1]].vPosition));

			vNormal = XMVector3Normalize(XMVector3Cross(vSrv, vTmp));

			//Normal값 누적
			XMStoreFloat3(&(*pVertices)[iIndices[0]].vNormal,
				XMLoadFloat3(&(*pVertices)[iIndices[0]].vNormal) + vNormal);

			XMStoreFloat3(&(*pVertices)[iIndices[1]].vNormal,
				XMLoadFloat3(&(*pVertices)[iIndices[1]].vNormal) + vNormal);

			XMStoreFloat3(&(*pVertices)[iIndices[2]].vNormal,
				XMLoadFloat3(&(*pVertices)[iIndices[2]].vNormal) + vNormal);


			(*pIndices)[iNumIndices++] = iIndices[0];
			(*pIndices)[iNumIndices++] = iIndices[2];
			(*pIndices)[iNumIndices++] = iIndices[3];


			vSrv = XMLoadFloat3(&((*pVertices)[iIndices[2]].vPosition))
				- XMLoadFloat3(&((*pVertices)[iIndices[0]].vPosition));

			vTmp = XMLoadFloat3(&((*pVertices)[iIndices[3]].vPosition))
				- XMLoadFloat3(&((*pVertices)[iIndices[2]].vPosition));
			vNormal = XMVector3Normalize(XMVector3Cross(vSrv, vTmp));

			//Normal값 누적
			XMStoreFloat3(&(*pVertices)[iIndices[0]].vNormal,
				XMLoadFloat3(&(*pVertices)[iIndices[0]].vNormal) + vNormal);

			XMStoreFloat3(&(*pVertices)[iIndices[2]].vNormal,
				XMLoadFloat3(&(*pVertices)[iIndices[2]].vNormal) + vNormal);

			XMStoreFloat3(&(*pVertices)[iIndices[3]].vNormal,
				XMLoadFloat3(&(*pVertices)[iIndices[3]].vNormal) + vNormal);


		}
	}
	

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		m_pIndices[i] = (*pIndices)[i];
	}


	return S_OK;
}
