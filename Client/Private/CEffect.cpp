#include "CEffect.h"
#include "CEffectData_Manager.h"
#include "CShader.h"
#include "CEffectPoolManager.h"


USING(Client)

CEffect::CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect::Initialize_Prototype()
{

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;



    return S_OK;
}

HRESULT CEffect::Initialize_Copytype(void* pArg)
{
    EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

    m_pEffectData_Manager = CEffectData_Manager::GetInstance();
    m_pEffectPool_Manager = CEffectPoolManager::GetInstance();

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;
    m_eRenderGroup = pDesc->eRenderGroup;
    m_pShader = m_pGameInstance->Find_Shader(pDesc->ShaderName);
    m_DataName = pDesc->DataName;


    XMStoreFloat4x4(&CombinedMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&OriginMatrix, XMMatrixIdentity());

    /*spawn시에 필요한 행렬만들어주기*/

  //  Update_Matrix();


    return S_OK;
}
HRESULT CEffect::Render()
{
	__super::Render();
	if (FAILED(m_pShader->Bind_Vector("g_TintColor", m_pDataRef->vColor)))
		return E_FAIL;

    if (FAILED(m_pShader->Bind_Float("g_fIntensity", m_pDataRef->fIntensity)))
        return E_FAIL;

	return S_OK;
}

void CEffect::Spawn(const _float4x4* pSocketMatrix, const _float4x4* pParentMatrix)
{
    ScaleLerpTime = 0.f;
    MoveLerpTime = 0.f;


    CurrentScale = m_pDataRef->InitScale;
    CurrentMove = m_pDataRef->InitOffSet;
}

void CEffect::Play()
{
    ScaleLerpTime = 0.f;
    MoveLerpTime = 0.f;

    CurrentScale = m_pDataRef->InitScale;
    CurrentMove = m_pDataRef->InitOffSet;

}

void CEffect::Stop()
{
    m_fTime = 0.f;
    m_fAlpha = 1.f;
    m_fProgress = 0.f;
    ScaleLerpTime = 0.f;


}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
}

void CEffect::Make_LocalMatrix()
{
	_matrix Scaling = XMMatrixScaling(CurrentScale.x,
        CurrentScale.y,
        CurrentScale.z);

	_float Roll = XMConvertToRadians(m_pDataRef->InitRotation.x);
    _float Pitch = XMConvertToRadians(m_pDataRef->InitRotation.y);
    _float Yaw = XMConvertToRadians(m_pDataRef->InitRotation.z);

	_matrix Rotation = XMMatrixRotationRollPitchYaw(Roll, Pitch, Yaw);

	_matrix Translate = XMMatrixTranslation(CurrentMove.x,
        CurrentMove.y,
        CurrentMove.z);

	LocalMatrix = Scaling * Rotation * Translate;


}




void CEffect::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CEffect::Update(_float fTimeDelta)
{
    CheckTrue(m_bStop);
	__super::Update(fTimeDelta);

    m_fProgress += fTimeDelta * m_pDataRef->fSpeed;
    m_fTime += fTimeDelta;

    if (!m_bStop)
    {
        m_fTime += fTimeDelta;
        m_fProgress += fTimeDelta * m_pDataRef->fSpeed;

    }

    if (m_fTime >= m_pDataRef->fLifeTime)
    {
        if (!m_pDataRef->m_bLoop)
        {
            m_fAlpha -= fTimeDelta * m_fFadeOutSpeed;
            if (m_fAlpha <= 0)
            {
                Stop();
                m_pEffectPool_Manager->Request_Return(this);
                Set_Active(false);
            }
        }




    }
 
}

void CEffect::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

    CheckTrue(m_bStop);
    
    
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
}

#ifdef _DEBUG
void CEffect::Render_DebugImgui()
{
    CheckNull(m_pDataRef);

    const char* items[] = { "MESH", "PARTICLE" };
    int current_item = (int)m_pDataRef->eType; // 현재 내 타입

    if (ImGui::Combo("Effect Type", &current_item, items, IM_ARRAYSIZE(items)))
    {
        EFFECT_DESC_TYPE eNewType = (EFFECT_DESC_TYPE)current_item;
        m_pDataRef->eType = eNewType;
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

    }
	__super::Render_DebugImgui();
    if (ImGui::ColorEdit4("Color", (float*)&m_pDataRef->vColor))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);
        Make_LocalMatrix();
    }

    if (ImGui::DragFloat("Itensity", (float*)&m_pDataRef->fIntensity))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

    }

    if (ImGui::DragFloat4("InitOffSet", (float*)&m_pDataRef->InitOffSet))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);
        Make_LocalMatrix();
        //Update_Matrix();
    }

    if (ImGui::DragFloat4("InitRotation", (float*)&m_pDataRef->InitRotation))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);
        Make_LocalMatrix();
        // Update_Matrix();
    }

    if (ImGui::DragFloat4("InitScale", (float*)&m_pDataRef->InitScale))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);
        Make_LocalMatrix();
        CurrentScale = m_pDataRef->InitScale;

        // Update_Matrix();
    }

    if (ImGui::DragFloat("LifeTime", (float*)&m_pDataRef->fLifeTime))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

    }

    if (ImGui::DragFloat("Speed", (float*)&m_pDataRef->fSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

    }

    if (ImGui::Checkbox("Loop", (bool*)&m_pDataRef->m_bLoop))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

    }

   
    if (ImGui::Button("Play"))
        Play();


    if (ImGui::Button("Stop"))
        Stop();


    if (ImGui::Button("Save"))
    {
        m_pEffectData_Manager->Save_To_Json(m_DataName, m_pDataRef);


    }
}
#endif // _DEBUG
void CEffect::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);
}
