#pragma once
#include "CBase.h"

/*
1. 현재 활동중인 씬을 저장한다.
2. 씬을 바꾸라는 명령을 내린다.
3. 씬 체인지는 바꾸기 전 현재 씬을 정리한 뒤, 바꿀 씬을 현재씬 정보로 저장한다.

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
    HRESULT         Level_Changer(_uint iSceneID, class CLevel* pNewLevel);
    
    
    
    HRESULT         Update(const _float fTimeDelta);
    void            Render();



public:
    static CLevel_Manager* Create();


private:
    class  CGameInstance* m_pGameInstance = { nullptr };
    _uint           m_iCurrentLevelID = {};
    class   CLevel* m_pCurrentLevel = { nullptr };

private:
    virtual void Free();

};
NS_END

