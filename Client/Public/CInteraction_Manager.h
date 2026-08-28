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
                // 상호작용 객체가 생성되거나 제거될 때 호출한다.
    void        RegisterInteractable(CIInteractable* pObj);
    void        UnRegisterInteractable(const CIInteractable* pObj);
    void        RequestAddCandidate(CIInteractable* pObj);
    void        RequestRemoveCandidate(CIInteractable* pObj);
    void        PurgeInteractable(CIInteractable* pObj);

    void        Update(_float fTimeDelta);
    bool        OnInteractKeyPresed();      // 상호작용 키 입력을 처리한다.
    void        Clear();                    // 씬 전환 중 상호작용 상태를 정리한다.

    bool        Check_InteractiveType(InteractionType eType);

    void        Add_Interaction(CIInteractable* pObj);
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
    void                        RequestCandidateState(CIInteractable* pObj, bool bAdd);
    void                        ApplyPendingCandidates();

    struct CANDIDATE_REQUEST
    {
        CIInteractable* pObject = nullptr;
        bool            bAdd = false;
    };

private:
    
    CIInteractable*              m_pCurrentTarget = nullptr;
    CIInteractable*             m_pPreTarget = nullptr;

    list<CIInteractable*>        m_InteractableObjects;  // 현재 씬에 등록된 모든 상호작용 객체이다.
    std::vector<CIInteractable*> m_Candidates;           // 프레임별 대상 탐색에 사용하는 플레이어 겹침 순서를 유지한다.
    std::vector<CANDIDATE_REQUEST> m_PendingCandidateRequests;

public:
    void        Set_MainPlayer(CGameObject* pObj);
private:
    CPlayer*            m_pMainPlayer = nullptr;
    CGameInstance*      m_pGameInstance = nullptr;

};
NS_END

