#pragma once

#include "CEffect.h"

namespace Engine
{
    class CShader;
    class CTexture;
};

struct TrailNode
{
	_float3 vTop;
	_float3 vBottom;
	_float	m_fTime = 0.f;
};

NS_BEGIN(Client)
class CTrailEffect
    :public CEffect
{
public:
    struct TrailDesc: CEffect::EFFECT_DESC
    {
        _float3 TopOffset = _float3(0.f, 1.5f,0.f);
        _float3 BottomOffSet= _float3(0.f, 0.f, 0.f);

        _float fWidth = 1.f;

        _float fLifeTime = 0.3f;

    };
protected:
    CTrailEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTrailEffect(const CTrailEffect& rhs);
    virtual ~CTrailEffect() = default;

#ifdef _DEBUG
public:
    virtual void        Render_DebugImgui();
#endif // _DEBUG


public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

public:
    static CTrailEffect* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext,void *pArg);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


private:
    deque<TrailNode> m_TrailList;

    //따라다닐 행렬 포인터
    const _float4x4*        m_pWeaponMatrix = nullptr; // 무기의 월드 행렬
    const _float4x4*        m_ParentMatrix; // (필요하다면 플레이어 월드 행렬도)

    ComPtr<ID3D11Buffer> m_pVB = nullptr;

private:
    HRESULT         Ready_Component();
private:
    _uint       m_iMaxRect = 50;//몇개그릴건지?
    _float      m_fWidth = 1.f;    //넙이
    _float      m_fLifeTime = 3.f;

public:
    void Start_Trail(const _float4x4* pWeaponWorldMatrix,const _float4x4* ParentMatrix);
    void Stop_Trail();

private:
    _float3 m_fTopOffSet;
    _float3 m_fBottomOffSet;

private:
    _uint           m_iVertexStride = {};       //정점 구조체 하나의 크기
    CTexture* m_pTexture = nullptr;



};

NS_END
