#include "CItem_Manager.h"


IMPLEMENT_SINGLETON(CItem_Manager)

USING(Client)

HRESULT CItem_Manager::Initialize()
{
    m_ItmInfos.resize((size_t)ItemType::END);

    ITMINFO* pShieldItem = new ITMINFO;
    pShieldItem->ItemType = ItemType::SHIELD;
    pShieldItem->ItemDesc = L"방패를 되찾았다!\nT키를 누르는 동안 적을 튕겨낼 수 있다.";
    pShieldItem->TexKey = L"Shield";
    m_ItmInfos[ENUM_TO_UINT(ItemType::SHIELD)]=(pShieldItem);

    ITMINFO* pSwordItem = new ITMINFO;
    pSwordItem->ItemType = ItemType::SWROD;
    pSwordItem->ItemDesc = L"칼을 받았다!\nB키를 눌러 공격하고 방패와 조합이 가능하다.";
    pSwordItem->TexKey = L"Sword";
    m_ItmInfos[ENUM_TO_UINT(ItemType::SWROD)] = (pSwordItem);

    ITMINFO* pMushroomItem = new ITMINFO;
    pMushroomItem->ItemType = ItemType::MUSHROOM;
    pMushroomItem->ItemDesc = L"버섯을 주웠다!\n향긋한 냄새가 콧속에 퍼진다.";
    pMushroomItem->TexKey = L"Mushroom";
    m_ItmInfos[ENUM_TO_UINT(ItemType::MUSHROOM)] = (pMushroomItem);

    ITMINFO* pLetterItem = new ITMINFO;
    pLetterItem->ItemType = ItemType::LETTER;
    pLetterItem->ItemDesc = L"쪽지를 주웠다!\n아이가 남긴 쪽지이다. 어서 읽어보자.";
    pLetterItem->TexKey = L"Letter";
    m_ItmInfos[ENUM_TO_UINT(ItemType::LETTER)] = (pLetterItem);

    ITMINFO* pFeatherItem = new ITMINFO;
    pFeatherItem->ItemType = ItemType::FEATHER;
    pFeatherItem->ItemDesc = L"깃털을 얻었다!\n모으면 무슨일이 생길까...?";
    pFeatherItem->TexKey = L"Feather";
    m_ItmInfos[ENUM_TO_UINT(ItemType::FEATHER)] = (pFeatherItem);

    ITMINFO* pPowerBracelet = new ITMINFO;
    pFeatherItem->ItemType = ItemType::FEATHER;
    pFeatherItem->ItemDesc = L"파워팔찌를 얻었다!\n힘이 넘쳐난다!";
    pFeatherItem->TexKey = L"PowerBracelet";
    m_ItmInfos[ENUM_TO_UINT(ItemType::POWER_BRACELET)] = (pPowerBracelet);

    ITMINFO* pMagicPowder = new ITMINFO;
    pMagicPowder->ItemType = ItemType::MAGIC_POWDER;
    pMagicPowder->ItemDesc = L"마법의 가루를 손에 넣었다!\n쓸 곳을 찾아보자!";
    pMagicPowder->TexKey = L"MagicPowder";
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

