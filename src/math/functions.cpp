
#include "../bigfx.h"


namespace bigfx
{


// "Next Largest Power of 2
// Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
// that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
// the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
// largest power of 2. For a 32-bit value:"
uint32 NextPowerOfTwo( uint32 x )
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x+1;
}

bool IsPowerOfTwo( uint32 x )
{
	return (x != 0) && ((x & (x - 1)) == 0);
}

// transforms even the sequence 0,1,2,3,... into reasonably good random numbers 
// challenge: improve on this in speed and "randomness"!
uint32 randhash(uint32 seed)
{
	uint32 i=(seed^12345391u)*2654435769u;
	i^=(i<<6)^(i>>26);
	i*=2654435769u;
	i+=(i<<5)^(i>>12);
	return i;
}

} /* namespace bigfx */

