#pragma once
#include "CBase.h"
#include "ModelData.h"

/*애니메이션에 사용되는 뼈 1개의 상태를 저장하는 클래스*/
/*KeyFrame: 시간에 따른 뼈의 상태*/
/*상태와 상태를 보간하여 새로운상태를만든다.*/
/*만들어낸 상태는 CBone의 m_Trnasformatrix에 전달해준다.*/
NS_BEGIN(Engine)
class ENGINE_DLL CChannel :
    public CBase
{
private:
    CChannel();
    virtual ~CChannel() = default;


public:
    _int            Get_BoneIndex_ByChannel() { return m_iBoneIndex; }

public:
    _matrix         Get_CurrentKeyFrameBoneSRT(_uint* pCurrentKeyFrameIdx);
    

    HRESULT Initialize(class CModel* pModel, json& Json, const char* AnimFilePath, int AnimIdx, int index);
    void    Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition,_uint* pCurrentKeyFrameIdx );
    KEYFRAME*    Get_KeyFrame(int idx) { if (idx >= m_KeyFrames.size())return nullptr;else return &m_KeyFrames[idx]; }
    const vector<KEYFRAME>& Get_KeyFrames() { return m_KeyFrames; }

            //지금 키프레임값의 SRT와 다음애니메이션보간
    void    UpdateTransformMatrix_Blned_By_PrevToCurr(const vector<class CBone*>& Bones,_matrix ExistMatrix,_float Ratio);


                //현재 본의 SRT와 다음애니메이션 보간
    void    UpdateTransformMatrix_To_Target(const vector<class CBone*>& Bones,_matrix CurSRT,_float Ratio);


    _uint       Get_CurrentKeyFrameIndex(_float fCurrentPosition,_uint* pIndex);
private:
    _uint				m_iNumKeyFrames = {};		//이 채널의 전체 키프레임, SRT 변환중 가장 큰 것의 사이즈
    vector<KEYFRAME>	m_KeyFrames;
    _int				m_iBoneIndex = {};			//타겟이되는 본 인덱스
    
public:
    static CChannel* Create(class CModel* pModel,json& Json, const char* AnimFilePath,int AnimIdx, int index);
    virtual void Free() override;
};

NS_END

