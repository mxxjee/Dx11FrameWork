#pragma once
#include "Engine_Define.h"

struct SaveFilePath
{
	string					m_SavePathBase = "";		//저장경로의 베이스경로 + (    .json)

	string					m_CurrentSaveFilePath="";	//저장시 저장되는 경로.
	
	
	vector<string>			m_SaveFiles;		//이미 저장된 세이브파일들의 경로.
	//List캐싱용..
	vector<string>              m_SaveFileNames;
	vector<const char*>         m_SaveFileNamesStr;
};

struct LoadFilePath
{
	string				m_CurrentLoadFilePath="";		//로드 시 현재 읽어온 경로. 덮어쓰기 일 경우 이를 이용
	int					LoadFileIdx = 0;
};