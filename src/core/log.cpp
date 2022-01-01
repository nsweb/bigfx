
#include "../bigfx.h"

namespace bigfx
{

void BBLog::Print( char const* file, int line, char const* category, Type type, char const* format, ... )
{
	va_list ap;
	va_start(ap, format);

	char const* log_prefix[] =
	{
		"Info",
		"Warning",
		"ERROR",
	};
	String buffer_string = String::Printf("%s(%d) [%s] %s : %s", file, line, log_prefix[type], category, format);
	bx::debugPrintfVargs(buffer_string.c_str(), ap);

	va_end(ap);
}

} /* namespace bigfx */

