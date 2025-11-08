#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CInput_Manager;

/*게임상에서 사용되는 단축키를 등록하는 매니저*/
/*키와 이벤트를 저장*/

class CHotKey_Manager :
    public CBase
{
public:
    using HotKeyCallback = function<void()>;
    struct HotKey
    {
        KeyCode     eKeyCode;
        bool        Ctrl;
        bool        Shift;
        bool       alt;

        HotKeyCallback          m_CallBack = nullptr;

    };

private:
    CHotKey_Manager();
    virtual ~CHotKey_Manager() = default;

public:
    void        Update();

public:
    HRESULT         Register_HotKey(KeyCode eKode, bool bCtrl, bool bShift, bool bAlt, HotKeyCallback Func);
    bool            CheckHotKeyPressed(const HotKey& Key);

public:
    static CHotKey_Manager* Create();

public:
    virtual void Free() override;


private:
    vector<HotKey>      m_HotKeys;
    CInput_Manager*     m_pInputManager = nullptr;
};
NS_END

