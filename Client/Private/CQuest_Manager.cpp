#include "CQuest_Manager.h"
#include "CAction_Condition.h"
#include "CLocation_Condition.h"
#include "ItemCollect_Condition.h"
#include "CGameManager.h"
#include "CVariable_Condition.h"

#include "CPlayer.h"
#include <iostream>
#include <sstream>

USING(Client)

IMPLEMENT_SINGLETON(CQuest_Manager)

HRESULT CQuest_Manager::Initialize()
{
	m_Facts.resize((size_t)QuestFact::END);
	m_Facts[ENUM_TO_UINT(QuestFact::SAVE_CHILD)] = false;



	LoadQuestDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Quests/Quests.json");

	//size_t Mom_HashKey = hash<string>()("NPC_Mom");
	//m_mapNPCChapterProgress[Mom_HashKey].BaseChapterID = "Q1001_START";		//대화시작챕터:START
	//m_mapNPCChapterProgress[Mom_HashKey].CurrentChapterID = m_mapNPCChapterProgress[Mom_HashKey].BaseChapterID;		//대화시작챕터:START

	
	size_t Marin_HashKey = hash<string>()("NPC_Marin");
	m_mapNPCChapterProgress[Marin_HashKey].BaseChapterID = "Q1001_START";		//대화시작챕터:START
	m_mapNPCChapterProgress[Marin_HashKey].CurrentChapterID = m_mapNPCChapterProgress[Marin_HashKey].BaseChapterID;		//대화시작챕터:START

	size_t Tarin_HashKey = hash<string>()("NPC_Tarin");
	m_mapNPCChapterProgress[Tarin_HashKey].BaseChapterID = "Tarin_START";		//대화시작챕터:START
	m_mapNPCChapterProgress[Tarin_HashKey].CurrentChapterID = m_mapNPCChapterProgress[Tarin_HashKey].BaseChapterID;		//대화시작챕터:START


	size_t Mom_HashKey = hash<string>()("NPC_Mom");
	m_mapNPCChapterProgress[Mom_HashKey].BaseChapterID = "Q1002_START";		//대화시작챕터:START
	m_mapNPCChapterProgress[Mom_HashKey].CurrentChapterID = m_mapNPCChapterProgress[Mom_HashKey].BaseChapterID;		//대화시작챕터:START


	size_t KidGreen_HashKey = hash<string>()("NPC_Kid_Green");
	m_mapNPCChapterProgress[KidGreen_HashKey].BaseChapterID = "Q1002_START";		//대화시작챕터:START
	m_mapNPCChapterProgress[KidGreen_HashKey].CurrentChapterID = m_mapNPCChapterProgress[KidGreen_HashKey].BaseChapterID;		//대화시작챕터:START

	size_t KidBlue_HashKey = hash<string>()("NPC_Kid_Blue");
	m_mapNPCChapterProgress[KidBlue_HashKey].BaseChapterID = "Q1002_ACTIVE_PERM";		//대화시작챕터:START
	m_mapNPCChapterProgress[KidBlue_HashKey].CurrentChapterID = m_mapNPCChapterProgress[KidBlue_HashKey].BaseChapterID;		//대화시작챕터:START


	size_t Fairy_HashKey = hash<string>()("NPC_Fairy");
	m_mapNPCChapterProgress[Fairy_HashKey].BaseChapterID = "Q1003_START";		//대화시작챕터:START
	m_mapNPCChapterProgress[Fairy_HashKey].CurrentChapterID = m_mapNPCChapterProgress[Fairy_HashKey].BaseChapterID;		//대화시작챕터:START


	size_t Witch_HashKey = hash<string>()("NPC_Witch");
	m_mapNPCChapterProgress[Witch_HashKey].BaseChapterID = "Witch_start";		//대화시작챕터:START
	m_mapNPCChapterProgress[Witch_HashKey].CurrentChapterID = m_mapNPCChapterProgress[Witch_HashKey].BaseChapterID;		//대화시작챕터:START

	return S_OK;
}

