#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImgui_Base abstract : public CBase
{
public:
	typedef struct Imgui_Desc
	{
		wstring	Tag						= L"";


	}IMGUI_DESC;
protected:
	CImgui_Base(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CImgui_Base() = default;


public:
	virtual		HRESULT Initialize(void* pArg);
	virtual		void	Update()=0;
	virtual		void	Render()=0;

	const wstring& Get_Tag() { return m_Tag; }

protected:
	wstring	m_Tag = L"";				//오브젝트 식별용 Tag
	ComPtr<ID3D11Device>			m_pDevice;
	ComPtr<ID3D11DeviceContext>		m_pContext;


public:
	virtual void Free();


};
NS_END