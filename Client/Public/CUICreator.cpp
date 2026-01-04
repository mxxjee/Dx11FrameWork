#include "CUICreator.h"
#include "CUI.h"
#include "CGameInstance.h"
#include "MathUtils.h"
#include "CFontUI.h"
#include "CInventory_Manager.h"
#include "CGameManager.h"
#include "CPlayer.h"
#include "CGameObject.h"
#include "CLoadingUI.h"



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

        Desc.fSizeX = 35.f;
        Desc.fSizeY = 38.f;
        Desc.fX = 50.f + (i * 35.f);
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


            HeartGroup.push_back(pInstance);

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
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
                    if (pUI)
                    {
                        if (pUI->Get_Idx() >= (*iHp))
                        {
                            if (pUI->Is_Active())
                            {
                                pUI->Set_ActiveAnim(1, [pUI]()
                                    {
                                        pUI->Get_UIComp()->PlayAnim(
                                            UIAnimType::ALPHA,
                                            _float4(1.f, 0.f, 0.f, 0.f), // Start: 보임
                                            _float4(0.f, 0.f, 0.f, 0.f), // Target: 안 보임 (투명)
                                            5.f,   // Speed
                                            false, // Loop
                                            true   // bAutoDisable: ★ 애니메이션 끝나면 자동으로 Set_Active(false) 해줌
                                        );
                                    });


                                pUI->OnActivated(false);
                            }
                         
                        } 
                    }
                }
            }
        });

    m_pGameInstance->RegisterEvent(L"OnHeartHealed", [](void* pData)
        {
            int* iHp = reinterpret_cast<int*>(pData);
            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"HeartGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
                    if (pUI)
                    {
                        int iTargetIdx = (int)pUI->Get_Idx();

                        // 내 인덱스가 현재 체력보다 작으면 (살아야 할 운명)
                        if (iTargetIdx < (*iHp))
                        {
                            //
                            if (!pUI->Is_Active())
                            {
                                pUI->Set_Active(true);

                                // 새로 살아나는 놈만 페이드 인 (0 -> 1)
                                pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA,
                                    _float4(0.f, 0.f, 0.f, 0.f), // Start
                                    _float4(1.f, 0.f, 0.f, 0.f), // Target
                                    5.f,  // Speed
                                    false, // Loop
                                    false // AutoDisable (켜지는 거니까 끄면 안 됨)
                                );
                            }
                            // 이미 켜져있는 놈은 아무것도 안 함 (가만히 둠)
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

        Desc.fSizeX = 200.f*0.7f;
        Desc.fSizeY = 90.f*0.7f;
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


            Interaction_PopUP_CarryGroup.push_back(pInstance);

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
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
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
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
                    if (pUI)
                    {
                        if(pUI->Is_Active())
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

        Desc.fSizeX = 200.f * 0.7f;
        Desc.fSizeY = 90.f * 0.7f;
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


            Interaction_PopUP_TalkGroup.push_back(pInstance);

        }
    }

    m_pGameInstance->Register_UIGroup(Interaction_PopUP_TalkGroup);
    m_pGameInstance->SetActiveGroup(Interaction_PopUP_TalkGroup.Key, false);

    m_pGameInstance->RegisterEvent(L"OnTalkUIShow", [](void* pData)
        {
            _vector* pPos = static_cast<_vector*>(pData);
            _vector OffSet = XMVectorSet(100.f, 40.f, 0.f, 0.f);

            (*pPos) += OffSet;

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_TalkGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
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
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
                    if (pUI)
                    {
                        pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 20.f, false, true);

                    }
                }
            }
        });
    return S_OK;
}

HRESULT UICreator::Create_Interaction_GetUI(wstring LayerTag)
{
    UIGroup     Interaction_PopUP_GetGroup;
    Interaction_PopUP_GetGroup.Key = L"Interaction_PopUP_GetGroup";

    wstring ObjTags[] = { L"Interact_Talk_BG",L"Interaction_A_Get" };
    wstring TextureKeys[] = { L"Interact_BG",L"Interaction_A_Get" };

    for (int i = 0; i < sizeof(ObjTags) / sizeof(ObjTags[0]); ++i)
    {
        //BG
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = ObjTags[i];
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

        Desc.TextureKey = TextureKeys[i];

        Desc.iIdx = 0;

        Desc.fSizeX = 200.f * 0.7f;
        Desc.fSizeY = 90.f * 0.7f;
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


            Interaction_PopUP_GetGroup.push_back(pInstance);

        }
    }

    m_pGameInstance->Register_UIGroup(Interaction_PopUP_GetGroup);
    m_pGameInstance->SetActiveGroup(Interaction_PopUP_GetGroup.Key, false);

    m_pGameInstance->RegisterEvent(L"OnGetUIShow", [](void* pData)
        {
            _vector* pPos = static_cast<_vector*>(pData);
            _vector OffSet = XMVectorSet(100.f, 40.f, 0.f, 0.f);

            (*pPos) += OffSet;

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_GetGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
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

    m_pGameInstance->RegisterEvent(L"OnGetUIHide", [](void* pData)
        {

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_GetGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
                    if (pUI)
                    {
                        pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 20.f, false, true);

                    }
                }
            }
        });
    return S_OK;
}

