#include "CInputLayout.h"

CInputLayout::CInputLayout(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice)
{
}


HRESULT CInputLayout::Initialize_Prototype(const vector<D3D11_INPUT_ELEMENT_DESC>& desc, UINT iNumElements, const void* pShaderByteCode, SIZE_T ByteCodeLength)
{
	return S_OK;
}

HRESULT CInputLayout::Initialize_Copytype(void* pArg)
{
	return S_OK;
}

CInputLayout* CInputLayout::Create(ComPtr<ID3D11Device> pDevice, const vector<D3D11_INPUT_ELEMENT_DESC>& desc, UINT iNumElements, const void* pShaderByteCode, SIZE_T ByteCodeLength)
{
	return nullptr;
}

void CInputLayout::Set_InputLayout()
{
	m_pContext->IASetInputLayout(m_pInputLayout.Get());
}

void CInputLayout::Free()
{
	__super::Free();
}
