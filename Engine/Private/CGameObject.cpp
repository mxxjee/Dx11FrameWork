#include "CGameObject.h"
#include "CGameInstance.h"
#include "CComponent.h"
#include "MathUtils.h"


CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& rhs)
    :m_pDevice(rhs.m_pDevice),
    m_pContext(rhs.m_pContext),
    m_pGameInstance(CGameInstance::GetInstance()),
    m_pTransformCom(nullptr)
{

    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{

    return S_OK; 
}

HRESULT CGameObject::Initialize_Copytype(void* pArg)
{
  
    /*컴포넌트 생성과 컴포넌트에게 desc를 보냄*/
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    /*리소스관련이나 desc를 멤버에 저장*/
    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGameObject::Update_Priority(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Update_Late(_float fTimeDelta)
{
    if (m_pTransformCom)
        m_pTransformCom->Update_Matrix();
}

void CGameObject::Update_Render(_float fTimeDelta)
{
   
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

HRESULT CGameObject::Ready_Components(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
    CTransform::TRANSFORM_DESC tempDesc;   // 임시 생성용
    CComponent::COMPONENT_DESC* compDesc = nullptr;

    //TransformDesc가 없으면 새로 만든다
    if (!pDesc->TransformDesc)
        pDesc->TransformDesc = &tempDesc;

  
    compDesc = static_cast<CComponent::COMPONENT_DESC*>(pDesc->TransformDesc);
    compDesc->pOwner = this;

    // Transform 생성 및 추가
    CComponent* pTransform = dynamic_cast<CTransform*>(m_pGameInstance->Clone_Prototype(
            PROTOTYPE::COMPONENT,
            0,
            PROTO_COMPONENT_NAME(L"Transform"),
            pDesc->TransformDesc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::TRANSFORM,
        pTransform,
        reinterpret_cast<CComponent**>(&m_pTransformCom)
    )))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameObject::Ready_Resource(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    m_pTarget=pDesc->pTarget;
    tag = pDesc->ObjTag;

    return S_OK;
}

HRESULT CGameObject::Add_Component(COMPONENT_TYPE eType, CComponent* pComp, CComponent** pOut)
{
    auto iter = m_Components.find(eType);
    if (iter != m_Components.end())
        return E_FAIL;

    m_Components.emplace(eType, pComp);
    *pOut = pComp;
    pComp->Set_Owner(this);

    Safe_AddRef(pComp);

    return S_OK;
}

CComponent* CGameObject::Get_Component(COMPONENT_TYPE eType)
{
    auto iter = m_Components.find(eType);
    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}

#ifdef  _DEBUG

void CGameObject::Render_Transform_Imgui()
{
    ImGui::Text("Name : %s", WStringToUTF8(tag).c_str());
    CheckNull(m_pTransformCom);

	ImGui::Separator();

	_float4 s, t, r;

	XMStoreFloat4(&s, m_pTransformCom->Get_SRT(SRTType::SCALE));
	XMStoreFloat4(&t, m_pTransformCom->Get_SRT(SRTType::TRANSFORM));
	XMStoreFloat4(&r, m_pTransformCom->Get_SRT(SRTType::ROTATION));



	_float3 rResult = MathUtils::QuaternionToEuler(XMLoadFloat4(&r));

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
	ImGui::BulletText("Position X:%f, Y:%f, Z:%f", t.x, t.y, t.z);
	ImGui::PopStyleColor();

	ImGui::Separator();
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::BulletText("Scale X:%f, Y:%f, Z:%f", s.x, s.y, s.z);
	ImGui::PopStyleColor();

	ImGui::Separator();
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
	ImGui::BulletText("Rotation X:%f, Y:%f, Z:%f",
		rResult.x,
		rResult.y,
		rResult.z);
	ImGui::PopStyleColor();


}


#endif //  _DEBUG

void CGameObject::Free()
{
    __super::Free();
    Safe_Release(m_pTransformCom);

    for (auto& pair : m_Components)
        Safe_Release(pair.second);

    Safe_Release(m_pGameInstance);
}
