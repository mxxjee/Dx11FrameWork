#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define			ENUM_TO_UINT(ENUM)			static_cast<unsigned int>(ENUM)

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#define			NS_BEGIN(NAMESPACE)			namespace NAMESPACE {
#define			NS_END						}


#define CHECK(p) assert(SUCCEEDED(p))
#define			USING(NAMESPACE)			using namespace NAMESPACE;


#define CheckTrue(x) { if(x == true) return; }
#define CheckFalse(x) { if(x == false) return; }
#define CheckNull(x) { if(x == nullptr) return; }
#define CheckTrueResult(x, y) { if(x == true) return y; }
#define CheckFalseResult(x, y) { if(x == false) return y;}
#define CheckNullResult(x, y) { if(x == nullptr) return y;}


#define PROTO_OBJ_NAME(name) L"Proto_GameObject_" + _wstring(name)
#define PROTO_COMPONENT_NAME(name) L"Proto_Component_" + _wstring(name)

#define WORLD_UP XMVectorSet(0.f,1.f,0.f,0.f)
#define WORLD_RIGHT XMVectorSet(1.f,0.f,0.f,0.f)
#define WORLD_LOOK XMVectorSet(0.f,0.f,1.f,0.f)


#define ENABLE_BITMASK_OPERATORS(x)                     \
inline x operator|(x a, x b) {                          \
    return static_cast<x>(static_cast<int>(a) | static_cast<int>(b)); } \
inline x& operator|=(x& a, x b) {                       \
    a = a | b; return a; }                              \
inline x operator&(x a, x b) {                          \
    return static_cast<x>(static_cast<int>(a) & static_cast<int>(b)); } \
inline x& operator&=(x& a, x b) {                       \
    a = a & b; return a; }                              \
inline x operator~(x a) {                               \
    return static_cast<x>(~static_cast<int>(a)); }


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
