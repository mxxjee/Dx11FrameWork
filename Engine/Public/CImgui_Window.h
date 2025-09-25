#pragma once
#include "CImgui_Base.h"
class CImgui_Window :
    public CImgui_Base
{
public:
    CImgui_Window();
    CImgui_Window(const string& Tag);
    virtual ~CImgui_Window() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize() override;
    virtual void Update() override;
    virtual void Render() override;


public:
    virtual void Free();

protected:
    bool    m_bOpen = false;
};

