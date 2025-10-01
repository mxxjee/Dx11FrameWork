#pragma once
#include "CBase.h"
#include "Engine_LevelTypes.h"

/*
1. 현재 활동중인 씬을 저장한다.
2. 씬을 바꾸라는 명령을 내린다.
3. 씬 체인지는 바꾸기 전 현재 씬을 정리한 뒤, 바꿀 씬을 현재씬 정보로 저장한다.

4. 씬은 스택으로 관리한다.
5. 씬은 changetype에 맞게 push/overlay/replacetop 을 판단
6. 지우려는 씬은 바로삭제되지 않고 destory stack에 쌓이며, late update에서 비워진다.

*/

NS_BEGIN(Engine)
class CLevel_Manager :
    public CBase
{
private:
    explicit CLevel_Manager();
   ~CLevel_Manager() = default;

public:
    /*씬을 바꾸기 전 이전 씬을 정리하고, 저장하던 씬 정보 갱신.*/
    HRESULT         Level_Changer(_uint iSceneID, LevelArgs& args);
   
    void            Update_Priority(const _float fTimeDelta);
    void            Update(const _float fTimeDelta);
    void            Update_Late(const _float fTimeDelta);
    void            Update_Render(const _float fTimeDelta);

    void            Render();


public:
    CLevel*         Get_PrevLevel();
    CLevel*         Get_CurrentLevel();
    const vector<CLevel*>* Get_LevelStack() { return &m_Stack; }
    void            Pop_Level();

      

private:
    void            Clear_DestroyStack();
    void            ActiveTop(CLevel* pNewLevel,LEVELCHANGETYPE eChangeType);   //Top교체 및 활성화
    void            PopIfTransient();     //임시씬이 있을 경우 바로 삭제 하고 이후처리.

    void            ReplaceTop_Level(_uint iSceneID, CLevel* pNewLevel);
    void            Push_Level(_uint iSceneID, CLevel* pNewLevel);
    void            Overlay_Level(_uint iSceneID, CLevel* pNewLevel);


public:
    static CLevel_Manager* Create();


private:
    class  CGameInstance*   m_pGameInstance = { nullptr };
    _uint                   m_iCurrentLevelID = {};

    //씬 스택 (활성화된 씬만 모아둠)
    vector<CLevel*>         m_Stack;

    //지연삭제를 위한 STL
    vector<CLevel*>         m_tDestroy;        

private:
    virtual void Free();

};
NS_END

