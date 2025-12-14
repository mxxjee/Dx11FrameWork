#pragma once
#include "CBase.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CQuest_Manager;

class CDialogue_Manager :
    public CBase
{
    DECLARE_SINGLETON(CDialogue_Manager)

private:
    explicit CDialogue_Manager() {};
    virtual ~CDialogue_Manager() = default;

public:
    HRESULT         Initialize();//파일을 읽는다.

public:
    //실제 JSOn파싱
    void        LoadScriptDatabase(const string& strFilePath);

                //대화시작(NPC대화시작시 호출)
    void    StartDialogue(string strModelID);

                //대화진행시 호출되는함수(a)
    bool    AdvanceDialogueStep();     


public:
    ///////Quest랑 관련된 함수들//////
    //NPC이름과 ㅓ챕터 ID기반으로 대화 챕터 로드
    const DialogueChapter* Get_ChapterDefinition(
        const string& strModelID,
        const string& strCurrentChapterID);

    
            //대화 종료 후 영구 챕터 갱신 로직
    void TryAdvanceChapter(const string& ModelID, const string& strCurrentChapterID);
    const wstring& Get_CurrentText() { return m_CurrentWstr; }
private:

                    //dialogue에 정의된 행동수행
    void        ExecuteActionCommand(const std::string& strCommand, const Dialog_Action_Timing& strTiming);
    void        ParseDialogueChapter(const json& jChap, DialogueChapter& chap);

private:
           //Key:Hash(NPC_MODEL), Value:모든 NPC들의 대화 스크립트데이터
    UMap<size_t, NPC_DialogueScript> m_mapScripts;     
    
    
                //현재 대화중 dialoguechapter포인터, 대화끝나면 null초기화
    DialogueChapter* m_pActiveChapter = nullptr;


    string      m_strActiveNPC = "";        //현재대화중 NPC
    wstring     m_CurrentWstr = L"";
};
NS_END
