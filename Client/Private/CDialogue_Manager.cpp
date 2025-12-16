#include "CDialogue_Manager.h"
#include "CQuest_Manager.h"
#include <iostream>
#include "CGameInstance.h"

IMPLEMENT_SINGLETON(CDialogue_Manager)

USING(Client)

HRESULT CDialogue_Manager::Initialize()
{
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Mom_Script.json");
    return S_OK;
}

void CDialogue_Manager::StartDialogue(string strModelID)
{
    /*이미대화중인지 체크 */
    CheckTrue(m_pActiveChapter != nullptr);

    
    size_t HashKey = hash<string>()(strModelID);

    string strChapterID = CQuest_Manager::GetInstance()->Get_Optimal_ChapterID(strModelID);

    if (m_mapScripts.count(HashKey) && m_mapScripts.at(HashKey).Chapters.count(strChapterID))
    {
        m_pActiveChapter = &m_mapScripts.at(HashKey).Chapters.at(strChapterID);
        m_strActiveNPC = strModelID;

        //인덱스초기화
        m_pActiveChapter->iCurrentStepIndex = 0;

        CGameInstance::GetInstance()->BroadCastEvent(L"OnDialogueUIShow", nullptr);

        //첫번째문장실행
        AdvanceDialogueStep();

    }
}

const DialogueChapter* CDialogue_Manager::Get_ChapterDefinition(const string& strModelID, const string& strCurrentChapterID)
{
    size_t HashKey = hash<string>()(strModelID);

    //해시키가존재하지 않으면 return
    CheckTrueResult(m_mapScripts.count(HashKey) == 0,nullptr);


    const auto& npcScript = m_mapScripts.at(HashKey);
    CheckTrueResult(npcScript.Chapters.empty(), nullptr);


    return &npcScript.Chapters.at(strCurrentChapterID);
}

void CDialogue_Manager::TryAdvanceChapter(const string& ModelID, const string& strCurrentChapterID)
{
    CQuest_Manager* pQM = CQuest_Manager::GetInstance();
    const DialogueChapter* pChapter = Get_ChapterDefinition(ModelID, strCurrentChapterID);

    //챕터에맞는 퀘스트수행
    CheckNull(pChapter);

    _uint iOfferedQuest = pChapter->iQuestOffered;
    _uint iRequiredQuest = pChapter->iQuestRequiredToUnlock;

    // CASE 1: 퀘스트 시작 (NONE이면 바로 넘어가게)
    if (iOfferedQuest != 0 && pQM->Get_QuestState(iOfferedQuest) == QuestState::NONE)
    {
        pQM->Start_Quest(iOfferedQuest);
        if (!pChapter->strNextPermanentChapterID.empty())
        {
            pQM->Set_NPC_Chapter(ModelID, pChapter->strNextPermanentChapterID);
        }
    }

    // CASE 2: 퀘스트 완료
    if (iRequiredQuest != 0 && pQM->Get_QuestState(iRequiredQuest) == QuestState::COMPLETABLE)
    {
        pQM->Complete_Quest(iRequiredQuest);
        if (!pChapter->strNextPermanentChapterID.empty())
        {
            pQM->Set_NPC_Chapter(ModelID, pChapter->strNextPermanentChapterID);
        }
    }

    
}




bool CDialogue_Manager::AdvanceDialogueStep()
{
    CheckNullResult(m_pActiveChapter, false);

    DialogueChapter* pChapter = m_pActiveChapter;
    size_t iTotalSteps = pChapter->steps.size();

    //한번더 체크해서 끝내기
    if (pChapter->iCurrentStepIndex > 0)
    {
        const DialogueStep& prevStep = pChapter->steps[pChapter->iCurrentStepIndex - 1];

        if(prevStep.actionTiming==Dialog_Action_Timing::ON_FINISH)
            ExecuteActionCommand(prevStep.strActionCommand, prevStep.actionTiming);
    }


    //챕터종료 확인하기
    if (pChapter->iCurrentStepIndex >= iTotalSteps)
    {
    
        TryAdvanceChapter(m_strActiveNPC, pChapter->ChapterID);

        m_pActiveChapter = nullptr;
        m_strActiveNPC = "";
        return false;//대화 종료.
    }

    //각 문장마다 action설정한거 실행해주기
    const DialogueStep& CurrentStep = pChapter->steps[pChapter->iCurrentStepIndex];
    
    
    if (CurrentStep.actionTiming == Dialog_Action_Timing::ON_START)
        ExecuteActionCommand(CurrentStep.strActionCommand, CurrentStep.actionTiming); 
    
    m_CurrentWstr = CurrentStep.strText;
    CGameInstance::GetInstance()->BroadCastEvent(L"UpdateNPCText", &m_CurrentWstr);


    pChapter->iCurrentStepIndex++;

    return true;        //대화진행중
}

