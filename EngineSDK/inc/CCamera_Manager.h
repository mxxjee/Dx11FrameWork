#pragma once
#include "CBase.h"


NS_BEGIN(Engine)

class CGameObject;

class CCamera_Manager :
    public CBase
{
public:
    //카메라 등록 및 제거..
    void        RegisterCamera(const _wstring& Tag, CGameObject* pObj, bool isOrtho);
    void        UnRegisterCamera(const _wstring& Tag, bool isOrtho);


    //메인카메라 설정
    bool        SetMainPerspectiveCamera(const _wstring& tag);
    bool        SetMainOrthoCamera(const _wstring& tag);

    //뷰,투영행렬가져오기
    const _float4x4& GetViewMatrix(bool isOrtho = false) const;
    const _float4x4& GetProjMatrix(bool isOrtho = false) const;
    const _matrix GetMulViewProjMatrix(bool isOrtho = false) const;

    void    Bind_ViewProjMatrix(bool isOrtho);

    //메인 카메라 가져오기
    CGameObject*           GetMainPerspectiveCamera();
    CGameObject*           GetMainOrthoCamera();

public:
    void        Update_MainCamera(_float fTimeDelta);
    void        LateUpdate_MainCamera(_float fTimeDelta);
public:
    static CCamera_Manager* Create();
    virtual void        Free() override;

private:
    UMap<_wstring, CGameObject*>    m_mapPerspectiveCams;
    UMap<_wstring, CGameObject*>    m_mapOrthoCams;

    CGameObject* m_pMainPerspectiveCamera = nullptr;
    CGameObject* m_pMainOrthoCamera = nullptr;
};
NS_END

