
#ifndef BB_SPLINES_H
#define BB_SPLINES_H


namespace bigfx
{

// From http://stackoverflow.com/questions/9489736/catmull-rom-curve-with-no-cusps-and-no-self-intersections/23980479#23980479
struct CubicSpline
{
	vec3 c0, c1, c2, c3;
	void Eval( float u, vec3& pos, vec3& tan ) const
	{
		float u2 = u * u;
		float u3 = u2 * u;
		pos = c0 + c1*u + c2*u2 + c3*u3;
        tan = c1 + 2.f*c2*u + 3.f*c3*u2;
	}
    void Eval2( float u, vec3& pos, vec3& tan, vec3& tan2 ) const
    {
        float u2 = u * u;
        float u3 = u2 * u;
        pos = c0 + c1*u + c2*u2 + c3*u3;
        tan = c1 + 2.f*c2*u + 3.f*c3*u2;
        tan2 = 2.f*c2 + 6.f*c3*u;
    }

	void InitNonuniformCatmullRom( const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float dt0, float dt1, float dt2 );
	void InitCubicSpline( const vec3& p0, const vec3& p1, const vec3& t0, const vec3& t1 );
};

struct ControlPoint
{
    ControlPoint() {}

	vec3 m_pos;

	float m_knot;
    float m_dist;
};
    
} /* namespace bigfx */


#endif // BB_SPLINES_H
