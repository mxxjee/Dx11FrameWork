#pragma once
#include "CEffect.h"
#include "EffectData.h"

NS_BEGIN(Engine)
class CVIBuffer_Particle_Point;
class CTexture;
NS_END

NS_BEGIN(Client)
class CParticle :
    public CEffect
{
public:
    typedef struct ParticleDesc :public CEffect::EFFECT_DESC
    {
       
        string    passName = "Default";
    }PARTICLE_DESC;
private:
    //CPU상에서 시뮬레이션할 개별 입자들에 대한 정보
    struct PARTICLE_INFO
    {
        _bool       bAlive = false;
        _float      fAge = 0.f;
        _float      fLifeTime = 0.f;

        _float3         vPos;//현재위치(로컬)
        _float3         vDir;//이동방향
        _float         fSpeed;


        _float          fRotation;
        _float          fRotationSpeed;


        _float2             vSizeStart;
        _float2             vSizeEnd;

    };
protected:
    CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CParticle(const CParticle& rhs); 
    virtual ~CParticle() = default;



private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Copytype(void* pArg) override;

    virtual void    Update(_float fTimeDelta) override;
    virtual void    Update_Late(_float fTimeDelta) override;
    virtual void    Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void    Play() override;
    virtual void    Stop() override;

#ifdef _DEBUG
    virtual void    Render_DebugImgui() override;
#endif
protected:
    virtual HRESULT         Bind_ShaderResources();
    void        Reset_Textures();   //텍스처 key바뀔때마다 호출(원래이썬텍스쳐 교체)

private:
    class CVIBuffer_Particle_Point* m_pVIBufferCom = nullptr;
    class CTexture* m_Textures[ENUM_TO_UINT(EFFECT_TEXTYPE::END)] = { nullptr };


    // 편의를 위해 m_pDataRef를 ParticleData*로 캐스팅해서 저장해둘 포인터
    ParticleEffetData* m_pParticleData = nullptr;

    // 입자 상태 관리 풀
    vector<PARTICLE_INFO>   m_ParticlePool;

private:
    HRESULT Ready_Components();
    HRESULT Ready_Resource();

    void    Reset_All_Particles();             // 전체 풀 초기화 (ImGui Apply / Play 시)
    void    Reset_Single_Particle(PARTICLE_INFO& tParticle); // 개별 입자 리스폰

    void        Render_TextureList();
public:
    static CParticle* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

protected:
    string                  m_PassName = "";

    bool            m_bNeedToReset = false;
    ParticleEffetData   m_LocalData;
    bool            m_bNew = false;
};
NS_END

