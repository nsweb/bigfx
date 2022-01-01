

#ifndef BB_CORELOG_H
#define BB_CORELOG_H

#include <stdint.h>
#include <cstdarg>

namespace bigfx
{

#define BB_LOG(CategoryName, Verbosity, Format, ...) \
{ \
	BBLog::Print(__FILE__, __LINE__, #CategoryName, BBLog::Verbosity, Format, ##__VA_ARGS__ ); \
}


class BIGFX_API BBLog
{
public:

	enum Type
	{
		Log,
		Warn,
		Error,
	};

    static void Print( char const* File, int Line, char const* Category, Type type, char const* Format, ... );

};

} /* namespace bigfx */

#endif // BB_CORELOG_H