HRESULT UICreator::Create_Interaction_SeeUI(wstring LayerTag)
{
    /*조사 ui*/
    UIGroup     Interaction_PopUP_SeeGroup;
    Interaction_PopUP_SeeGroup.Key = L"Interaction_PopUP_SeeGroup";

    wstring ObjTags[] = { L"Interact_Talk_BG",L"Interaction_A_See" };
    wstring TextureKeys[] = { L"Interact_BG",L"Interaction_A_See" };

    for (int i = 0; i < sizeof(ObjTags) / sizeof(ObjTags[0]); ++i)
    {
        //BG
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = ObjTags[i];
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

        Desc.TextureKey = TextureKeys[i];

        Desc.iIdx = 0;

        Desc.fSizeX = 200.f * 0.7f;
        Desc.fSizeY = 90.f * 0.7f;
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


            Interaction_PopUP_SeeGroup.push_back(pInstance);

        }
    }

    m_pGameInstance->Register_UIGroup(Interaction_PopUP_SeeGroup);
    m_pGameInstance->SetActiveGroup(Interaction_PopUP_SeeGroup.Key, false);

    m_pGameInstance->RegisterEvent(L"OnSeeUIShow", [](void* pData)
        {
            _vector* pPos = static_cast<_vector*>(pData);
            _vector OffSet = XMVectorSet(100.f, 40.f, 0.f, 0.f);

            (*pPos) += OffSet;

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_SeeGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
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

    m_pGameInstance->RegisterEvent(L"OnSeeUIHide", [](void* pData)
        {

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_SeeGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
                    if (pUI)
                    {
                        pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 20.f, false, true);

                    }
                }
            }
        });
    return S_OK;

}

HRESULT UICreator::Create_Interaction_OpenUI(wstring LayerTag)
{
    /*조사 ui*/
    UIGroup     Interaction_PopUP_OpenGroup;
    Interaction_PopUP_OpenGroup.Key = L"Interaction_PopUP_Open_Group";

    wstring ObjTags[] = { L"Interact_Talk_BG",L"Interaction_A_Open" };
    wstring TextureKeys[] = { L"Interact_BG",L"Interaction_A_Open" };

    for (int i = 0; i < sizeof(ObjTags) / sizeof(ObjTags[0]); ++i)
    {
        //BG
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = ObjTags[i];
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

        Desc.TextureKey = TextureKeys[i];

        Desc.iIdx = 0;

        Desc.fSizeX = 200.f * 0.7f;
        Desc.fSizeY = 90.f * 0.7f;
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


            Interaction_PopUP_OpenGroup.push_back(pInstance);

        }
    }

    m_pGameInstance->Register_UIGroup(Interaction_PopUP_OpenGroup);
    m_pGameInstance->SetActiveGroup(Interaction_PopUP_OpenGroup.Key, false);

    m_pGameInstance->RegisterEvent(L"OnOpenUIShow", [](void* pData)
        {
            _vector* pPos = static_cast<_vector*>(pData);
            _vector OffSet = XMVectorSet(100.f, 40.f, 0.f, 0.f);

            (*pPos) += OffSet;

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_Open_Group");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
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

    m_pGameInstance->RegisterEvent(L"OnOpenUIHide", [](void* pData)
        {

            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"Interaction_PopUP_Open_Group");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i.second);
                    if (pUI)
                    {
                        pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 20.f, false, true);

                    }
                }
            }
        });
    return S_OK;
}

HRESULT UICreator::Create_NPC_Dialogue_UI(wstring LayerTag)
{
#pragma region NPC_DialogueBox
    UIGroup NPC_DialogueBox;
    NPC_DialogueBox.Key = L"NPC_DialogueBox";

#pragma region 박스만들기
    _float OriginY = (g_iWinSizeY >> 1) + 200.f;

    CUI::tagUIDesc        DialogueBoxDesc = {};

    DialogueBoxDesc.ObjTag = L"DialogueBox";
    DialogueBoxDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    DialogueBoxDesc.TextureKey = L"DialogueBox_3";

    DialogueBoxDesc.iIdx = 0;

    DialogueBoxDesc.fSizeX = 600.f;
    DialogueBoxDesc.fSizeY = 250.f * 0.8f;
    DialogueBoxDesc.fX = g_iWinSizeX >> 1;
    DialogueBoxDesc.fY = OriginY;


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    DialogueBoxDesc.TransformDesc = &TransDesc;

    //AlphaAnim등록
    CUIComponent::UICOMP_DESC  UIDesc = {};
    DialogueBoxDesc.UICompDesc = &UIDesc;

    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &DialogueBoxDesc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
            return E_FAIL;


        NPC_DialogueBox.push_back(pInstance);

    }
