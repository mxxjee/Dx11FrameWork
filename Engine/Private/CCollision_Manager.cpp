#include "CCollision_Manager.h"
#include "CCollider_Base.h"
#include "CGameObject.h"
#include "CGameInstance.h"


CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize(_uint MaxGroup)
{
    m_CollisionGroupMax = MaxGroup;

    m_pGameInstance = CGameInstance::GetInstance();

    m_CollisionTable.resize(MaxGroup);
    for (_uint i = 0; i < MaxGroup; ++i)
        m_CollisionTable[i].resize(MaxGroup,false);

 

    return S_OK;
}

HRESULT CCollision_Manager::Register_Collider(CCollider_Base* pCollider, _uint iSceneID)
{
    //씬에맞는 풀 가져오기(없으면 자동생성)
    SceneColliderGroupList& sceneGroups = m_mapSceneColliders[iSceneID];
    
    //새로만들어진거면  resize
    if (sceneGroups.empty())
    {
        sceneGroups.resize(m_CollisionGroupMax);
    }
    
    _uint ColGroup = pCollider->Get_ColGroup();
    if (ColGroup < m_CollisionGroupMax)
    {
        sceneGroups[ColGroup].push_back(pCollider);
        Safe_AddRef(pCollider);
    }
    else
    {
        MSG_BOX("Collider Group Index Out of Bounds.");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CCollision_Manager::UnRegister_Collider(CCollider_Base* pCollider, _uint iSceneID)
{
    auto it = m_mapSceneColliders.find(iSceneID);
    if (it == m_mapSceneColliders.end())
        return E_FAIL;

    SceneColliderGroupList& sceneGroups = it->second;
    _uint colGroup = pCollider->Get_ColGroup();

    if (colGroup >= m_CollisionGroupMax || sceneGroups.empty())
        return S_OK;

    ColliderGroupList& group = sceneGroups[colGroup];

    //효율적제거. 요소를찾고 마지막요소와 바뀐디 마지막을 pop
    auto iter = find(group.begin(), group.end(), pCollider);
    if (iter != group.end())
    {
        if (iter != group.end() - 1)
        {
            iter_swap(iter, group.end() - 1);
        }

        group.pop_back();
        Safe_Release(pCollider);


    }

    return S_OK;
}

void CCollision_Manager::Clear_SceneColliders(_uint iSceneID)
{
    auto iter = m_mapSceneColliders.find(iSceneID);
    if (iter == m_mapSceneColliders.end())
        return;

    SceneColliderGroupList& sceneGroup = iter->second;
    for (auto GroupIt = sceneGroup.begin(); GroupIt != sceneGroup.end(); ++GroupIt)
    {
        ColliderGroupList& group = (*GroupIt);
        for (auto colIt = group.begin(); colIt != group.end(); ++colIt)
        {
            Safe_Release(*colIt);
        }

        group.clear();
    }

    m_mapSceneColliders.erase(iter);
}

void CCollision_Manager::Set_Enable_Collision(_uint iSrcGroup, _uint iDstGroup, bool bEnable)
{
    m_CollisionTable[iSrcGroup][iDstGroup] = bEnable;
    m_CollisionTable[iDstGroup][iSrcGroup] = bEnable;

}

void CCollision_Manager::Update_CollisionGroup(_float fTimeDelta)
{
    CheckTrue(m_pGameInstance->Get_IsLoading());

    //Static,현재레벨,static vs 현재레벨
    _uint iCurrentDynamicID = m_pGameInstance->Get_CurrentLevelID();


    // Static 그룹 참조 (ID 0)
    std::map<_uint, SceneColliderGroupList>::iterator itStatic = m_mapSceneColliders.find(0);
    const SceneColliderGroupList* StaticGroupsPtr = (itStatic != m_mapSceneColliders.end()) ? &itStatic->second : nullptr;

    // Dynamic 그룹 참조 (Current ID)
    std::map<_uint, SceneColliderGroupList>::iterator itDynamic = m_mapSceneColliders.find(iCurrentDynamicID);
    const SceneColliderGroupList* DynamicGroupsPtr = (itDynamic != m_mapSceneColliders.end()) ? &itDynamic->second : nullptr;

    if (!DynamicGroupsPtr) 
        return; // 활성화된 동적 씬이 없으면 검사할 필요 없음


    //BeginCollisionFrame 호출 (Static과 Dynamic)
    if (StaticGroupsPtr)
        BeginEventOnGroups(*StaticGroupsPtr);

    BeginEventOnGroups(*DynamicGroupsPtr);
    



    // --- 2. 통합 충돌 검사 루프 (3가지 쌍 모두 검사) ---
    for (_uint g1 = 0; g1 < m_CollisionGroupMax; ++g1)
    {
        for (_uint g2 = g1; g2 < m_CollisionGroupMax; ++g2)
        {
            if (!m_CollisionTable[g1][g2]) continue;

            // 2-A. Dynamic vs Dynamic 충돌 (CurrentGroups vs CurrentGroups)
            CheckCollisionPair((*DynamicGroupsPtr)[g1], (*DynamicGroupsPtr)[g2], (g1 == g2));

            if (StaticGroupsPtr)
            {
                // 2-B. Static vs Static 충돌
                CheckCollisionPair((*StaticGroupsPtr)[g1], (*StaticGroupsPtr)[g2], (g1 == g2));

                // 2-C. Static vs Dynamic 충돌 (교차 검사)
                // Static L1 vs Dynamic L2
                CheckCollisionPair((*StaticGroupsPtr)[g1], (*DynamicGroupsPtr)[g2], false);

                // Dynamic L1 vs Static L2 (역방향 검사)
                if (g1 != g2)
                {
                    CheckCollisionPair((*DynamicGroupsPtr)[g1], (*StaticGroupsPtr)[g2], false);
                }
            }
        }
    }


    // --- 3. ResolveEvents 호출 (Static과 Dynamic 모두 한 번만) ---
    if (StaticGroupsPtr)
        ResolveEventsOnGroups(*StaticGroupsPtr);

    ResolveEventsOnGroups(*DynamicGroupsPtr);
    
}



void CCollision_Manager::CheckCollisionPair(const ColliderGroupList& L1, const ColliderGroupList& L2, _bool bSameGroup)
{
    for (std::vector<CCollider_Base*>::const_iterator AIt = L1.begin(); AIt != L1.end(); ++AIt)
    {
        CCollider_Base* A = *AIt;
        if (!A || !A->Is_Active()) continue;

        for (std::vector<CCollider_Base*>::const_iterator BIt = L2.begin(); BIt != L2.end(); ++BIt)
        {
            CCollider_Base* B = *BIt;

            if (!B || !B->Is_Active()) continue;
            if (A == B) continue;

            // 같은 그룹일 경우 A > B 조건을 사용해 중복 검사 방지
            if (bSameGroup && A > B) continue;

            if (A->Intersect(B) && B->Intersect(A))
            {
                A->RegisterCurrentCollision(B);
                B->RegisterCurrentCollision(A);

                // 밀어내기 로직
                if (!A->Get_IsTrriger())
                {
                    _float3 vOutA = {};
                    if (A->Push_Collision(B, vOutA))
                        B->Get_Owner()->PushOut(vOutA);
                }

                if (!B->Get_IsTrriger())
                {
                    _float3 vOutB = {};
                    if (B->Push_Collision(A, vOutB))
                        A->Get_Owner()->PushOut(vOutB);
                }
            }
        }
    }
}

void CCollision_Manager::BeginEventOnGroups(const SceneColliderGroupList& Groups)
{
   
    for (std::vector<ColliderGroupList>::const_iterator groupIt = Groups.begin();
        groupIt != Groups.end(); ++groupIt)
    {
        const ColliderGroupList& group = *groupIt;
        for (ColliderGroupList::const_iterator ColIt = group.begin(); ColIt != group.end(); ++ColIt)
        {
            CCollider_Base* Col = *ColIt;
            if (Col && Col->Is_Active())
                Col->BeginCollisionFrame();
        }
    }
}

void CCollision_Manager::ResolveEventsOnGroups(const SceneColliderGroupList& Groups)
{
    ///Event

   
    for (std::vector<ColliderGroupList>::const_iterator groupIt = Groups.begin();
        groupIt != Groups.end(); ++groupIt)
    {
        const ColliderGroupList& group = *groupIt;
        for (ColliderGroupList::const_iterator colIt = group.begin(); colIt != group.end(); ++colIt)
        {
            CCollider_Base* col = *colIt;
            if (col && col->Is_Active())
                col->ResolveEvents();
        }
    }
}

CCollision_Manager* CCollision_Manager::Create(_uint MaxGroup)
{
    CCollision_Manager* pInstance = new CCollision_Manager();
    if (FAILED(pInstance->Initialize(MaxGroup)))
    {
        MSG_BOX("FAiled to Create:Collision_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollision_Manager::Free()
{
    for (std::map<_uint, SceneColliderGroupList>::iterator mapIt = m_mapSceneColliders.begin();
        mapIt != m_mapSceneColliders.end(); ++mapIt)
    {
        SceneColliderGroupList& sceneGroups = mapIt->second;
        for (SceneColliderGroupList::iterator groupIt = sceneGroups.begin(); groupIt != sceneGroups.end(); ++groupIt)
        {
            ColliderGroupList& group = *groupIt;
            for (ColliderGroupList::iterator pColIt = group.begin(); pColIt != group.end(); ++pColIt)
            {
                Safe_Release(*pColIt);
            }
        }
    }
    m_mapSceneColliders.clear();
}
