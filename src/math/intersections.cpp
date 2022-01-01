
#include "../bigfx.h"
#include "intersections.h"

namespace bigfx
{

#if 0
//////////////////////////////////////////////////////////////////////////
// Explicit instantiation
//template struct Intersect<double>;
//template struct Intersect<float>;

/* Fast Ray-Box Intersection by Andrew Woo */
template <typename T>
bool Intersect<T>::RayAABB( Vec3<T> const& min_box, Vec3<T> const& max_box, Vec3<T> const& origin, Vec3<T> const& dir, Vec3<T>& coord )
{
	const int32 num_dim = 3;
	const int32 RIGHT = 0;
	const int32 LEFT = 1;
	const int32 MIDDLE = 2;

	bool is_inside = true;
	int32 quadrant[num_dim];
	int32 i, which_plane;
	T maxT[num_dim];
	T candidate_plane[num_dim];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for(i=0; i<num_dim; i++)
	{
		if(origin[i] < min_box[i])
		{
			quadrant[i] = LEFT;
			candidate_plane[i] = min_box[i];
			is_inside = false;
		}
		else if (origin[i] > max_box[i]) 
		{
			quadrant[i] = RIGHT;
			candidate_plane[i] = max_box[i];
			is_inside = false;
		}
		else	
		{
			quadrant[i] = MIDDLE;
		}
	}

	/* Ray origin inside bounding box */
	if(is_inside)	
	{
		coord = origin;
		return true;
	}

	/* Calculate T distances to candidate planes */
	for (i = 0; i < num_dim; i++)
	{
		if (quadrant[i] != MIDDLE && dir[i] !=0.)
			maxT[i] = (candidate_plane[i] - origin[i]) / dir[i];
		else
			maxT[i] = -1.;
	}

	/* Get largest of the maxT's for final choice of intersection */
	which_plane = 0;
	for (i = 1; i < num_dim; i++)
	{
		if (maxT[which_plane] < maxT[i])
			which_plane = i;
	}

	/* Check final candidate actually inside box */
	if (maxT[which_plane] < 0.) 
		return false;

	for (i = 0; i < num_dim; i++)
	{
		if (which_plane != i) 
		{
			coord[i] = origin[i] + maxT[which_plane] * dir[i];
			if (coord[i] < min_box[i] || coord[i] > max_box[i])
				return false;
		} 
		else 
		{
			coord[i] = candidate_plane[i];
		}
	}

	return true;		/* ray hits box */
}

template <typename T>
T Intersect<T>::SquaredDistancePointSegment(Vec3<T> const& point, Vec3<T> const& seg0, Vec3<T> const& seg1, T& t_on_segment)
{
	Vec3<T> V_seg0 = point - seg0;
	Vec3<T> dir_seg = seg1 - seg0;
	T t = bigfx::max(dot(dir_seg, V_seg0), (T)0.0);

	T dot_dd = dot(dir_seg, dir_seg);
	t = min(t, dot_dd) / dot_dd;
	V_seg0 += dir_seg*(-t);

	t_on_segment = t;
	return dot(V_seg0, V_seg0);
}

template <typename T>
T Intersect<T>::RaySphereIntersection(Vec3<T> const& ray_orig, Vec3<T> const& ray_dir, Vec3<T> const& center, T radius)
{
	// get the offset vector
	Vec3<T> offset = center - ray_orig;

	// get the distance along the ray to the center point of the sphere
	T ray_dist = dot(ray_dir, offset);
	if (ray_dist <= T(0.))
	{
		// moving away from object
		return T(-1.);
	}

	// get the squared distances
	T off_2 = dot(offset, offset);
	T rad_2 = radius * radius;
	if (off_2 <= rad_2)
	{
		// we're in the sphere
		return T(0.);
	}

	// find hit distance squared
	T d_2 = rad_2 - (off_2 - ray_dist * ray_dist);
	if (d_2 < T(0.))
	{
		// ray passes by sphere without hitting
		return T(-1.);
	}

	// get the distance along the ray
	T dist = ray_dist - sqrt(d_2);

	return dist;
}

template <typename T>
T Intersect<T>::RayBoxIntersection(Vec3<T> const& ray_orig, Vec3<T> const& ray_dir, Vec3<T> const& box_ratio)
{
	T tx1 = (-box_ratio.x - ray_orig.x) / ray_dir.x;
	T tx2 = (box_ratio.x - ray_orig.x) / ray_dir.x;

	T tmin = min(tx1, tx2);
	T tmax = max(tx1, tx2);

	T tz1 = (-box_ratio.z - ray_orig.z) / ray_dir.z;
	T tz2 = (box_ratio.z - ray_orig.z) / ray_dir.z;

	tmin = max(tmin, min(tz1, tz2));
	tmax = min(tmax, max(tz1, tz2));

	T ty1 = (-box_ratio.y - ray_orig.y) / ray_dir.y;
	T ty2 = (box_ratio.y - ray_orig.y) / ray_dir.y;

	tmin = max(tmin, min(ty1, ty2));
	tmax = min(tmax, max(ty1, ty2));

	if (tmax < tmin)
		return T(-1.);

	return tmin;
}
#endif // 0

} /* namespace bigfx */

