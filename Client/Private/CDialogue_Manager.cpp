#include "CDialogue_Manager.h"
#include "CQuest_Manager.h"
#include <iostream>
#include "CGameInstance.h"
#include "CEventBus_Manager.h"
#include "CPlayer.h"
#include "CGameManager.h"
#include "CLevel_Spawn.h"
#include "CInventory_Manager.h"
#include "CFadeScreen.h"
#include "CLevel_Town.h"
#include "CNPC.h"
#include "CNPC_KidRed.h"


IMPLEMENT_SINGLETON(CDialogue_Manager)

USING(Client)

HRESULT CDialogue_Manager::Initialize()
{
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Mom_Script.json");
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Marin_Script.json");
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Tarin_Script.json");
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Kid_Green_Script.json");
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Kid_Blue_Script.json");
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Fairy_Script.json");
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Witch_Script.json");
    LoadScriptDatabase("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Scripts/Kid_Red_Script.json");


    
    m_pGameInstance = CGameInstance::GetInstance();

    Register_EventListner();

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


    //CASE 3: 퀘스트없이 그냥 넘어가는대화
    if (iOfferedQuest == 0 && iRequiredQuest == 0)
    {
        if (!pChapter->strNextPermanentChapterID.empty())
            pQM->Set_NPC_Chapter(ModelID, pChapter->strNextPermanentChapterID);
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
    m_CurrentSpeaker = CurrentStep.strSpeaker;
    if (CurrentStep.IsHaveSound)
        m_pGameInstance->PlaySoundW(CurrentStep.SoundFile, CHANNELID::SOUND_NPC_SFX1, g_VoiceVolume);

    
    CGameInstance::GetInstance()->BroadCastEvent(L"UpdateNPCText", &m_CurrentWstr);
    CGameInstance::GetInstance()->BroadCastEvent(L"UpdateNPCName", &m_CurrentSpeaker);


    pChapter->iCurrentStepIndex++;

    return true;        //대화진행중
}

void CDialogue_Manager::Register_EventListner()
{
    CheckNull(m_pGameInstance);

    m_pGameInstance->RegisterListners("EndCutScene", [this](const GameEvent& evt)
        {
            CGameObject* pPlayer = static_cast<CGameObject*>(evt.Payload.Ptrs.at("Player"));
            CLevel* pCurrentLevel = m_pGameInstance->Get_CurrentLevel();
            
            CheckNull(pCurrentLevel);
            CLevel_Spawn* pLevel_Spawn = dynamic_cast<CLevel_Spawn*>(pCurrentLevel);
            CheckNull(pLevel_Spawn);

            CPlayer* ppPlayer = dynamic_cast<CPlayer*>(pPlayer);
            CheckNull(ppPlayer);

            ppPlayer->EndCutScene();
            pLevel_Spawn->EndCutScene();


        });

    m_pGameInstance->RegisterListners("On_PlayerHeal", [this](const GameEvent& evt)
        {
            CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
            CheckNull(pPlayer);
       
            pPlayer->On_Heal(5);



        });
}

void CDialogue_Manager::ExecuteActionCommand(const std::string& strCommand, const Dialog_Action_Timing& strTiming)
{
    if (strCommand.empty())
        return;
    CGameObject* pOwner = CGameManager::GetInstance()->Get_MainPlayer();
    if (strCommand == "EndCutScene")
    {
        CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
        GameEvent Event;
        EventPayload payload;
        Event.Payload = payload;

        Event.Payload.Ptrs["Player"] = pPlayer;
        Event.Name = "EndCutScene";

        m_pGameInstance->Emit(Event);

    }

    else if (strCommand == "End_BlueKid")
    {
        CQuest_Manager::GetInstance()->Set_FactCheckValue(CQuest_Manager::QuestFact::SAVE_CHILD, true);


    }

    else if (strCommand == "On_PlayerHeal")
    {
        CQuest_Manager::GetInstance()->Set_FactCheckValue(CQuest_Manager::QuestFact::SAVE_CHILD, true);

        GameEvent Event;
        Event.Name = "On_PlayerHeal";
        CGameInstance::GetInstance()->Emit(Event);

    }

    else if (strCommand == "Fairy_End")
    {
        CQuest_Manager::GetInstance()->Set_FactCheckValue(CQuest_Manager::QuestFact::SAVE_CHILD, true);
        
        GameEvent Event;
        Event.Name = "Fairy_End";

        CGameInstance::GetInstance()->Emit(Event);

    }


    else if (strCommand == "Mom_Finish")
    {
        CQuest_Manager::GetInstance()->Set_FactCheckValue(CQuest_Manager::QuestFact::SAVE_CHILD, true);

        GameEvent Event;
        Event.Name = "Mom_Finish";

        CGameInstance::GetInstance()->Emit(Event);

        CNPC* pMomNPC = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::ROOM), L"NPC_Layer", L"NPC_Mom"));
        CNPC* pDadNPC = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::ROOM), L"NPC_Layer", L"NPC_Dad"));

        if (pMomNPC&& pDadNPC)
        {
            pMomNPC->Set_Expression(CNPC::EXPRESSION::HAPPY);
            pDadNPC->Set_Expression(CNPC::EXPRESSION::HAPPY);
        }

    }


    else if (strCommand == "Get_Shield")
    {
        CPlayer* pPlayer=CGameManager::GetInstance()->Get_MainPlayer();
        CheckNull(pPlayer);
        CInventory_Manager::GetInstance()->Request_Add_To_Inven(ItemType::SHIELD,1);
        CInventory_Manager::GetInstance()->Request_Add_To_Inven(ItemType::SWROD, 1);

    }
    else if (strCommand == "Witch_Mix")
    {
        GameEvent gameEvent;
        gameEvent.Name = "Play_Witch_Mix";

        CInventory_Manager::GetInstance()->Use_Item(ItemType::MUSHROOM, 1);

        m_pGameInstance->Emit(gameEvent);

    }

    else if (strCommand == "Witch_give")
    {
        GameEvent gameEvent;
        gameEvent.Name = "Witch_give";

        m_pGameInstance->Emit(gameEvent);

    }


    else if (strCommand == "Get_Power")
    {
        CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
        CheckNull(pPlayer);
        CInventory_Manager::GetInstance()->Request_Add_To_Inven(ItemType::POWER_BRACELET, 1);

    }
   

   else if (strCommand == "On_TarinEvent_End")
    {
        //1초뒤 fadein
        m_pGameInstance->Invoke(1.f, 0.f, false, false, [this]()
            {


                UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
                CFadeScreen* pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));


                CheckNull(pFadeScreen);
                pFadeScreen->PlayFadeIn();

            }, pOwner);
       


        //그이후 텔포
           //1초뒤 fadein
        m_pGameInstance->Invoke(1.5f, 0.f, false, false, [this]()
            {


                CLevel_Town* pTown = dynamic_cast<CLevel_Town*>(m_pGameInstance->Get_CurrentLevel());
                if (pTown)
                    pTown->Teleport_RichardHouse();


            }, pOwner);

    }

   else if (strCommand == "End_RedKid")
    {
       
        CNPC* pKid_Red_NPC = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red"));
      
        if (pKid_Red_NPC)
        {
            CNPC_KidRed* ppNPC = dynamic_cast<CNPC_KidRed*>(pKid_Red_NPC);
            if (ppNPC)
                ppNPC->End_RedKid();
        }
    
    }

   else if (strCommand == "KidRed_Angry")
    {
        CNPC* pKid_Red_NPC = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red"));
        pKid_Red_NPC->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::ENDING), L"NPC_Layer", L"NPC_Mom"));


        if (pKid_Red_NPC)
        {
            CNPC_KidRed* ppNPC = dynamic_cast<CNPC_KidRed*>(pKid_Red_NPC);
            if (ppNPC)
                ppNPC->Set_Camera_To_NPC(CNPC::EXPRESSION::SAD,L"panic");
        }

    }

    else if (strCommand == "Mom_Angry")
    {
        CNPC* pMamasha = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::ENDING), L"NPC_Layer", L"NPC_Mom"));
        pMamasha->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red"));


        if (pMamasha)
        {
            CNPC* ppNPC = dynamic_cast<CNPC*>(pMamasha);
            if (ppNPC)
                ppNPC->Set_Camera_To_NPC(CNPC::EXPRESSION::SAD,L"talk");
        }

    }

    else if (strCommand == "KidRed_Sad")
    {
        CNPC* pKid_Red_NPC = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red"));

        if (pKid_Red_NPC)
        {
            CNPC_KidRed* ppNPC = dynamic_cast<CNPC_KidRed*>(pKid_Red_NPC);
            if (ppNPC)
                ppNPC->Set_Camera_To_NPC(CNPC::EXPRESSION::SAD, L"wait");
        }

    }

    else if (strCommand == "KidRed_Apologize")
    {
        CNPC* pKid_Red_NPC = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red"));
        pKid_Red_NPC->Set_Target(CGameManager::GetInstance()->Get_MainPlayer());
        if (pKid_Red_NPC)
        {
            CNPC_KidRed* ppNPC = dynamic_cast<CNPC_KidRed*>(pKid_Red_NPC);
            if (ppNPC)
                ppNPC->Set_Camera_To_NPC(CNPC::EXPRESSION::SAD, L"talk");
        }

    }


    else if (strCommand == "Mom_Happy")
    {
        CNPC* pMamasha = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::ENDING), L"NPC_Layer", L"NPC_Mom"));
        pMamasha->Set_Target(CGameManager::GetInstance()->Get_MainPlayer());
        if (pMamasha)
        {
            CNPC* ppNPC = dynamic_cast<CNPC*>(pMamasha);
            if (ppNPC)
                ppNPC->Set_Camera_To_NPC(CNPC::EXPRESSION::HAPPY,L"talk");
        }

    }

    else if (strCommand == "Link_Talk")
    {
        CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
        pPlayer->Set_Target(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::ENDING), L"NPC_Layer", L"NPC_Mom"));

        if (pPlayer)
        {
            pPlayer->Get_ActionControl()->m_bTalk = true;
            pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::TALK));

            GameEvent InteractionNPC;
            InteractionNPC.Name = "Enter_Interaction_NPC";
            InteractionNPC.Payload.Floats["Float_X"] = 0.f;
            InteractionNPC.Payload.Floats["Float_Y"] = 5.f;
            InteractionNPC.Payload.Floats["Float_Z"] = -5.f;

            InteractionNPC.Payload.Ptrs["NPC"] = pPlayer;

            m_pGameInstance->Emit(InteractionNPC);

            /*크레딧올라오기/카메라 초기화*/
            /*모든 챕터를 Ending으로변경*/
            m_pGameInstance->Invoke(3.f, 0.f, false, false, [this,pPlayer]()
                {
                    CNPC* pKid_Red_NPC = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red"));
                    if (pKid_Red_NPC)
                        pKid_Red_NPC->Set_TriggerBoxEnable(true);
                    GameEvent Event;
                    Event.Name = "Ending_Camera";
                    Event.Payload.Ptrs["Player"] = CGameManager::GetInstance()->Get_MainPlayer();

                    Event.Payload.Floats["Float_X"] = 0.f;
                    Event.Payload.Floats["Float_Y"] = 7.f;
                    Event.Payload.Floats["Float_Z"] = -8.f;

                    CGameInstance::GetInstance()->Emit(Event);

                    pPlayer->Set_Target(nullptr);

                    pPlayer->Get_ActionControl()->m_bTalk = false;
                    pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));

                    CGameManager::GetInstance()->Set_EndingStep(EndingStep::ENDINGMESSAGE);
                    CGameManager::GetInstance()->Set_UseCutScene(false);




                },CGameManager::GetInstance()->Get_MainPlayer());
        }

    }
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
            
            string strSpeaker = jStep.at("Speaker").get<string>();
            step.strSpeaker = StringToWString(strSpeaker);

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
                
            if (jStep.contains("Sound"))
            {
                string SoundName = jStep.at("Sound").get<string>();
                step.IsHaveSound = true;
                step.SoundFile = StringToWString(SoundName);
            }
               

            chap.steps.push_back(step);
        }
    }
}
