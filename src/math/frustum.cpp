
#include "../bigfx.h"

//#include <cstdlib> /* free() */
//#include <cstring> /* strdup() */


//using namespace std;

namespace bigfx
{

#if 0
template<> std::ostream &operator<<(std::ostream &stream, ivec2 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, icmplx const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, ivec3 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, ivec4 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, iquat const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, vec2 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, cmplx const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, vec3 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, vec4 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, quat const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, mat4 const &m)
{
    stream << "((" << m[0][0] << ", " << m[1][0]
            << ", " << m[2][0] << ", " << m[3][0] << "), ";
    stream << "(" << m[0][1] << ", " << m[1][1]
           << ", " << m[2][1] << ", " << m[3][1] << "), ";
    stream << "(" << m[0][2] << ", " << m[1][2]
           << ", " << m[2][2] << ", " << m[3][2] << "), ";
    stream << "(" << m[0][3] << ", " << m[1][3]
           << ", " << m[2][3] << ", " << m[3][3] << "))";
    return stream;
}
#endif

} /* namespace bigfx */

