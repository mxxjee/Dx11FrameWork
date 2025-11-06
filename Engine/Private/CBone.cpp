#include "CBone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const BoneData& Data)
{
   
    string Name = WStringToUTF8(Data.BoneName);

    strcpy_s(m_szName, MAX_PATH, Name.c_str());

    m_iParentBoneIndex = Data.BoneIndex;


    XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&Data.m_OffsetMatrix)));
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&Data.TransformationMatrix)));

    //나중에 연산할것임.. 일단 Identity()초기화
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CBone::Update_CombinedTransformMatrix(const vector<CBone*>& Bones, _matrix Transformation)
{
    /*1. 먼저 내 로컬 transform 계산
    2. 이후 부모의 matrix 가져와서 연산
    3. 이후 offset곱하기 */

    //만약 parent===-1이라면(까장 최상위노드라면),그냥 루트에 transformation곱하기
    if (m_iParentBoneIndex == -1)
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) * Transformation);
    }


    else
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
    }

}

bool CBone::Compare_Name(const char* pName)
{
    return !strcmp(pName, m_szName);
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
