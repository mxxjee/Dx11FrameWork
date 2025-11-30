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

    void                        Clear_Terrains();

public:
    CTerrain_Base* Check_Picking();
    CTerrain_Base* Get_PickTerrain() { return m_pPickTerrain; }
    float           Get_PickDist() { return m_fPickDist; }


public:
    HRESULT            Save_All_Terrains(const string& path,int iNum=-1);
    HRESULT             Load_Terrains_MapTool(const string& LoadPath);
    const vector<tagLoadTerrainData>& Load_Terrains_Runtime(const string& LoadPath);

    void                        RequestDestroy(CTerrain_Base* pObj);
    void                        ProcessDestroy();

public:
    static CTerrain_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
   virtual void                Free() override;
    _float3             Get_PickingWorldPos() { return PickingWolrdPos; }
    
    const UMap<_wstring, CTerrain_Base*>& Get_TerrainMap() { return m_TerrainMap;}
private:
    ComPtr<ID3D11Device>		m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };

    UMap<_wstring, CTerrain_Base*>      m_TerrainMap;
    queue<CTerrain_Base*>                m_DestroyQueue;


private:
    ENGINE_DESC             m_EngineDesc;
    Triangle                 PickLocalTriangle;
    _float3                    PickingWolrdPos;     //terrain픽킹한 월드좌표

    CTerrain_Base*          m_pPickTerrain = nullptr;

    float                   m_fPickDist = 0.f;
    class                   CImGui_Manager* m_pImguiManager = { nullptr };
private:
    vector<LOADTERRAINDATA>         LoadDatas;



};


NS_END
