#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CCollider_Base:
    public CComponent
{
protected:
    CCollider_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCollider_Base(const CCollider_Base& Prototype);
    virtual ~CCollider_Base() = default;

#ifdef DEBUG

    virtual     HRESULT Render();

#endif // DEBUG

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);

public:
    virtual      HRESULT   Update_Collider(class CTransform* pTransform)=0;
    virtual      bool   Intersects_Ray(_vector origin, _vector rayDir, _float& Dist,class CTransform* pTransform)=0;


#ifdef _DEBUG
public:
    HRESULT             Render();
private:
    PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
    BasicEffect* m_pEffect = { nullptr };
    ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif // _DEBUG

public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;

protected:
    COLLIDER_TYPE            m_eType;      //콜라이더 타입,어느충돌?
    _matrix                  m_WolrdMatrix;      //오너의 월드매트릭스
    _bool                    m_bActive = true;   //활성화 여부
    _bool                    m_bDebugDraw = false;
    class                    CBounding* m_pBounding = { nullptr };     //생성할 바운딩박스 

};
NS_END