#pragma endregion
#pragma region 말하는 폰트만들기
    ///폰트먼저만들기
    CFontUI::FONTUI_DESC FontUIDesc;
    FontUIDesc.FontName = L"Dialogue_Default";
    FontUIDesc.vDefaultFontColor = _float4(1.f, 1.f, 1.f, 0.5f);
    FontUIDesc.ObjTag = L"Dialogue_Text";
    FontUIDesc.fSizeX = 0.5f;
    FontUIDesc.fSizeY = 0.5f;
    FontUIDesc.m_bUseTypingEffect = true;
    FontUIDesc.vPosition = _float2(638.58f, 544.80f);

    FontUIDesc.fX = DialogueBoxDesc.fX + 25.f;
    FontUIDesc.fY = OriginY + 600.f;

    FontUIDesc.Depth = 0.5f - (0.01f);

    FontUIDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

    TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    FontUIDesc.TransformDesc = &TransDesc;

    UIDesc = {};
    FontUIDesc.UICompDesc = &UIDesc;

    pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FontUI"), &FontUIDesc);
    CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
        return E_FAIL;


    NPC_DialogueBox.push_back(pInstance);


    wstring Text = L"안녕하세요";

    //폰트 이벤트 바인딩
    CGameInstance::GetInstance()->RegisterEvent(L"UpdateNPCText", [](void* pData)
        {
            wstring* Text = static_cast<wstring*>(pData);

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"NPC_DialogueBox");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"Dialogue_Text");
                CheckNull(pObj);

                CFontUI* pText = dynamic_cast<CFontUI*>(pObj);
                CheckNull(pText);
                pText->Set_Text(*Text);
              
            }
        });
#pragma endregion
#pragma region 이름폰트만들기

    CFontUI::FONTUI_DESC NameUIDesc;

    NameUIDesc.FontName = L"Zelda_Default";
    NameUIDesc.vDefaultFontColor = _float4(0.8f, 0.8f, 0.8f, 1.f);
    NameUIDesc.ObjTag = L"Dialogue_Text_Name";
    NameUIDesc.fSizeX = 0.4f;
    NameUIDesc.fSizeY = 0.4f;

    NameUIDesc.vPosition = _float2(389.58f, 472.22f);

    NameUIDesc.fX = DialogueBoxDesc.fX - (DialogueBoxDesc.fX * 0.35f);
    NameUIDesc.fY = OriginY + 300.f;

    NameUIDesc.Depth = 0.5f - (0.01f);

    NameUIDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

    TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    NameUIDesc.TransformDesc = &TransDesc;

    UIDesc = {};
    NameUIDesc.UICompDesc = &UIDesc;

    pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FontUI"), &NameUIDesc);
    pInstance = dynamic_cast<CGameObject*>(pObj);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
        return E_FAIL;


    NPC_DialogueBox.push_back(pInstance);


    //폰트 이벤트 바인딩
    CGameInstance::GetInstance()->RegisterEvent(L"UpdateNPCName", [](void* pData)
        {
            wstring* Text = static_cast<wstring*>(pData);

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"NPC_DialogueBox");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"Dialogue_Text_Name");
                CheckNull(pObj);

                CFontUI* pText = dynamic_cast<CFontUI*>(pObj);
                CheckNull(pText);
                pText->Set_Text(*Text);

            }
            
   
        });



#pragma endregion

#pragma region arrow icon
    CUI::tagUIDesc        ArrowIconDesc = {};

    ArrowIconDesc.ObjTag = L"Dialogue_Arrow";
    ArrowIconDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    ArrowIconDesc.TextureKey = L"Dialogue_Arrow";

    ArrowIconDesc.iIdx = 0;

    ArrowIconDesc.fSizeX = 72 * 0.8f;
    ArrowIconDesc.fSizeY = 54 * 0.8f;
    ArrowIconDesc.fX = g_iWinSizeX >> 1;
    ArrowIconDesc.fY = OriginY + 100.f;


    TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    ArrowIconDesc.TransformDesc = &TransDesc;

    //AlphaAnim등록
    UIDesc = {};
    ArrowIconDesc.UICompDesc = &UIDesc;

    pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &ArrowIconDesc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
            return E_FAIL;


        NPC_DialogueBox.push_back(pInstance);

    }
#pragma endregion

    m_pGameInstance->Register_UIGroup(NPC_DialogueBox);
    m_pGameInstance->SetActiveGroup(L"NPC_DialogueBox", false);



#pragma region 대화상자 등장/끝 이벤트
    m_pGameInstance->RegisterEvent(L"OnDialogueUIShow", [](void* pData)
        {

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"NPC_DialogueBox");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    if (pObj->Get_Tag() == L"Dialogue_Arrow")
                        continue;

                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Set_ActiveAnim(0, [pUI]()
                        {
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false);
                        });

                    if (!pUI->Is_Active())
                        pUI->OnActivated(true);
                }
              
            }
        });

    m_pGameInstance->RegisterEvent(L"OnDialogueUIHide", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"NPC_DialogueBox");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
  
                    CUI* pUI = dynamic_cast<CUI*>(pair.second);
                    if (pair.second->Get_Tag().find(L"Text") != wstring::npos)
                        pair.second->Set_Active(false);


                    CheckNull(pUI);

                 
                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true);
                      

                }

            }
        });
  
    /*대화모두 출력후 화살표나오는 이벤트*/
    m_pGameInstance->RegisterEvent(L"OnDialogueArrowShow", [](void* pData)
        {

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"NPC_DialogueBox");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"Dialogue_Arrow");
                CheckNull(pObj);
                
                CUI* pUI = dynamic_cast<CUI*>(pObj);
                CheckNull(pUI);

                pUI->Set_ActiveAnim(0, [pUI]()
                    {
                        pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, true, false, true);
                    });
                if (!pUI->Is_Active())
                    pUI->OnActivated(true);
            }
        });

    m_pGameInstance->RegisterEvent(L"OnDialogueArrowHide", [](void* pData)
        {

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"NPC_DialogueBox");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"Dialogue_Arrow");
                CheckNull(pObj);

                CUI* pUI = dynamic_cast<CUI*>(pObj);
                CheckNull(pUI);

              
               pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true, false);
                  
 
            }
        });

