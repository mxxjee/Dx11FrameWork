#include "CInputLayout.h"

CInputLayout::CInputLayout(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
}


HRESULT CInputLayout::Initialize_Prototype(const vector<D3D11_INPUT_ELEMENT_DESC>& desc, UINT iNumElements, const void* pShaderByteCode, SIZE_T ByteCodeLength)
{
	
	HRESULT hr = m_pDevice->CreateInputLayout(desc.data(), (_uint)desc.size(), pShaderByteCode, ByteCodeLength, m_pInputLayout.GetAddressOf());
	
	return S_OK;
}

HRESULT CInputLayout::Initialize_Copytype(void* pArg)
{
	return S_OK;
}



CInputLayout* CInputLayout::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
	const vector<D3D11_INPUT_ELEMENT_DESC>& desc, UINT iNumElements, const void* pShaderByteCode, SIZE_T ByteCodeLength)
{
	CInputLayout* pInstance = new CInputLayout(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(desc, iNumElements, pShaderByteCode, ByteCodeLength)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInputLayout::Set_InputLayout()
{
	m_pContext->IASetInputLayout(m_pInputLayout.Get());
}

void CInputLayout::Free()
{
	__super::Free();
}
