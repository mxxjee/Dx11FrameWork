#pragma once
#include "CBase.h"
/*콜라이더에 대한 타입에 대한 충돌 비교를 bool로 저장*/
NS_BEGIN(Engine)
class CCollider_Base;
class CGameInstance;

typedef std::vector<CCollider_Base*>     ColliderGroupList;//각 충돌그룹 리스트
typedef std::vector<ColliderGroupList>  SceneColliderGroupList;     //전체충돌그룹
class CCollision_Manager :
    public CBase
{

private:
    explicit CCollision_Manager();
    virtual ~CCollision_Manager() = default;


public:
    HRESULT     Initialize(_uint MaxGroup);

                //씬에 맞는 콜라이더 등록
    HRESULT     Register_Collider(CCollider_Base* pCollider,_uint iSceneID);


            //객체 삭제 시 호출해줘야함. 콜라이더 등록해제
    HRESULT     UnRegister_Collider(CCollider_Base* pCollider, _uint iSceneID);

                    //특정씬의 콜리전그룹 해제
    void        Clear_SceneColliders(_uint iSceneID);

                    //각 그룹간의 충돌 설정 
    void        Set_Enable_Collision(_uint iSrcGroup, _uint iDstGroup,bool bEnable);


                    //매프레임마다 충돌 검사 (현재 활성화된 씬의 것만)
    void            Update_CollisionGroup(_float fTimeDelta);


private:
    //각 그룹별 충돌체크
    void            CheckCollisionPair(const ColliderGroupList& L1,
                                        const ColliderGroupList& L2, _bool bSameGroup);

    void            BeginEventOnGroups(const SceneColliderGroupList& Groups);
    void            ResolveEventsOnGroups(const SceneColliderGroupList& Groups);
public:
    static CCollision_Manager* Create(_uint MaxGroup);
    virtual void    Free();

#if defined(_DEBUG)
public:
    void            Reset_BaselineStats();
    void            Dump_BaselineStats() const;
#endif

private:
    map<_uint, SceneColliderGroupList>   m_mapSceneColliders;

    _uint       m_CollisionGroupMax = 0;
    
    //그룹별 충돌 테이블
    vector<vector<bool>>       m_CollisionTable;
    CGameInstance* m_pGameInstance;

#if defined(_DEBUG)
    unsigned long long  m_iBaselineMeasuredFrames = 0;
    double              m_dBaselineTotalMicroseconds = 0.0;
    double              m_dBaselineMaxMicroseconds = 0.0;
#endif

};
NS_END