#pragma endregion


   //화살표이벤트 연결
    CGameObject* pTarget = NPC_DialogueBox.Find(L"Dialogue_Text");
    CheckNullResult(pTarget,E_FAIL);

    CFontUI* pFontUI = dynamic_cast<CFontUI*>(pTarget);
    CheckNullResult(pFontUI,E_FAIL);

    //대화텍스트출력다됐을때, arrow 활성화ㅐ
    pFontUI->Set_FontEndFunction([]()
        {
            CGameInstance::GetInstance()->BroadCastEvent(L"OnDialogueArrowShow",nullptr);
        });

    pFontUI->Set_FontStartFunction([]()
        {
            CGameInstance::GetInstance()->BroadCastEvent(L"OnDialogueArrowHide",nullptr);
        });


    return S_OK;
}

HRESULT UICreator::Create_Loading_UI(wstring LayerTag, vector<CLoadingUI*>& UIVec)
{
    float       m_fOriginX = g_iWinSizeX-100.f;
    float       m_fOriginY = g_iWinSizeY-100.f;

    float       m_fOffsetX = 200.f;



    /// 채우는 UI
    for (int i = 0; i < 1; ++i)
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"Loading_UI" + to_wstring(i);
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"MarkerWarp_00^w";


        Desc.iIdx = i;

        Desc.fSizeX = 320.f*0.5f;
        Desc.fSizeY = 320.f*0.5f;

        Desc.fX = m_fOriginX;
        Desc.fY = m_fOriginY;
        Desc.Depth = 0.009f;


        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.fRotationPerSec = 10.f;
        TransDesc.fSpeedPerSec = 5.f;
        Desc.TransformDesc = &TransDesc;

        //AlphaAnim등록
        CUIComponent::UICOMP_DESC UIDesc = {};
        

        Desc.UICompDesc = &UIDesc;

        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fStart = _float4(0.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fTarget = _float4(1.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].m_fSpeed = 2.f;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bLoop = true;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bAutoDisable = false;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bPingpong = true;


        CBase* pFindObj = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::LOADING), L"UI_Layer", Desc.ObjTag);
        if (!pFindObj)
        {
            CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Loading"), &Desc);
            if (pObj)
            {
                CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
                if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::LOADING), LayerTag, pInstance)))
                    return E_FAIL;

                CLoadingUI* pUI = dynamic_cast<CLoadingUI*>(pObj);
                UIVec.push_back(pUI);

            }
        }

        else
        {
            CLoadingUI* pUI = dynamic_cast<CLoadingUI*>(pFindObj);
            UIVec.push_back(pUI);
        }

        

    }
    return S_OK;
}

HRESULT UICreator::Create_ItemGet_UI(wstring LayerTag)
{
    ///아이템 아이콘
    //BG
    UIGroup             ItemGetIcon;    //아이콘,뒤에 반짝거리는거? 이펙트인가? 일단 그룹으로 만들어놓기.
    ItemGetIcon.Key = L"ItemGetIconGroup";

    CUI::tagUIDesc        IconDesc = {};

    IconDesc.ObjTag = L"ItemGet_Icon";
    IconDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

    IconDesc.TextureKey = L"Shield";

    IconDesc.iIdx = 0;

    IconDesc.fSizeX = 120.f;
    IconDesc.fSizeY = 120.f;
    IconDesc.fX = g_iWinSizeX >> 1;
    IconDesc.fY = g_iWinSizeY >> 1;
    IconDesc.Depth = 0.4f;

    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    IconDesc.TransformDesc = &TransDesc;

    //AlphaAnim등록
    CUIComponent::UICOMP_DESC UIDesc = {};
    IconDesc.UICompDesc = &UIDesc;

    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &IconDesc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
            return E_FAIL;


        ItemGetIcon.push_back(pInstance);

    }

    m_pGameInstance->Register_UIGroup(ItemGetIcon);
    m_pGameInstance->SetActiveGroup(ItemGetIcon.Key, false);

                                                            //텍스처 키 보내주기
    m_pGameInstance->RegisterEvent(L"OnItemGetIconShow", [](void* pData)
        {
            CInventory_Manager::ItemGetEvent* ItemEvent = static_cast<CInventory_Manager::ItemGetEvent*>(pData);
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"ItemGetIconGroup");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"ItemGet_Icon");
                if (pObj)
                {
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                   
                    //위치설정(플레이어 기준 오프셋)
                    CPlayer* pPlayer=CGameManager::GetInstance()->Get_MainPlayer();
                   
                    
                    _vector OffSet = XMVectorSet(ItemEvent->OffSet.x,ItemEvent->OffSet.y,ItemEvent->OffSet.z,1.f);
                    _vector vOrigin = pPlayer->Get_Transform()->Get_State(STATE::POSITION);
                    _vector ShowPos = MathUtils::WorldToScreen(vOrigin,
                        m_pGameInstance->Get_ViewMatrix(0), m_pGameInstance->Get_ProjMatrix(0), g_iWinSizeX, g_iWinSizeY);

                    pUI->Get_Transform()->Set_State(STATE::POSITION,
                        MathUtils::ScreenToWorld_UI(OffSet+ ShowPos, g_iWinSizeX, g_iWinSizeY));

                    pUI->Set_Texture(ItemEvent->TexKey);
                    
                 
                    pUI->Set_ActiveAnim(0, [pUI]()
                        {
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false, false);
                            
                            //스케일애니메이션 재생안됨 확인
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::SCALE, _float4(0.1f, 0.1f, 0.1f, 0.f), _float4(120*1.5f, 120.f*1.5f, 1.f, 0.f),10.f, false, false, false);

                        });
                    if (!pUI->Is_Active())
                        pUI->OnActivated(true);
                      

                }
            }
        });

    m_pGameInstance->RegisterEvent(L"OnItemGetIconHide", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"ItemGetIconGroup");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"ItemGet_Icon");
                if (pObj)
                {
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true, false);
                    pUI->Get_UIComp()->PlayAnim(UIAnimType::SCALE, _float4(120.f * 0.7f, 120.f * 0.7f, 1.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true, false);

                }
            }
        });
    //아이템설명UI
    return S_OK;
}