void CQuest_Manager::Update(_float fTimeDelta)
{
	//매프레임마다 검사
	for (const _uint iQuestID : m_ActiveQuests)
	{
		auto it = m_mapQuestDataBase.find(iQuestID);
		if (it == m_mapQuestDataBase.end())
			continue;

		QuestData& data = it->second;
		if (data.m_eState != QuestState::ACTIVE)
			continue;

		bool bAllMet = true;

		bool b = true;
		// 모든 조건 객체의 IsSatisfied()를 Polling
		for (auto& pCond : data.conditions)
		{
			//이게만족된거면,, OCMPLETE로벼 뱐걍
			if (!pCond->IsSatisfied())
			{
				bAllMet = false;
				break;

			}

		}
		if (bAllMet)
			data.m_eState = QuestState::COMPLETABLE;

	}

}




void CQuest_Manager::Start_Quest(_uint iQuestID)
{
	auto it = m_mapQuestDataBase.find(iQuestID);
	CheckTrue(it == m_mapQuestDataBase.end());
	CheckTrue(m_mapQuestDataBase.empty());
	CheckTrue(it->second.m_eState != QuestState::NONE);


	it->second.m_eState = QuestState::ACTIVE;
	m_ActiveQuests.push_back(iQuestID);

}

void CQuest_Manager::Complete_Quest(_uint iQuestID)
{
	auto iter = m_mapQuestDataBase.find(iQuestID);
	CheckTrue(iter == m_mapQuestDataBase.end());
	CheckTrue(m_mapQuestDataBase.empty());
	CheckTrue(iter->second.m_eState == QuestState::COMPLETED);

	QuestData& data = iter->second;

	// 보상 실행 (Reward 함수 호출)
	if (data.reward)
	{
		data.reward();
	}

	//퀘스트 상태를 COMPLETED로 영구 변경
	data.m_eState = QuestState::COMPLETED;

	//  Active 목록 (m_ActiveQuests)에서 ID 제거
	m_ActiveQuests.erase(std::remove(m_ActiveQuests.begin(), m_ActiveQuests.end(), iQuestID), m_ActiveQuests.end());

	// 4. 완료 기록 (중복 방지)
	m_CompletedQuestIDs.push_back(iQuestID);
}

void CQuest_Manager::Free()
{
	__super::Free();

	for (auto& pair : m_mapQuestDataBase)
	{

		for (auto& cond : pair.second.conditions)
			delete cond;

	}

}

QuestState CQuest_Manager::Get_QuestState(_uint iQuestID)
{
	auto it = m_mapQuestDataBase.find(iQuestID);
	if (it == m_mapQuestDataBase.end()) return QuestState::NONE;

	// 데이터베이스 객체가 가진 m_eState를 바로 반환
	return it->second.m_eState;
}

string CQuest_Manager::Get_NPC_Chapter(const string& strModelID)
{
	size_t Hash = hash<string>()(strModelID);

	CheckTrueResult(m_mapNPCChapterProgress.count(Hash) == 0, "Q1001_START");


	return m_mapNPCChapterProgress.at(Hash).CurrentChapterID;
}

void CQuest_Manager::Set_NPC_Chapter(const string& strModelID, const string& strChaperID)
{
	size_t HashKey = hash<string>()(strModelID);
	m_mapNPCChapterProgress[HashKey].CurrentChapterID = strChaperID;

	//세이브할 시 여기 세ㅐ이브해야함.
}


string CQuest_Manager::Get_Optimal_ChapterID(const string& ModelID)
{
	size_t HashKey = hash<string>()(ModelID);
	_uint iTrackingQuestID = 0;

	CheckTrueResult(m_mapNPCChapterProgress.find(HashKey) == m_mapNPCChapterProgress.end(),"");

	string strBaseChapterID = m_mapNPCChapterProgress[HashKey].BaseChapterID;

	//퀘스트번호추출
	if (strBaseChapterID.length() >= 5 && strBaseChapterID[0] == 'Q')
	{
		iTrackingQuestID = std::stoul(strBaseChapterID.substr(1, 4));
	}


	if (iTrackingQuestID == 0)
		return Get_NPC_Chapter(ModelID);

	//퀘스트 상태확인
	QuestState state = Get_QuestState(iTrackingQuestID);
	if (state == QuestState::ACTIVE)
	{
		return "Q" + to_string(iTrackingQuestID) + "_ACTIVE_PERM";
	}

	else if (state == QuestState::COMPLETABLE)
	{
		return "Q" + to_string(iTrackingQuestID) + "_COMPLETABLE_READY";
	}

	else if (state == QuestState::COMPLETED)
		return "Q" + std::to_string(iTrackingQuestID) + "_DONE";

	else
		return Get_NPC_Chapter(ModelID);

	// 퀘스트가 ACTIVE 상태라면 strBaseChapterID (예: Q1001_ACTIVE_PERM) 그대로 반환
	// 퀘스트가 NONE 상태이고 strBaseChapterID가 Q[번호]_START라면 그대로 반환
	return strBaseChapterID;
}

