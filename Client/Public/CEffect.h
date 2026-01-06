#pragma once
#include "CGameObject.h"
#include "Client_Defines.h"
#include "EffectData.h"

namespace Engine
{
    class CShader;
}

NS_BEGIN(Client)

class CEffectPoolManager;
class CEffectData_Manager;


class CEffect :
    public CGameObject
{
public:
    typedef struct tagEffectDesc : public CGameObject::tagGameObjectDesc
    {
        wstring      ShaderName = L"Default";
        _uint eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        wstring      DataName = L"";

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
    _uint       Get_SceneID() { return m_iSceneID; }

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
    CEffectPoolManager* m_pEffectPool_Manager = nullptr;

public:
    _matrix        Get_LocalMatrix() { return LocalMatrix; }
    _float4x4       Get_LocalMatrix_ByFloat4x4() { _float4x4 Result; XMStoreFloat4x4(&Result, LocalMatrix); return Result; }

    void        Set_ParentMatrix(const _float4x4* pParentMatrix) { m_pParentMatrix = pParentMatrix; }
    void        Set_SocketMatrix(const _float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }
protected:
    CShader*            m_pShader = { nullptr };
    _uint                   m_eRenderGroup = 0;
    float                   m_fTime = 0.f;      //현재 파티클 진행시간
    bool                    m_bStop = false;

    wstring             m_DataName = L"";

    _matrix             LocalMatrix;

protected:
    void        Make_LocalMatrix();

protected:
    _float              m_fAlpha = 1.f;
    _float              m_fFadeOutSpeed = 2.f;
    _float              m_fProgress = 1.f;

    const _float4x4* m_pParentMatrix = nullptr;
    const _float4x4* m_pSocketMatrix = nullptr;


    _float      ScaleLerpTime = 0.f;
    _float4     CurrentScale;
    _float4x4   CombinedMatrix;
};

NS_END