HRESULT UICreator::Create_ItemGet_Desc_UI(wstring LayerTag)
{

    UIGroup ItemGetDescGroup;
    ItemGetDescGroup.Key = L"ItemGetDescGroup";
    

#pragma region 박스만들기
    _float OriginY = (g_iWinSizeY >> 1) + 250.f;

    CUI::tagUIDesc        ItemGetFrameDesc = {};

    ItemGetFrameDesc.ObjTag = L"ItemGetFrame";
    ItemGetFrameDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    ItemGetFrameDesc.TextureKey = L"KeyGetFame2";

    ItemGetFrameDesc.iIdx = 0;

    ItemGetFrameDesc.fSizeX = 1166 * 0.7f;
    ItemGetFrameDesc.fSizeY = 365.f * 0.7f;
    ItemGetFrameDesc.fX = g_iWinSizeX >> 1;
    ItemGetFrameDesc.fY = OriginY;


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    ItemGetFrameDesc.TransformDesc = &TransDesc;

    //AlphaAnim등록
    CUIComponent::UICOMP_DESC  UIDesc = {};
    ItemGetFrameDesc.UICompDesc = &UIDesc;

    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &ItemGetFrameDesc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
            return E_FAIL;


        ItemGetDescGroup.push_back(pInstance);

    }
#pragma endregion
#pragma region 말하는 폰트만들기
    ///폰트먼저만들기
    CFontUI::FONTUI_DESC FontUIDesc;
    FontUIDesc.FontName = L"Dialogue_Default";
    FontUIDesc.vDefaultFontColor = _float4(1.f, 1.f, 1.f, 0.5f);
    FontUIDesc.ObjTag = L"ItemGetDesc_Text";
    FontUIDesc.fSizeX = 0.5f;
    FontUIDesc.fSizeY = 0.5f;
    FontUIDesc.m_bUseTypingEffect = true;
    FontUIDesc.vPosition = _float2(638.58f, 580.80f);

    FontUIDesc.fX = ItemGetFrameDesc.fX + 25.f;
    FontUIDesc.fY = OriginY + 50.f;
    FontUIDesc.m_bRandomColor = true;
    FontUIDesc.Depth = 0.5f - (0.01f);

    FontUIDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

    TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    FontUIDesc.TransformDesc = &TransDesc;

    UIDesc = {};
    FontUIDesc.UICompDesc = &UIDesc;

    pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FontUI"), &FontUIDesc);
    CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
        return E_FAIL;


    ItemGetDescGroup.push_back(pInstance);
    m_pGameInstance->Register_UIGroup(ItemGetDescGroup);



    //폰트 이벤트 바인딩
    CGameInstance::GetInstance()->RegisterEvent(L"UpdateItemDescText", [](void* pData)
        {
            CInventory_Manager::ItemGetEvent* pItemEvent = static_cast<CInventory_Manager::ItemGetEvent*>(pData);
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"ItemGetDescGroup");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"ItemGetDesc_Text");
                CheckNull(pObj);

                CFontUI* pText = dynamic_cast<CFontUI*>(pObj);
                CheckNull(pText);
                pText->Set_Text(pItemEvent->ItemDesc);

            }
        });
