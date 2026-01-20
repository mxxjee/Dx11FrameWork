#pragma once
#include "Engine_Define.h"


inline void UnbindAllShaderResources(ID3D11DeviceContext* ctx)
{
    ID3D11ShaderResourceView* nullSRVs[128] = {};
    ID3D11UnorderedAccessView* nullUAVs[8] = {};
    UINT initialCounts[8] = {};

    // 모든 셰이더 스테이지의 SRV 해제
    ctx->VSSetShaderResources(0, 128, nullSRVs);
    ctx->PSSetShaderResources(0, 128, nullSRVs);
    ctx->GSSetShaderResources(0, 128, nullSRVs);
    ctx->HSSetShaderResources(0, 128, nullSRVs);
    ctx->DSSetShaderResources(0, 128, nullSRVs);
    ctx->CSSetShaderResources(0, 128, nullSRVs);

    // UAV 해제 (특히 CS에 남아있으면 RTV 바인딩 때도 걸립니다)
    ctx->CSSetUnorderedAccessViews(0, 8, nullUAVs, initialCounts);
}

inline void UnbindAllRenderTargets(ID3D11DeviceContext* ctx)
{
    // RTV/DSV 모두 해제 후 새 RTV를 물리는 게 안전
    ctx->OMSetRenderTargets(0, nullptr, nullptr);
}