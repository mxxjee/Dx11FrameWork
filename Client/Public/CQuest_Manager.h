#pragma once
#include "CBase.h"
#include "Client_Defines.h"
#include "CDialogue_Manager.h"
#include "CQuestCondition.h"

NS_BEGIN(Client)

class CQuest_Manager :
    public CBase
{
public:
    struct BaseData
    {
        string BaseChapterID = "";
        string CurrentChapterID = "";
    };

    struct QuestData
    {
        _uint iQuestID;
        wstring strTitle = L"";	//퀘스트타이틀(UI표시)
        wstring	strDescription;	//퀘스트 설명


        vector<class CQuestCondition*> conditions;		//퀘스트를 완료하기 위한 조건

        function<void()>	   reward = nullptr;		//reward로 수행할 함수
        QuestState			m_eState = QuestState::NONE;		//퀘스트 진행상황
    };

    DECLARE_SINGLETON(CQuest_Manager)
private:
    explicit CQuest_Manager() {};
    virtual ~CQuest_Manager() = default;

public:
            //데이터 로드 및 NPC각 각 시작점설정
    HRESULT         Initialize();
    void            Update(_float fTimeDelta);//매프레임마다 퀘스트검사확인

private:
    //Json으로 작성한 퀘스트정보 등록
    void    LoadQuestDatabase(const string& strFilePath);
    CQuestCondition* Make_Condition_From_Json(const json& json_cond);

public:
                //매개변수로들어온 퀘스트아디에 대한 플레이어의 퀘스트상태반환
    QuestState Get_QuestState(_uint iQuestID);

                    //NPC가 퀘스트를 내릴때, 이거 호출(Active목록에넣기)
    void        Start_Quest(_uint iQuestID);
                //퀘스트를 완료처리한다.
    void        Complete_Quest(_uint iQuestID);

    void        Free();
  //  void        Completable_Quest(_uint iQuestID);

    //퀘스트를 완료처리한다.
   // void        Complete_Quest(_uint iQuestID);

public:
    ///////////About NPC///////////////////////
                    //NPC의 영구챕터진행 ID반환
    string Get_NPC_Chapter(const string& strModelID);

            //NPC의 영구챕터진행도 갱신
    void    Set_NPC_Chapter(const string& strModelID, const string& strChaperID);



                //NPC의 영구챕터와 퀘스트 상태를 기반으로 현재 보여줄 챕터 ID반환
    string  Get_Optimal_ChapterID(const string& ModelID);


private:
    std::function<void()>        Make_Reward_Function(_uint iQuestID);
private:

                    //매프레임마다 검사할 quests인덱스만 기록
    vector<_uint>       m_ActiveQuests;


                    //모든 퀘스트정보
    UMap<_uint, QuestData>  m_mapQuestDataBase;

                //이미완료된 퀘스트들의 idx(중복방지)
    vector<_uint>       m_CompletedQuestIDs;      

                //각 NPC에 대한 영구 챕터 진행도 저장
    UMap<size_t, BaseData>    m_mapNPCChapterProgress;
};

NS_END