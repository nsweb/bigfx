
#ifndef BB_COREMAP_RH_H
#define BB_COREMAP_RH_H

#include "hash.h"

namespace bigfx
{

/** Implementation of robin hood hashing */
template<typename K, typename V> 
class MapRH : protected Hash<K>
{
public:
	struct Pair
	{
		K Key;
		V Value;
		Pair() {}
		Pair(K&& InKey, V&& InValue) : Key(std::move(InKey)), Value(std::move(InValue)) {}
	};

	static const int32 LOAD_RATIO = 9;

	MapRH() :
		m_pairs(nullptr),
		m_hash_table(nullptr),
		//m_NextTable(nullptr),
		m_hash_size(0),
		//m_Mask(0),
		m_active_pair_count(0)
	{

	}
	~MapRH()
	{
		free_buffers();

		m_hash_size = 0;
		//m_Mask = 0;
		m_active_pair_count = 0;
	}

	MapRH( MapRH&& Other) : 
		m_pairs(Other.m_Pairs),
		m_hash_table(Other.m_HashTable),
		m_hash_size(Other.m_HashSize),
		//m_Mask(Other.m_Mask),
		m_active_pair_count(Other.m_NbActivePairs)	
	{
		Other.m_Pairs = nullptr;
		Other.m_HashTable = nullptr;
		Other.m_HashSize = 0;
	}

	MapRH( MapRH const& Other) : 
		m_hash_size(Other.m_hash_size),
		//m_Mask(Other.m_Mask),
		m_active_pair_count(Other.m_active_pair_count)	

	{
		copy_buffers( Other );
	}

	MapRH& operator=( MapRH&& Other )		
	{ 
		std::swap( m_pairs, Other.m_Pairs );
		std::swap( m_hash_table, Other.m_HashTable );
		std::swap( m_hash_size, Other.m_HashSize );
		//m_Mask = Other.m_Mask;
		m_active_pair_count = Other.m_NbActivePairs;
		return *this; 
	}

	MapRH& operator=( MapRH const& Other ) 
	{
		free_buffers();

		m_hash_size = Other.m_hash_size;		
		//m_Mask = Other.m_Mask;
		m_active_pair_count = Other.m_active_pair_count;

		copy_buffers( Other );
		return *this; 
	}

	void free_buffers()
	{
		for( uint32 i = 0; i < m_hash_size; ++i)
		{
			if( elem_hash(i) != 0 )
				m_pairs[i].~Pair();
		}
		BB_FREE(m_pairs);
		BB_FREE(m_hash_table);
	}

	void copy_buffers( MapRH const& Other )
	{
		if( Other.m_hash_size )
		{
			m_pairs	= (Pair*) Memory::Malloc( Other.m_hash_size * sizeof(Pair), ALIGN_OF(Pair) );
			// Cannot memcpy here, can Pair is not necessarily POD
			for( uint32 i = 0; i < Other.m_hash_size; ++i )
			{
				if( Other.elem_hash(i) != 0 )
				{
					new (&m_pairs[i]) Pair();	
					m_pairs[i] = Other.m_pairs[i];
				}
			}

			m_hash_table = (uint32*) Memory::Malloc( Other.m_hash_size * sizeof(uint32) );
			Memory::Memcpy( m_hash_table, Other.m_hash_table, Other.m_hash_size * sizeof(uint32) );
		}
		else
		{
			m_pairs = nullptr;
			m_hash_table = nullptr;
		}
	}

	uint32 hash_key( K const& Key ) const
	{			
		uint32 HashValue = ((Hash<K> const &)*this)(Key);
 
		// MSB is used to indicate a deleted elem, so
		// clear it
		HashValue &= 0x7fffffff;
 
		// Ensure that we never return 0 as a hash,
		// since we use 0 to indicate that the elem has never
		// been used at all.
		HashValue |= HashValue==0;
		return HashValue; 
	}

	static bool is_deleted( uint32 HashValue )
	{
		// MSB set indicates that this hash is a "tombstone"
		return (HashValue >> 31) != 0;
	}
 
