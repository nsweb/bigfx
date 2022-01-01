

#include "../bigfx.h"

namespace bigfx 
{

#define BB_VEC_2_CONST(type, name, a, b) \
    template<> \
    Vec2<type> const Vec2<type>::name = Vec2<type>((type)a, (type)b);

#define BB_VEC_3_CONST(type, name, a, b, c) \
    template<> \
    Vec3<type> const Vec3<type>::name = Vec3<type>((type)a, (type)b, (type)c);

#define BB_VEC_4_CONST(type, name, a, b, c, d) \
    template<> \
    Vec4<type> const Vec4<type>::name = Vec4<type>((type)a, (type)b, (type)c, (type)d);

#define BB_MAT_CONST(type, name, a) \
    template<> \
    Mat2<type> const Mat2<type>::name = Mat2<type>((type)a); \
    \
    template<> \
    Mat3<type> const Mat3<type>::name = Mat3<type>((type)a); \
    \
    template<> \
    Mat4<type> const Mat4<type>::name = Mat4<type>((type)a);

#define BB_ALL_CONST_INNER(type) \
    BB_VEC_2_CONST(type, one, 1, 1) \
    BB_VEC_2_CONST(type, mone,-1,-1) \
    BB_VEC_2_CONST(type, zero, 0, 0) \
    BB_VEC_2_CONST(type, axis_x, 1, 0) \
    BB_VEC_2_CONST(type, axis_y, 0, 1) \
    \
    BB_VEC_3_CONST(type, one, 1, 1, 1) \
    BB_VEC_3_CONST(type, mone,-1,-1,-1) \
    BB_VEC_3_CONST(type, zero, 0, 0, 0) \
    BB_VEC_3_CONST(type, axis_x, 1, 0, 0) \
    BB_VEC_3_CONST(type, axis_y, 0, 1, 0) \
    BB_VEC_3_CONST(type, axis_z, 0, 0, 1) \
    \
    BB_VEC_4_CONST(type, one, 1, 1, 1, 1) \
    BB_VEC_4_CONST(type, mone,-1,-1,-1,-1) \
    BB_VEC_4_CONST(type, zero, 0, 0, 0, 0) \
    BB_VEC_4_CONST(type, axis_x, 1, 0, 0, 0) \
    BB_VEC_4_CONST(type, axis_y, 0, 1, 0, 0) \
    BB_VEC_4_CONST(type, axis_z, 0, 0, 1, 0) \
    BB_VEC_4_CONST(type, axis_w, 0, 0, 0, 1) \
    \
    BB_MAT_CONST(type, identity, 1)

BB_ALL_CONST_INNER(float)
BB_ALL_CONST_INNER(double)

BB_ALL_CONST_INNER(int8_t)
BB_ALL_CONST_INNER(uint8_t)
BB_ALL_CONST_INNER(int16_t)
BB_ALL_CONST_INNER(uint16_t)
BB_ALL_CONST_INNER(int32_t)
BB_ALL_CONST_INNER(uint32_t)
BB_ALL_CONST_INNER(int64_t)
BB_ALL_CONST_INNER(uint64_t)

}; /* namespace bigfx */

