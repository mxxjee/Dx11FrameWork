#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImguiBase abstract : public CBase
{

public:
	CImguiBase();
	virtual ~CImguiBase() = default;


public:
	virtual		HRESULT Initialize();
	virtual		void	Update();
	virtual		void	Late_Updte();
	virtual		void	Render();

	const _wstring& Get_Tag() { return m_Tag; }

protected:
	_wstring	m_Tag = L"";
	ComPtr<ID3D11Device>		m_pDevice;
	ComPtr<ID3D11DeviceContext>		m_pContext;

public:
	virtual void Free();

};
NS_END