	int probe_distance(uint32 HashValue, uint32 slot_index) const
	{	
		return (slot_index + m_hash_size - desired_pos(HashValue)) % m_hash_size;// & m_Mask;
	}

	uint32 desired_pos( uint32 HashValue ) const
	{
		return HashValue % m_hash_size;//& m_Mask;
	}
 
	uint32& elem_hash(uint32 ix)
	{
		return m_hash_table[ix];
	}
	uint32 elem_hash(uint32 ix) const
	{
		return m_hash_table[ix];
	}

	void reserve( uint32 HashSize, bool bForcePowerOfTwo = true )
	{
		if( HashSize > m_hash_size )
		{
			// Get more entries
			uint32 OldHashSize = m_hash_size;
			Pair* OldPairs = m_pairs;
			uint32* OldHashTable = m_hash_table;

			m_hash_size = (bForcePowerOfTwo ? (bigfx::IsPowerOfTwo(HashSize) ? HashSize : bigfx::NextPowerOfTwo(HashSize)) : HashSize);
			//m_Mask = m_HashSize - 1;

			m_hash_table = (uint32*) Memory::Malloc( m_hash_size * sizeof(uint32) );
			Memory::Memset( m_hash_table, 0, m_hash_size * sizeof(uint32) );			// flag all new elems as free

			uint32 Alignment = ALIGN_OF(Pair);
			uint32 SizePair = m_hash_size * sizeof(Pair);
			m_pairs	= (Pair*) Memory::Malloc( SizePair, Alignment );

			// Reinsert old data
			for( uint32 i = 0; i < OldHashSize; ++i )
			{
				auto& e = OldPairs[i];
				uint32 HashValue = OldHashTable[i];

				if( HashValue != 0 && !is_deleted(HashValue) )
				{
					insert_helper( std::move(e.Key), std::move(e.Value), HashValue );
					e.~Pair();
				}
			}

			BB_FREE(OldPairs);
			BB_FREE(OldHashTable);
		}
	}

	/** Call this function to optimize allocated mem, when map is static and will not receive new elements */
	bool optimize( int32 LoadRatio = LOAD_RATIO )
	{
		MapRH backup_map( *this );

		uint32 OldHashSize = m_hash_size;
		Pair* OldPairs = m_pairs;
		uint32* OldHashTable = m_hash_table;

		m_hash_size = (m_active_pair_count * 10) / LoadRatio;

		m_hash_table = (uint32*) Memory::Malloc( m_hash_size * sizeof(uint32) );
		Memory::Memset( m_hash_table, 0, m_hash_size * sizeof(uint32) );			// flag all new elems as free

		m_pairs	= (Pair*) Memory::Malloc( m_hash_size * sizeof(Pair), ALIGN_OF(Pair) );

		// Try to reinsert old data
		uint32 insert_idx;
		for( insert_idx = 0; insert_idx < OldHashSize; ++insert_idx )
		{
			auto& e = OldPairs[insert_idx];
			uint32 HashValue = OldHashTable[insert_idx];

			if( HashValue != 0 && !is_deleted(HashValue) )
			{
				Pair* inserted_pair = insert_helper( std::move(e.Key), std::move(e.Value), HashValue );
				if( inserted_pair )
					e.~Pair();
				else
					break;
			}
		}

		BB_FREE(OldPairs);
		BB_FREE(OldHashTable);

		if( insert_idx != OldHashSize )
		{
			// Unable to reinsert old element, abort
			*this = std::move( backup_map );
			return false; 
		}

		return true;
	}

	const Pair*	Find( K const& Key ) const
	{
		//if( !m_HashTable )	return nullptr;	// Nothing has been allocated yet

		// Compute hash value for this key
		uint32 HashValue = hash_key( Key );
		return Find( Key, HashValue );
	}

