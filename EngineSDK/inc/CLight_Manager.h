#pragma once
#include "CBase.h"
#include "ConstantStruct.h"


NS_BEGIN(Engine)
class CLight_Manager :
    public CBase
{

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
public:
    HRESULT         Bind_Directional_Light(class CShader* pShader,class CVIBuffer_Rect* pVIBuffer,const LIGHT_DESC* pLightDesc);
    HRESULT         Bind_Point_Light(class CShader* pShader,class CVIBuffer_Rect* pVIBuffer);
    void            Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
    void        Clear_PointLightBuffer();
public:
    static CLight_Manager* Create(_uint iLevelNum);
    virtual void Free() override;

public:
        //씬 개수만큼 들고있도록함.
    vector<class CLight*>           m_DirectionalLights;
    vector<list<class CLight*>>     m_Lights;



private:
    PointLightBuffer               m_LightValues;
    int                             m_PointLightNum=0;    //활성화 되어있는 조명개수

    DirectionLightBuffer            m_DirectionLightBuffer;
    int                            m_iLevelID = 0;
};
NS_END

