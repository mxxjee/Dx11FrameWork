#pragma once
#include "CBase.h"
#include "Client_Defines.h"
#include "CInteractionObject.h"

namespace Engine
{
    class CGameInstance;
}

NS_BEGIN(Client)
class CIInteractable;
class CPlayer;


 class CInteraction_Manager :
    public CBase
{
    DECLARE_SINGLETON(CInteraction_Manager)

private:
    explicit CInteraction_Manager() {};
    virtual ~CInteraction_Manager() = default;

public:
                //Interactionobject생성시에 불러주기
    void        RegisterInteractable(CIInteractable* pObj);
    void        UnRegisterInteractable(const CIInteractable* pObj);

    void        Update(_float fTimeDelta);
    bool        OnInteractKeyPresed();      //A키 눌렀을때 호출
    void        Clear();                //씬넘어가면 반드시호출해서 비우자.

    bool        Check_InteractiveType(InteractionType eType);

    void        Change_Scene(_uint iLevelID);

    HRESULT         Initialize();

public:
    CIInteractable*              Get_CurrentTarget() { return m_pCurrentTarget; }
    CIInteractable*              Get_PreTarget() { return m_pPreTarget;}
    void                        Reset_CurrentTarget() { m_pCurrentTarget = nullptr; }
    void                        Set_CurrentTarget(CIInteractable* pObj);
public:
    virtual void                Free() override;
public:
    HRESULT                 Load_Data(string SceneName, vector< DefaultInteractionData>	&	Infos ,const string& LoadPath);
    
                                                   
private:
    CIInteractable*              Find_Object(const CIInteractable* pObj);

private:
    
    CIInteractable*              m_pCurrentTarget = nullptr;
    CIInteractable*             m_pPreTarget = nullptr;

    list<CIInteractable*>        m_InteractableObjects;  //현재 레벨에 맞는, 필드에 존재하는 모든 상호작용한 오브젝트들을 등록

public:
    void        Set_MainPlayer(CGameObject* pObj);
private:
    CPlayer*            m_pMainPlayer = nullptr;
    CGameInstance*      m_pGameInstance = nullptr;
};
NS_END

