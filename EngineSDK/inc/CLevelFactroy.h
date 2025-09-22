#pragma once
#include "CBase.h"
#include "Engine_LevelTypes.h"


/*
LevelFactory를 사용하는 이유:
1. 씬 생성을 명시적 & 일관되게 관리하기 위해
2. 새로운 씬 추가 시 등록하여 사용
*/

NS_BEGIN(Engine)
class CLevelFactroy :
    public CBase
{
private:
    explicit CLevelFactroy();
    ~CLevelFactroy() = default;

public:
    void        Register(_uint iSceneID, LevelCreator Creator);
    CLevel* Create(_uint iSceneID,LevelArgs& _Arg);

public:
    static      CLevelFactroy* Create();
    virtual     void        Free();


private:
    unordered_map<_uint, LevelCreator> m_creators;

};
NS_END

