#pragma once
#include "CBase.h"
#include "Engine_Define.h"

NS_BEGIN(Engine)

class CTimer final :    public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer();

public:
	_float					Get_TimeDelta() const { return m_fTimeDelta; }

public:
	HRESULT			Ready_Timer();
	void			Update_Timer();

private:
	LARGE_INTEGER		m_FrameTime;
	LARGE_INTEGER		m_FixTime;
	LARGE_INTEGER		m_LastTime;
	LARGE_INTEGER		m_CpuTick;

	_float				m_fTimeDelta;

public:
	_float				GetFPS() const { return m_fFPS; }

public:
	static CTimer* Create();
private:
	virtual void	Free();


private:
	float m_fFPSTime = 0.f;
	int   m_iFrameCount = 0;
	float m_fFPS = 0.f;

};

NS_END
