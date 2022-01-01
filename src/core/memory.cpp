


#include "../bigfx.h"

namespace bigfx
{

void* Memory::Malloc( size_t Count, uint32 Alignment )
{
	return ::malloc( Count );
}
//void* Memory::Realloc( void* Original, size_t Count, uint32 Alignment )
//{
//	return ::realloc( Original, Count );
//}
void Memory::Free( void* Original )
{
	return ::free( Original );
}

} /* namespace bigfx */

// Operator new / delete overload
//
//void* operator new( size_t Size )// throw(std::bad_alloc)
//{
//    return bigfx::Memory::Malloc( Size );
//}
//void operator delete( void* Ptr )// throw()
//{
//    bigfx::Memory::Free( Ptr );
//}
//
//void* operator new[]( size_t Size ) //throw(std::bad_alloc)
//{
//    return bigfx::Memory::Malloc( Size );
//}
//void operator delete[]( void* Ptr )// throw()
//{
//    bigfx::Memory::Free( Ptr );
//}
//
