#pragma once

#include "Client_Defines.h"
#include "Engine_Enum.h"
#include "CBase.h"

namespace Engine
{
	class CGameInstance;
}

NS_BEGIN(Client)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;


public:
	HRESULT Initialize();
	void	Update(_float fTimeDelta);
	void	Render();

private:
	HRESULT			Start_Level(LEVEL_ID iLevelID,LEVELCHANGETYPE eChangeType);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;

private:
	 CGameInstance* pGameInstance;
	_float4		ClearColor = _float4(0.f, 0.f, 1.f, 1.f);

};

NS_END