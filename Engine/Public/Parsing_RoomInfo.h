#pragma once
#include "Engine_Define.h"
#include "Parsing_TriggerInfo.h"



struct ENGINE_DLL LoadRoomInfo
{

};
struct ENGINE_DLL NPCInfo
{
	string ModelName = "";
	_float3 vPos;
	_float3 vScale;
	_float3 vRotation;


};

struct ENGINE_DLL PositionInfo
{
	string TargetName = "";
	_float3 vPos;

	json ToJson() const
	{
		json j;
		j["Target"] = TargetName;
		j["Pos"] = { vPos.x,vPos.y,vPos.z };
	

		return j;
	}

	static PositionInfo	LoadJson(const json& j)
	{
		PositionInfo Info;
		Info.vPos = _float3(j["Pos"][0], j["Pos"][1], j["Pos"][2]);
		Info.TargetName = j["Target"];
		return Info;
	}
};


struct ENGINE_DLL RoomInfo
{
	string RoomName = "";
	_float3 vPos;
	_float3 vScale;
	_float3 vRotation;

	vector<PositionInfo>		m_Positions;
	vector<RoomTrigger>			m_RoomTriggers;

};