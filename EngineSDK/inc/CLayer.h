#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CLayer final
	:public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT			Add_GameObject(class CGameObject* pGameObject);
	
	void			Update_Priority(_float fTimeDelta);
	void			Update(_float fTimeDelta);
	void			Update_Late(_float fTimeDelta);


public:
	static		CLayer* Create();
	virtual		void Free();
private:
	list<class CGameObject*>		m_GameObjects;
};
NS_END
