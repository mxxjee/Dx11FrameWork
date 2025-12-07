#pragma once
#include "CMapModel.h"
NS_BEGIN(Engine)

class ENGINE_DLL CMapInteractObject :
    public CMapModel
{
public:
    enum class InteractionType
    {
        CAVEROCK,ROCK, LAWN, GRASS, END
    };
public:
    typedef struct MapInteraction_DESC : CMapModel::MAPMODEL_DESC
    {
        _uint                   eInteractionType = 0;


    }MapInteraction_DESC;


protected:
    CMapInteractObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapInteractObject(const CMapInteractObject& rhs);
    virtual ~CMapInteractObject() = default;


public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    void        Update_Priority(_float fTimeDelta);
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

public:
    virtual void Save_To_Json(json& Json) override;
    static CMapInteractObject* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);

    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    virtual     void    Free() override;
private:
    _uint               m_eInteractionType = 0;

};

NS_END