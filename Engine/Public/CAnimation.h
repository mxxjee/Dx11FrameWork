#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CAnimation :
    public CBase
{
private:
    CAnimation();
    virtual ~CAnimation() = default;



public:
    HRESULT     Initialize(class CModel* pModel, const char* filePath, _uint idx);

    //현재 재생바의 위치에 따라 본의 matrix를 업데이트하도록 시키는 함수, 리턴값:애니메이션끝났는지 여부
    bool        Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);
    void        Set_Loop(bool b) { m_bLoop = b; }



private:
    _uint               m_iNumChannels = {};
    vector<class CChannel*> m_Channels;

    _char               m_szName[MAX_PATH] = {};
    _float              m_fTickPerSecond = {};
    _float              m_fDuration = {};
    _float              m_fCurrentTrackPosition = {};
    bool                m_bLoop = false;


public:
    static  CAnimation* Create(class CModel* pModel,const char* filePath,_uint idx);
    virtual void Free() override;

};
NS_END
