#include "CSoundMgr.h"

CSoundMgr* CSoundMgr::m_pInstance = nullptr;

CSoundMgr::CSoundMgr()
    : m_pSystem(nullptr)
{
    for (int i = 0; i < MAXCHANNEL; ++i)
        m_pChannelArr[i] = nullptr;
}

CSoundMgr::~CSoundMgr()
{
    Release();
}

void CSoundMgr::Init()
{
    FMOD_System_Create(&m_pSystem, FMOD_VERSION);
    FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
    LoadSoundFile("../../Resource/Sound/", L"");
}

void CSoundMgr::Release()
{
    for (auto& kv : m_mapSound)
    {
        FMOD_Sound_Release(kv.second);
    }
    m_mapSound.clear();

    FMOD_System_Close(m_pSystem);
    FMOD_System_Release(m_pSystem);
    m_pSystem = nullptr;
}

void CSoundMgr::PlaySound(const std::wstring& soundKey, CHANNELID eID, float fVolume)
{
    auto iter = m_mapSound.find(soundKey);
    if (iter == m_mapSound.end())
        return;

    FMOD_BOOL isPlaying = FALSE;
    FMOD_Channel_IsPlaying(m_pChannelArr[eID], &isPlaying);

    FMOD_Channel_Stop(m_pChannelArr[eID]);

    if (!isPlaying)
    {
        FMOD_System_PlaySound(
            m_pSystem,
            iter->second,
            0,
            FALSE,
            &m_pChannelArr[eID]
        );
    }
    else
    {
        FMOD_Channel_Stop(m_pChannelArr[eID]);
        FMOD_System_PlaySound(
            m_pSystem,
            iter->second,
            0,
            FALSE,
            &m_pChannelArr[eID]
        );
    }

    FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
    FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(const std::wstring& soundKey, float fVolume)
{
    auto iter = m_mapSound.find(soundKey);
    if (iter == m_mapSound.end())
        return;

    FMOD_System_PlaySound(
        m_pSystem,
        iter->second,
        0,
        FALSE,
        &m_pChannelArr[SOUND_BGM]
    );
    FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
    FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
    FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
    FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
    for (int i = 0; i < MAXCHANNEL; ++i)
        FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
    FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
    FMOD_System_Update(m_pSystem);
}



void CSoundMgr::LoadSoundFile(const std::string& path, const std::wstring& prefix)
{
    _finddata_t fd;
    std::string fullPath = path + "*.*";
    intptr_t handle = _findfirst(fullPath.c_str(), &fd);
    if (handle == -1)
        return;

    do
    {
        if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0)
            continue;

        std::string relativePath = path + fd.name;

        if (fd.attrib & _A_SUBDIR)
        {
            // 폴더라면 재귀적으로 들어가기
            std::string subPath = path + fd.name + "/";
            std::wstring subPrefix = prefix + std::wstring(fd.name, fd.name + strlen(fd.name)) + L"/";
            LoadSoundFile(subPath, subPrefix);
        }
        else
        {
            // 파일이면 사운드 로드
            std::wstring wname;
            int wlen = MultiByteToWideChar(CP_ACP, 0, fd.name, -1, NULL, 0);
            wname.resize(wlen - 1);
            MultiByteToWideChar(CP_ACP, 0, fd.name, -1, &wname[0], wlen - 1);

            // 전체 파일 경로 (ANSI)
            char szFullPath[256];
            sprintf_s(szFullPath, "%s%s", path.c_str(), fd.name);

            FMOD_SOUND* pSound = nullptr;
            FMOD_RESULT res = FMOD_System_CreateSound(
                m_pSystem,
                szFullPath,
                FMOD_DEFAULT,
                0,
                &pSound
            );

            if (res == FMOD_OK)
            {
                m_mapSound.emplace(prefix + wname, pSound);
            }
        }

    } while (_findnext(handle, &fd) == 0);

    _findclose(handle);
}