#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#define			NS_BEGIN(NAMESPACE)			namespace NAMESPACE {
#define			NS_END						}

#define			USING(NAMESPACE)			using namespace NAMESPACE;


#define CheckTrue(x) { if(x == true) return; }
#define CheckFalse(x) { if(x == false) return; }
#define CheckNull(x) { if(x == nullptr) return; }
#define CheckTrueResult(x, y) { if(x == true) return y; }
#define CheckFalseResult(x, y) { if(x == false) return y;}
#define CheckNullResult(x, y) { if(x == nullptr) return y;}



#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)//Engine 프로젝트에선 export
#else
#define ENGINE_DLL		_declspec(dllimport)//Client프로젝트에선 import를 의미
#endif

#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&)= delete;		

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned int CLASSNAME::DestroyInstance( void ) {		\
			unsigned int iRefCnt = {0};							\
			if(nullptr != m_pInstance)	{						\
				iRefCnt = m_pInstance->Release();				\
				if(0 == iRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return iRefCnt;										\
}


#endif // Engine_Macro_h__
