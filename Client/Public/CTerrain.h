#pragma once
#include "CTerrain_Base.h"

/*맵툴에서 파싱한 오브젝트타입이 Terrain 인경우 이걸로 생성한다.*/
namespace Engine
{
    class CVIBuffer_Terrain;
    class CShader;
    class CTexture;
    class CBody;
}

NS_BEGIN(Client)
class CTerrain final:
    public CTerrain_Base
{
public:
    typedef struct tagTerrainObjectDesc :CGameObject::GAMEOBJECT_DESC
    {
        void* BodyDesc = nullptr;
        _float2 vIndex = _float2(0.f, 0.f);

    }TERRAINOBJECT_DESC;

protected:
    CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTerrain(const CTerrain& Prototype);
    virtual ~CTerrain() = default;

public:
    virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
    
    virtual void Update_Priority(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render();


private:
    HRESULT     Ready_Components(void* pArg);
    HRESULT     Ready_PartObjects(void* pArg);

public:
    static CTerrain* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

public:
    void Update_Render_MiniMapPriority() override;
private:
    CBody* m_pBody = nullptr;

};

NS_END

