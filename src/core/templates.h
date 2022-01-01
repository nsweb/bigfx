

#ifndef BB_TEMPLATES_H
#define BB_TEMPLATES_H

namespace bigfx
{

#define UNUSED(...)

#if defined _MSC_VER
#define BB_ASSERT_LOG(Cond, Format, ...) \
{ \
	if( !(Cond) ) \
	{ \
		BBLog::Print( __FILE__, __LINE__, "Assert", BBLog::Error, Format, ##__VA_ARGS__ ); \
		__debugbreak(); \
		std::abort(); \
	} \
}
#else
#define BB_ASSERT_LOG(Cond, Format, ...) \
    { \
        if( !(Cond) ) \
        { \
            BBLog::Print( __FILE__, __LINE__, "Assert", BBLog::Error, Format, ##__VA_ARGS__ ); \
            __builtin_trap();  \
            std::abort(); \
        } \
    }
#endif
#define BB_ASSERT(Cond)			BX_ASSERT(Cond, "")
//#define BB_ASSERT(Cond, ...)	BX_ASSERT(Cond, ...)

#define BB_FREE(p)			{ if(p) Memory::Free(p); (p)=nullptr; }
#define BB_DELETE(p)		{ if(p) delete(p); (p)=nullptr; }
#define BB_DELETE_ARRAY(p)	{ if(p) delete[](p); (p)=nullptr; }

#define ALIGN_OF(p)			(__alignof(p))
#define COUNT_OF(_Array)	(sizeof(_Array) / sizeof(_Array[0]))
#define CACHE_LINE			32
    
#if defined _MSC_VER
#       define CACHE_ALIGN  __declspec(align(CACHE_LINE))
#else
#       define CACHE_ALIGN  __attribute__ ((aligned(CACHE_LINE)))
#endif
    
#if !defined _MSC_VER
    #define DECL_THREAD __thread
#else
    #define DECL_THREAD __declspec(thread)
#endif

template< class T >
inline void swap( T& A, T& B )
{
	const T Tmp = A;
	A = B;
	B = Tmp;
}

#define CLASS_EQUIP_BASE_H( klass ) \
private:\
	static Name m_ClassName;\
public:\
	static	Name		StaticClass()               { return m_ClassName;		}\
	virtual Name		GetClassName()              { return m_ClassName;		}\
	virtual bool		IsA( Name const& ClassName ) 	{ if( ClassName == StaticClass() ) return true; return false; }

#define CLASS_EQUIP_H( klass, klassParent ) \
private:\
	typedef klassParent Super;\
	static Name m_ClassName;\
public:\
	static	Name		StaticClass()               { return m_ClassName;		}\
	virtual Name		GetClassName() override		{ return m_ClassName;		}\
	virtual bool		IsA( Name const& ClassName ) override	{ if( ClassName == StaticClass() ) return true; return Super::IsA( ClassName ); }

#define CLASS_EQUIP_CPP( klass ) \
Name klass::m_ClassName( #klass )

#undef BB_MATH_CONSTANT

} /* namespace bigfx */

#endif // BB_TEMPLATES_H

