#pragma once
#include "CBase.h"
#include "CTerrain_Base.h"

/*맵툴에서 설치한 터레인(FILED) 모델들 관리함..!!!*/

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

    //픽킹한 삼각형의 점리턴 ( 로컬좌표)
    Triangle*                    PickTerrain(const _wstring& Key);

public:
    static CTerrain_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
   virtual void                Free() override;
    _float3             Get_PickingWorldPos() { return PickingWolrdPos; }
    
    const UMap<_wstring, CTerrain_Base*>& Get_TerrainMap() { return m_TerrainMap;}
    CTerrain_Base* Get_SelectObject() { return m_pSelectObject; }
private:
    ComPtr<ID3D11Device>		m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };

    UMap<_wstring, CTerrain_Base*>      m_TerrainMap;

    CTerrain_Base* m_pSelectObject = nullptr;

private:
    ENGINE_DESC             m_EngineDesc;
    Triangle                 PickLocalTriangle;
    _float3                    PickingWolrdPos;     //terrain픽킹한 월드좌표

};


NS_END
