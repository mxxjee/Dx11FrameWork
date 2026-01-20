#pragma once
#include "CMeshEffect.h"

NS_BEGIN(Client)
class CMeshEffect_RollCut :
    public CMeshEffect
{

public:
    struct Effect_RollCutDesc : public CMeshEffect::tagMeshEffectDesc
    {
        _float fRoationPerSec = 5.f;

    };
public:
    CMeshEffect_RollCut(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMeshEffect_RollCut(const CMeshEffect_RollCut& rhs);
    virtual ~CMeshEffect_RollCut() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();



public:
    static CMeshEffect_RollCut* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual HRESULT         Bind_ShaderResources();
    virtual void    Play();
    virtual void     Stop();



#ifdef _DEBUG
public:
    virtual void        Render_DebugImgui();
#endif // _DEBUG

private:
    _float fRotationPerSpeed = 5.f;

};
NS_END
