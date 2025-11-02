#pragma once
#include "CMapObject.h"

NS_BEGIN(Engine)
class CModel;
class ENGINE_DLL CMapModel :
    public CMapObject
{
public:
    typedef struct tagMapModel_DESC : CMapObject::MapObject_DESC
    {
        _wstring    modelName;
        void* modelDesc = nullptr;

    }MAPMODEL_DESC;

protected:
    CMapModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapModel(const CMapModel& rhs);
    virtual ~CMapModel() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();
public:
    HRESULT                     Ready_Components(void* pArg);
    HRESULT                     Ready_Resources(void* pArg);
    HRESULT                     Bind_ShaderResources();

public:
    static CMapModel* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

protected:
    CModel* m_pModel = nullptr;

};

NS_END

