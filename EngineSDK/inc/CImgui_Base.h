#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImgui_Base abstract : public CBase
{
public:
	typedef struct Imgui_Desc
	{
		string	Tag						= "";


	}IMGUI_DESC;
protected:
	CImgui_Base(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CImgui_Base() = default;


public:
	virtual		HRESULT Initialize(void* pArg);
	virtual		void	Update_Priority() {};
	virtual		void	Update()=0;
	virtual		void	Render()=0;

	const string& Get_Tag() { return m_Tag; }
	void		Set_Active(bool _b) { m_bActive = _b; }
protected:
	string	m_Tag = "";				//오브젝트 식별용 Tag
	ComPtr<ID3D11Device>			m_pDevice;
	ComPtr<ID3D11DeviceContext>		m_pContext;


public:
	virtual void Free();

protected:
	bool        m_bActive = { true };

};
NS_END