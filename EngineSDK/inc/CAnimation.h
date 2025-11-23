#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CChannel;
class CGameObject;

class ENGINE_DLL CAnimation :
    public CBase
{
private:
    CAnimation();
    CAnimation(const CAnimation& Prototype);
    virtual ~CAnimation() = default;



public:
    HRESULT     Initialize(class CModel* pModel, json& Json, const char* filePath, _uint idx);

    //현재 재생바의 위치에 따라 본의 matrix를 업데이트하도록 시키는 함수, 리턴값:애니메이션끝났는지 여부
    bool        Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDeltal);
      
    void        Set_Loop(bool b) { m_bLoop = b; }
    void        Set_CurrentTrackPosition(_float TrackPos) { m_fCurrentTrackPosition = TrackPos; }  
    void        Set_TickPerSecond(_float fSpeed) { m_fTickPerSecond = fSpeed; }


    void        Reset_Animtion();
public:
    const char* Get_Name() { return m_szName; }
    _float         Get_CurrentTrackPoistion() { return m_fCurrentTrackPosition; }
    _uint       Get_CurrentKeyFrameIndex(int num) { return m_CurrentKeyFrameIndices[num]; }
   
    CChannel*         Get_Channel_BoneIdx(int BoneNum);
    _matrix           Get_CurrentKeyFrameBonSRT(int BoneIdx);
    int               Get_ChannelIdx(int BoneNum);

public:
    void                Set_RootMotionEnalbe(bool b) {m_bEnableRootMotion = b;}
    _float3             Get_RootDelta() { return RootDelta; }
private:
    void                Update_RootMotion(const vector<class CBone*>& Bones, CChannel* pChannel);
    
private:
    _uint               m_iNumChannels = {};
    vector<class CChannel*> m_Channels;
    vector<_uint>           m_CurrentKeyFrameIndices;


    _char               m_szName[MAX_PATH] = {};
    _float              m_fTickPerSecond = {};
    _float              m_fDuration = {};
    _float              m_fCurrentTrackPosition = {};
    bool                m_bLoop = false;


    bool                m_bEnableRootMotion = false;

public:
    static  CAnimation* Create(class CModel* pModel,json& Json,const char* filePath,_uint idx);
    CAnimation* Clone();
    virtual void Free() override;

private:
    _float3         RootDelta = _float3(0.f, 0.f, 0.f);

private:
    CGameObject* m_pOwner = nullptr;

};
NS_END
