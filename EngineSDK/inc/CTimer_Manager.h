#pragma once

#include "CBase.h"
#include "CTimer.h"

NS_BEGIN(Engine)

class CTimer_Manager : public CBase
{

private:
	explicit CTimer_Manager();
	~CTimer_Manager()=default;

public:
	_float			Get_TimeDelta(const _tchar* pTimerTag);
	
public:
	void			Compute_TimeDelta(const _tchar* pTimerTag);
	HRESULT			Add_Timer(const _tchar* pTimerTag);

public:
	static CTimer_Manager* Create();
public:
	_float	Get_FPS(const _tchar* pTimerTag);

private:
	CTimer* Find_Timer(const _tchar* pTimerTag);

private:
	map<const _wstring, CTimer*>		m_mapTimer;		//키값 : const _tchar*에서 const _wstring으로 변경
																	//const _tchar*로 key값 저장 시 상수문자열로 저장됨.
																	//상수문자열은 한번 할당 시 주소가 바뀌지않음
																	// Debug모드에서는 리터럴이 동일한 주소를 가리키는 경우가 많아서 find가 성공하는것처럼 보임.
																	//하지만 Release모드에서 이를 실행했을때, 상수문자열의 주소가 바뀔 수 있어 find가 불가할 수 있음.
																	//즉, "TimerA"라는 동일한 텍스트라도 포인터 값이 다르면 다른키로 취급될 수 있다.
																	

																	//그러므로 ==연산자가 존재하는 wstring으로 변경

private:
	virtual void		Free();
};

NS_END