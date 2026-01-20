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
        m_LocalData = *(static_cast<MeshEffectData*>(pData));
        m_pTransformCom->Rotation(_float3(m_LocalData.InitRotation.x, m_LocalData.InitRotation.y, m_LocalData.InitRotation.z));
        m_pTransformCom->Set_Scale(m_LocalData.InitScale);

        _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
        m_pTransformCom->Set_State(STATE::POSITION, vPos + XMVectorSetW(XMLoadFloat4(&m_LocalData.InitOffSet), 0.f));



    }

    m_pDataRef = &m_LocalData;
    

    CurrentScale = m_pDataRef->InitScale;
    CurrentMove = m_pDataRef->InitOffSet;
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
   
    bool m_bAnimated = m_LocalData.bUseMoveAnim || m_LocalData.bUseScaleAnim || m_LocalData.bUseRotationAnim;

    if (m_LocalData.bUseScaleAnim)
    {
        //진행도 누적
        ScaleLerpTime += fTimeDelta * m_LocalData.fScaleSpeed;
        if (ScaleLerpTime > 1.f) ScaleLerpTime = 1.f;

        //보간 값 계산
        XMStoreFloat4(&CurrentScale, XMVectorLerp(
            XMLoadFloat4(&m_LocalData.InitScale),
            XMLoadFloat4(&m_LocalData.TargetScale),
            ScaleLerpTime));

        //루프 처리 (끝나면 다시 0으로)
        if (ScaleLerpTime >= 1.f && m_LocalData.m_bLoop)
            ScaleLerpTime = 0.f;



    }

    if (m_LocalData.bUseMoveAnim)
    {
        //진행도 누적
        MoveLerpTime += fTimeDelta * m_LocalData.fMoveSpeed;
        if (MoveLerpTime > 1.f) MoveLerpTime = 1.f;

        //보간 값 계산


        _vector vMoveDir = XMVector4Normalize(XMLoadFloat4(&m_LocalData.vMoveDir));

        //originmarix가있다면 그 look방향 * 내가갈방향
        if (!XMMatrixIsIdentity(XMLoadFloat4x4(&OriginMatrix)))
        {
            _vector vLook = XMLoadFloat4x4(&OriginMatrix).r[ENUM_TO_UINT(STATE::LOOK)];
            vMoveDir = vLook * XMVector4Normalize(XMLoadFloat4(&m_LocalData.vMoveDir));

        }

        XMStoreFloat4(&CurrentMove, XMVectorLerp(XMLoadFloat4(&m_LocalData.InitOffSet),
            XMLoadFloat4(&m_LocalData.InitOffSet) * vMoveDir,
            MoveLerpTime));

        
        XMStoreFloat4(&CurrentMove, XMVectorSetW(XMLoadFloat4(&CurrentMove), 1.f));
       

        //루프 처리 (끝나면 다시 0으로)
        if (MoveLerpTime >= 1.f && m_LocalData.m_bLoop)
            MoveLerpTime = 0.f;


    }

    if (!m_bAnimated)
    {
        //애니메이션을 안 쓸 때는 실시간으로 InitScale을 따라가야 ImGui 수정이 반영됨!
        CurrentScale = m_LocalData.InitScale;
        ScaleLerpTime = 0.f;
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
    __super::Spawn();

    Set_Active(true);

    m_fTime = 0.f;
    m_fAlpha = 1.f;
    m_fProgress =0.f;
    ScaleLerpTime = 0.f;

    m_bStop = false;
}

void CQuadEffect::Play()
{
    __super::Play();
    Set_Active(true);

    m_fTime = 0.f;
    m_fAlpha = 1.f;
    m_fProgress = 0.f;

    ScaleLerpTime = 0.f;

    Make_LocalMatrix();

    if (m_pParentMatrix && m_pSocketMatrix)
    {
        //먼저 socket*m_pParentMatrix

        _matrix SocketWorld = XMMatrixMultiply(XMLoadFloat4x4(m_pSocketMatrix),
            XMLoadFloat4x4(m_pParentMatrix));

        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix, SocketWorld));
    }

    //ParentMAtrix만있을경우
    else if (m_pParentMatrix)
    {
        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix, XMLoadFloat4x4(m_pParentMatrix)));

    }



    else
    {

        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix, XMLoadFloat4x4(&OriginMatrix)));

    }

    m_pTransformCom->Set_WorldMatrix(CombinedMatrix);
    m_bStop = false;
}

void CQuadEffect::Stop()
{
    
    __super::Stop();

    m_bStop = true;

}


#ifdef _DEBUG

void CQuadEffect::Render_DebugImgui()
{
    if (ImGui::Checkbox("bUseScaleAnim", (bool*)&m_LocalData.bUseScaleAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

    }

    if (ImGui::DragFloat4("TargetScale", (float*)&m_LocalData.TargetScale))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    if (ImGui::DragFloat("ScaleSpeed", (float*)&m_LocalData.fScaleSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    ///////////
    if (ImGui::Checkbox("bUseRotationAnim", (bool*)&m_LocalData.bUseRotationAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

    }

    if (ImGui::DragFloat4("RotationAxis", (float*)&m_LocalData.vRotationAxis))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    if (ImGui::DragFloat("RotationSpeed", (float*)&m_LocalData.fRotationSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }
    //////////////
    if (ImGui::Checkbox("bUseMoveAnim", (bool*)&m_LocalData.bUseMoveAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

    }

    if (ImGui::DragFloat4("MoveDir", (float*)&m_LocalData.vMoveDir))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    if (ImGui::DragFloat("MoveSpeed", (float*)&m_LocalData.fMoveSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }

    __super::Render_DebugImgui();
    //////////////////////

    /////////////
}
#endif // _DEBUG
