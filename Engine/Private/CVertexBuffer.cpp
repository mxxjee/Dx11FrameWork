#include "CVertexBuffer.h"

CVertexBuffer::CVertexBuffer()
{
}

CVertexBuffer::CVertexBuffer(ComPtr<ID3D11Device> pDevice)
	:m_pDevice{ pDevice }
{
}

CVertexBuffer::~CVertexBuffer()
{
}

void CVertexBuffer::Free()
{
	__super::Free();
}

