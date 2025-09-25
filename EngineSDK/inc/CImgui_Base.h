#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImgui_Base abstract : public CBase
{

public:
	CImgui_Base();
	CImgui_Base(const string& Tag);
	virtual ~CImgui_Base() = default;


public:
	virtual		HRESULT Initialize()=0;
	virtual		void	Update()=0;
	virtual		void	Render()=0;

	const string& Get_Tag() { return m_Tag; }

protected:
	string	m_Tag = "";
	ComPtr<ID3D11Device>		m_pDevice;
	ComPtr<ID3D11DeviceContext>		m_pContext;

public:
	virtual void Free();

};
NS_END