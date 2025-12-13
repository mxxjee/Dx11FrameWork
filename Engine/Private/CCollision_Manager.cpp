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
    m_pColliderGroups.resize(MaxGroup);
  
    for (_uint i = 0; i < MaxGroup; ++i)
        m_CollisionTable[i].resize(MaxGroup,false);

    for (_uint i = 0; i < MaxGroup; ++i)
        m_pColliderGroups[i].resize(MaxGroup, nullptr);


    return S_OK;
}

HRESULT CCollision_Manager::Register_Collider(CCollider_Base* pCollider)
{
    m_pColliderGroups[pCollider->Get_ColGroup()] .push_back(pCollider);
    Safe_AddRef(pCollider);

    return S_OK;
}

HRESULT CCollision_Manager::UnRegister_Collider(CCollider_Base* pCollider)
{
    _uint colGroup = pCollider->Get_ColGroup();
    vector<CCollider_Base*>&    m_pCollision= m_pColliderGroups[colGroup];


    auto iter = find(m_pColliderGroups[colGroup].begin(), m_pColliderGroups[colGroup].end(), pCollider);
    if (iter != m_pColliderGroups[colGroup].end())
    {
        m_pCollision.erase(
            remove(m_pCollision.begin(), m_pCollision.end(), pCollider),
            m_pCollision.end());

        Safe_Release(pCollider);

    }

    return S_OK;
}

void CCollision_Manager::Set_Enable_Collision(_uint iSrcGroup, _uint iDstGroup, bool bEnable)
{
    m_CollisionTable[iSrcGroup][iDstGroup] = bEnable;
    m_CollisionTable[iDstGroup][iSrcGroup] = bEnable;

}

void CCollision_Manager::Update_CollisionGroup(_float fTimeDelta)
{
    CheckTrue(m_pGameInstance->Get_IsLoading());

    //프레임시작: 모ㅓㅓ든 collider begincollisionFrame()
    for (_uint i = 0; i < m_CollisionGroupMax; ++i)
    {
        for (auto* Col : m_pColliderGroups[i])
        {
            if (Col && Col->Is_Active())
                Col->BeginCollisionFrame();

        }
    }

    ///밀기 수행
    for (_uint g1 = 0; g1 < m_CollisionGroupMax; ++g1)
    {
        for (_uint g2 = g1; g2 < m_CollisionGroupMax; ++g2)
        {
            if (!m_CollisionTable[g1][g2])
                continue;


            auto& L1 = m_pColliderGroups[g1];
            auto& L2 = m_pColliderGroups[g2];


            for (auto* A : L1)
            {
                if (!A || !A->Is_Active())
                    continue;

                for (auto* B : L2)
                { 
                    if (!B || !B->Is_Active())
                        continue;

                    if (A == B)
                        continue;

                   
                    if (g1 == g2 && A > B)
                        continue;

                    if(A->Intersect(B) && B->Intersect(A))
                    {
                        A->RegisterCurrentCollision(B);
                        B->RegisterCurrentCollision(A);


                        if (!A->Get_IsTrriger())
                        {
                            _float3 vOutA = {};
                            if (A->Push_Collision(B, vOutA))
                                B->Get_Owner()->PushOut(vOutA);
                        }

                        if(!B->Get_IsTrriger())
                        {
                            _float3  vOutB = {};
                            if (B->Push_Collision(A, vOutB))
                                A->Get_Owner()->PushOut(vOutB);
                          
                        }
                    }
                }
            }
        }
    }

    ///Event

    for (_uint g = 0; g < m_CollisionGroupMax; g++)
    {
        for (auto* col : m_pColliderGroups[g])
        {
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
    for (_uint i = 0; i < m_CollisionGroupMax; ++i)
    {
        for (auto& pCol : m_pColliderGroups[i])
            Safe_Release(pCol);

    }
}
