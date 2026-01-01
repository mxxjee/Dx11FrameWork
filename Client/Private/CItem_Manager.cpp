#include "CItem_Manager.h"
#include "CPlayer.h"
#include "CInventory_Manager.h"

IMPLEMENT_SINGLETON(CItem_Manager)

USING(Client)

HRESULT CItem_Manager::Initialize()
{
    m_ItmInfos.resize((size_t)ItemType::END);

    ITMINFO* pShieldItem = new ITMINFO;
    pShieldItem->ItemType = ItemType::SHIELD;
    pShieldItem->ItemDesc = L"방패를 되찾았다!\nT키를 누르는 동안 적을 튕겨낼 수 있다.";
    pShieldItem->TexKey = L"Shield";
    pShieldItem->InfoDesc = L"이름을 새긴 방패";
    m_ItmInfos[ENUM_TO_UINT(ItemType::SHIELD)] = (pShieldItem);

    ITMINFO* pSwordItem = new ITMINFO;
    pSwordItem->ItemType = ItemType::SWROD;
    pSwordItem->ItemDesc = L"칼을 받았다!\nB키를 눌러 공격하고 방패와 조합이 가능하다.";
    pSwordItem->TexKey = L"Sword";
    pSwordItem->InfoDesc = L"타린에게 받은 검";
    m_ItmInfos[ENUM_TO_UINT(ItemType::SWROD)] = (pSwordItem);

    ITMINFO* pMushroomItem = new ITMINFO;
    pMushroomItem->ItemType = ItemType::MUSHROOM;
    pMushroomItem->ItemDesc = L"버섯을 주웠다!\n향긋한 냄새가 콧속에 퍼진다.";
    pMushroomItem->TexKey = L"Mushroom";
    pMushroomItem->InfoDesc = L"숲 속의 이상한 버섯";

    m_ItmInfos[ENUM_TO_UINT(ItemType::MUSHROOM)] = (pMushroomItem);

    ITMINFO* pLetterItem = new ITMINFO;
    pLetterItem->ItemType = ItemType::LETTER;
    pLetterItem->ItemDesc = L"쪽지를 주웠다!\n아이가 남긴 쪽지이다. 어서 읽어보자.";
    pLetterItem->TexKey = L"Letter";
    pLetterItem->InfoDesc = L"알 수 없는 쪽지";
    m_ItmInfos[ENUM_TO_UINT(ItemType::LETTER)] = (pLetterItem);

    ITMINFO* pFeatherItem = new ITMINFO;
    pFeatherItem->ItemType = ItemType::FEATHER;
    pFeatherItem->ItemDesc = L"깃털을 얻었다!\n모으면 무슨일이 생길까...?";
    pFeatherItem->TexKey = L"Feather";
    pFeatherItem->InfoDesc = L"지니고 있으면 가볍게 점프할 수 있다.";
    m_ItmInfos[ENUM_TO_UINT(ItemType::FEATHER)] = (pFeatherItem);

    ITMINFO* pPowerBracelet = new ITMINFO;
    pPowerBracelet->ItemType = ItemType::POWER_BRACELET;
    pPowerBracelet->ItemDesc = L"파워팔찌를 얻었다!\n힘이 넘쳐난다!뭐든지 들 수 있을 것 같은 기분!";
    pPowerBracelet->TexKey = L"PowerBracelet";
    pPowerBracelet->InfoDesc = L"힘이 솟아나는 신비한 팔찌.";
    m_ItmInfos[ENUM_TO_UINT(ItemType::POWER_BRACELET)] = (pPowerBracelet);

    ITMINFO* pMagicPowder = new ITMINFO;
    pMagicPowder->ItemType = ItemType::MAGIC_POWDER;
    pMagicPowder->ItemDesc = L"마법의 가루를 손에 넣었다!\n쓸 곳을 찾아보자!";
    pMagicPowder->TexKey = L"MagicPowder";
    pMagicPowder->m_bQuick = true;
    pMagicPowder->InfoDesc= L"마법의 힘이 담긴 신비한 가루.";
    pMagicPowder->PlayerState = ENUM_TO_UINT(CPlayer::PLAYER_STATE::POWDER);

    pMagicPowder->m_AfterGetFunc = []()
    {
        CGameInstance::GetInstance()->BroadCastEvent(L"OnShowInvenSlot", nullptr);
        
        wstring str = L"MagicPowder";

        /// X슬롯에 등록
        CInventory_Manager::GetInstance()->Set_SlotKey(ItemType::MAGIC_POWDER,KeyCode::X);
    };

    m_ItmInfos[ENUM_TO_UINT(ItemType::MAGIC_POWDER)] = (pMagicPowder);

    return S_OK;
}

void CItem_Manager::Free()
{
    for (auto& pInfo : m_ItmInfos)
        Safe_Delete(pInfo);

}

ITMINFO* CItem_Manager::Get_ItemInfo(ItemType eType)
{

    return m_ItmInfos[ENUM_TO_UINT(eType)];
}

