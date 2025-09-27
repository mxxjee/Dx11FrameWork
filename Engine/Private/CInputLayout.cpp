#include "CInputLayout.h"

CInputLayout::CInputLayout(ComPtr<ID3D11Device> pDevice)
	:m_pDevice(pDevice)
{
}



CInputLayout::~CInputLayout()
{
}

void CInputLayout::Create(const vector<D3D11_INPUT_ELEMENT_DESC>& desc, ComPtr<ID3DBlob> blob)
{
	const UINT32 count = desc.size();
	m_pDevice.Get()->CreateInputLayout(desc.data(), count, blob->GetBufferPointer(), blob->GetBufferSize(), m_pInputLayout.GetAddressOf());

}

void CInputLayout::Free()
{
	__super::Free();
}
