#include "CUICreator.h"
#include "CUI.h"
#include "CGameInstance.h"
#include "MathUtils.h"


CGameInstance* UICreator::m_pGameInstance = CGameInstance::GetInstance();

HRESULT UICreator::Create_HeartGroup(wstring LayerTag)
{
    UIGroup     HeartGroup;
    HeartGroup.Key = L"HeartGroup";

    for (int i = 0; i < 5; ++i)
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"Hp_UI" + to_wstring(i);
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"Hp";

        Desc.iIdx = i;

        Desc.fSizeX = 38.f;
        Desc.fSizeY = 38.f;
        Desc.fX = 50.f + (i * 45.f);
        Desc.fY = 50.f;

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.fRotationPerSec = 10.f;
        TransDesc.fSpeedPerSec = 5.f;
        Desc.TransformDesc = &TransDesc;

        //AlphaAnim등록
        CUIComponent::UICOMP_DESC UIDesc = {};
        /*    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].fStart = _float4(Desc.fX, Desc.fY, 1.f, 1.f);
            UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].fTarget = _float4(Desc.fX - 10.f, Desc.fY, 1.f, 1.f);
            UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].m_fSpeed = 0.1f;
            UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].bLoop = true;*/

        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fStart = _float4(1.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fTarget = _float4(0.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].m_fSpeed = 5.f;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bLoop = false;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bAutoDisable = true;

        Desc.UICompDesc = &UIDesc;





        CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Desc);
        if (pObj)
        {
            CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
                return E_FAIL;


            HeartGroup.Objects.push_back(pInstance);

        }

    }

    m_pGameInstance->Register_UIGroup(HeartGroup);
    m_pGameInstance->RegisterEvent(L"OnHeartDamaged", [](void* pData)
        {
            int* iHp = reinterpret_cast<int*>(pData);
            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"HeartGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i);
                    if (pUI)
                    {
                        if (pUI->Get_Idx() == (*iHp))
                        {
                            pUI->Set_ActiveAnim(1, [pUI]()
                                {
                                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA);


                                    // pUI->Get_UIComp()->PlayAnim(UIAnimType::POSITION);
                                });


                            pUI->OnActivated(false);
                        }
                    }
                }
            }
        });

	return S_OK;
}

HRESULT UICreator::Create_Interaction_UI(wstring LayerTag)
{
    UIGroup     Interaction_PopUP_CarryGroup;
    Interaction_PopUP_CarryGroup.Key = L"Interaction_PopUp_Carry";

    wstring ObjTags[] = { L"Interact_Carry_BG",L"Interaction_A_Carry"};
    wstring TextureKeys[] = { L"Interact_BG", L"Interaction_A_Carry"};

    for (int i = 0; i < sizeof(ObjTags)/sizeof(ObjTags[0]); ++i)
    {
        //BG
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = ObjTags[i];
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

        Desc.TextureKey = TextureKeys[i];

        Desc.iIdx = 0;

        Desc.fSizeX = 200.f*0.8f;
        Desc.fSizeY = 90.f*0.8f;
        Desc.fX = g_iWinSizeX >> 1;
        Desc.fY = g_iWinSizeY >> 1;
        Desc.Depth = 0.5f - (0.01f * i);

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.fRotationPerSec = 10.f;
        TransDesc.fSpeedPerSec = 5.f;
        Desc.TransformDesc = &TransDesc;

        //AlphaAnim등록
        CUIComponent::UICOMP_DESC UIDesc = {};
        Desc.UICompDesc = &UIDesc;

        CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Desc);
        if (pObj)
        {
            CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
                return E_FAIL;


            Interaction_PopUP_CarryGroup.Objects.push_back(pInstance);

        }
    }
  
    m_pGameInstance->Register_UIGroup(Interaction_PopUP_CarryGroup);
    m_pGameInstance->SetActiveGroup(Interaction_PopUP_CarryGroup.Key, false);

    m_pGameInstance->RegisterEvent(L"OnCarryUIShow", [](void* pData)
        {
            _vector* pPos = static_cast<_vector*>(pData);
            _vector OffSet = XMVectorSet(0.f, 100.f, 0.f, 0.f);

            (*pPos) += OffSet;

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUp_Carry");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i);
                    if (pUI)
                    {
                        
                        pUI->Get_Transform()->Set_State(STATE::POSITION, 
                            MathUtils::ScreenToWorld_UI((*pPos),g_iWinSizeX,g_iWinSizeY));

						pUI->Set_ActiveAnim(0, [pUI]()
							{
                                pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false);
							});

                        if(!pUI->Is_Active())
						    pUI->OnActivated(true);

					}
                }
            }
        });

    m_pGameInstance->RegisterEvent(L"OnCarryUIHide", [](void* pData)
        {
            
            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUp_Carry");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i);
                    if (pUI)
                    {
                     pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f),  20.f, false, true);

                    }
                }
            }
        });
    return S_OK;
}

HRESULT UICreator::Create_Interaction_TalkUI(wstring LayerTag)
{
    UIGroup     Interaction_PopUP_TalkGroup;
    Interaction_PopUP_TalkGroup.Key = L"Interaction_PopUP_TalkGroup";

    wstring ObjTags[] = { L"Interact_Talk_BG",L"Interaction_A_Talk" };
    wstring TextureKeys[] = { L"Interact_BG",L"Interaction_A_Talk" };

    for (int i = 0; i < sizeof(ObjTags) / sizeof(ObjTags[0]); ++i)
    {
        //BG
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = ObjTags[i];
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

        Desc.TextureKey = TextureKeys[i];

        Desc.iIdx = 0;

        Desc.fSizeX = 200.f * 0.8f;
        Desc.fSizeY = 90.f * 0.8f;
        Desc.fX = g_iWinSizeX >> 1;
        Desc.fY = g_iWinSizeY >> 1;
        Desc.Depth = 0.5f - (0.01f * i);

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.fRotationPerSec = 10.f;
        TransDesc.fSpeedPerSec = 5.f;
        Desc.TransformDesc = &TransDesc;

        //AlphaAnim등록
        CUIComponent::UICOMP_DESC UIDesc = {};
        Desc.UICompDesc = &UIDesc;

        CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Desc);
        if (pObj)
        {
            CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
                return E_FAIL;


            Interaction_PopUP_TalkGroup.Objects.push_back(pInstance);

        }
    }

    m_pGameInstance->Register_UIGroup(Interaction_PopUP_TalkGroup);
    m_pGameInstance->SetActiveGroup(Interaction_PopUP_TalkGroup.Key, false);

    m_pGameInstance->RegisterEvent(L"OnTalkUIShow", [](void* pData)
        {
            _vector* pPos = static_cast<_vector*>(pData);
            _vector OffSet = XMVectorSet(0.f, 40.f, 0.f, 0.f);

            (*pPos) += OffSet;

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_TalkGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i);
                    if (pUI)
                    {

                        pUI->Get_Transform()->Set_State(STATE::POSITION,
                            MathUtils::ScreenToWorld_UI((*pPos), g_iWinSizeX, g_iWinSizeY));

                        pUI->Set_ActiveAnim(0, [pUI]()
                            {
                                pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false);
                            });

                        if (!pUI->Is_Active())
                            pUI->OnActivated(true);

                    }
                }
            }
        });

    m_pGameInstance->RegisterEvent(L"OnTalkUIHide", [](void* pData)
        {

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_TalkGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i);
                    if (pUI)
                    {
                        pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 20.f, false, true);

                    }
                }
            }
        });
    return S_OK;
}
