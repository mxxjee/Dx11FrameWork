#pragma once
#include "CBase.h"


NS_BEGIN(Engine)
class CCameraComponent;


class CCamera_Manager :
    public CBase
{
public:
    //카메라 등록 및 제거..
    void        RegisterCamera(const _wstring& Tag, CCameraComponent* pComp, bool isOrtho);
    void        UnRegisterCamera(const _wstring& Tag, bool isOrtho);


    //메인카메라 설정
    bool        SetMainPerspectiveCamera(const _wstring& tag);
    bool        SetMainOrthoCamera(const _wstring& tag);

    //뷰,투영행렬가져오기
    const Matrix& GetViewMatrix(bool isOrtho = false) const;
    const Matrix& GetProjMatrix(bool isOrtho = false) const;

    //메인 카메라 가져오기
    CCameraComponent*           GetMainPerspectiveCamera();
    CCameraComponent*           GetMainOrthoCamera();

public:
    static CCamera_Manager* Create();


private:
    UMap<_wstring, CCameraComponent*>    m_mapPerspectiveCams;
    UMap<_wstring, CCameraComponent*>    m_mapOrthoCams;

    CCameraComponent* m_pMainPerspectiveCamera = nullptr;
    CCameraComponent* m_pMainOrthoCamera = nullptr;
};
NS_END

