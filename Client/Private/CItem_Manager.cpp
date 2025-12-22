#include "CItem_Manager.h"


IMPLEMENT_SINGLETON(CItem_Manager)

USING(Client)

HRESULT CItem_Manager::Initialize()
{
    m_ItmInfos.resize((size_t)ItemType::END);

    ITMINFO* pShield_SlashItem = new ITMINFO;
    pShield_SlashItem->ItemType = ItemType::SHIELD_SLASH;
    pShield_SlashItem->ItemDesc = L"방패를 되찾았다!\nT키를 누르는 동안 적을 튕겨낼 수 있다.";
    pShield_SlashItem->TexKey = L"Shield";
    m_ItmInfos[ENUM_TO_UINT(ItemType::SHIELD_SLASH)]=(pShield_SlashItem);

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
    pShield_SlashItem->ItemType = ItemType::FEATHER;
    pFeatherItem->ItemDesc = L"깃털을 얻었다!\n모으면 무슨일이 생길까...?";
    pFeatherItem->TexKey = L"Feather";
    m_ItmInfos[ENUM_TO_UINT(ItemType::FEATHER)] = (pFeatherItem);

    ITMINFO* pPowerBracelet = new ITMINFO;
    pFeatherItem->ItemType = ItemType::FEATHER;
    pFeatherItem->ItemDesc = L"파워팔찌를 얻었다!\n힘이 넘쳐난다!";
    pFeatherItem->TexKey = L"PowerBracelet";
    m_ItmInfos[ENUM_TO_UINT(ItemType::POWER_BRACELET)] = (pPowerBracelet);


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

