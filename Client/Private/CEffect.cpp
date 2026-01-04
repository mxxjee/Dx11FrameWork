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


HRESULT CEffect::Render()
{
	__super::Render();
	if (FAILED(m_pShader->Bind_Vector("g_TintColor", m_LocalData.vColor)))
		return E_FAIL;

	return S_OK;
}

void CEffect::Spawn(const _float4x4* pSocketMatrix, const _float4x4* pParentMatrix)
{
}

void CEffect::Play()
{
}

void CEffect::Stop()
{
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
}

void CEffect::Make_LocalMatrix()
{
	_matrix Scaling = XMMatrixScaling(m_LocalData.InitScale.x,
		m_LocalData.InitScale.y,
		m_LocalData.InitScale.z);

	_float Roll = XMConvertToRadians(m_LocalData.InitRotation.x);
	_float Pitch = XMConvertToRadians(m_LocalData.InitRotation.y);
	_float Yaw = XMConvertToRadians(m_LocalData.InitRotation.z);

	_matrix Rotation = XMMatrixRotationRollPitchYaw(Roll, Pitch, Yaw);

	_matrix Translate = XMMatrixTranslation(m_LocalData.InitOffSet.x,
		m_LocalData.InitOffSet.y,
		m_LocalData.InitOffSet.z);

	LocalMatrix = Scaling * Rotation * Translate;


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

	return S_OK;
}

void CEffect::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CEffect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CEffect::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

    _float4x4 CombinedMatrix;

    if (m_pParentMatrix)
        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix,
            XMLoadFloat4x4(m_pParentMatrix)));

    else
        CombinedMatrix = *m_pTransformCom->Get_WorldMatrixPtr();

    m_pTransformCom->Set_WorldMatrix(CombinedMatrix);
}


void CEffect::Render_DebugImgui()
{
	__super::Render_DebugImgui();
    if (ImGui::ColorEdit4("Color", (float*)&m_LocalData.vColor))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
    }

    if (ImGui::DragFloat4("InitOffSet", (float*)&m_LocalData.InitOffSet))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
        //Update_Matrix();
    }

    if (ImGui::DragFloat4("InitRotation", (float*)&m_LocalData.InitRotation))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
        // Update_Matrix();
    }

    if (ImGui::DragFloat4("InitScale", (float*)&m_LocalData.InitScale))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
        // Update_Matrix();
    }

    if (ImGui::DragFloat("LifeTime", (float*)&m_LocalData.fLifeTime))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::DragFloat("Speed", (float*)&m_LocalData.fSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::Checkbox("Loop", (bool*)&m_LocalData.m_bLoop))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::Button("Play"))
        Play();


    if (ImGui::Button("Stop"))
        Stop();


    if (ImGui::Button("Save"))
    {
        m_pEffectData_Manager->Save_To_Json(m_DataName, m_LocalData);


    }
}
void CEffect::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);
}
