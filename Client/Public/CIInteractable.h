#pragma once
#include "Client_Defines.h"

NS_BEGIN(Client)
class CIInteractable
{
public:
	virtual bool           IsInteratable()=0;      //상호작용가능한지 조건 ( 보통 거리?)

	virtual void    Enter_InteractRange()=0;
	virtual void    Stay_InteractRange(_float fTimeDelta) =0;       //상호작용가능한 범위에잇을떄 계속호출
	virtual void    Exit_InteractRange()=0;      //ㅓ범위나갔을때 호출

	virtual void	Enter_Interaction() = 0;
	virtual void	Stay_Interaction(_float fTimeDelta) =0;
	virtual void	Exit_Interaction()=0;

	virtual _int	Get_Interaction_Priority()=0;	//여러개의 interaction이 존재할떄, 우선순위


public:
	bool		m_bPrevRange = false;		//이전프레임
	bool		m_bPrevInteracting = false;	//이전프레임의 상호작용상태(Exit/Enter판단위해서)

};
NS_END


