#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CSoundMgr
{
public:
	static CSoundMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CSoundMgr;

		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	CSoundMgr();
	virtual ~CSoundMgr();
public:
	void Init();
	void Release();

public:
	void PlaySound(const wstring& pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const std::wstring& soundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);


private:
	void LoadSoundFile(const std::string& path, const std::wstring& prefix);

private:
	static CSoundMgr* m_pInstance;

	// 사운드 리소스 정보를 갖는 객체 
	map<std::wstring, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
};

NS_END