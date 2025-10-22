#include "CVIBuffer_Terrain.h"
#include "VertexData.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
    :CVIBuffer{Prototype}
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
{
	if (Prototype.m_pIndices)
	{
		m_pIndices = new _uint[m_iNumIndices];
		memcpy((_uint*)m_pIndices, (_uint*)Prototype.m_pIndices, sizeof(_uint) * m_iNumIndices);

	}
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightFileMapPath)
{
	//1.높이맵 읽어오자
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(pHeightFileMapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;


	//비트맵의 구조, 한 픽셀당 32bit(4byte)
	BITMAPFILEHEADER		fh{};
	BITMAPINFOHEADER		ih{};
	_uint*					pPixels = { nullptr };			//픽셀 읽어오기 위해, 높이값에 사용

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);
	
	//ih정보에 픽셀개수있음
	pPixels = new _uint[ih.biWidth * ih.biHeight];

	//픽셀읽어오기
	ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);



	///읽어온 높이맵 값으로 버퍼만들기
#pragma region VertexBuffer

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iIndexStride = 4;

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iNumVertexBuffers = 1;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//[1. 정점 버퍼를 정의하기 위한 정보]
	D3D11_BUFFER_DESC VertexDesc;
	


	
	VertexDesc.ByteWidth = m_iVertexStride * m_iNumVertices;		//할당할 크기
	VertexDesc.Usage = D3D11_USAGE_DEFAULT;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//바인딩 플래그(사용 용도)
	VertexDesc.CPUAccessFlags = 0;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	VertexDesc.MiscFlags = 0;
	VertexDesc.StructureByteStride = m_iVertexStride;

	//[2. 정점 버퍼를 초기화 하기 위한 정보]
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	//위치값 기록을 위한 동적배열(따로 멤버 보관)
	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3)*m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIdx = i * m_iNumVerticesX + j;
			// pPixels[iIndex]

	//    11111111 10101111 10101111 10101111

	//&   00000000 00000000 00000000 11111111

	//    00000000 00000000 00000000 10101111
			pVertices[iIdx].vPosition = _float3((float)j, (pPixels[iIdx] & 0x000000ff) / 10.0f, (float)i);
			pVertices[iIdx].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			pVertices[iIdx].vNormal = _float3(0.f, 0.f, 0.f);


			m_pVertexPositions[iIdx] = pVertices[iIdx].vPosition;

		}
	}


	D3D11_SUBRESOURCE_DATA VertexData;
	VertexData.pSysMem = pVertices;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;


#pragma endregion


	//[1.인덱스 버퍼를 만들기 위한 정보세팅]
	D3D11_BUFFER_DESC IndexDesc;




	IndexDesc.ByteWidth = m_iIndexStride * m_iNumIndices;		//할당할 크기
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;					 //cpu/gpu가 어떻게 읽을건지에 대한 플래그 설정
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;			//바인딩 플래그(사용 용도)
	IndexDesc.CPUAccessFlags = 0;							//CPU의 접근권한 설정 , 0일 경우 접근불가, 보통 0은 DEFAULT/IMMUTABLE과 함께 사용
	IndexDesc.MiscFlags = 0;
	IndexDesc.StructureByteStride = m_iIndexStride;

	/*인덱스 정의*/
	_uint* pIndices = new _uint[m_iNumIndices];
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

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];
			
			_vector vSrv, vTmp, vNormal;

			vSrv=XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vTmp= XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal= XMVector3Normalize(XMVector3Cross(vSrv, vTmp));
			
			//Normal값 누적
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];


			vSrv = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vTmp = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrv, vTmp));

			//Normal값 누적
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}


	//법선벡터 정규화
	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}


	//정점버퍼를 만들고, 초기화 시키는함수
	if (FAILED(m_pDevice->CreateBuffer(&VertexDesc, &VertexData, m_pVB.GetAddressOf())))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixels);

	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexDesc, &IndexData, m_pIB.GetAddressOf())))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		m_pIndices[i] = (pIndices)[i];
	}

	Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Copytype(void* pArg)
{
    return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pHeightFileMapPath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pHeightFileMapPath)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
