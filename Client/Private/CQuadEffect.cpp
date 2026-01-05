#include "CQuadEffect.h"
#include "CEffectData_Manager.h"
#include "CVIBuffer_Rect.h"
#include "CShader.h"
#include "CTexture.h"
#include "CEffectPoolManager.h"
#include "CLayer.h"


USING(Client)


CQuadEffect::CQuadEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CEffect(pDevice,pContext)
{
}

CQuadEffect::CQuadEffect(const CQuadEffect& rhs)
    :CEffect(rhs)
{
    m_pTexture = nullptr;
    m_pVIBufferCom = nullptr;

}

HRESULT CQuadEffect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CQuadEffect::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;


    EffectData* pData = m_pEffectData_Manager->Find_Data(m_DataName);
    if (pData)
    {
        m_LocalData = *pData;
        m_pTransformCom->Rotation(_float3(m_LocalData.InitRotation.x, m_LocalData.InitRotation.y, m_LocalData.InitRotation.z));
        m_pTransformCom->Set_Scale(m_LocalData.InitScale);

        _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
        m_pTransformCom->Set_State(STATE::POSITION, vPos + XMVectorSetW(XMLoadFloat4(&m_LocalData.InitOffSet), 0.f));

    

    }
    /*spawn시에 필요한 행렬만들어주기*/

  //  Update_Matrix();
    Make_LocalMatrix();

    return S_OK;
}

void CQuadEffect::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CQuadEffect::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckTrue(m_bStop);
    m_fProgress += fTimeDelta*m_LocalData.fSpeed;
    m_fTime += fTimeDelta;

    if(m_fTime>=m_LocalData.fLifeTime)
    {
        if (!m_LocalData.m_bLoop)
        {
            //서서히사라지기..
            m_fAlpha -= fTimeDelta * m_fFadeOutSpeed;
            if (m_fAlpha <= 0)
            {
                //풀에게돌려주고, 리스트에서삭제
                Stop();
                m_pEffectPool_Manager->Request_Return(this);
                Set_Active(false);

            }
               
        }
    }
}

void CQuadEffect::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CQuadEffect::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CQuadEffect::Render()
{
    __super::Render();
    
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;          //IA단계

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CQuadEffect::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0", 0)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Float("g_Alpha", m_fAlpha)))
        return E_FAIL;


    _float4 fProgress = _float4(m_fProgress, 0.f, 0.f, 0.f);
    m_pGameInstance->CopyData_Buffer("LoadingBuffer", &fProgress, sizeof(_float4));


    return S_OK;
}

HRESULT CQuadEffect::Ready_Component(void* pArg)
{
    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), pArg));
    if (FAILED(__super::Add_Component(COMPONENT_TYPE::VIBUFFER_RECT, pBuffer, (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;


    return S_OK;
}

HRESULT CQuadEffect::Ready_Resource(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    CQuadEffect::QUADEFFECT_DESC* pQuad_Desc = static_cast<QUADEFFECT_DESC*>(pArg);

    m_passName = pQuad_Desc->PassName;

    if (pQuad_Desc->TextureKey != L"")
    {
        m_pTexture = m_pGameInstance->Find_Texture(pQuad_Desc->TextureKey);
        if (m_pTexture)
            Safe_AddRef(m_pTexture);

    }

    return S_OK;
}


CQuadEffect* CQuadEffect::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CQuadEffect* pInstance = new CQuadEffect(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CQuadEffect ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

CGameObject* CQuadEffect::Clone(void* pArg)
{
    CQuadEffect* pInstance = new CQuadEffect(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CQuad ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CQuadEffect::Free()
{
    Safe_Release(m_pTexture);


    Safe_Release(m_pVIBufferCom);
    __super::Free();
 
}

void CQuadEffect::Set_Texture(const _wstring& NewTexKey)
{
    CTexture* pNewTex = m_pGameInstance->Find_Texture(NewTexKey);
    if (pNewTex)
    {
        Safe_Release(m_pTexture);
        m_pTexture = pNewTex;
    }
}


void CQuadEffect::Spawn(const _float4x4* pSocketMatrix, const _float4x4* pParentMatrix)
{
    Set_Active(true);

    m_bStop = false;
    m_fTime = 0.f;
    m_fAlpha = 1.f;
    m_fProgress = 1.f;
}

void CQuadEffect::Play()
{
    Set_Active(true);

    m_bStop = false;
    m_fTime = 0.f;
    m_fAlpha = 1.f;
    m_fProgress = 1.f;

}

void CQuadEffect::Stop()
{
    m_bStop = true;

    m_fTime = 0.f;
    m_fAlpha = 1.f;
    m_fProgress = 0.f;
}


void CQuadEffect::Render_DebugImgui()
{
    __super::Render_DebugImgui();
  

}