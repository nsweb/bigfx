
#ifndef BB_COREMAP_H
#define BB_COREMAP_H

#include "hash.h"

namespace bigfx
{

template<typename K, typename V> 
class Map : protected Hash<K>
{
public:
	struct Pair
	{
		K Key;
		V Value;
	};

	Map() :
		m_Pairs(nullptr),
		m_HashTable(nullptr),
		m_NextTable(nullptr),
		m_HashSize(0),
		m_Mask(0),
		m_NbActivePairs(0)
	{

	}
	~Map()
	{
		BB_DELETE_ARRAY(m_Pairs);
		BB_FREE(m_HashTable);
		BB_FREE(m_NextTable);
		m_HashSize = 0;
		m_Mask = 0;
		m_NbActivePairs = 0;
	}

	void reserve( uint32 HashSize )
	{
		if( HashSize > m_HashSize )
		{
			// Get more entries
			m_HashSize = (bigfx::IsPowerOfTwo(HashSize) ? HashSize : bigfx::NextPowerOfTwo(HashSize));
			m_Mask = m_HashSize - 1;

			BB_FREE(m_HashTable);
			m_HashTable = (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );
			Memory::Memset( m_HashTable, 0xFF, m_HashSize * sizeof(uint32) );

			Pair* NewPairs	= new Pair[m_HashSize];/* (Pair*) Memory::Malloc( m_HashSize * sizeof(Pair) );*/
			// Copy old data
			for( uint32 i = 0; i < m_NbActivePairs; ++i )
				NewPairs[i] = m_Pairs[i];
			BB_DELETE_ARRAY(m_Pairs);
			m_Pairs = NewPairs;

			BB_FREE(m_NextTable);
			m_NextTable	= (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );

			for( uint32 i=0; i < m_NbActivePairs; i++ )
			{
				uint32 HashValue = ((Hash<K> const &)*this)(m_Pairs[i].Key) & m_Mask;
				m_NextTable[i] = m_HashTable[HashValue];
				m_HashTable[HashValue] = i;
			}
		}
	}

	const Pair*	Find( K const& Key ) const
	{
		if( !m_HashTable )	return nullptr;	// Nothing has been allocated yet

		// Compute hash value for this key
		uint32 HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;

		return Find( Key, HashValue );
	}

	Pair* Find( K const& Key, uint32 HashValue ) const
	{
		if( !m_HashTable )	return nullptr;	// Nothing has been allocated yet

		// Look for it in the table
		uint32 Offset = m_HashTable[HashValue];

		while( Offset != INDEX_NONE && m_Pairs[Offset].Key != Key )
		{
			//BB_ASSERT(m_Pairs[Offset].mID0!=INVALID_USER_ID);
			Offset = m_NextTable[Offset];		// Better to have a separate array for this
		}
		if( Offset == INDEX_NONE )	return nullptr;

		BB_ASSERT( Offset < m_HashSize );
		// Match m_Pairs[Offset] => the pair is persistent
		return &m_Pairs[Offset];
	}

	Pair* Add( K const& Key, V const& Value )
	{
		// Compute hash value for this key
		uint32 HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;
		Pair* P = Find(Key, HashValue);
		if(P)
			return P;	// Persistent pair

		// This is a new pair
		reserve( m_NbActivePairs + 1 );
		//if( m_NbActivePairs >= m_HashSize )
		//{
		//	// Get more entries
		//	m_HashSize = bigfx::NextPowerOfTwo(m_NbActivePairs + 1);
		//	m_Mask = m_HashSize - 1;

		//	BB_FREE(m_HashTable);
		//	m_HashTable = (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );
		//	Memory::Memset( m_HashTable, 0xFF, m_HashSize * sizeof(uint32) );

		//	// Get some bytes for new entries
		//	Pair* NewPairs	= new Pair[m_HashSize];/* (Pair*) Memory::Malloc( m_HashSize * sizeof(Pair) );*/		BB_ASSERT(NewPairs);
		//	uint32* NewNext	= (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );	BB_ASSERT(NewNext);

		//	// Copy old data if needed
		//	for( uint32 i = 0; i < m_NbActivePairs; ++i )
		//		NewPairs[i] = m_Pairs[i];

		//	for( uint32 i=0; i < m_NbActivePairs; i++ )
		//	{
		//		uint32 HashValue = ((Hash<K> const &)*this)(m_Pairs[i].Key) & m_Mask;
		//		m_NextTable[i] = m_HashTable[HashValue];
		//		m_HashTable[HashValue] = i;
		//	}

		//	// Delete old data
		//	BB_FREE(m_NextTable);
		//	BB_DELETE_ARRAY(m_Pairs);

		//	// Assign new pointer
		//	m_Pairs = NewPairs;
		//	m_NextTable = NewNext;

		//	// Recompute hash value with new hash size
		//	HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;
		//}

		// Recompute hash value with new hash size
		HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;

		Pair* p = &m_Pairs[m_NbActivePairs];
		p->Key = Key;
		p->Value = Value;

		m_NextTable[m_NbActivePairs] = m_HashTable[HashValue];
		m_HashTable[HashValue] = m_NbActivePairs++;

		return p;
	}

