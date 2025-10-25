#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CLight_Manager :
    public CBase
{
private:
    /*셰이더 바인딩용*/
    struct LightList
    {
        int                     m_LightsNum;

        vector<_float4>         m_LightPositions;
        vector<_float>         m_LightRanges;
        
        vector<_float4>         m_LightDiffuses;
        vector<_float4>         m_LightAmbients;
        vector<_float4>         m_LightSpeculars;

        void        clear()
        {
            m_LightsNum = 0;
            m_LightPositions.clear();
            m_LightRanges.clear();
            m_LightDiffuses.clear();
            m_LightAmbients.clear();
            m_LightSpeculars.clear();
        }


    };
private:
    CLight_Manager();
    virtual ~CLight_Manager() = default;

private:
    HRESULT         Initialize(_uint iLevelNum);

public:
    const       LIGHT_DESC* Get_LightDesc(_uint iLevelID,_uint iIndex);
    class CLight*          Get_Light(_uint iLevelID, _uint iIndex);

    HRESULT         Bind_Lights(class CShader* pShader);

public:
    HRESULT         Add_Light(_uint iLevelID, const LIGHT_DESC& LightDesc);

                    //셰이더로 던져주기 위한 리스트를 채운다.
    void            Add_LightValue(const LIGHT_DESC* LightDesc);
private:
    HRESULT         Bind_Directional_Light(class CShader* pShader,const LIGHT_DESC* pLightDesc);
    HRESULT         Bind_Point_Light(class CShader* pShader);

public:
    static CLight_Manager* Create(_uint iLevelNum);
    virtual void Free() override;

public:
        //씬 개수만큼 들고있도록함.
    vector<class CLight*>           m_DirectionalLights;
    vector<list<class CLight*>>     m_Lights;

    LightList               m_LightValues;

private:

};
NS_END

