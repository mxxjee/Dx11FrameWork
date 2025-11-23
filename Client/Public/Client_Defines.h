#pragma once

#include <Windows.h>
#include <process.h>
#include <cstring>

#include "SimpleMath/SimpleMath.h"
#include "MathUtils.h"


extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
	static		bool		m_bDrawDebug = true;


	enum class LEVEL_ID {STATIC,LOGO,LOADING,TOWN,UI,MAPTOOL,END};
	enum class RENDERGROUP {PRIORITY,NONALPHA,ALPHA, WORLD_UI_MINIMAP,UI,END};
	//enum class LEV
}

//////////Player Input관련////////////////
typedef struct tagPlayerInput
{
	bool m_bisAttack = false;
	bool m_bisMove = false;
	bool m_bisShieldRelease = false;
	bool m_bisShield = false;
	bool m_bisJump = false;
	bool m_bInteract = false;

}PLAYER_INPUT;


typedef struct tagActionControl
{
	bool                m_bCanAttack = true;
	bool				m_bCanShield = true;

	bool                m_bCanMove = true;
	vector<HOLDKEY_DATA>		m_Holds;
	bool				m_bFixDir = false;

	//////////////////특수한행동들 , 이들은 서로 겹칠 수 없다.//////////
	bool				m_bLadder = false;
	bool				m_bPush = false;
	bool				m_bCarry = false;
	bool				m_bItemGet = false;
	bool				m_bTalk = false;

	void		Reset()
	{
		m_bCanAttack = true;
		m_bCanMove = true;

		for (size_t i = 0; i < m_Holds.size(); ++i)
		{
			m_Holds[i].HOLDKEY_DATA::Reset();
		}
		//m_bHolds = { false,false };
		m_bFixDir = false;
		m_bCanShield = true;
		m_bLadder = false;
		m_bPush = false;
		m_bCarry = false;
		m_bItemGet = false;
		m_bTalk = false;
	}



	bool		IsHold(int idx)
	{
		return m_Holds[idx].m_bHeld;
	}

	void		SetHold(int idx, bool b)
	{
		m_Holds[idx].m_bHeld = b;

	}


}ACTION_CONTROL;


//////////Player Input관련////////////////


/////////아이템정보/////////
typedef struct tagItemInfo
{
	_wstring	ItemTag=L"";
	bool		m_bQuick=false;	//퀵슬롯에 장착가능한지.
	KeyCode		KeyCode=KeyCode::None;		//퀵슬롯에 장착했을경우, 키코드



}ITMINFO;

enum InteractionType
{
	OBJECT,NPC,END
};

using namespace Client;
using namespace std;


#define		REGISTERLEVEL()