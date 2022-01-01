


#include "../bigfx.h"
#include "name.h"

namespace bigfx
{

NameManager* NameManager::m_pStaticInstance = nullptr; 

NameManager::NameManager()
{
	m_pStaticInstance = this;
}

NameManager::~NameManager()
{
	m_pStaticInstance = nullptr;
}

NameHandle NameManager::FindOrAddHandle( String const& str )
{
	const NameMap::Pair* NameEntry = m_StringTable.Add( str, m_StringTable.m_NbActivePairs );

	return NameEntry->Value;
}

const String& NameManager::GetString( NameHandle Handle )
{
	if( Handle < m_StringTable.m_NbActivePairs )
	{
		return m_StringTable.m_Pairs[Handle].Key;
	}

	return m_StringTable.m_Pairs[0].Key;
}

void NameManager::Init()
{
	HashData::InitHashData();

	m_StringTable.reserve( 4096 );
	
	FindOrAddHandle( "None" );
}
NameManager* NameManager::GetStaticInstance() 
{ 
	if( !m_pStaticInstance )
	{
		m_pStaticInstance = new NameManager();
		m_pStaticInstance->Init();
	}
	return m_pStaticInstance; 
}

} /* namespace bigfx */

