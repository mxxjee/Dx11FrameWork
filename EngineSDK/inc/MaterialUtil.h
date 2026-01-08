#pragma once

namespace Engine
{
	class CTexture;

}


typedef struct ENGINE_DLL tagDissolveDesc
{
	bool		m_bEnableDissolve = false;
	CTexture* m_pDissolveTexture = nullptr;
	float		m_fClipDissolve = 0.f;

}DISSOLVE_DESC;

