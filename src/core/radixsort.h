
// Radix sort (from Pierre Terdiman)

#ifndef BB_RADIXSORT_H
#define BB_RADIXSORT_H

namespace bigfx
{

enum eRadixHint
{
	RADIX_SIGNED,		//!< Input values are signed
	RADIX_UNSIGNED,		//!< Input values are unsigned

	RADIX_FORCE_DWORD = 0x7fffffff
};

class RadixSort
{
public:

	RadixSort();
	~RadixSort();

	// Sorting methods
	RadixSort&		Sort(const uint32* input, uint32 nb, eRadixHint hint=RADIX_SIGNED);
	RadixSort&		Sort(const float* input, uint32 nb);

	// Access to results. mRanks is a list of indices in sorted order, i.e. in the order you may further process your data
	const uint32*		GetRanks()			const	{ return m_Ranks;		}

	// mIndices2 gets trashed on calling the sort routine, but otherwise you can recycle it the way you want.
	uint32*			GetRecyclable()		const	{ return m_Ranks2;		}

	bool			SetRankBuffers(uint32* ranks0, uint32* ranks1);

private:
	uint32		m_CurrentSize;		//!< Current size of the indices list
	uint32*		m_Ranks;				//!< Two lists, swapped each pass
	uint32*		m_Ranks2;
	// Stats
	uint32		mTotalCalls;		//!< Total number of calls to the sort routine
	uint32		mNbHits;			//!< Number of early exits due to coherence
	// Stack-radix
	bool		m_bDeleteRanks;		//!<
	// Internal methods
	void			CheckResize(uint32 nb);
	bool			Resize(uint32 nb);
};

#define StackRadixSort(name, ranks0, ranks1)	\
	RadixSort name;								\
	name.SetRankBuffers(ranks0, ranks1);


} /* namespace bigfx */

#endif // BB_RADIXSORT_H
