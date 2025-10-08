#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CInputLayout :
  public CBase
{
private:
	CInputLayout(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CInputLayout()=default;

public:
	HRESULT     Initialize_Prototype(const vector<D3D11_INPUT_ELEMENT_DESC>& desc, UINT iNumElements, const void* pShaderByteCode, SIZE_T ByteCodeLength);
	HRESULT     Initialize_Copytype(void* pArg);


	ComPtr<ID3D11InputLayout>	Get_ComPtr() { return m_pInputLayout; }
	CInputLayout*				Create(ComPtr<ID3D11Device> pDevice,const vector<D3D11_INPUT_ELEMENT_DESC>& desc, UINT iNumElements,const void* pShaderByteCode, SIZE_T ByteCodeLength);

public:
	void						Set_InputLayout();
public:
	virtual void Free() override;


private:
	ComPtr<ID3D11Device>		m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pContext;
	ComPtr<ID3D11InputLayout>	m_pInputLayout;
};

NS_END
