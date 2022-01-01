
//#include "./zlib/Inc/zlib.h"
#include "zlib.h"

namespace Compression
{

/** Compress a buffer using zlib, will fail if CompressedBuffer is too small */
bool CompressZLIB( void* CompressedBuffer, uint32& CompressedSize, const void* UncompressedBuffer, uint32 UncompressedSize );

/** Uncompress a buffer using zlib, will fail if UncompressedBuffer is too small */
bool UncompressZLIB( void* UncompressedBuffer, uint32 UncompressedSize, const void* CompressedBuffer, uint32 CompressedSize );

} // namespace Compression