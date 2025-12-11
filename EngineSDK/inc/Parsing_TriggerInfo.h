#pragma once
#include "Engine_Define.h"

struct ENGINE_DLL TriggerInfo
{

	_float3 vPos;
	_float3 vScale;
	_float3 vRotation;

	_float3 vCenter;
	_float3 vExtents=_float3(0.1f,0.1f,0.1f);

};

class CMapObjet;

struct ENGINE_DLL RoomTrigger : TriggerInfo
{
	CMapObject* pOwner = nullptr;
	string	m_NextRoomID = "";

	json ToJson() const 
	{
		json j;
		//j["Type"] = "RoomTrigger";
		j["Pos"] = { vPos.x,vPos.y,vPos.z };
		j["Scale"] = { vScale.x,vScale.y,vScale.z };
		j["Rotation"] = { vRotation.x,vRotation.y,vRotation.z };
		j["Collider_Center"] = { vCenter.x,vCenter.y,vCenter.z };
		j["Collider_Extents"] = { vExtents.x,vExtents.y,vExtents.z };
		j["NextRoomID"] = m_NextRoomID;

		return j;
	}

	static RoomTrigger	LoadJson(const json& j)
	{
		RoomTrigger Info;
		Info.vPos = _float3(j["Pos"][0], j["Pos"][1], j["Pos"][2]);
		Info.vScale = _float3(j["Scale"][0], j["Scale"][1], j["Scale"][2]);
		Info.vRotation = _float3(j["Rotation"][0], j["Rotation"][1], j["Rotation"][2]);
		
		
		Info.vCenter = _float3(j["Collider_Center"][0], j["Collider_CenterPos"][1], j["Collider_Center"][2]);
		Info.vExtents = _float3(j["Collider_Extents"][0], j["Collider_Extents"][1], j["Collider_Extents"][2]);
		Info.m_NextRoomID = string(j["NextRoomID"]);

		return Info;
	}

};