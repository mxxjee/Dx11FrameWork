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


	virtual void		Imgui_Render_Properties(_float3* vScale,_float3* vPosition,_float3* vRotation)=0;//Imgui창 Inspector Render
	virtual void		Show_Gizmo() = 0;


	virtual void Edit_Move(DIRECTION eDir,float fSpeed,float _fTimeDelta)=0;
	virtual void Fix_Y(_float Y)=0;

	virtual void Update_SelectMode() {};


protected:
	bool			m_bSelected = false;
	MapObjType              m_eObjType;


};
NS_END