#pragma once

#include "Engine_Define.h"



NS_BEGIN(Engine)


class ENGINE_DLL CMapToolObject
{
public:
    typedef struct tagMapToolObjDesc
    {
        std::wstring m_Name;
        _float3 vPosition, vRotation, vScale;
    }MAPTOOLOBJ_DESC;

protected:
    CMapToolObject();
    CMapToolObject(const CMapToolObject& rhs);
    virtual ~CMapToolObject() = default;

public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/

    void        Update_Priority(_float fTimeDelta);
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();


public:
    _wstring name = L"";
    _float3     vPos, vRotate, Scale;
};

NS_END

