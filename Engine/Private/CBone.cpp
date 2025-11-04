#include "CBone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const BoneData& Data)
{
    string Name = string(Data.BoneName.begin(), Data.BoneName.end());

    strcpy_s(m_szName, MAX_PATH, Name.c_str());
    m_TransformationMatrix = Data.PreTransformMatrix;
    m_iParentBoneIndex = Data.BoneIndex;


    //나중에 연산할것임.. 일단 Identity()초기화
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

CBone* CBone::Create(const BoneData& Data)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize(Data)))
    {
        MSG_BOX("Failed to Created : CBone");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBone::Free()
{
    __super::Free();
}
