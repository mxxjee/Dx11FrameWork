#pragma once
#include "CQuestCondition.h"


/*visitedtrigger에 의해서 변수가 true가되면 완료된거.*/

NS_BEGIN(Client)
class CLocation_Condition :
    public CQuestCondition
{
public:
                        //트리거로 계산할건지, 씬타입만 판단할건지
    enum LOCATION_TYPE{TYPE_TIRRGER_REGION, TYPE_SCENE_ID};
public:
    CLocation_Condition();
    CLocation_Condition(_uint iTargetID);
    virtual ~CLocation_Condition();

public:
    void    Set_Visited(bool b) { m_bVisited = b; }

public:
    //매프레임마다 조건체크를 통해 true/false를반환
    virtual bool IsSatisfied();

    
private:
    LOCATION_TYPE m_eType = TYPE_TIRRGER_REGION;

    _uint   m_iTargetSceneID = 0;
    bool    m_bVisited = false;
    
private:
    class CGameInstance* m_pGameInstance = nullptr;
};

NS_END

