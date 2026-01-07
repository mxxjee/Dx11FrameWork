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
                    

	enum class LEVEL_ID {STATIC,LOGO,LOADING,SPAWN,TOWN,ROOM,DUNGEON,BOSS,ENDING,UI,MAPTOOL,END};
	enum class COLLISION_GROUP{
		PLAYER,PLAYER_WEAPON,
		MONSTER,MONSTER_WEAPON,
		INTERACTION,
		TRIGGER,
		OBJECT,
		PARTICLE,
		WALL,
		END};
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


	bool				m_bFall = false;

	//이건 리셋X
	float				m_fDamage = 0.0f;

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


typedef struct tagMonsterActionControl
{
	bool	m_bIntroEnd = false;

	//////////////////////
	float	m_bDamage = 0.f;	
	bool	m_bMove = false;		//true라면 walk, false람녀 idle/ range&&move라면 attack가능
	
	bool	m_bAttack = false;

	bool	m_bRange = false;		//true라면 detect		//공격범위내에있는지
	bool	m_bChase = false;

	bool	m_bThink = false;			//true라면 잠깐 idle상태의 대기상태로진입..
	
	bool	m_bDead = false;	//죽음처리됐는지( 애니메이션 호출용)
	
	void Reset()
	{
		m_bIntroEnd = false;
		m_bDamage = 0.f;
		m_bMove = false;

		m_bAttack = false;

		m_bRange = false;
		m_bChase = false;
	}

}MONSTER_ACTION_CONTORL;

//////////Player Input관련////////////////


/////////아이템정보/////////
enum class ItemType
{
	SHIELD,//방패
	SWROD,//칼
	MUSHROOM ,	//숲에서발견한 버섯
	LETTER,	//아이가 남기고간 쪽지
	FEATHER,	//깃털
	POWER_BRACELET,//파워팔찌
	MAGIC_POWDER,//마녀한테 받은가루
	END

};

typedef struct tagItemInfo
{
	ItemType	ItemType = ItemType::END;
	wstring		ItemDesc = L"";	//아이템설명
	wstring		InfoDesc = L"";		//인벤에 표시할 아이템설명
	wstring		TexKey = L"";		//아이템 UI 이름
	int			PlayerState = -1;
	bool		m_bQuick=false;	//퀵슬롯에 장착가능한지.
	std::function<void()>        m_AfterGetFunc = nullptr;       //얻고나서 함수실행할거
	bool				m_bCanUse = true;


}ITMINFO;


struct InvenSlot
{
	ItemType			ItemType = ItemType::END;
	int					count = 0;
	KeyCode					m_eQuickKeyCode = KeyCode::End;
	ITMINFO*			m_pItemInfo = nullptr;		//원본아이템 info

	bool				m_bUsedInUI = false;
};

struct stringID
{
	unsigned int m_Hash;//실제 키 해시값

#ifdef _DEBUG
	wstring		m_DebugName;
#endif

	stringID() {};
	stringID(wstring ModelName)
	{
		m_Hash = (unsigned int)hash<wstring>()(ModelName);

	}
};


/*각 숫자는 우선순위를의미한다*/
enum InteractionType
{
	OBJECT=10,
	NPC=100,
	END
};

enum Interact_Object_Type
{
	CAVEROCK,
	ROCK,
	LAWN,
	GRASS,
	ITEM,
	BIGROCK,
	IRONBALL
};

enum Interact_Behavior_Type
{
	NONE,
	ITEMABLE,
	PUSHABLE,
	CARRYABLE,
	Interact_Behavior_Type_END
};

/////Quest관련///////
//각 퀘스트에 대한 플레이어 진행상태
enum class QuestState : _uint
{
	NONE = 0,//퀘스트없음
	ACTIVE = 1,	//진행중
	COMPLETABLE = 2,		//완료가능(NPC에게 전달)
	COMPLETED = 3			//완료됨(보상까지받고난 완벽한 완료)


};

enum class QuestType {COLLECT_ITEM, KILL_MONSTER,VISIT_LOCATION,ACTION};


//퀘스트에 필요한 오브젝트 관련 데이터 정의
//만약 오브젝트의 종류를 여러개 가져오는 퀘스트일경우,
//오브젝트 당 하나로 이걸 정의



////////NPC_Dialogues/////////
enum class Dialog_Action_Timing { ON_START, ON_FINISH };		//대화할떄 이벤트 시작 타이밍(대화 나오고 바로시작할건지, 대화끝나고 시작할건지)




//1. DialogueStep->UI에 표시할 하나의 블럭
struct DialogueStep
{
	wstring strSpeaker = L"";//대화의 주체(모델이름 혹은 태그)
	wstring strText = L"";		//UI에 표시할 텍스트

	Dialog_Action_Timing actionTiming= Dialog_Action_Timing::ON_START;
	string strActionCommand = "";		//내릴 명령(이벤트 이름, 이벤트버스 통해처리)

};

////2. 챕터단위
struct DialogueChapter
{
	string ChapterID="";
	vector<DialogueStep>		steps;			//순차적으로 진행될 대화블럭

	_uint	iQuestOffered = 0;			//이 챕터 이후 제공할 퀘스트번호 (어떤퀘스트인지?)
	_uint	iQuestRequiredToUnlock = 0;			//(완료판단)다음 영구챕터로 넘어갈 조건ㄱ퀘스트 ID? 영구챕터 : 다음에 말을 걸었을때 시작할 챕터번호

	size_t iCurrentStepIndex = 0;				//현재 이 챕터에서 실행되고잇는 dialogeustep[idx]

	string strNextPermanentChapterID="";			//퀘스트 넘어가면 수행할 다음 챕터
};


//한 NPC의 모든 챕터데이터
struct NPC_DialogueScript
{
	string strModelID;
	UMap<string, DialogueChapter>		Chapters;			//Key :Chaptername

};

//////////////////RoomPackage///////
namespace Engine
{
	class CGameObject;
}
struct RoomPackage
{
	string			m_RoomName = "";

	list<Engine::CGameObject*> EnvObjs;		//맵 오브젝트들
	list<Engine::CGameObject*>	 NPCs;
	list<Engine::CGameObject*>	Triggers;
	list<Engine::CGameObject*>	Interactions;

	string			m_navMeshFile = "";
	_float3			m_vPlayerSpawnPoin=_float3(0.f,0.f,0.f);


	RoomPackage() {};

	~RoomPackage()
	{
		for (auto& pObj : EnvObjs)
			Safe_Release(pObj);

		for (auto& pObj : NPCs)
			Safe_Release(pObj);

		for (auto& pObj : Triggers)
			Safe_Release(pObj);

		for (auto& pObj : Interactions)
			Safe_Release(pObj);

		EnvObjs.clear();
		NPCs.clear();
		Triggers.clear();
		Interactions.clear();



	}

	void Set_Active(bool b)
	{
		for (auto& pObj : EnvObjs)
			pObj->Set_Active(b);

		for (auto& pObj : NPCs)
			pObj->Set_Active(b);

		for (auto& pObj : Triggers)
			pObj->Set_Active(b);

		for (auto& pObj : Interactions)
			pObj->Set_Active(b);
	}
};
using namespace Client;
using namespace std;


#define		REGISTERLEVEL()