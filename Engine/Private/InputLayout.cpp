#include "InputLayout.h"

InputLayout::InputLayout(ID3D11Device* pDevice)
	:m_pDevice(pDevice)
{
}



InputLayout::~InputLayout()
{
}

void InputLayout::Create(const vector<D3D11_INPUT_ELEMENT_DESC>& desc, ComPtr<ID3DBlob> blob)
{
	const UINT32 count = desc.size();
	m_pDevice.Get()->CreateInputLayout(desc.data(), count, blob->GetBufferPointer(), blob->GetBufferSize(), m_pInputLayout.GetAddressOf());

}
