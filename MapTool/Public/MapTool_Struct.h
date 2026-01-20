#pragma once
#include "Engine_Define.h"

struct SaveFilePath
{
	string					m_SavePathBase = "";		//저장경로의 베이스경로 + (    .json)

	string					m_CurrentTerrainSaveFilePath = "";	//저장시 저장되는 경로.
	string					m_CurrentNavSaveFilePath = "";
	string					m_CurrentInteractionFilePath = "";


	vector<string>			m_TerrainSaveFiles;		//이미 저장된 세이브파일들의 경로.
	vector<string>			m_NavSaveFiles;		//이미 저장된 세이브파일들의 경로.
	vector<string>			m_InteractionFiles;		//이미 저장된 세이브파일들의 경로.
	vector<string>			m_RoomFiles;		//이미 저장된 세이브파일들의 경로.


			
	//List캐싱용..
	vector<string>              m_SaveTerrainFileNames;
	vector<string>				m_SaveNavFileNames;
	vector<string>				m_SaveInteractionFileNames;
	vector<string>				m_SaveRoomFileNames;



	vector<const char*>         m_TerrainSaveFileNamesStr;
	vector<const char*>			m_NavSaveFileNamesStr;
	vector<const char*>			m_InteractionSaveFileNamesStr;
	vector<const char*>			m_RoomSaveFileNamesStr;



	SaveFilePath()
	{

	}
};

struct LoadFilePath
{
	string				m_CurrentLoadTerrainFilePath="";		//로드 시 현재 읽어온 경로. 덮어쓰기 일 경우 이를 이용
	string				m_CurrentLoadNavFilePath = "";
	string				m_CurrentLoadInteractionFilePath = "";
	string				m_CurrentLoadRoomFilePath = "";

	
	int					LoadTerarinFileIdx = 0;//Terrain용
	int					LoadNavFileIdx = 0;//navmesh용
	int					LoadInteractionFileIdx = 0;//Interaction용
	int					LoadRoomFileIdx = 0;//Interaction용
};