	Pair* Find( K const& Key, uint32 HashValue ) const
	{
		if( !m_hash_size )	return nullptr;	// Nothing has been allocated yet

		// Look for it in the table
		uint32 pos = desired_pos(HashValue);
		int32 dist = 0;
		for(;;)
		{							
			if( elem_hash(pos) == 0 ) 
				return nullptr;
			else if (dist > probe_distance(elem_hash(pos), pos)) 
				return nullptr;
			else if (elem_hash(pos) == HashValue && m_pairs[pos].Key == Key) 
				return &m_pairs[pos];				

			pos = (pos+1) % m_hash_size;// & m_Mask;
			++dist;
		}
	}

	Pair* Add( K Key, V Value )
	{
		uint32 HashValue = hash_key( Key );
		Pair* P = Find(Key, HashValue);
		if(P)
			return P;	// Persistent pair

		if( ++m_active_pair_count > (m_hash_size*LOAD_RATIO) / 10 )
			reserve( m_hash_size + 1 );

		return insert_helper( std::move(Key), std::move(Value), HashValue );
	}

	void construct(uint32 pos, uint32 HashValue, K&& Key, V&& Value)
	{
		new (&m_pairs[pos]) Pair(std::move(Key), std::move(Value));	
		elem_hash(pos) = HashValue;
	}

	Pair* insert_helper( K&& Key, V&& Value, uint32 HashValue )
	{
		// This is a new pair
		uint32 pos = desired_pos(HashValue);
		uint32 insert_pos = INDEX_NONE;
		int32 dist = 0;
		for(;;)
		{			
			if(elem_hash(pos) == 0)
			{
				construct( pos, HashValue, std::move(Key), std::move(Value) );
				return insert_pos != INDEX_NONE ? &m_pairs[insert_pos] : &m_pairs[pos];
			}

			// If the existing elem has probed less than us, then swap places with existing
			// elem, and keep going to find another slot for that elem.
			int existing_elem_probe_dist = probe_distance(elem_hash(pos), pos);
			if (existing_elem_probe_dist < dist)
			{	
				if(is_deleted(elem_hash(pos)))
				{
					construct( pos, HashValue, std::move(Key), std::move(Value) );
					return insert_pos != INDEX_NONE ? &m_pairs[insert_pos] : &m_pairs[pos];
				}

				//uint32 hash_temp = elem_hash(pos);
				//HashValue = hash_temp;
				//elem_hash(pos) = hash_temp;
				std::swap(HashValue, elem_hash(pos));
				std::swap(Key, m_pairs[pos].Key);
				std::swap(Value, m_pairs[pos].Value);
				dist = existing_elem_probe_dist;	

				if( insert_pos == INDEX_NONE )
					insert_pos = pos;
			}

			pos = (pos+1) % m_hash_size;// & m_Mask;
			++dist;			
		}
	}

	inline uint32 GetPairIndex(const Pair* pair) const
	{
		return ((uint32)((size_t(pair) - size_t(m_pairs))) / sizeof(Pair));
	}

	bool Remove( K const& Key )
	{
		const uint32 HashValue = hash_key( Key );
		const Pair* P = Find( Key, HashValue );
		if(!P)	
			return false;
		BB_ASSERT(P->Key==Key);
		uint32 idx = GetPairIndex(P);

		m_pairs[idx].~Pair();
		elem_hash(idx) |= 0x80000000; // mark as deleted
		--m_active_pair_count;

		return true;
	}

	uint32 GetReservedSize() const			{ return m_hash_size;			}
	uint32 GetActivePairCount() const		{ return m_active_pair_count;		}
	Pair* GetPairAt( uint32 Index ) const 	{ BB_ASSERT(Index >= 0 && Index < m_hash_size); return &m_pairs[Index]; }
	bool IsPairValid( uint32 Index ) const 	
	{ 
		BB_ASSERT(Index >= 0 && Index < m_hash_size); 
		uint32 HashValue = m_hash_table[Index]; 
		return ( HashValue != 0 && !is_deleted(HashValue) );
	}

public:
	Pair*			m_pairs;
	uint32*			m_hash_table;
	uint32			m_hash_size;
	uint32			m_active_pair_count;

};

} /* namespace bigfx */


#endif // BB_COREMAP_RH_H

