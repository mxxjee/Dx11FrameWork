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
class CFadeScreen;

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;


public:
	HRESULT Initialize();
	HRESULT Initialize_Cilent();

	HRESULT Ready_Fonts();
	void	Set_Collision_Rules();		//그룹간 충돌 설정

	void	Update_Priority(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Update_Late(float fTimeDelta);
	void	Update_Render(float fTimeDelta);
	void	Render();



private:
	void			Register_Levels();
	HRESULT			Start_Level(LEVEL_ID iLevelID,LEVELCHANGETYPE eChangeType);
	

public:
	void		Create_FadeScreen();
	void		Create_LoadingUI();

private:
#ifdef _DEBUG
	/////////////////Create ImguiWindow/////////////////
	void		CreateLevelDebugWindow();
	void		CreateObjectDebugWindow();
	void		CreateCameraDebugWindow();
	void		CreateStateDebugWindow();
#endif

private:
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr <ID3D11DeviceContext> m_pContext;

public:
	static CMainApp* Create();
	virtual void Free() override;

private:
	 CGameInstance* m_pGameInstance;
	 _float4		ClearColor = COLOR_BLACK;

	vector<RenderStates>			m_RenderStates;


private:
	CImGui_Manager* pImGui_Manager = { nullptr };
	int iTargetCameraIdx = 0;
	CFadeScreen* pFadeScreen = nullptr;

};

NS_END