#pragma once
#include "CBase.h"
NS_BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CAnimNotify :
    public CBase
{
public:
    typedef struct tagAnimNotifyDesc
    {
        _uint       iKeyFrame = 0;    //어느키프레임에 실행할건지?
         GameEvent   GameEvent;        //실행할 이벤트

    }AnimNotify_DESC;
private:
    CAnimNotify();
    virtual ~CAnimNotify() = default;

public:
    HRESULT      Initialize(void* pArg);
    void        NotifyBegin();  //노티파이를 실행하면 실행되는함수
    virtual void        Free() override;

public:
    static CAnimNotify* Create(void* pArg);
    _uint       Get_Frame() { return m_KeyFrame; }
private:
    _uint           m_KeyFrame = 0;     //어느키프레임에 실행할건지
    GameEvent          m_Event;

private:
    CGameInstance* m_pGameInstance = nullptr;
};
NS_END
