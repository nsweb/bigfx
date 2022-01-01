
#ifndef BB_COREARRAY_H
#define BB_COREARRAY_H

 
namespace bigfx
{
	
/*
 * The base array type.
 *
 * Contains an m_data memory array of Elements, of which only the first
 * m_count are allocated. The rest is uninitialized memory.
 */

template<typename T> 
class /*BIGFX_API*/ Array
{
public:
    typedef T Element;

    inline Array() : m_data(0), m_count(0), m_reserved(0)
    {
    }

    inline ~Array()
    {
        for (int i = 0; i < m_count; i++)
            m_data[i].~Element();
        delete[] reinterpret_cast<uint8_t *>(m_data);
    }

    Array(Array const& that) : m_data(0), m_count(0), m_reserved(0)
    {
        /* reserve the exact number of values instead of what the other
         * array had reserved. Just a method for not wasting too much. */
        reserve(that.m_count);
        for (int i = 0; i < that.m_count; i++)
            new(&m_data[i]) Element(that[i]);
        m_count = that.m_count;
    }

	Array(Array&& that) : 
		m_data(that.m_data),
		m_count(that.m_count),
		m_reserved(that.m_reserved)	
	{
		that.m_data = nullptr;
		that.m_count = 0;
	}

    Array& operator=(Array const& that)
    {
        if ((uintptr_t)this != (uintptr_t)&that)
        {
            /* FIXME: there is an opportunity for optimisation here if we
             * find a way to ask reserve not to create new elements, since
             * we're going to overwrite them anyway. */
            if (m_reserved < that.m_count)
            {
                /* If not enough space, reserve memory, overwrite the first
                 * elements, then use placement new directly for the
                 * remaining elements. */
                reserve(that.m_count);
                for (int i = 0; i < m_count && i < that.m_count; i++)
                    m_data[i] = Element(that[i]);
                for (int i = m_count; i < that.m_count; i++)
                    new(&m_data[i]) Element(that[i]);
            }
            else
            {
                /* If enough space, overwrite the common elements, then
                 * use placement new for the elements in the other array
                 * that we do not have, and finally destroy the remaining
                 * elements. */
                for (int i = 0; i < m_count && i < that.m_count; i++)
                    m_data[i] = Element(that[i]);
                for (int i = m_count; i < that.m_count; i++)
                    new(&m_data[i]) Element(that[i]);
                for (int i = that.m_count; i < m_count; i++)
                    m_data[i].~Element();
            }
            m_count = that.m_count;
        }
        return *this;
    }

	Array& operator=( Array&& that )		
	{ 
		std::swap( m_data, that.m_data );
		std::swap( m_count, that.m_count );
		std::swap( m_reserved, that.m_reserved );
		return *this; 
	}

    Array& operator+=(Array const &that)
    {
        int todo = that.m_count;
        reserve(m_count + that.m_count);
        for (int i = 0; i < todo; i++)
            new(&m_data[m_count + i]) Element(that[i]);
        m_count += todo;
        return *this;
    }

    Array operator+(Array const &that) const
    {
        Array ret;
        ret.reserve(m_count + that.m_count);
        ret += *this;
        ret += that;
        return ret;
    }

    inline Element& operator[](int n)
    {
        /* Allow array[0] even if size is zero so that people can
         * always use &array[0] to get a pointer to the data. */
        BB_ASSERT(n >= 0);
        BB_ASSERT((unsigned)n < (unsigned)m_count || (!n && !m_count));
        return m_data[n];
    }

    inline Element const& operator[](int n) const
    {
        BB_ASSERT(n >= 0);
        BB_ASSERT(n < m_count || (!n && !m_count));
        return m_data[n];
    }

    inline Element& Last()
    {
        BB_ASSERT(m_count > 0);
        return m_data[m_count - 1];
    }

    inline Element *Data()
    {
        return m_data;
    }

    inline Element const *Data() const
    {
        return m_data;
    }

    inline Element const& Last() const
    {
        BB_ASSERT(m_count > 0);
        return m_data[m_count - 1];
    }

    inline Array& operator<<(Element const &item)
    {
        if (m_count >= m_reserved)
        {
            Element tmp(item);
            reserve(m_count * 13 / 8 + 8);
            new (&m_data[m_count++]) Element(tmp);
        }
        else
        {
            new (&m_data[m_count++]) Element(item);
        }
        return *this;
    }

    inline void push_back(Element const &item)
    {
        *this << item;
    }

    inline Element pop_back()
    {
        BB_ASSERT(m_count > 0);
        Element tmp = Last();
        erase(m_count - 1, 1);
        return tmp;
    }

