#pragma once
#include "CBase.h"
#include "Client_Defines.h"


NS_BEGIN(Client)

class CEffectData_Manager :
    public CBase
{

   
public:
    DECLARE_SINGLETON(CEffectData_Manager)


private:
    explicit CEffectData_Manager() {};
    virtual ~CEffectData_Manager() = default;


public:
    //데이터 로드
    HRESULT         Initialize();
    void            Register_Data(wstring& ModelName, EffectData& Data);

    void            Update_Data(wstring& ModelName, EffectData& Data);//Info,json 파일 갱신
    
    EffectData*     Find_Data(wstring ModelName);

    HRESULT         Load_AllData();

    HRESULT             Save_To_Json(wstring& ModelName, EffectData Data);


protected:
    virtual void	Free();		//객체 삭제 직전에, 안전하게 다른 자원들을 끊어내기 위함(멤버변수 정리)



private:
    UMap<_uint, EffectData*>      m_Datas;

    
};

NS_END