#include "CUICreator.h"
#include "CUI.h"
#include "CGameInstance.h"
#include "MathUtils.h"
#include "CFontUI.h"


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
            _vector OffSet = XMVectorSet(0.f, 40.f, 0.f, 0.f);

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

HRESULT UICreator::Create_Loading_UI(wstring LayerTag)
{
    CUI::tagUIDesc        Desc = {};

    Desc.ObjTag = L"Loading_UI";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    Desc.TextureKey = L"loadingUI";

    Desc.iIdx = 0;

    Desc.fSizeX = 320.f*0.7f;
    Desc.fSizeY = 320.f * 0.7f;
    Desc.fX = 1100.f;
    Desc.fY = 600.f;

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
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bLoop = true;
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bAutoDisable = false;
    UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bPingpong = true;

    Desc.UICompDesc = &UIDesc;





    CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Desc);
    if (pObj)
    {
        CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::LOADING), LayerTag, pInstance)))
            return E_FAIL;

        CUI* pUI = dynamic_cast<CUI*>(pObj);
        if (pUI)
            pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA);

    }

    return S_OK;
}
