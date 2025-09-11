#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Client)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	virtual void Free() override;

};

NS_END