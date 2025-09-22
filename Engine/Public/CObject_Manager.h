#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CObject_Manager :
    public CBase
{

private:
    CObject_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CObject_Manager() = default;


public:
    HRESULT             Initialize(_uint iNumLevels);
    void                Update_Priority(_float fTimeDelta);
    void                Update(_float fTimeDelta);
    void                Update_Late(_float fTimeDelta);

public:
    //원본 오브젝트를 찾아서 사본을 생성하고, 이를 레이어에 추가하는 함수.
    HRESULT Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag,
        _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);


private:
    class CLayer* Find_Layer(_uint iNumLevel, const _wstring& strLayerTag);

private:
    ComPtr<ID3D11Device>            m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>     m_pContext = { nullptr };
    class CGameInstance*             m_pGameInstance = { nullptr };

private:
    _uint                       m_iNumLevels = {};
    vector<unordered_map<_wstring, class CLayer*>> m_Layers;
private:
    static  CObject_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumLevels);
    virtual         void    Free() override;

};
NS_END

