#pragma once

#include "MapTool_Defines.h"
#include "CBase.h"

NS_BEGIN(Engine)
class CImGui_Manager;
class CGameInstance;
NS_END

NS_BEGIN(MapTool)

class CMainTool final: public CBase
{

private:
	CMainTool();
	virtual ~CMainTool() = default;

public:
	HRESULT Initialize();
	void	Update(_float fTimeDelta);
	void	Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CMainTool* Create();
	virtual void Free() override;

private:
	CGameInstance* pGameInstance;
	CImGui_Manager* pImGui_Manager = { nullptr };
	_float4		ClearColor = _float4(0.f, 0.f, 1.f, 1.f);

};

NS_END