#pragma endregion

    ////////////활성/비활성화 

    m_pGameInstance->RegisterEvent(L"OnItemDescUIShow", [](void* pData)
        {
            CInventory_Manager::ItemGetEvent* ItemEvent = static_cast<CInventory_Manager::ItemGetEvent*>(pData);
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"ItemGetDescGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Set_ActiveAnim(0, [pUI]()
                        {
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false);
                        });

                    if (!pUI->Is_Active())
                        pUI->OnActivated(true);
                }

            }
        });

    m_pGameInstance->RegisterEvent(L"OnItemDescUIHide", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"ItemGetDescGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    if (pObj->Get_Tag() == L"ItemGetDesc_Text")
                    {
                        pObj->Set_Active(false);
                        continue;

                    }
                        
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true, false);

                }
            

            }
        });


    m_pGameInstance->SetActiveGroup(ItemGetDescGroup.Key, false);

    
    return S_OK;
}

HRESULT UICreator::Create_See_Desc_UI(wstring LayerTag)
{
    UIGroup SeeDescGroup;
    SeeDescGroup.Key = L"SeeDescGroup";


#pragma region 박스만들기
    _float OriginY = (g_iWinSizeY >> 1) + 250.f;

    CUI::tagUIDesc        ItemSeeFrameDesc = {};

    ItemSeeFrameDesc.ObjTag = L"ItemSeeFrameDesc";
    ItemSeeFrameDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    ItemSeeFrameDesc.TextureKey = L"See_BG";

    ItemSeeFrameDesc.iIdx = 0;

    ItemSeeFrameDesc.fSizeX = 1814 * 0.3f;
    ItemSeeFrameDesc.fSizeY = 759 * 0.3f;
    ItemSeeFrameDesc.fX = g_iWinSizeX >> 1;
    ItemSeeFrameDesc.fY = OriginY;


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    ItemSeeFrameDesc.TransformDesc = &TransDesc;

    //AlphaAnim등록
    CUIComponent::UICOMP_DESC  UIDesc = {};
    ItemSeeFrameDesc.UICompDesc = &UIDesc;

    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &ItemSeeFrameDesc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
            return E_FAIL;


        SeeDescGroup.push_back(pInstance);

    }
#pragma endregion
#pragma region 말하는 폰트만들기
    ///폰트먼저만들기
    CFontUI::FONTUI_DESC FontUIDesc;
    FontUIDesc.FontName = L"Dialogue_Default";
    FontUIDesc.vDefaultFontColor = _float4(1.f, 1.f, 1.f, 0.5f);
    FontUIDesc.ObjTag = L"SeeDesc_Text";
    FontUIDesc.fSizeX = 0.5f;
    FontUIDesc.fSizeY = 0.5f;
    FontUIDesc.m_bUseTypingEffect = true;
    FontUIDesc.vPosition = _float2(638.58f, 580.80f);

    FontUIDesc.fX = ItemSeeFrameDesc.fX + 25.f;
    FontUIDesc.fY = OriginY + 50.f;
    FontUIDesc.Depth = 0.5f - (0.01f);

    FontUIDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);

    TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    FontUIDesc.TransformDesc = &TransDesc;

    UIDesc = {};
    FontUIDesc.UICompDesc = &UIDesc;

    pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FontUI"), &FontUIDesc);
    CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), LayerTag, pInstance)))
        return E_FAIL;


    SeeDescGroup.push_back(pInstance);
    m_pGameInstance->Register_UIGroup(SeeDescGroup);



    //폰트 이벤트 바인딩
    CGameInstance::GetInstance()->RegisterEvent(L"UpdateSeeDescText", [](void* pData)
        {
            wstring* pWText = static_cast<wstring*>(pData);

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"SeeDescGroup");
            if (pGroup)
            {
                CGameObject* pObj = pGroup->Find(L"SeeDesc_Text");
                CheckNull(pObj);

                CFontUI* pText = dynamic_cast<CFontUI*>(pObj);
                CheckNull(pText);
                pText->Set_Text((*pWText));

            }
        });
#pragma endregion

    ////////////활성/비활성화 

    m_pGameInstance->RegisterEvent(L"OnSeeDescUIShow", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"SeeDescGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Set_ActiveAnim(0, [pUI]()
                        {
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false);
                        });

                    if (!pUI->Is_Active())
                        pUI->OnActivated(true);
                }

            }
        });

    m_pGameInstance->RegisterEvent(L"OnSeeDescUIHide", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"SeeDescGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    if (pObj->Get_Tag() == L"SeeDesc_Text")
                    {
                        pObj->Set_Active(false);
                        continue;

                    }

                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true, false);

                }


            }
        });


    m_pGameInstance->SetActiveGroup(SeeDescGroup.Key, false);


    return S_OK;
}

HRESULT UICreator::Create_InvenSlot(wstring LayerTag)
{
    UIGroup InvenSlotGroup;
    InvenSlotGroup.Key = L"QuickSlotGroup";

    float fX = 100.f;
    float fY = g_iWinSizeY - 100.f;

#pragma region ItemSlotBackground
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"InvenSlot_Icon";
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"ItemSlot";

        Desc.iIdx = 0;

        Desc.fSizeX = 192.f * 0.7f;
        Desc.fSizeY = 192.f * 0.7f;
        Desc.fX = fX;
        Desc.fY = fY;
        Desc.Depth = 0.5f;
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


            InvenSlotGroup.push_back(pInstance);

        }
    }
  
#pragma endregion

