#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL InputLayout :
  public CBase
{
public:
	InputLayout(ComPtr<ID3D11Device>	pDevice);
	~InputLayout();


	ComPtr<ID3D11InputLayout>	Get_ComPtr() { return m_pInputLayout; }
	void		Create(const vector<D3D11_INPUT_ELEMENT_DESC>& desc, ComPtr<ID3DBlob> blob);

public:
	virtual void Free() override;


private:
	ComPtr<ID3D11Device>		m_pDevice;
	ComPtr<ID3D11InputLayout>	m_pInputLayout;
};

NS_END
