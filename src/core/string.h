
// The String class
// ----------------
// A very simple String class, based on Array.
//

#ifndef BB_CORESTRING_H
#define BB_CORESTRING_H

#include "templates.h"
#include "array.h"


namespace bigfx
{

class BIGFX_API String : protected Array<char>
{
private:
    typedef Array<char> Super;

public:
    inline String()
      : Super()
    {
        push_back('\0');
    }

    inline String(char const *str)
      : Super()
    {
        using namespace std;
        BB_ASSERT(str);
        resize((int)strlen(str));
        memcpy(&(*this)[0], str, Len() + 1);
    }

    inline String(char const *str, int count)
      : Super()
    {
        using namespace std;
        BB_ASSERT(str);
        resize(count);
        memcpy(&(*this)[0], str, count);
        ((Super &)*this)[count] = '\0';
    }

    String(String const &s)
      : Super((Super const &)s)
    {
    }

	String(String&& s)
		: Super(std::move(s))
	{
	}

	String& operator=( String&& that )
	{ 
		return (String&) Super::operator=( std::move(that) );
	}
    String& operator=( String const& that )
    {
        return (String&) Super::operator=( that );
    }
    //const volatile String&& operator=(String&) volatile const && = delete;

    inline char &operator [](int n)
    {
        /* Allow n == Len() because we might have reasonable reasons
         * to access that hidden null character. */
        BB_ASSERT(n >= 0);
        BB_ASSERT((unsigned)n <= (unsigned)Len());
        return ((Super &)*this)[n];
    }

    inline char const &operator [](int n) const
    {
        BB_ASSERT(n >= 0);
        BB_ASSERT(n <= Len());
        return ((Super const &)*this)[n];
    }

    inline char &Last()
    {
        BB_ASSERT(Len() > 0);
        return (*this)[Len() - 1];
    }

    inline char const &Last() const
    {
        BB_ASSERT(Len() > 0);
        return (*this)[Len() - 1];
    }

    inline int Len() const
    {
        return ((Super const &)*this).size() - 1;
    }

    /* Return a C string */
    inline char const *c_str() const
    {
        return &(*this)[0];
    }

    /* Non-const version; make sure to not overflow the internal array */
    inline char *c_str()
    {
        return &(*this)[0];
    }

    /* Does not initialise the newly allocated characters */
    void resize(int count)
    {
        BB_ASSERT(count >= 0);
        ((Super &)*this).resize(count + 1);
        ((Super &)*this).Last() = '\0';
    }

	void erase(int pos, int todelete)
	{
		BB_ASSERT(pos + todelete <= Len());
		((Super &)*this).erase(pos, todelete);
	}

    String Sub(int start, int count) const
    {
        BB_ASSERT(start >= 0);
        BB_ASSERT(count >= 0);
        BB_ASSERT(start + count <= Len());
        return String(&(*this)[start], count);
    }

    int IndexOf(char token) const
    {
        using namespace std;

        char const *tmp = strchr(c_str(), token);
        return tmp ? (int)(intptr_t)(tmp - c_str()) : -1;
    }

    int IndexOf(char const* token, int start=0) const
    {
		BB_ASSERT(start >= 0);
		BB_ASSERT(start <= Len());

        using namespace std;

        char const *tmp = strstr(c_str() + start, token);
        return tmp ? (int)(intptr_t)(tmp - c_str()) : -1;
    }

    int LastIndexOf(char token) const
    {
        using namespace std;

        char const *tmp = strrchr(c_str(), token);
        return tmp ? (int)(intptr_t)(tmp - c_str()) : -1;
    }

    int Replace(const char old_token, const char new_token, bool all_occurence=false)
    {
        using namespace std;

        int res = 0;
        char *tmp = NULL;
        while ((tmp = strrchr(c_str(), old_token)))
        {
            *tmp = new_token;
            res++;
            if (!all_occurence)
                break;
        }
        return res;
    }

    inline String& ToLower()
    {
        char* p = c_str();
        for (int i = 0; i < Len(); ++i)
            if ('A' <= p[i] && p[i] <= 'Z')
                p[i] += 'a' - 'A';
        return *this;
    }

    inline String& ToUpper()
    {
        char* p = c_str();
        for (int i = 0; i < Len(); ++i)
            if ('a' <= p[i] && p[i] <= 'z')
                p[i] += 'A' - 'a';
        return *this;
    }

    int LastIndexOf(char const* token, int start = INDEX_NONE) const
    {
        using namespace std;

        if (start == INDEX_NONE)
            start = Len();
        int token_len = (int)strlen(token);
        for (int i = start - token_len; i >= 0; --i)
            if (strstr(c_str() + i, token))
                return i;
        return -1;
    }

    bool StartsWith(String const &s) const
    {
        using namespace std;
        return Len() >= s.Len()
                && memcmp(c_str(), s.c_str(), s.Len()) == 0;
    }

    bool EndsWith(String const &s) const
    {
        using namespace std;
        return Len() >= s.Len()
                && memcmp(c_str() + Len() - s.Len(), s.c_str(), s.Len()) == 0;
    }

	void insert( String const &s, int pos )
	{
		BB_ASSERT(pos >= 0);
		BB_ASSERT(pos <= Len());
		int old_count = Len();
		resize(Len() + s.Len());

		Memory::Memmove( &(*this)[pos + s.Len()], &(*this)[pos], old_count - pos );
		Memory::Memcpy( &(*this)[pos], &s[0], s.Len() );
	}

    inline String operator +(String const &s) const
    {
        String ret(*this);
        return ret += s;
    }

    inline String& operator +=(String const &s)
    {
        using namespace std;
        int old_count = Len();
        resize(Len() + s.Len());
        Memory::Memcpy(&(*this)[old_count], &s[0], Len() - old_count);
        return *this;
    }

    inline String operator +(char c) const
    {
        String ret(*this);
        return ret += c;
    }

    inline String& operator +=(char c)
    {
        ((Super &)*this).Last() = c;
        ((Super &)*this).push_back('\0');
        return *this;
    }

    inline bool operator ==(String const &s) const
    {
        using namespace std;
        return Len() == s.Len()
                && Memory::Memcmp(c_str(), s.c_str(), Len()) == 0;
    }

    inline bool operator !=(String const &s) const
    {
        return !(*this == s);
    }

    inline bool operator ==(char const* sz) const
    {
        /* We parse the C string twice because of strlen + memcmp
         * but it's probably still faster than doing it by hand. */
        using namespace std;
        int sz_len = (int)strlen(sz);
        return Len() == sz_len
                && Memory::Memcmp(c_str(), sz, sz_len) == 0;
    }

    inline bool operator !=(char const* sz) const
    {
        return !(*this == sz);
    }

	static String Printf(char const *format, ...);
	static String vaPrintf(char const *format, va_list ap);
};

inline bool operator ==(char const* sz, String const &s)
{
    return s == sz;
}

inline bool operator !=(char const* sz, String const &s)
{
    return s != sz;
}

} /* namespace bigfx */

#endif // BB_CORESTRING_H