bool CQuest_Manager::Get_FactCheckValue(QuestFact eType)
{
	return m_Facts[ENUM_TO_UINT(eType)];

}

void CQuest_Manager::Set_FactCheckValue(QuestFact eType, bool bValue)
{
	m_Facts[ENUM_TO_UINT(eType)] = bValue;

}

std::function<void()>    CQuest_Manager::Make_Reward_Function(_uint iQuestID)
{
	switch (iQuestID)
	{
	case 1001:
		return[]()
		{
			OutputDebugString(L"Make_Reward_By_1001");
		};

		break;

	case 1002:
		return[]()
		{
			OutputDebugString(L"Make_Reward_By_1002");
		};

		break;
	}

	return nullptr;
}


void CQuest_Manager::LoadQuestDatabase(const string& strFilePath)
{

	ifstream file(strFilePath);
	if (!file.is_open())
		return;

	json json_data;

	try {
		// [최종 점검] 파일 내용을 문자열로 강제 로딩 후 파싱
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string json_content = buffer.str();

		if (json_content.empty())
		{
			std::cerr << "FILE IS EMPTY or UNREADABLE: " << strFilePath << std::endl;
			return;
		}

		json_data = json::parse(json_content);
	}
	catch (const nlohmann::json::parse_error& e)
	{
		std::cerr << "QUEST JSON PARSE ERROR! File: " << strFilePath
			<< "\nReason: " << e.what()
			<< "\nByte Index: " << e.byte << std::endl;
		return;
	}

	for (const auto& json_quest : json_data.at("Quests"))
	{
		QuestData data;
		data.iQuestID = json_quest.at("iQuestID").get<int>();
		string str = json_quest.at("strTitle");
		
		data.strTitle = StringToWString(str);

		string Desc= json_quest.at("strDescription").get<string>();
		data.strDescription = StringToWString(Desc);

		//조건객체 생성 및 등록
		if (json_quest.contains("Conditions") && json_quest.at("Conditions").is_array())
		{
			const json& jConditions = json_quest.at("Conditions");
			for (auto const& jCond : jConditions)
			{
				CQuestCondition* pCondition = Make_Condition_From_Json(jCond);
				if (pCondition)
					data.conditions.push_back(pCondition);
			}
		}

		data.reward = Make_Reward_Function(data.iQuestID);

		

		m_mapQuestDataBase.emplace(data.iQuestID, data);
	}
}

CQuestCondition* CQuest_Manager::Make_Condition_From_Json(const json& json_cond)
{
	string strType = json_cond.at("Type").get<string>();

	if (strType == "COLLET_ITEM")
	{
		string strtemType = json_cond.at("ItemType").get<string>();

		ItemType itemType;
		if (strtemType == "Mushroom")
			itemType = ItemType::MUSHROOM;

		int count = json_cond.at("Count").get<int>();
		return new ItemCollect_Condition(itemType, count);

	}

	else if (strType == "ACTION")
	{
		string State = json_cond.at("State").get<string>();
		CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();

		if (State == "CARRY")
		{
			return new CAction_Condition(pPlayer,ENUM_TO_UINT(CPlayer::PLAYER_STATE::CARRY));
		}
	}

	else if (strType == "LOCATION")
	{
		string State = json_cond.at("State").get<string>();
		CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();

		if (State == "GO_TOWN")
		{
			return new CLocation_Condition(ENUM_TO_UINT(LEVEL_ID::TOWN));
		}
	}

	else if (strType == "VARIABLE")
	{
		string State = json_cond.at("State").get<string>();
		CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();

		if (State == "SAVE_CHILD")
		{
			return new CVariable_Condition(QuestFact::SAVE_CHILD,true);
		}
	}
	return nullptr;
}
