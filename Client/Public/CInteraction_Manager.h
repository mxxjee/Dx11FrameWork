#pragma once
#include "CBase.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CIInteractable;

 class CInteraction_Manager :
    public CBase
{
    DECLARE_SINGLETON(CInteraction_Manager)

private:
    explicit CInteraction_Manager() {};
    virtual ~CInteraction_Manager() = default;

public:
    void        RegisterInteractable(string SceneName,CIInteractable* pObj);
    void        UnRegisterInteractable(string SceneName, const CIInteractable* pObj);

    void        Update(_float fTimeDelta);
    bool        OnInteractKeyPresed();      //A키 눌렀을때 호출
    void        Clear();                //씬넘어가면 반드시호출해서 비우자.

    bool        Check_InteractiveType(InteractionType eType);
public:
    CIInteractable*              Get_CurrentTarget() { return m_pCurrentTarget; }
public:
    virtual void                Free() override;
    HRESULT                     Set_MainInteratables(string SceneName);

private:
    CIInteractable*              Find_Object(string SceneName,const CIInteractable* pObj);
    vector<CIInteractable*>*    Find_InteractionObjects_By_SceneName(string SceneName);

private:
    CIInteractable*              m_pCurrentTarget = nullptr;
    list<CIInteractable*>        m_InteractableObjects;  //필드에 존재하는 모든 상호작용한 오브젝트들을 등록


private:                //키값 : 씬 이름을 hash함수를 통해 size_t로 바꾼거
    UMap<size_t , vector<CIInteractable*>> m_sceneInteractbles;
    vector<CIInteractable*>*                MainInteractbles;


};
NS_END

