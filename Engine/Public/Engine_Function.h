#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}



	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int	Safe_AddRef(T& Instance)
	{
		unsigned int	iRefCnt = 0;

		if (nullptr != Instance)
			iRefCnt = Instance->AddRef();
	
		return iRefCnt;
	}


	template<typename T>
	unsigned int	Safe_Release(T& Instance)
	{
		unsigned int	iRefCnt = 0;

		if (nullptr != Instance)
		{
			iRefCnt = Instance->Release();
			if (iRefCnt == 0)
				Instance = nullptr;

		
		}

		return iRefCnt;

	}

	inline std::string WStringToUTF8(const std::wstring& wstr)
	{
		if (wstr.empty()) return {};
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string result(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &result[0], size_needed, NULL, NULL);
		return result;
	}
}
#endif // Engine_Function_h__