	inline uint32 GetPairIndex(const Pair* pair) const
	{
		return ((uint32)((size_t(pair) - size_t(m_Pairs))) / sizeof(Pair));
	}

	bool Remove( K const& Key )
	{
		const uint32 HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;
		const Pair* P = Find( Key, HashValue );
		if(!P)	return false;
		BB_ASSERT(P->Key==Key);

		const uint32 PairIndex = GetPairIndex(P);

		// Walk the hash table to fix m_NextTable
		uint32 Offset = m_HashTable[HashValue];
		BB_ASSERT(Offset!=INDEX_NONE);

		uint32 Previous=INDEX_NONE;
		while(Offset!=PairIndex)
		{
			Previous = Offset;
			Offset = m_NextTable[Offset];
		}

		// Let us go/jump us
		if(Previous!=INDEX_NONE)
		{
			BB_ASSERT(m_NextTable[Previous]==PairIndex);
			m_NextTable[Previous] = m_NextTable[PairIndex];
		}
		// else we were the first
		else 
			m_HashTable[HashValue] = m_NextTable[PairIndex];
		// we're now free to reuse next[pairIndex] without breaking the list

#if BB_BUILD_DEBUG
		m_NextTable[PairIndex]=INDEX_NONE;
#endif

		// Fill holes
		if(1)
		{
			// 1) Remove last pair
			const uint32 LastPairIndex = m_NbActivePairs-1;
			if(LastPairIndex==PairIndex)
			{
				m_NbActivePairs--;
				return true;
			}

			const Pair* Last = &m_Pairs[LastPairIndex];
			const uint32 LastHashValue = ((Hash<K> const &)*this)(Last->Key) & m_Mask;

			// Walk the hash table to fix m_NextTable
			uint32 Offset = m_HashTable[LastHashValue];
			BB_ASSERT(Offset!=INDEX_NONE);

			uint32 Previous=INDEX_NONE;
			while(Offset!=LastPairIndex)
			{
				Previous = Offset;
				Offset = m_NextTable[Offset];
			}

			// Let us go/jump us
			if(Previous!=INDEX_NONE)
			{
				BB_ASSERT(m_NextTable[Previous]==LastPairIndex);
				m_NextTable[Previous] = m_NextTable[LastPairIndex];
			}
			// else we were the first
			else m_HashTable[LastHashValue] = m_NextTable[LastPairIndex];
			// we're now free to reuse m_NextTable[LastPairIndex] without breaking the list

#if BB_BUILD_DEBUG
			m_NextTable[LastPairIndex]=INDEX_NONE;
#endif

			// Don't invalidate entry since we're going to shrink the array

			// 2) Re-insert in free slot
			m_Pairs[PairIndex] = m_Pairs[LastPairIndex];
#if BB_BUILD_DEBUG
			BB_ASSERT(m_NextTable[PairIndex]==INDEX_NONE);
#endif
			m_NextTable[PairIndex] = m_HashTable[LastHashValue];
			m_HashTable[LastHashValue] = PairIndex;

			m_NbActivePairs--;
		}
		return true;
	}

#if 0
	void Serialize( File* DataFile )
	{
		if( DataFile->IsReading() )
		{
			uint32 NewHashSize = 0;
			DataFile->Serialize( NewHashSize );
			reserve( NewHashSize );
		}
		else
		{
			DataFile->Serialize( m_HashSize );
		}

		DataFile->Serialize( m_NbActivePairs );
		DataFile->Serialize( m_Pairs, sizeof(Pair) * m_NbActivePairs );
		DataFile->Serialize( m_HashTable, sizeof(uint32) * m_HashSize );
		DataFile->Serialize( m_NextTable, sizeof(uint32) * m_HashSize );
	}
#endif

	uint32 GetPairCount()			{ return m_NbActivePairs; }
	Pair* GetPairAt( uint32 Index )	{ BB_ASSERT(Index >= 0 && Index < m_NbActivePairs); return &m_Pairs[Index]; }

public:
	Pair*			m_Pairs;
	uint32*			m_HashTable;
	uint32*			m_NextTable;
	uint32			m_HashSize;
	uint32			m_Mask;
	uint32			m_NbActivePairs;

};

} /* namespace bigfx */


#endif // BB_COREMAP_H

