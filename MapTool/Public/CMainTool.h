#pragma once

#include "MapTool_Defines.h"
#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Engine)
class CImGui_Manager;
class CGameInstance;
class CMapObject_Manager;
NS_END


NS_BEGIN(MapTool)

class CMainTool final: public CBase
{

private:
	CMainTool();
	virtual ~CMainTool() = default;

public:
	HRESULT Initialize();
	HRESULT Initialize_MapTool();


	void	Update_Priority(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Update_Late(float fTimeDelta);
	void	Update_Render(float fTimeDelta);
	void	Render();


private:
	void    CreateSamplerStates();
	void    CreateBlendStates();
	void    CreateRasterizerStates();
	void    CreateDepthStencilStates();


private:
	void		CreateTerrainDebugWindow();
	void		CreateLayerDebugWindow();
	void		CreateObjectInspectorWindow();
private:
	void			Reigster_Levels();
	HRESULT			Start_Level(LEVEL_ID iLevelID, LEVELCHANGETYPE eChangeType);
private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

public:
	static CMainTool* Create();
	virtual void Free() override;

private:
	CGameInstance* pGameInstance;
	CImGui_Manager* pImGui_Manager = { nullptr };
	CMapObject_Manager* pMapObject_Manager = { nullptr };

	_float4		ClearColor = _float4(0.f, 0.f, 1.f, 1.f);
	vector<RenderStates>			m_RenderStates;

};

NS_END