#pragma once

/*

Base : 모든 클래스의 부모가 될 클래스, 고로 추상클래스로 만든다.
Abstract키워드가 붙은 클래스는 순수가상함수가 없어도 추상클래스가 된다.

Base 클래스는 레퍼런스 카운트를 관리하는 역할을 한다.
1) 레퍼런스 카운트를 증가시킨다.
2) 레퍼런스 카운트를 감소시키거나 객체를 삭제 시킨다.

레퍼런스 카운트란, 참조시 댕글링 포인터나 삭제된 포인터를 가리키게 하지 않도록 참조 대상의 수를 설정하는 것을 의미

레퍼런스 카운트는 포인터 참조 시 증가시킨다.



*/

#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase()=default;		//컴파일러가 사용하는 기본 소멸자를 사용하겠음.



public:
	unsigned int	AddRef();		//레퍼런스 카운트를 증가시킨다, 증가시킨 이후의 값을 리턴
	unsigned int	Release();		//레퍼런스 카운트를 감소시킨다. 감소시킨 이전의 값을 리턴한다.
									//							리턴값을 통해 감소/삭제를 판단하기 위해서이다.
									//							만약 리턴값이 0이라면, 삭제를 의미
									//							만약 리턴값이 0이아닌 수라면, 레퍼런스 카운트 감소 의미
private:
	unsigned int		m_iRefCnt = 0;



protected:
	virtual void	Free();		//객체 삭제 직전에, 안전하게 다른 자원들을 끊어내기 위함(멤버변수 정리)


};
NS_END
