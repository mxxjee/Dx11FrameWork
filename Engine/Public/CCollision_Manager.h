#pragma once
#include "CBase.h"
/*콜라이더에 대한 타입에 대한 충돌 비교를 bool로 저장*/
NS_BEGIN(Engine)
class CCollider_Base;

class CCollision_Manager :
    public CBase
{

private:
    explicit CCollision_Manager();
    virtual ~CCollision_Manager() = default;


public:
    HRESULT     Initialize(_uint MaxGroup);

                // 콜라이더 등록
    HRESULT     Register_Collider(CCollider_Base* pCollider);


            //객체 삭제 시 호출해줘야함. 콜라이더 등록해제
    HRESULT     UnRegister_Collider(CCollider_Base* pCollider);


                    //각 그룹간의 충돌 설정 
    void        Set_Enable_Collision(_uint iSrcGroup, _uint iDstGroup,bool bEnable);


                    //매프레임마다 충돌 
    void            Update_CollisionGroup(_float fTimeDelta);
public:
    static CCollision_Manager* Create(_uint MaxGroup);
    virtual void    Free();
private:
    _uint       m_CollisionGroupMax = 0;

    //그룹별 충돌체 그룹
    vector<vector<CCollider_Base*>>     m_pColliderGroups;

    //그룹별 충돌 테이블
    vector<vector<bool>>       m_CollisionTable;


};
NS_END

