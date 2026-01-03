#pragma once
#include "CGameObject.h"
#include "Client_Defines.h"
#include "EffectData.h"

namespace Engine
{
    class CShader;
}

NS_BEGIN(Client)
class CEffectData_Manager;
class CEffect :
    public CGameObject
{
public:
    typedef struct tagEffectDesc : public CGameObject::tagGameObjectDesc
    {
        wstring      ShaderName = L"Default";
        _uint eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);

    }EFFECT_DESC;


#ifdef _DEBUG
public:
    virtual void        Render_DebugImgui();
#endif // _DEBUG


protected:
    CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CEffect(const CEffect& rhs);
    virtual ~CEffect() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    CShader* Get_Shader() { return m_pShader; }


public:
    const EffectData* Get_EffectData() { return &m_LocalData; }

public:

    virtual void    Spawn(const _float4x4* pSocketMatrix = nullptr, const _float4x4* pParentMatrix = nullptr);
    virtual void    Play();
    virtual void     Stop();
    virtual void     Free() override;
protected:
    EffectData              m_LocalData;
    CEffectData_Manager* m_pEffectData_Manager = nullptr;

protected:
    CShader*            m_pShader = { nullptr };
    _uint                   m_eRenderGroup = 0;
    float                   m_fTime = 0.f;      //현재 파티클 진행시간
    bool                    m_bStop = false;
};

NS_END