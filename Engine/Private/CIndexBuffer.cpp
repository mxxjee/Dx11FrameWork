
#include "CIndexBuffer.h"

CIndexBuffer::CIndexBuffer()
{
}

CIndexBuffer::CIndexBuffer(ComPtr<ID3D11Device> pDevice)
	:m_pDevice{pDevice}
{
}

CIndexBuffer::~CIndexBuffer()
{
}

void CIndexBuffer::Create(const vector<UINT32>& _indices)
{
	_stride = sizeof(UINT32);
	_count = static_cast<UINT32>(_indices.size());

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = (UINT32)sizeof(UINT32) * _indices.size();

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = _indices.data();
	HRESULT hr = m_pDevice->CreateBuffer(&desc, &data, m_pIndexBuffer.GetAddressOf());

}

void CIndexBuffer::Free()
{
	__super::Free();
}
