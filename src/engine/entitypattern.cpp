

#include "../bigfx.h"
#include "entitypattern.h"

namespace bigfx
{



void EntityPattern::SetComponentList( int N, ... )
{
	m_Components.clear();

	va_list ap;
	va_start( ap, N );

	for( int i=0; i<N; ++i )
	{
		Name key = va_arg( ap, char* );
		m_Components.push_back( key );
	}

	va_end(ap);
}
void EntityPattern::SetOptionalComponentList( int N, ... )
{

}


} /* namespace bigfx */