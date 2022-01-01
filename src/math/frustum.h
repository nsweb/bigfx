

#ifndef BB_MATHFRUSTUM_H
#define BB_MATHFRUSTUM_H


namespace bigfx
{

template <typename T> class Frustum;
typedef Frustum<float> frustum;
typedef Frustum<double> dfrustum;

/*constexpr*/
#define BB_CONSTEXPR 


/*
 * Frustum
 */

template <typename T> class /*BIGFX_API*/ Frustum
{
public:
	enum eIntersectionStatus
	{
		FullyInside,
		Intersect,
		Outside,
	};
	enum eFrustumPlane
	{
		eFP_Near,
		eFP_Far,
		eFP_Right,
		eFP_Left,
		eFP_Top,
		eFP_Bottom,
		eFP_MAX,
	};
	enum eFrustumPoint
	{
		eFPT_NTL,
		eFPT_NTR,
		eFPT_NBL,
		eFPT_NBR,
		eFPT_FTL,
		eFPT_FTR,
		eFPT_FBL,
		eFPT_FBR,
		eFPT_MAX,
	};
public:
    inline BB_CONSTEXPR Frustum() {}
	Frustum( CameraView const& view, bool is_local_to_view )
	{
		BuildFromView( view, is_local_to_view );
	}

	void BuildFromView( CameraView const& view, bool is_local_to_view )
	{
		T fov_y = (T)view.m_fParameters[eCP_FOV];
		fov_y *= ((T)D_PI / (T)180.0);

		T cf_y = cos(fov_y * (T)0.5);
		T sf_y = sin(fov_y * (T)0.5);
		T tf_y = sf_y / cf_y;
		T half_fov_x = atan2( tf_y * (T)view.m_fParameters[eCP_ASPECT], (T)1.0 );
		T cf_x = cos(half_fov_x);
		T sf_x = sin(half_fov_x);

		auto ComputeFunc = [&] ( Vec3<T> const& eye, Vec3<T> const& right, Vec3<T> const& up, Vec3<T> const& front )
		{
			// Compute planes
			m_planes[eFP_Near].xyz = front;
			m_planes[eFP_Far].xyz = -front;
			m_planes[eFP_right].xyz = right * -cf_x + front * sf_x;
			m_planes[eFP_Left].xyz = right * cf_x + front * sf_x;
			m_planes[eFP_Top].xyz = up * -cf_x + front * sf_x;
			m_planes[eFP_Bottom].xyz = up * cf_x + front * sf_x;

			Vec3<T> PNear = eye + front * (T)view.m_fParameters[eCP_NEAR];
			Vec3<T> PFar = eye + front * (T)view.m_fParameters[eCP_FAR];
			m_planes[eFP_Near].w = -dot( PNear, m_planes[eFP_Near].xyz );
			m_planes[eFP_Far].w = -dot( PFar, m_planes[eFP_Far].xyz );
			for( int32 i = eFP_right; i < eFP_MAX; ++i )
				m_planes[i].w = -dot( eye, m_planes[i].xyz );

			// Compute points
			T y_near = tf_y * (T)view.m_fParameters[eCP_NEAR];
			T x_near = y_near * (T)view.m_fParameters[eCP_ASPECT];
			T y_far = tf_y * (T)view.m_fParameters[eCP_FAR];
			T x_far = y_far * (T)view.m_fParameters[eCP_ASPECT];
			m_points[eFPT_NTL] = PNear + up * y_near - right * x_near;
			m_points[eFPT_NTR] = PNear + up * y_near + right * x_near;
			m_points[eFPT_NBL] = PNear - up * y_near - right * x_near;
			m_points[eFPT_NBR] = PNear - up * y_near + right * x_near;
			m_points[eFPT_FTL] = PFar + up * y_far - right * x_far;
			m_points[eFPT_FTR] = PFar + up * y_far + right * x_far;
			m_points[eFPT_FBL] = PFar - up * y_far - right * x_far;
			m_points[eFPT_FBR] = PFar - up * y_far + right * x_far;
		};

		if( is_local_to_view )
		{
			Vec3<T> eye(0,0,0);
			Vec3<T> right(1,0,0);
			Vec3<T> up(0,1,0);
			Vec3<T> front(0,0,-1);
			ComputeFunc( eye, right, up, front );
		}
		else
		{
			Mat4<T> CamToWorldMat( view.m_Transform.GetRotation() );
			Vec3<T> eye = view.m_Transform.GetTranslation();
			Vec3<T> right = CamToWorldMat.v0.xyz;
			Vec3<T> up = CamToWorldMat.v1.xyz;
			Vec3<T> front = -CamToWorldMat.v2.xyz;
			ComputeFunc( eye, right, up, front );
		}
	}
  
    template<typename U>
    friend std::ostream &operator<<(std::ostream &stream, Transform<U> const &v);

	eIntersectionStatus AABBIntersection( Vec3<T> const& box_min, Vec3<T> const& box_max ) const
	{
		// check box outside/inside of frustum
		int SumOut = 0;
		for( int i = 0; i < eFP_MAX; i++ )
		{
			int Out = 0;
			Out += ((dot( m_planes[i], Vec4<T>(box_min.x, box_min.y, box_min.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_planes[i], Vec4<T>(box_max.x, box_min.y, box_min.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_planes[i], Vec4<T>(box_min.x, box_max.y, box_min.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_planes[i], Vec4<T>(box_max.x, box_max.y, box_min.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_planes[i], Vec4<T>(box_min.x, box_min.y, box_max.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_planes[i], Vec4<T>(box_max.x, box_min.y, box_max.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_planes[i], Vec4<T>(box_min.x, box_max.y, box_max.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_planes[i], Vec4<T>(box_max.x, box_max.y, box_max.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			if( Out==8 ) 
				return Outside;
			SumOut += Out;
		}

		// check frustum outside/inside box
		int Out;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_points[i].x > box_max.x) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_points[i].x < box_min.x) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_points[i].y > box_max.y) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_points[i].y < box_min.y) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_points[i].z > box_max.z) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_points[i].z < box_min.z) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;

		return SumOut > 0 ? Intersect : FullyInside;
	}

	eIntersectionStatus SphereIntersection( Vec3<T> const& SphereCenter, T SphereRadius )
	{
		Vec4<T> VSphere( SphereCenter, (T)1.0 );
		int In = 0;
		float d;
		if( (d = dot( m_planes[eFP_Near], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_planes[eFP_Far], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_planes[eFP_right], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_planes[eFP_Left], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_planes[eFP_Top], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_planes[eFP_Bottom], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;

		return In < 6 ? Intersect : FullyInside;
	}

    Vec4<T> m_planes[eFP_MAX];
	Vec3<T> m_points[eFPT_MAX];
};


#undef BB_CONSTEXPR


} /* namespace bigfx */

#endif // BB_MATHFRUSTUM_H

