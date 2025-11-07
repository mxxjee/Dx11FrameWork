#pragma once
#include "CBase.h"
#include "ModelData.h"

NS_BEGIN(Engine)
class CBone final:
    public CBase
{
private:
    CBone();
    virtual ~CBone() = default;

public:
    HRESULT     Initialize(const BoneData& Data);
    void        Update_CombinedTransformMatrix(const vector<CBone*>& Bones, _matrix Transformation);

public:
    _float4x4       Get_OffSetMatrix() { return m_OffsetMatrix; }
    void Set_TransformationMatrix(const _float4x4& TransformationMatrix) {
        m_TransformationMatrix = TransformationMatrix;
    }
public:
    bool            Compare_Name(const char* pName);

private :
    _char				m_szName[MAX_PATH] = {};
    _float4x4			m_TransformationMatrix = {}; /* 이 뼈만의 상태(크 * 자 * 이) */
    _float4x4			m_CombinedTransformationMatrix = {}; /* 내`TransformationMatrix * 부모`s CombinedTransformationMatrix */
    _int				m_iParentBoneIndex = { -1 };
    _float4x4	        m_OffsetMatrix;

public:
    _matrix     Get_CombinedTransformationMatrix()
    {
        return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
    }

public:
    static CBone* Create(const BoneData& Data);
    virtual void Free() override;
};


NS_END
