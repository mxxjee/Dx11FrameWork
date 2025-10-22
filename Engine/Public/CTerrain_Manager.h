#pragma once
#include "CBase.h"
#include "CTerrain_Base.h"

NS_BEGIN(Engine)
class CTerrain_Manager :
    public CBase
{

private:
    CTerrain_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CTerrain_Manager() = default;

public:
    HRESULT         Register_Terrain(const _wstring& Key, CTerrain_Base* pTerrain);
    HRESULT         UnRegister_Terrain(const _wstring& Key);


    void            Update_Priority(_float fTimeDelta);
    void            Update(_float fTimeDelta);
    void            Update_Late(_float fTimeDelta);
    void            Update_Render(_float fTimeDelta);

    CTerrain_Base*          Find_Terrain(const _wstring& Key);
    bool                    PickTerrain(const _wstring& Key);

public:
    static CTerrain_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual void                Free() override;


private:
    ComPtr<ID3D11Device>		m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };

    UMap<_wstring, CTerrain_Base*>      m_TerrainMap;


private:
    ENGINE_DESC             m_EngineDesc;
};


NS_END
