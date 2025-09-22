#include "CLevelFactroy.h"
#include "CLevel.h"

USING(Engine)

CLevelFactroy::CLevelFactroy()
{
}

void CLevelFactroy::Register(const _wstring& tag, LevelCreator Creator)
{
	CheckNull(Creator);
	CheckTrue(tag.empty());

	auto iter = m_creators.find(tag);

	/*중복 key값 허용X*/
	if (iter != m_creators.end())
		return;

	m_creators.emplace(tag,Creator);

}

CLevel* CLevelFactroy::Create(const _wstring& tag, ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, LevelArgs& _Arg)
{
	auto it = m_creators.find(tag);
	if (it == m_creators.end())
		return nullptr; 

	else
	{
		return it->second(_pDevice, _pContext, _Arg);
	}
}


CLevelFactroy* CLevelFactroy::Create()
{
	return new CLevelFactroy();
}

void CLevelFactroy::Free()
{
	__super::Free();

	m_creators.clear();
}