void CDialogue_Manager::ExecuteActionCommand(const std::string& strCommand, const Dialog_Action_Timing& strTiming)
{
    if (strCommand.empty())
        return;


}

void CDialogue_Manager::LoadScriptDatabase(const string& strFilePath)
{
    ifstream file(strFilePath);
    if (!file.is_open())
    {
        MSG_BOX("Failed to Open");
        return;
    }

    json json_data;
    try {
        // file >> json_data; 사용
        file >> json_data;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        // 정확한 오류 메시지를 출력하여 원인을 파악합니다.
        // e.what(): 오류 메시지, e.byte: 오류 발생 위치 (바이트 인덱스)
        std::cerr << "JSON PARSE ERROR! File: " << strFilePath
            << "\nReason: " << e.what()
            << "\nByte Index: " << e.byte << std::endl;

        // 프로그램 실행 중단
        // JSON_THROW(e); // 이전에 사용된 매크로처럼 예외를 다시 던지거나,
        // return;         // 함수를 종료합니다.

        return;
    }

    CheckTrue(!json_data.contains("NPCScripts") || !json_data["NPCScripts"].is_array());

    for (const auto& jScript : json_data.at("NPCScripts"))
    {
        //NPC스크립트 순회
  
       NPC_DialogueScript script;
       script.strModelID = jScript.at("ModelID").get<string>();
       
       //모든챕터가져오기
       for (const auto& jChap : jScript.at("Chapters"))
       {
          DialogueChapter chap;
           ParseDialogueChapter(jChap, chap);
           script.Chapters[chap.ChapterID] = chap;
       }
       
       size_t HashKey = hash<string>()(script.strModelID);
       
       m_mapScripts[HashKey] = script;
        
    }
    



}


void CDialogue_Manager::ParseDialogueChapter(const json& jChap, DialogueChapter& chap)
{
    chap.ChapterID = jChap.at("ChapterID").get<string>();

    //퀘스트 연동 필드 및 다음 챕터 필드 파싱
    if (jChap.contains("iQuestOffered"))
        chap.iQuestOffered = jChap.at("iQuestOffered").get<_uint>();

    if (jChap.contains("iQuestRequiredToUnlock"))
        chap.iQuestRequiredToUnlock = jChap.at("iQuestRequiredToUnlock").get<_uint>();

    if (jChap.contains("strNextPermanentChapterID"))
        chap.strNextPermanentChapterID = jChap.at("strNextPermanentChapterID").get<string>();


    //
    if (jChap.contains("Steps") && jChap.at("Steps").is_array())
    {
        for (const auto& jStep : jChap.at("Steps"))
        {
            DialogueStep step;
            step.strSpeaker = jStep.at("Speaker").get<string>();

            string strText = jStep.at("Text").get<string>();
            step.strText = StringToWString(strText);

            // 액션 커맨드는 필수가 아니므로 contains로 확인
            if (jStep.contains("strActionCommand"))
                step.strActionCommand = jStep.at("strActionCommand").get<string>();


            if (jStep.contains("actionTiming"))
            {
                string stractionTiming= jStep.at("actionTiming").get<string>();
                if (stractionTiming == "ON_FINISH")
                    step.actionTiming = Dialog_Action_Timing::ON_FINISH;

                else
                    step.actionTiming = Dialog_Action_Timing::ON_START;

            }
                

            chap.steps.push_back(step);
        }
    }
}
