#pragma once
#include "CBase.h"

namespace Engine
{
 
    class CGameInstance;


}
NS_BEGIN(Client)
class CEffect;

class CEffectPoolManager :
    public CBase
{
public:
    DECLARE_SINGLETON(CEffectPoolManager)


private:
    explicit CEffectPoolManager() {};
    virtual ~CEffectPoolManager() = default;

public:
    //원본 로드
    HRESULT         Initialize();
    
    CEffect*         Request_Spawn(const wstring& ProtoTag,void* pArg);
    HRESULT         Request_Return(CEffect* pObj);

protected:
    virtual void	Free();		//객체 삭제 직전에, 안전하게 다른 자원들을 끊어내기 위함(멤버변수 정리)



private:
    UMap<_uint, list<CEffect*>>      m_ClonDatas;
    CGameInstance* m_pGameInstance = nullptr;


};

NS_END