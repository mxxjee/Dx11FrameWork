#include "CMonster.h"
#include "CModel.h"
#include "CGameInstance.h"

USING(Client)
CMonster::CMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CModelObject(pDevice, pContext), iHp(0),iMaxHp(0)
{
}

CMonster::CMonster(const CMonster& rhs)
    : CModelObject(rhs),
    iHp(rhs.iHp),
    iMaxHp(rhs.iMaxHp)
{
}

HRESULT CMonster::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

   

    return S_OK;
}

HRESULT CMonster::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_pGameInstance->Random(0.0f, 20.f),
        3.0f,
        m_pGameInstance->Random(0.0f, 20.f),
        1.f
    ));
    
    int Rand = rand() % m_pModel->Get_NumAnim();
    m_pModel->Set_Animation(Rand, true);
    return S_OK;
}

void CMonster::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


    m_pModel->Play_Animation(fTimeDelta);
}

void CMonster::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMonster::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CMonster::Render()
{
    __super::Render();



    return S_OK;
}

HRESULT CMonster::Ready_Resource(void* pArg)
{

    CheckNullResult(pArg, E_FAIL);
    MONSTER_DESC* pMonsterDesc = static_cast<MONSTER_DESC*>(pArg);

    iMaxHp = pMonsterDesc->MaxHp;
    iHp = iMaxHp;

    iAttack = pMonsterDesc->MaxHp;


    return S_OK;
}

CMonster* CMonster::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMonster* pInstance = new CMonster(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMonster ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
    CMonster* pInstance = new CMonster(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMonster ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMonster::Free()
{
  
    __super::Free();
}
