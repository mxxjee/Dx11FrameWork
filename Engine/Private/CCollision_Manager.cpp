#include "CCollision_Manager.h"
#include "CCollider_Base.h"

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize(_uint MaxGroup)
{
    m_CollisionGroupMax = MaxGroup;

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
    for (_uint i = 0; i < m_CollisionGroupMax; ++i)
    {
            //중복검사 방지패턴
        for (_uint j = i; j < m_CollisionGroupMax; ++j)
        {
            // 충돌규칙이 false라면, continue
            if (!m_CollisionTable[i][j])
                continue;

            auto& SrcList = m_pColliderGroups[i];
            auto& DstList = m_pColliderGroups[j];

            for (auto& Src : SrcList)
            {
                for (auto& Dst : DstList)
                {
                    if (Src == Dst || !Src||!Dst)
                        continue;

                    Src->Reset_Collision();
                    Dst->Reset_Collision();

					if (Src->Intersect(Dst) && Dst->Intersect(Src))
					{
						Src->OnCollision(Dst);
						Dst->OnCollision(Src);


					}
                }
            }

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
