#pragma once
#include "CMapObject.h"

namespace Engine
{
    class CVIBuffer_Triangle;
    class CShader;
}


NS_BEGIN(MapTool)
class CTerrain_Highlight :
    public CMapObject
{
public:
    typedef struct tagHighlightDesc : CMapObject::MapObject_DESC
    {
       
        class CGameObject* pOwner = nullptr;
        void* TriangleBuffer = nullptr;

    }HIGHLIGHT_DESC;

private:
    CTerrain_Highlight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTerrain_Highlight(const CTerrain_Highlight& Prototype);
    virtual ~CTerrain_Highlight() = default;

public:
    virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
    virtual void Update_Priority(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render();

public:
    static CTerrain_Highlight* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

public:
    Triangle Get_Triangle() { return m_Triangle; }

private:
    HRESULT Ready_Component(void* pArg);
    HRESULT Ready_Resources(void* pArg);
    HRESULT Bind_ShaderResources();
private:
    CVIBuffer_Triangle*     m_pTriangleBuffer=nullptr;
    
private:


    Triangle            m_Triangle;
};
NS_END