#pragma region 아이템표시할 아이콘
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"Item_Icon";
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"Shield";

        Desc.iIdx = 0;

        Desc.fSizeX = 120.f * 0.7f;
        Desc.fSizeY = 120.f * 0.7f;
        Desc.fX = fX;
        Desc.fY = fY;
        Desc.Depth = 0.49f;

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


            CUI* pUI = dynamic_cast<CUI*>(pInstance);
            CheckNullResult(pUI,E_FAIL);

            InvenSlotGroup.push_back(pInstance);
        }
    }
#pragma endregion
#pragma region 슬롯X
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"Item_X";
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"Item_X";

        Desc.iIdx = 0;

        Desc.fSizeX = 192.f * 0.35f;
        Desc.fSizeY = 192.f * 0.35f;
        Desc.fX = fX-35.f;
        Desc.fY = fY+30.f;
        Desc.Depth = 0.49f;
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


            CUI* pUI = dynamic_cast<CUI*>(pInstance);
            CheckNullResult(pUI,E_FAIL);
            
            InvenSlotGroup.push_back(pInstance);

        }
    }
#pragma endregion
 
    ////////////활성/비활성화
    m_pGameInstance->RegisterEvent(L"OnShowInvenSlot", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"QuickSlotGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);
                    
                    pUI->Set_ActiveAnim(0, [pUI]()
                        {
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false);
                        });

                    if (!pUI->Is_Active())
                        pUI->OnActivated(true);
                }


            }
        });

    m_pGameInstance->RegisterEvent(L"OnHideInvenSlot", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"QuickSlotGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);


                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true, false);

                }


            }
        });


    m_pGameInstance->RegisterEvent(L"UpdateInvenSlotIcon", [](void* pData)
        {
            wstring* pItemSlotTexKey = static_cast<wstring*>(pData);
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"QuickSlotGroup");
            if (pGroup)
            {

                if (!pGroup->Is_Active())
                    m_pGameInstance->SetActiveGroup(pGroup->Key, true);

                CGameObject* pICon = pGroup->Find(L"Item_Icon");
                CheckNull(pICon);

                CUI* pUI = dynamic_cast<CUI*>(pICon);
                CheckNull(pUI);

                pUI->Set_Texture(*pItemSlotTexKey);
                  

            }
        });

    m_pGameInstance->RegisterEvent(L"CleanInvenSlotIcon", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"QuickSlotGroup");
            if (pGroup)
            {

                if (!pGroup->Is_Active())
                    m_pGameInstance->SetActiveGroup(pGroup->Key, true);

                CGameObject* pICon = pGroup->Find(L"Item_Icon");
                CheckNull(pICon);

                CUI* pUI = dynamic_cast<CUI*>(pICon);
                CheckNull(pUI);

                pUI->Set_Texture(L"");


            }
        });

    m_pGameInstance->Register_UIGroup(InvenSlotGroup);
    m_pGameInstance->SetActiveGroup(InvenSlotGroup.Key, false);

    return S_OK;
}

HRESULT UICreator::Create_InventorySceneLine(wstring LayerTag)
{
    UIGroup     InventoryLineGroup;
    InventoryLineGroup.Key = L"InventoryLineGroup";

    float fBeginfX = g_iWinSizeX>>1;
    float fBeginfY = 100.f;

    //슬롯크기
    float fSizeX = 1250.f;
    float fSizeY = 30.f;

    float fOffSetY = 500.f;

    for (int i = 0; i < 2; ++i)
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"InvenSlot_Icon" + to_wstring(i);
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"Inventory_Line";

        Desc.iIdx = i;

        Desc.fSizeX = fSizeX;
        Desc.fSizeY = fSizeY;
        Desc.fX = fBeginfX;
        Desc.fY = fBeginfY + (fOffSetY * i);

        Desc.Depth = 0.49f;
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
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::UI), LayerTag, pInstance)))
                return E_FAIL;


            InventoryLineGroup.push_back(pInstance);

        }
    }

    m_pGameInstance->Register_UIGroup(InventoryLineGroup);

    return S_OK;
}

HRESULT UICreator::Create_InventoryItemInfo(wstring LayerTag)
{
    CUI::tagUIDesc        Desc = {};

    _float fSizeX = 300 * 0.8f;
    _float fSizeY = 300 * 0.8f;

    Desc.ObjTag = L"Inven_ItemInfo";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    Desc.TextureKey = L"Shield";

    Desc.iIdx = 0;

    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;
    Desc.fX = (g_iWinSizeX >> 1) - 300.f;
    Desc.fY = (g_iWinSizeY >> 1) -40.f;

    Desc.Depth = 0.48f;
    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    Desc.TransformDesc = &TransDesc;

    //AlphaAnim등록
    CUIComponent::UICOMP_DESC UIDesc = {};
    Desc.UICompDesc = &UIDesc;

    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"UI_ItemInfo"), &Desc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::UI), LayerTag, pInstance)))
            return E_FAIL;

    }
    return S_OK;
    return S_OK;
}

