


#ifndef BB_CORENAME_H
#define BB_CORENAME_H

#include "map.h"

namespace bigfx
{

typedef uint32 NameHandle;
typedef Map<String,NameHandle> NameMap;

class NameManager
{
private: 
	static NameManager* m_pStaticInstance; 
	NameManager();
	~NameManager();
	void Init();

public:
	NameMap m_StringTable;
	//uint32 m_GlobalHandle;

public:

	static NameManager* GetStaticInstance();
	NameHandle FindOrAddHandle( String const& str );
	const String& GetString( NameHandle Handle );
};

class BIGFX_API Name
{
private:

	NameHandle	m_Handle;
	uint32		m_Num;

public:
    inline Name()
    {
        m_Handle = 0;
		m_Num = INDEX_NONE;
    }

    inline Name(char const *str)
    {
        m_Handle = NameManager::GetStaticInstance()->FindOrAddHandle( String(str) );
		m_Num = INDEX_NONE;
    }

    inline Name( String const& str )
    {
		m_Handle = NameManager::GetStaticInstance()->FindOrAddHandle( str );
		m_Num = INDEX_NONE;
    }

    inline char const* c_str() const
    {
        return NameManager::GetStaticInstance()->GetString( m_Handle ).c_str();
    }
	inline String ToString() const
	{
		return NameManager::GetStaticInstance()->GetString( m_Handle );
	}

    inline bool operator ==( Name const& Other ) const
    {
        return m_Handle == Other.m_Handle && m_Num == Other.m_Num;
    }
};


} /* namespace bigfx */

#endif // BB_CORENAME_H

