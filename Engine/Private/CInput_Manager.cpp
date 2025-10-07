#include "CInput_Manager.h"
#include <tchar.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

CInput_Manager::CInput_Manager()
{
	ZeroMemory(m_LastClickTime, sizeof(m_LastClickTime));
}
CInput_Manager::~CInput_Manager()
{
	Free();
}

HRESULT CInput_Manager::Init_Input(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyboard->Acquire();

	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Manager::Update_Input()
{
	memcpy(m_PrevKeyState, m_CurrKeyState, sizeof(m_CurrKeyState));
	m_PrevMouseRaw = m_CurrMouseRaw;
	m_PrevMousePos = m_MouseState.position;

	if (FAILED(m_pKeyboard->GetDeviceState(sizeof(m_CurrKeyState), m_CurrKeyState)))
		m_pKeyboard->Acquire();

	if (FAILED(m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_CurrMouseRaw)))
		m_pMouse->Acquire();

	UpdateMouseState();
}
POINT CInput_Manager::GetMousePos() const
{
	return m_MouseState.position;
}

_float2 CInput_Manager::GetMousePosF() const
{

	return _float2(static_cast<float>(m_MouseState.position.x),
		static_cast<float>(m_MouseState.position.y));

}
POINT CInput_Manager::GetMouseDelta() const
{
	POINT pt;
	pt.x = static_cast<long>(m_CurrMouseRaw.lX);
	pt.y = static_cast<long>(m_CurrMouseRaw.lY);
	return pt;
}
void CInput_Manager::UpdateMouseState()
{
	m_MouseState.leftButton = (m_CurrMouseRaw.rgbButtons[0] & 0x80) != 0;
	m_MouseState.rightButton = (m_CurrMouseRaw.rgbButtons[1] & 0x80) != 0;
	m_MouseState.middleButton = (m_CurrMouseRaw.rgbButtons[2] & 0x80) != 0;
	m_MouseState.wheelDelta = m_CurrMouseRaw.lZ;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	m_MouseState.position = pt;
}

CInput_Manager* CInput_Manager::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Manager* pInstance = new CInput_Manager();

	if (FAILED(pInstance->Init_Input(hInst, hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

EInputState CInput_Manager::GetKeyState(KeyCode key) const
{
	BYTE code = static_cast<BYTE>(key);
	bool now = (m_CurrKeyState[code] & 0x80) != 0;
	bool prev = (m_PrevKeyState[code] & 0x80) != 0;

	if (now && !prev)
		return EInputState::Pressed;
	if (now && prev)
		return EInputState::Held;
	if (!now && prev)
		return EInputState::Released;
	return EInputState::None;
}

bool CInput_Manager::IsKeyPressed(KeyCode key) const { return GetKeyState(key) == EInputState::Pressed; }
bool CInput_Manager::IsKeyHeld(KeyCode key) const { return GetKeyState(key) == EInputState::Held; }
bool CInput_Manager::IsKeyReleased(KeyCode key) const { return GetKeyState(key) == EInputState::Released; }

bool CInput_Manager::IsMouseButtonPressed(int button) const
{
	return (m_CurrMouseRaw.rgbButtons[button] & 0x80) && !(m_PrevMouseRaw.rgbButtons[button] & 0x80);
}

bool CInput_Manager::IsMouseButtonHeld(int button) const
{
	return (m_CurrMouseRaw.rgbButtons[button] & 0x80) && (m_PrevMouseRaw.rgbButtons[button] & 0x80);
}

bool CInput_Manager::IsMouseButtonReleased(int button) const
{
	return !(m_CurrMouseRaw.rgbButtons[button] & 0x80) && (m_PrevMouseRaw.rgbButtons[button] & 0x80);
}

bool CInput_Manager::IsMouseDragging() const
{
	return m_MouseState.leftButton && (m_PrevMousePos.x != m_MouseState.position.x || m_PrevMousePos.y != m_MouseState.position.y);
}

bool CInput_Manager::IsMouseMoved() const
{
	return m_PrevMousePos.x != m_MouseState.position.x || m_PrevMousePos.y != m_MouseState.position.y;
}

bool CInput_Manager::IsDoubleClick(int button) const
{
	DWORD currTime = timeGetTime();
	bool pressed = IsMouseButtonPressed(button);
	//250m/s
	if (pressed && (currTime - m_LastClickTime[button]) < 250)
	{
		return true;
	}
	if (pressed)
	{
		if (button >= 0 && button < 3)
			m_LastClickTime[button] = currTime;
	}
	return false;
}

void CInput_Manager::BindKey(const string& actionName, KeyCode key)
{
	m_KeyBindings[actionName] = key;
}

bool CInput_Manager::IsActionPressed(const string& actionName) const
{
	auto it = m_KeyBindings.find(actionName);
	if (it == m_KeyBindings.end())
		return false;
	return IsKeyPressed(it->second);
}

bool CInput_Manager::IsActionHeld(const string& actionName) const
{
	auto it = m_KeyBindings.find(actionName);
	if (it == m_KeyBindings.end())
		return false;
	return IsKeyHeld(it->second);
}

bool CInput_Manager::IsActionReleased(const string& actionName) const
{
	auto it = m_KeyBindings.find(actionName);
	if (it == m_KeyBindings.end())
		return false;
	return IsKeyReleased(it->second);
}

void CInput_Manager::Free()
{
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
		m_pKeyboard = nullptr;
	}

	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
		m_pMouse = nullptr;
	}

	if (m_pInput)
	{
		m_pInput->Release();
		m_pInput = nullptr;
	}
}