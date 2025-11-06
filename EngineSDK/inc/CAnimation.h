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
    HRESULT     Initialize(const char* filePath, _uint idx);
    void        Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);




private:
    _uint               m_iNumChannels = {};
    vector<class CChannel*> m_Channels;

    _char               m_szName[MAX_PATH] = {};
    _float              m_fTickPerSecond = {};
    _float              m_fDuration = {};
    _float              m_fCurrentTrackPosition = {};


public:
    static  CAnimation* Create(const char* filePath,_uint idx);
    virtual void Free() override;

};
NS_END
