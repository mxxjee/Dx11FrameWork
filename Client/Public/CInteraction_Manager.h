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
    void        RegisterInteractable(CIInteractable* pObj);
    void        UnRegisterInteractable(const CIInteractable* pObj);

    void        Update(_float fTimeDelta);
    bool        OnInteractKeyPresed();      //A키 눌렀을때 호출
    void        Clear();                //씬넘어가면 반드시호출해서 비우자.

    bool        Check_InteractiveType(InteractionType eType);
public:
    CIInteractable*              Get_CurrentTarget() { return m_pCurrentTarget; }
public:
    virtual void                Free() override;

private:
    CIInteractable*              Find_Object(const CIInteractable* pObj);

private:
    CIInteractable*              m_pCurrentTarget = nullptr;
    list<CIInteractable*>        m_InteractableObjects;  //필드에 존재하는 모든 상호작용한 오브젝트들을 등록


};
NS_END