HRESULT UICreator::Create_InventoryCursor(wstring LayerTag)
{
    CUI::tagUIDesc        Desc = {};

    _float fSizeX = 300 * 0.8f;
    _float fSizeY = 300 * 0.8f;

    Desc.ObjTag = L"InvenSlot_Cursor";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    Desc.TextureKey = L"ItemCursor";

    Desc.iIdx =0;

    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;
    Desc.fX = g_iWinSizeX>>1;
    Desc.fY = g_iWinSizeY >> 1;

    Desc.Depth = 0.47f;
    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    Desc.TransformDesc = &TransDesc;

    //AlphaAnim등록
    CUIComponent::UICOMP_DESC UIDesc = {};

    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::SCALE)].fStart = _float4(fSizeX, fSizeY, 1.f, 0.f);
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::SCALE)].fTarget = _float4(fSizeX*1.15f, fSizeY * 1.15f, 1.f, 0.f);
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::SCALE)].m_fSpeed = 20.f;
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::SCALE)].bLoop = true;
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::SCALE)].bAutoDisable = false;
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::SCALE)].bPingpong = true;

    Desc.UICompDesc = &UIDesc;

    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Cursor"), &Desc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::UI), LayerTag, pInstance)))
            return E_FAIL;

    }
    return S_OK;
}

HRESULT UICreator::Create_InventorySceneSlot(wstring LayerTag)
{
    UIGroup     InventorySlotGroup;
    InventorySlotGroup.Key = L"InventorySlotGroup";


    //그리드 시작 좌상단
    float fBeginfX = (g_iWinSizeX >> 1)+50;
    float fBeginfY = (g_iWinSizeY >> 1) -200.f;

    //슬롯크기
    float fSizeX = 192 * 0.63f;
    float fSizeY = 192 * 0.63f;

    float fOffSet = fSizeX*0.5f + 65.f;

    for (int i = 0; i < 4; ++i)
    {
        
        for (int j = 0; j < 4; ++j)
        {
            CUI::tagUIDesc        Desc = {};

           
            Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
            Desc.TextureKey = L"ItemSlot";

            Desc.iIdx = (i*4)+j;
            Desc.ObjTag = L"InvenSlot_Icon" + to_wstring(Desc.iIdx);

            Desc.fSizeX = fSizeX;
            Desc.fSizeY = fSizeY;
            Desc.fX = fBeginfX + (fOffSet*j);
            Desc.fY = fBeginfY+(fOffSet*i);

            Desc.Depth = 0.48f;
            CTransform::TRANSFORM_DESC TransDesc = {};
            TransDesc.fRotationPerSec = 10.f;
            TransDesc.fSpeedPerSec = 5.f;
            Desc.TransformDesc = &TransDesc;

            //AlphaAnim등록
            CUIComponent::UICOMP_DESC UIDesc = {};
            Desc.UICompDesc = &UIDesc;

            CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"InventorySlot"), &Desc);
            if (pObj)
            {
                CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
                if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::UI), LayerTag, pInstance)))
                    return E_FAIL;


                InventorySlotGroup.push_back(pInstance);

            }
        }

    }

    m_pGameInstance->Register_UIGroup(InventorySlotGroup);

    return S_OK;
}

HRESULT UICreator::Create_LevelUI(wstring LayerTag)
{
    UIGroup     LevelUIGroup;
    LevelUIGroup.Key = L"LevelUIGroup";


    //그리드 시작 좌상단
    float fBeginfX =  150.f;
    float fBeginfY = (g_iWinSizeY >> 1) - 200.f;

    //슬롯크기
    float fSizeX = 600 * 0.5f;
    float fSizeY = 300 * 0.5f;


	CUI::tagUIDesc        Desc = {};


	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
	Desc.TextureKey = L"";

	Desc.iIdx =0;
	Desc.ObjTag = L"Level_UI" + to_wstring(Desc.iIdx);

	Desc.fSizeX = fSizeX;
	Desc.fSizeY = fSizeY;
	Desc.fX = fBeginfX;
	Desc.fY = fBeginfY;

	Desc.Depth = 0.5f;
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


        LevelUIGroup.push_back(pInstance);



    }

    m_pGameInstance->Register_UIGroup(LevelUIGroup);

    m_pGameInstance->RegisterEvent(L"UpdateLevelUI", [](void* pData)
        {
            wstring* levelName= static_cast<wstring*>(pData);
         

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"LevelUIGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Set_Texture(*levelName);
                    pUI->Set_ActiveAnim(0, [pUI]()
                        {
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 5.f, false, false);
                        });

                    if (!pUI->Is_Active())
                        pUI->OnActivated(true);
                }

            }
        });

    m_pGameInstance->RegisterEvent(L"OnLevelUIShow", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"LevelUIGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Set_ActiveAnim(0, [pUI]()
                        {
                            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(0.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f), 10.f, false, false);
                        });

                    if (!pUI->Is_Active())
                        pUI->OnActivated(true);
                }

            }
        });

    m_pGameInstance->RegisterEvent(L"OnLevelUIHide", [](void* pData)
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"LevelUIGroup");
            if (pGroup)
            {
                for (auto& pair : pGroup->Objects)
                {
                    CGameObject* pObj = pair.second;
                    CUI* pUI = dynamic_cast<CUI*>(pObj);
                    CheckNull(pUI);

                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA, _float4(1.f, 0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), 10.f, false, true, false);

                }


            }
        });
    
    m_pGameInstance->SetActiveGroup(LevelUIGroup.Key, false);

    return S_OK;
}
