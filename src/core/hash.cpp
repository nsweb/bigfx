
#include "../bigfx.h"
#include "hash.h"

namespace bigfx
{

/*
 * Hash implementations
 */

uint32_t HashData::crc32_table[256] = {0};


/*
 * Helper hash functions
 */

static inline uint32_t Hash8(uint8_t x)
{
    uint32_t ret = 0xffffffffu;
    ret = HashData::crc32_table[(uint8_t)(ret ^ x)] ^ (ret >> 8);
    return ret ^ 0xffffffffu;
}

static inline uint32_t Hash16(uint16_t x)
{
    uint32_t ret = 0xffffffffu;
    ret = HashData::crc32_table[(uint8_t)(ret ^ x)] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 8))] ^ (ret >> 8);
    return ret ^ 0xffffffffu;
}

static inline uint32_t Hash32(uint32_t x)
{
    uint32_t ret = 0xffffffffu;
    ret = HashData::crc32_table[(uint8_t)(ret ^ x)] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 8))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 16))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 24))] ^ (ret >> 8);
    return ret ^ 0xffffffffu;
}

// Thomas Wang
//uint32_t FastHash32( uint32_t a)
//{
//	todo
//	a = (a+0x7ed55d16) + (a<<12);
//	a = (a^0xc761c23c) ^ (a>>19);
//	a = (a+0x165667b1) + (a<<5);
//	a = (a+0xd3a2646c) ^ (a<<9);
//	a = (a+0xfd7046c5) + (a<<3);
//	a = (a^0xb55a4f09) ^ (a>>16);
//	return a;
//}

static inline uint32_t Hash64(uint64_t x)
{
    uint32_t ret = 0xffffffffu;
    ret = HashData::crc32_table[(uint8_t)(ret ^ x)] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 8))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 16))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 24))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 32))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 40))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 48))] ^ (ret >> 8);
    ret = HashData::crc32_table[(uint8_t)(ret ^ (x >> 56))] ^ (ret >> 8);
    return ret ^ 0xffffffffu;
}

/*
 * Integer hash functions
 */

uint32_t Hash<int8_t>::operator ()(int8_t x) const
{
    return Hash8((uint8_t)x);
}

uint32_t Hash<uint8_t>::operator ()(uint8_t x) const
{
    return Hash8(x);
}

uint32_t Hash<int16_t>::operator ()(int16_t x) const
{
    return Hash16((uint16_t)x);
}

uint32_t Hash<uint16_t>::operator ()(uint16_t x) const
{
    return Hash16(x);
}

uint32_t Hash<int32_t>::operator ()(int32_t x) const
{
    return Hash32((uint32_t)x);
}

uint32_t Hash<uint32_t>::operator ()(uint32_t x) const
{
    return Hash32(x);
}

uint32_t Hash<int64_t>::operator ()(int64_t x) const
{
    return Hash64((uint64_t)x);
}

uint32_t Hash<uint64_t>::operator ()(uint64_t x) const
{
    return Hash64(x);
}

/*
 * Floating-point hash functions
 */

//uint32_t Hash<half>::operator ()(half f) const
//{
//    return Hash16(f.bits);
//}

uint32_t Hash<float>::operator ()(float f) const
{
    union { float tmp; uint32_t x; } u = { f };
    return Hash32(u.x);
}

uint32_t Hash<double>::operator ()(double f) const
{
    union { double tmp; uint64_t x; } u = { f };
    return Hash64(u.x);
}

/*
 * String and array hash functions
 */

static uint32_t HashCharString(char const *s)
{
    uint32_t ret = 0xffffffffu, ch;

    while ((ch = (uint8_t)*s++))
        ret = HashData::crc32_table[(uint8_t)(ret ^ ch)] ^ (ret >> 8);

    return ret ^ 0xffffffffu;
}

uint32_t Hash<char const *>::operator ()(char const *s) const
{
	uint32 HVal = HashCharString(s);
	//BB_LOG( TEST, Log, "Hash %s - val = %ud", s, HVal );
	return HVal;
}

uint32_t Hash<char const *>::operator ()(String const &s) const
{
	uint32 HVal = HashCharString(&s[0]);
	//BB_LOG( TEST, Log, "Hash %s - val = %ud", s.c_str(), HVal );
    return HVal;
}

uint32_t Hash<String>::operator ()(String const &s) const
{
	uint32 HVal = HashCharString(&s[0]);
	//BB_LOG( TEST, Log, "Hash %s - val = %ud", s.c_str(), HVal );
	return HVal;
}

uint32_t Hash<String>::operator ()(char const *s) const
{
	uint32 HVal = HashCharString(s);
	//BB_LOG( TEST, Log, "Hash %s - val = %ud", s, HVal );
	return HVal;
}

} /* namespace bigfx */

