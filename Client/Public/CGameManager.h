#pragma once
#include "CBase.h"
#include "Client_Defines.h"

namespace Engine
{
    class CGameObject;
    class CLayer;
    class CGameInstance;
    class CBase;
}


NS_BEGIN(Client)
class CPlayer;
class CGameManager :
    public CBase
{
    DECLARE_SINGLETON(CGameManager)

public:
    enum CUTSCENE_TYPE
    {
        START,ENDING,END
    };
private:
    explicit CGameManager() {};
    virtual ~CGameManager() = default;

public:
    void        Set_MainPlayer(CGameObject* pObj);
    void        Set_MainPlayer(CBase* pObj);

    CPlayer* Get_MainPlayer() { return m_pMainPlayer; }

    void        Set_LastPosition(_float4 vPosition);
    void        Set_LastPosition(_vector vPosition);


    void        Set_DefaultPosition(_vector vPosition);
    void        Set_DefaultPosition(_float4 vPosition);



    _float4     Get_LastPosition_By_Float4();
    _vector     Get_LastPosition_By_Vector();

    _float4     Get_DefaultPositio_By_Float4();
    _vector     Get_DefaultPositioBy_Vector();

    virtual void Free() override;

public:
    bool        Is_FirstSpawn() { return m_bFirstSpawn; }

public:
    void        Set_FirstLoading(bool b) { m_bFirstLoading = b; }
    void        Set_UseCutScene(bool b) { m_bUseCutScene = b; }
    void        Set_CutSceneType(CUTSCENE_TYPE eType) { m_eCutSceneType = eType; }

    void        Set_OpenInventory(bool b) { m_bOpenInventory = b; }
 

    bool        Get_IsFirstLoading() { return m_bFirstLoading; }
    bool        Get_UseCutScene() { return m_bUseCutScene;}
    CUTSCENE_TYPE Get_CutSceneType() { return m_eCutSceneType; }

    void        Set_EndingStep(EndingStep pStep) { m_eEndingStep = pStep; }
    EndingStep        Get_EndingStep() { return m_eEndingStep; }
    bool        Get_IsOpenInventory() { return m_bOpenInventory; }

private:
    CPlayer* m_pMainPlayer=nullptr;
  
    _float4  vLastPosition;
    _float4  vDefulatPosition;


    bool        m_bFirstSpawn = true;//첫시작일떄를 의미
    bool        m_bFirstLoading = true;//첫 로딩임을 의미
    bool        m_bUseCutScene = true;


    bool        m_bOpenInventory = false;

    CUTSCENE_TYPE m_eCutSceneType=CUTSCENE_TYPE::START;
    EndingStep      m_eEndingStep = EndingStep::END;

};
NS_END