    void Swap(int pos1, int pos2)
    {
        BB_ASSERT(pos1 >= 0);
        BB_ASSERT(pos2 >= 0);
        BB_ASSERT(pos1 < m_count);
        BB_ASSERT(pos2 < m_count);

        if (pos1 != pos2)
        {
            Element tmp = (*this)[pos1];
            (*this)[pos1] = (*this)[pos2];
            (*this)[pos2] = tmp;
        }
    }

    void erase(int pos, int todelete = 1)
    {
        BB_ASSERT(todelete >= 0);
        BB_ASSERT(pos - todelete >= -m_count - 1);
        BB_ASSERT(pos + todelete <= m_count);
        if (pos < 0)
            pos = m_count + pos;

        for (int i = pos; i + todelete < m_count; i++)
            m_data[i] = m_data[i + todelete];
        for (int i = m_count - todelete; i < m_count; i++)
            m_data[i].~Element();
        m_count -= todelete;
    }

    void erase_swap(int pos, int todelete = 1)
    {
        BB_ASSERT(todelete >= 0);
        BB_ASSERT(pos - todelete >= -m_count - 1);
        BB_ASSERT(pos + todelete <= m_count);
        if (pos < 0)
            pos = m_count + pos;

        for (int i = 0; i < todelete; i++)
        {
            if (pos + i < m_count - 1 - i)
                m_data[pos + i] = m_data[m_count - 1 - i];
            m_data[m_count - 1 - i].~Element();
        }
        m_count -= todelete;
    }
    
    void insert( const Element& item, int pos )
    {
        BB_ASSERT(pos >= 0);
        BB_ASSERT(pos <= m_count);
        if (m_count >= m_reserved)
            reserve(m_count+1);
        
        for (int i = m_count; i > pos; --i)
        {
            new (&m_data[i]) Element(m_data[i - 1]);
            m_data[i - 1].~Element();
        }
        new (&m_data[pos]) Element(item);
        ++m_count;
    }

	void insert( Array const &that, int pos )
	{
		BB_ASSERT(pos >= 0);
		BB_ASSERT(pos <= m_count);
		if (m_count + that.m_count > m_reserved)
			reserve(m_count + that.m_count);

		Memory::Memmove( &m_data[pos + that.m_count], &m_data[pos], that.m_count * sizeof(Element) );
		Memory::Memcpy( &m_data[pos], &that.m_data[0], that.m_count * sizeof(Element) );

		m_count += that.m_count;
	}

	void remove( const Element& item )
    {
        for( int i = 0; i < m_count; )
        {
        	if( m_data[i] == item )
				erase( i, 1 );
			else
				++i;
        }
    }

	void remove_swap( const Element& item )
	{
		for( int i = 0; i < m_count; )
		{
        	if( m_data[i] == item )
				erase_swap( i, 1 );
			else
				++i;
		}
	}

    void resize(int count, Element e = Element())
    {
        BB_ASSERT(count >= 0);
		reserve( count, true );

        /* Too many elements? Remove them. */
        for (int i = count; i < m_count; ++i)
            m_data[i].~Element();

        /* Not enough elements? Add some. */
        for (int i = m_count; i < count; ++i)
            new(&m_data[i]) Element(e);

        m_count = count;
    }

    inline void clear()
    {
        erase(0, m_count);
    }

    void reserve(int toreserve, bool nextpoweroftwo = false)
    {
        if (toreserve <= (int)m_reserved)
            return;

        if( nextpoweroftwo && !IsPowerOfTwo(toreserve) )
			toreserve = NextPowerOfTwo(toreserve);

        /* This cast is not very nice, because we kill any alignment
         * information we could have. But until C++ gives us the proper
         * tools to deal with it, we assume new uint8_t[] returns properly
         * aligned data. */
        Element *tmp = reinterpret_cast<Element *>(reinterpret_cast<uintptr_t>
                               (new uint8_t[sizeof(Element) * toreserve]));
        for (int i = 0; i < m_count; i++)
        {
            new(&tmp[i]) Element(m_data[i]);
            m_data[i].~Element();
        }
        if (m_data)
            delete[] reinterpret_cast<uint8_t *>(m_data);
        m_data = tmp;
        m_reserved = toreserve;
    }

    inline int size() const { return m_count; }
	inline int size_reserved() const { return m_reserved; }
    inline int Bytes() const { return m_count * sizeof(Element); }

	int find( const Element& Item ) const
	{
		for( int i = 0; i < m_count; ++i )
		{
        	if( m_data[i] == Item )
				return i;
		}
		return -1;
	}

	template <typename KeyType>
	int FindByKey( const KeyType& Key ) const
	{
		for( int i = 0; i < m_count; ++i )
		{
        	if( m_data[i] == Key )
				return i;
		}
		return -1;
	}

protected:
    Element *m_data;
    int m_count, m_reserved;
};

} /* namespace bigfx */

#endif // BB_COREARRAY_H