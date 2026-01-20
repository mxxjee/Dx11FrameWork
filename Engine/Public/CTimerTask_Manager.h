#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CGameObject;

class CTimerTask_Manager :
    public CBase
{
    static _uint    m_LastTraskId;

public:
    struct Task
    {
        float       m_fTime = 0.f;        //예약 시간(N초후)
        float       m_finterval = 0.f;     //Repeat시 다시 실행할 인터벌 N초
        bool        m_bRepeat = false;      //반복실행
        bool        cancelled = false;      //취소되었는지 여부, 오너가 없다면 실패
        std::function<void()> m_Callback = nullptr;
        _uint        m_id;                  //Task 고유아이디

        CGameObject* pOwner = nullptr;           //이 태스크를 예약한사람,actor가 삭제되면 자동으로 Task삭제.

    };
private:
    CTimerTask_Manager();
    virtual ~CTimerTask_Manager() = default;

public:
    static CTimerTask_Manager* Create();
    virtual void    Free();
public:
    void            Update(_float fTimeDelta);
    HRESULT         Invoke(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner);
    HRESULT         Repeat(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner);
    
                //이 owner의 모든 task를 취소한다.
    HRESULT         CancelTaskOf(CGameObject* pOwner);    
              
                //오브젝트 삭제시 이거 반드시호출.
private:
    HRESULT         AddTask(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner);
    HRESULT         CancelTask(_uint id);   
 
private:
    vector<Task>        m_Tasks;//총 태스크들, 순회의용도
    UMap<_uint, size_t> m_TaskIndex;    //TaskID (fisrt) / m_Tasks[idx](Second)
                                        //삭제의 용도


    unordered_map<CGameObject*, vector<_uint>> m_OwnerTasks;            //각 게임오브젝트가 오너인 태스크들 모음


};
NS_END

