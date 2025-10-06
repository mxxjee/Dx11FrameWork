#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CInput_Manager
	:public CBase
{
private:
	explicit CInput_Manager();
	virtual ~CInput_Manager();

public:
	//인풋매니저 장치초기화
	HRESULT Init_Input(HINSTANCE hInst, HWND hWnd);
	//업데이트용도
	void Update_Input();
	//특정한 키 상태조회(눌렷는지?!!)
	EInputState GetKeyState(KeyCode key) const;
	//키가 눌렷는지 확인
	bool IsKeyPressed(KeyCode key) const;
	bool IsKeyHeld(KeyCode key) const;
	bool IsKeyReleased(KeyCode key) const;
	//마우스 버튼 상태확인
	bool IsMouseButtonPressed(int button) const;
	bool IsMouseButtonReleased(int button) const;
	bool IsMouseButtonHeld(int button) const;
	//드래그중?
	bool IsMouseDragging() const;
	bool IsMouseMoved() const;
	//마우스 더블클릭여부
	bool IsDoubleClick(int button) const;
	//마우스 상태를넘겨줌
	const MouseState& GetMouseState() const { return m_MouseState; }
	//키바인딩(꾹 누르고있는거를 의미함)
	void BindKey(const string& actionName, KeyCode key);
	bool IsActionPressed(const string& actionName) const;
	bool IsActionHeld(const string& actionName) const;
	bool IsActionReleased(const string& actionName) const;

	POINT GetMousePos() const;
	_float2 GetMousePosF() const;
private:
	void Free() override;
	void UpdateMouseState();
	
public:
	static CInput_Manager* Create(HINSTANCE hInst, HWND hWnd);
private:
	//다이렉트 인풋 장치 포인터
	LPDIRECTINPUT8			m_pInput = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;
	//키보드의 현재입력과 이전입력상태저장을 위함
	BYTE m_CurrKeyState[256] = {};
	BYTE m_PrevKeyState[256] = {};
	//마우스의 현재입력과 이전입력상태저장을 위함
	DIMOUSESTATE m_CurrMouseRaw{};
	DIMOUSESTATE m_PrevMouseRaw{};
	//마우스 상태
	MouseState m_MouseState{};

	POINT m_PrevMousePos{};
	mutable DWORD m_LastClickTime[3] = {};

	UMap<string, KeyCode> m_KeyBindings;
	HWND m_hWnd = nullptr;
};
NS_END

