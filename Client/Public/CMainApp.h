#pragma once

#include "Client_Defines.h"
#include "Engine_Enum.h"
#include "CBase.h"
#include "ColorUtils.h"

namespace Engine
{
	class CGameInstance;
	class CImGui_Manager;
}

NS_BEGIN(Client)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;


public:
	HRESULT Initialize();
	void	Update_Priority(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Update_Late(float fTimeDelta);
	void	Update_Render(float fTimeDelta);
	void	Render();


private:
	void			Register_Levels();
	void			Register_Shaders();
	HRESULT			Start_Level(LEVEL_ID iLevelID,LEVELCHANGETYPE eChangeType);


private:
	/////////////////Create ImguiWindow/////////////////
	void		CreateLevelDebugWindow();
	void		CreateObjectDebugWindow();
	void		CreateCameraDebugWindow();

private:
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr <ID3D11DeviceContext> m_pContext;

public:
	static CMainApp* Create();
	virtual void Free() override;

private:
	 CGameInstance* pGameInstance;
	 _float4		ClearColor = COLOR_PINK;
	CImGui_Manager* pImGui_Manager = { nullptr };

};

NS_END