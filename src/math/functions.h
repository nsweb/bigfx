

#ifndef BB_MATHFUNCTIONS_H
#define BB_MATHFUNCTIONS_H


namespace bigfx
{

/* This is OUR namespace. Don't let Windows headers fuck with it. */
//#undef min
//#undef max

/* Standard cmath functions */
static inline double sqrt(double x) { return std::sqrt(x); }
static inline float sqrt(float x) { return std::sqrt(x); }

static inline double exp(double const &x) { return std::exp(x); }
static inline float exp(float const &x) { return std::exp(x); }

static inline double sin(double const &x) { return std::sin(x); }
static inline double cos(double const &x) { return std::cos(x); }
static inline double tan(double const &x) { return std::tan(x); }
static inline float sin(float const &x) { return std::sin(x); }
static inline float cos(float const &x) { return std::cos(x); }
static inline float tan(float const &x) { return std::tan(x); }

static inline double asin(double const &x) { return std::asin(x); }
static inline double acos(double const &x) { return std::acos(x); }
static inline double atan(double const &x) { return std::atan(x); }
static inline float asin(float const &x) { return std::asin(x); }
static inline float acos(float const &x) { return std::acos(x); }
static inline float atan(float const &x) { return std::atan(x); }

static inline double atan2(double const &y, double const &x)
{
    return std::atan2(y, x);
}

static inline float atan2(float const &y, float const &x)
{
    return std::atan2(y, x);
}

static inline double pow(double const &x, double const &y)
{
    return std::pow(x, y);
}

static inline float pow(float const &x, float const &y)
{
    return std::pow(x, y);
}

static inline double log(double const &x) { return std::log(x); }
static inline float log(float const &x) { return std::log(x); }
    
BIGFX_API uint32 NextPowerOfTwo( uint32 x );
BIGFX_API bool IsPowerOfTwo( uint32 x );
BIGFX_API uint32 randhash(uint32 seed);
static inline int32 rand()      { return std::rand();    }
static inline float randfloat()      { return (float)std::rand() / (float)RAND_MAX;    }
static inline void randinit(int32 seed)   { return srand(seed);    }
    

/* C++ doesn't define abs() and fmod() for all types; we add these for
 * convenience to avoid adding complexity to vector.h. */
static inline int8_t abs(int8_t x) { return std::abs(x); }
static inline uint8_t abs(uint8_t x) { return x; }
static inline int16_t abs(int16_t x) { return std::abs(x); }
static inline uint16_t abs(uint16_t x) { return x; }
static inline int32_t abs(int32_t x) { return std::abs(x); }
static inline uint32_t abs(uint32_t x) { return x; }

static inline int64_t abs(int64_t x) { return std::abs(x); }

static inline uint64_t abs(uint64_t x) { return x; }
static inline float abs(float x) { return std::abs(x); }
static inline double abs(double x) { return std::abs(x); }


static inline uint8_t fmod(uint8_t x, uint8_t y) { return x % y; }
static inline int8_t fmod(int8_t x, int8_t y) { return x % y; }
static inline uint16_t fmod(uint16_t x, uint16_t y) { return x % y; }
static inline int16_t fmod(int16_t x, int16_t y) { return x % y; }
static inline uint32_t fmod(uint32_t x, uint32_t y) { return x % y; }
static inline int32_t fmod(int32_t x, int32_t y) { return x % y; }
static inline uint64_t fmod(uint64_t x, uint64_t y) { return x % y; }
static inline int64_t fmod(int64_t x, int64_t y) { return x % y; }
static inline float fmod(float x, float y) { return std::fmod(x, y); }
static inline double fmod(double x, double y) { return std::fmod(x, y); }


static inline uint8_t floor(uint8_t x) { return x; }
static inline int8_t floor(int8_t x) { return x; }
static inline uint16_t floor(uint16_t x) { return x; }
static inline int16_t floor(int16_t x) { return x; }
static inline uint32_t floor(uint32_t x) { return x; }
static inline int32_t floor(int32_t x) { return x; }
static inline uint64_t floor(uint64_t x) { return x; }
static inline int64_t floor(int64_t x) { return x; }
static inline float floor(float x) { return std::floor(x); }
static inline double floor(double x) { return std::floor(x); }


static inline uint8_t ceil(uint8_t x) { return x; }
static inline int8_t ceil(int8_t x) { return x; }
static inline uint16_t ceil(uint16_t x) { return x; }
static inline int16_t ceil(int16_t x) { return x; }
static inline uint32_t ceil(uint32_t x) { return x; }
static inline int32_t ceil(int32_t x) { return x; }
static inline uint64_t ceil(uint64_t x) { return x; }
static inline int64_t ceil(int64_t x) { return x; }
static inline float ceil(float x) { return std::ceil(x); }
static inline double ceil(double x) { return std::ceil(x); }


#define BB_GENERIC_FUNC(T) \
    static inline T sq(T x) { return x * x; } \
    static inline T fract(T x) { return x - bigfx::floor(x); } \
    static inline T min(T x, T y) { return std::min(x, y); } \
    static inline T max(T x, T y) { return std::max(x, y); } \
    static inline T clamp(T x, T a, T b) { return min(max(x, a), b); }

#define BB_GENERIC_FUNC_SIGNED(T) \
    BB_GENERIC_FUNC(T) \
    static inline T sign(T x) { return (T)(((T)0 < x) - (x < (T)0)); }

#define BB_GENERIC_FUNC_UNSIGNED(T) \
    BB_GENERIC_FUNC(T) \
    static inline T sign(T x) { return (T)((T)0 < x); }

BB_GENERIC_FUNC_UNSIGNED(uint8_t)
BB_GENERIC_FUNC_SIGNED(int8_t)
BB_GENERIC_FUNC_UNSIGNED(uint16_t)
BB_GENERIC_FUNC_SIGNED(int16_t)
BB_GENERIC_FUNC_UNSIGNED(uint32_t)
BB_GENERIC_FUNC_SIGNED(int32_t)
BB_GENERIC_FUNC_UNSIGNED(uint64_t)
BB_GENERIC_FUNC_SIGNED(int64_t)
BB_GENERIC_FUNC_SIGNED(float)
BB_GENERIC_FUNC_SIGNED(double)

#undef BB_GENERIC_FUNC


static inline void sincos(double const &x, double *s, double *c)
{
	*s = std::sin(x);
	*c = std::cos(x);
}

static inline void sincos(float const &x, float *s, float *c)
{
	*s = std::sin(x);
	*c = std::cos(x);
}

static inline float lerp(float const &a, float const &b, float const &x)
{
	return a + (b - a) * x;
}

static inline double lerp(double const &a, double const &b, double const &x)
{
	return a + (b - a) * x;
}

// cubic Hermite curve aka smoothstep() (C1 continuity)
static inline float smoothstep(float x)
{
	x = bx::clamp(x, 0.f, 1.f);
	return x * x * (3.0f - 2.0f * x);
}

// quintic Hermite curve (C2 continuity)
static float interp_c2(float x)
{
	x = bx::clamp(x, 0.f, 1.f);
	return x * x * x * (x * (x * 6.f - 15.f) + 1.f);
}

} /* namespace bigfx */

#endif // BB_MATHFUNCTIONS_H