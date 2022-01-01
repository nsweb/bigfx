

#ifndef BB_JSON_H
#define BB_JSON_H

namespace bigfx
{

namespace json
{
typedef int32 TokenIdx;

enum TokenType
{
	OBJECT = 1,
	ARRAY = 2,
	STRING = 4,
	PRIMITIVE = 8
};

class Object
{
public:
	Object()	{}
	~Object()	{}
	bool		ParseFile( char const* path );
	bool		ParseString( char const* sz );
	TokenIdx	GetToken( char const* sz, TokenType type, TokenIdx parent_idx = INDEX_NONE );
	bool		GetStringValue( TokenIdx token_idx, String& val );
	bool		IsStringValue( TokenIdx token_idx, char const* sz );
	float		GetFloatValue( TokenIdx token_idx, float default_value = 0.f );
	int			GetIntValue(TokenIdx token_idx, int default_value = 0);
    int         GetArraySize( TokenIdx token_idx );
    bool		GetArrayStringValue( TokenIdx token_idx, int elt_idx, String& val );

private:
	Array<jsmntok_t> m_tokens;
	String			 m_str;
};
	

} /* namespace json */

} /* namespace bigfx */

#endif // BB_JSON_H
