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

#if defined(_DEBUG)
struct INTERACTION_BASELINE_STATS
{
    unsigned long long  iMeasuredFrames = 0;
    unsigned long long  iLatestRegisteredObjects = 0;
    unsigned long long  iMaxRegisteredObjects = 0;
    unsigned long long  iTotalRegisteredObjects = 0;
    unsigned long long  iLatestInRangeObjects = 0;
    unsigned long long  iMinInRangeObjects = ~0ull;
    unsigned long long  iMaxInRangeObjects = 0;
    unsigned long long  iTotalInRangeObjects = 0;
    unsigned long long  iIsInteractableCalls = 0;
    unsigned long long  iPriorityComparisons = 0;
    unsigned long long  iRangeEnters = 0;
    unsigned long long  iRangeExits = 0;
    unsigned long long  iCurrentTargetChanges = 0;
    unsigned long long  iInteractionStarts = 0;
    unsigned long long  iInteractionEnds = 0;
    unsigned long long  iCarryNotifyCalls = 0;
    unsigned long long  iCarryNotifyTargetMismatches = 0;
    unsigned long long  iChangeSceneCalls = 0;
    unsigned long long  iEarlyOutNoPlayer = 0;
    unsigned long long  iEarlyOutEmptyRegistry = 0;
    unsigned long long  iEarlyOutEpilogue = 0;
    unsigned long long  iEarlyOutCarry = 0;

    double              dFirstLoopTotalMicroseconds = 0.0;
    double              dFirstLoopMaxMicroseconds = 0.0;
    double              dSecondLoopTotalMicroseconds = 0.0;
    double              dSecondLoopMaxMicroseconds = 0.0;
    double              dUpdateTotalMicroseconds = 0.0;
    double              dUpdateMaxMicroseconds = 0.0;
    double              dChangeSceneTotalMicroseconds = 0.0;
    double              dChangeSceneMaxMicroseconds = 0.0;
};
#endif


 class CInteraction_Manager :
    public CBase
{
    DECLARE_SINGLETON(CInteraction_Manager)

private:
    explicit CInteraction_Manager() {};
    virtual ~CInteraction_Manager() = default;

public:
                // Called when an interaction object is created or destroyed.
    void        RegisterInteractable(CIInteractable* pObj);
    void        UnRegisterInteractable(const CIInteractable* pObj);

    void        Update(_float fTimeDelta);
    bool        OnInteractKeyPresed();      // Handles the interaction key.
    void        Clear();                    // Clears interaction state during scene changes.

    bool        Check_InteractiveType(InteractionType eType);

    void        Add_Interaction(CIInteractable* pObj);
    void        Change_Scene(_uint iLevelID);

    HRESULT         Initialize();

public:
    CIInteractable*              Get_CurrentTarget() { return m_pCurrentTarget; }
    CIInteractable*              Get_PreTarget() { return m_pPreTarget;}
    void                        Reset_CurrentTarget() { m_pCurrentTarget = nullptr; }
    void                        Set_CurrentTarget(CIInteractable* pObj);
#if defined(_DEBUG)
    const INTERACTION_BASELINE_STATS& Get_BaselineStats() const { return m_BaselineStats; }
    void                        Reset_BaselineStats();
    void                        Dump_BaselineStats() const;
    void                        Record_CarryNotifyTarget(CIInteractable* pTarget);
#endif
public:
    virtual void                Free() override;
public:
    HRESULT                 Load_Data(string SceneName, vector< DefaultInteractionData>	&	Infos ,const string& LoadPath);
    
                                                   
private:
    CIInteractable*              Find_Object(const CIInteractable* pObj);

private:
    
    CIInteractable*              m_pCurrentTarget = nullptr;
    CIInteractable*             m_pPreTarget = nullptr;

    list<CIInteractable*>        m_InteractableObjects;  // All registered interactables in the active scene.

public:
    void        Set_MainPlayer(CGameObject* pObj);
private:
    CPlayer*            m_pMainPlayer = nullptr;
    CGameInstance*      m_pGameInstance = nullptr;

#if defined(_DEBUG)
private:
    void                Commit_BaselineFrame(
                            unsigned long long iRegisteredObjects,
                            unsigned long long iInRangeObjects,
                            unsigned long long iIsInteractableCalls,
                            unsigned long long iPriorityComparisons,
                            unsigned long long iRangeEnters,
                            unsigned long long iRangeExits,
                            double dFirstLoopMicroseconds,
                            double dSecondLoopMicroseconds,
                            double dUpdateMicroseconds,
                            CIInteractable* pTargetAtFrameStart);
    void                Record_ChangeSceneBaseline(double dMicroseconds);

    INTERACTION_BASELINE_STATS  m_BaselineStats{};
    CIInteractable*             m_pBaselineInteractionStartTarget = nullptr;
    _uint                       m_iBaselineActiveLevel = 0;
#endif
};
NS_END

