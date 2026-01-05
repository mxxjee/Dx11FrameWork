#include "CEffectPoolManager.h"
#include "CEffect.h"
#include "CGameInstance.h"


IMPLEMENT_SINGLETON(CEffectPoolManager)

USING(Client)



HRESULT CEffectPoolManager::Initialize()
{
    m_pGameInstance = CGameInstance::GetInstance();
    m_ClonDatas.clear();

    return S_OK;
}

CEffect* CEffectPoolManager::Request_Spawn(const wstring& ProtoTag, void* pArg)
{
    //1. 먼저 풀에있는지 조사한다.태그로
    CEffect::EFFECT_DESC* pEffectDesc = static_cast<CEffect::EFFECT_DESC*>(pArg);
    _uint Hash = hash<wstring>()(pEffectDesc->DataName);

    auto iter = m_ClonDatas.find(Hash);
    //있다면, 비활성화되어있는것을 리턴한다.
    if (iter != m_ClonDatas.end())
    {
        for (auto& pObj : iter->second)
        {
            if (!pObj->Is_Active())
            {

                if (pObj->Get_SceneID() != m_pGameInstance->Get_CurrentLevelID())
                {
                    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_CurrentLevelID(), L"Particle_Layer", pObj)))
                        return nullptr;

                    pObj->Set_SceneID(m_pGameInstance->Get_CurrentLevelID());
                }
                pObj->Set_Active(true);
                return pObj;
            }

        }

    }


	//새로생성해서 리턴
    CGameObject::GAMEOBJECT_DESC* pDesc = static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);
    pDesc->ObjTag = pDesc->ObjTag + to_wstring(m_ClonDatas[Hash].size());


	CEffect* pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(ProtoTag), pArg));
	pEffect->Set_SceneID(m_pGameInstance->Get_CurrentLevelID());

   
	if (pEffect)
	{
       
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_CurrentLevelID(), L"Particle_Layer", pEffect)))
        {
            Safe_Release(pEffect);
            return nullptr;

        }
        Safe_AddRef(pEffect);//풀에저장하므로 addref
		m_ClonDatas[Hash].push_back(pEffect);

         Safe_Release(pEffect);
		return pEffect;

	}

   
    return nullptr;
}

HRESULT CEffectPoolManager::Request_Return(CEffect* pObj)
{
    CheckNullResult(pObj, E_FAIL);
    pObj->Set_Active(false);

    return S_OK;
}

void CEffectPoolManager::Free()
{
    for (auto& pair : m_ClonDatas)
    {
        for (auto& pObj : pair.second)
        {
            Safe_Release(pObj);
        }
    
    }

    m_ClonDatas.clear();
}
