#pragma once
#include "CBase.h"

/*후처리를 위한 렌더타겟들을 모아놓고, Renderer를 통해 바인드한다*/
NS_BEGIN(Engine)
class ENGINE_DLL CTarget_Manager :
    public CBase
{
private:
    CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CTarget_Manager() = default;

public:
                //렌더타겟을 등록하는함수
    HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
    
            //MRT : RenderTarget들을 그룹으로묶어 한번에 바인딩
    HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
    
            //MRT를 찾아서 바인딩
    HRESULT Begin_MRT(const _wstring& strMRTTag);
    
            //다시 복원
    HRESULT End_MRT();


                    //쉐이더에 렌더타겟의 SRV를바인딩
    HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
    //디버그용으로 ui로 띄우기 위한 준비를 하는함수,m_pRTV->Ready_Debug()
    HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);

    //디버그용으로 띄우기 위해 사각형을 띄우는 렌더
    HRESULT Render(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG


private:
    ComPtr<ID3D11Device>        m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>        m_pContext = { nullptr };

    map<const _wstring, class CRenderTarget*>        m_RenderTargets;
    map<const _wstring, list<class CRenderTarget*>>        m_MRTs;

                                            //원본 복원을 위해 백버퍼 저장해두기위함

    ComPtr<ID3D11RenderTargetView>          m_pBackBuffer = nullptr;
    ComPtr<ID3D11DepthStencilView>          m_pDSV = nullptr;


public:
    class CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag);
    HRESULT Unbind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName);
    
    //그룹찾아서 리턴
    class list<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);


public:
    static CTarget_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void Free() override;



};

NS_END

