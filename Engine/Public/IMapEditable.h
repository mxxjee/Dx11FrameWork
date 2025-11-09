#pragma once
#include "Engine_Define.h"

/*편집이가능한 오브젝트임을 나타내는 인터페이스*/
NS_BEGIN(Engine)
class ENGINE_DLL IMapEditable
{
public:
	virtual void OnSeletected(bool bSelected) = 0;
	virtual void Save_To_Json(json& Json) = 0;
	MapObjType Get_ObjType() { return m_eObjType; }
	virtual void Show_Gizmo() = 0;

protected:
	bool			m_bSelected = false;
	MapObjType              m_eObjType;


};
NS_END