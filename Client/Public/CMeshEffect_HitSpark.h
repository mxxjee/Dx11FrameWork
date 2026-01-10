#pragma once
#include "CMeshEffect.h"

NS_BEGIN(Client)
class CMeshEffect_HitSpark :
    public CMeshEffect
{
public:
    typedef struct tagMeshEffect_HitSpark_Desc:public CMeshEffect::MESHEFFECT_DESC
    {
        bool    bUseParentRotation = false;


    }HITSPARK_DESC;
public:
    CMeshEffect_HitSpark(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMeshEffect_HitSpark(const CMeshEffect_HitSpark& rhs);
    virtual ~CMeshEffect_HitSpark() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();



public:
    static CMeshEffect_HitSpark* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual HRESULT         Bind_ShaderResources();
    virtual void    Play();
    virtual void     Stop();


private:
    void        Make_PartWorldMatrix();


#ifdef _DEBUG
public:
    virtual void        Render_DebugImgui();
#endif // _DEBUG

private:
    _float4     m_InitPosition[4];
    _float      m_InitRotationZ[4];
    _float      m_InitSpeed=1.f;


    _float4      m_InitScale=_float4(0.2f,0.2f,0.2f,1.f);
    _float4     m_CurrentPos[4];

    _matrix     m_LocalMatrix[4];
    _matrix     m_PartsWorldMatrix[4];

    int         iTargetIdx = 0;

    bool        m_bUseParentRotation = false;


};
NS_END
