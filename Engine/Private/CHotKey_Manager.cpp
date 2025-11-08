#include "CHotKey_Manager.h"
#include "CInput_Manager.h"


CHotKey_Manager::CHotKey_Manager()
    :m_pInputManager(CInput_Manager::GetInstance())
{
    Safe_AddRef(m_pInputManager);
}

void CHotKey_Manager::Update()
{
    for (auto& Key : m_HotKeys)
    {
        if (CheckHotKeyPressed(Key))
        {
            if(Key.m_CallBack)
                Key.m_CallBack();
        }
            
        
    }
}

HRESULT CHotKey_Manager::Register_HotKey(KeyCode eKode, bool bCtrl, bool bShift, bool bAlt, HotKeyCallback Func)
{
    HotKey Data;

    Data.eKeyCode = eKode;
    Data.Ctrl = bCtrl;
    Data.Shift = bShift;
    Data.alt = bAlt;
    Data.m_CallBack = Func;

    m_HotKeys.emplace_back(Data);

    return S_OK;
}

bool CHotKey_Manager::CheckHotKeyPressed(const HotKey& Key)
{
    bool bPressedKey = m_pInputManager->IsKeyHeld(Key.eKeyCode);
    bool bPressedAlt = m_pInputManager->IsKeyHeld(KeyCode::Alt);
    bool bPressedShift = m_pInputManager->IsKeyHeld(KeyCode::LShift);
    bool bCtrl = m_pInputManager->IsKeyHeld(KeyCode::LControl);

    return (bPressedKey && bCtrl) == Key.Ctrl 
        && (bPressedShift == Key.Shift)
        && (bPressedAlt == Key.alt);

    return true;
}

CHotKey_Manager* CHotKey_Manager::Create()
{

    return new CHotKey_Manager;
}

void CHotKey_Manager::Free()
{
    Safe_Release(m_pInputManager);
}
