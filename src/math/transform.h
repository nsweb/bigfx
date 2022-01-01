

#ifndef BB_MATHTRANSFORM_H
#define BB_MATHTRANSFORM_H


namespace bigfx
{

template <typename T> struct Transform;
typedef Transform<float> transform;
typedef Transform<double> dtransform;

/*constexpr*/
#define BB_CONSTEXPR 


/*
 * Transform - rotation, translation, and no scale
 */

template <typename T> struct /*BIGFX_API*/ Transform
{
    inline BB_CONSTEXPR Transform() {}
    inline BB_CONSTEXPR Transform(Quat<T> const& Rot) : m_rotation(Rot), m_translation(0), m_scale(1) {}
    inline BB_CONSTEXPR Transform(Quat<T> const& Rot, Vec3<T> const& Trans, T Scale) : m_rotation(Rot), m_translation(Trans), m_scale(Scale) {}
	inline BB_CONSTEXPR Transform(Vec3<T> const& Trans) : m_rotation(1), m_translation(Trans), m_scale(1) {}

	inline Quat<T>& GetRotation()											{ return m_rotation;	}
	inline Quat<T> const & GetRotation() const								{ return m_rotation;	}
	inline Vec3<T>& GetTranslation()										{ return m_translation;	}
	inline Vec3<T> const & GetTranslation()	const							{ return m_translation;	}
	inline T& GetScale()													{ return m_scale;		}
	inline T  GetScale() const												{ return m_scale;		}
	inline void	SetRotation( Quat<T> const& Rot )							{ m_rotation = Rot;		}
	inline void	SetTranslation( Vec3<T> const& Trans )						{ m_translation = Trans; }
	inline void	SetScale( T Scale )											{ m_scale = Scale;		}
	inline void Set( Quat<T> const& Rot, Vec3<T> const& Trans )				{ m_rotation = Rot; m_translation = Trans; }
	inline void Set( Quat<T> const& Rot, Vec3<T> const& Trans, T Scale )	{ m_rotation = Rot; m_translation = Trans; m_scale = Scale; }
	inline Transform<T> operator * ( Transform<T> const& B ) const
	{
		Transform<T> Ret;
		Multiply( &Ret, this, &B );
		return Ret;
	}
	inline static void Multiply( Transform<T>* OutT, Transform<T> const * A, Transform<T> const * B )
	{
		Transform<T> Ret;
		Ret.m_rotation = B->m_rotation * A->m_rotation;
		Ret.m_translation = A->m_rotation * (B->m_translation * A->m_scale) + A->m_translation;
		Ret.m_scale = B->m_scale * A->m_scale;

		*OutT = Ret;
	}
	/**  Get this -> B transform */
	Transform<T> GetTransformTo( Transform<T> const& B ) const
	{
		Transform<T> Ret;

		BB_ASSERT( isNormalized( B.GetRotation() ) );

		Quat<T> BInvRot = ~B.m_rotation;
		Ret.m_rotation = BInvRot * m_rotation;
		Ret.m_translation = (BInvRot * (m_translation - B.m_translation)) / B.m_scale;
		Ret.m_scale = m_scale / B.m_scale;

		return Ret;
	}

	inline Vec3<T> TransformPositionInverse( Vec3<T> const& V ) const
	{
		return ( ~m_rotation * (V - m_translation) ) / m_scale;
	}
	inline Vec3<T> TransformVectorInverse( Vec3<T> const& V ) const
	{
		return ( ~m_rotation * V ) / m_scale;
	}
	inline Vec3<T> TransformPosition( Vec3<T> const& V ) const
	{
		Vec3<T> Ret = m_rotation*(V * m_scale);
		Ret += m_translation;
		return Ret;
	}
	inline Vec3<T> TransformVector( Vec3<T> const& V ) const
	{
		Vec3<T> Ret = m_rotation*(V * m_scale);
		return Ret;
	}
    inline Mat4<T> ToMat4() const
    {
        return Mat4<T>( m_rotation, m_translation, m_scale );
    }

	bool operator == (Transform<T> const & oth) const
	{
		return m_rotation == oth.m_rotation && m_translation == oth.m_translation && m_scale == oth.m_scale;
	}

    template<typename U>
    friend std::ostream &operator<<(std::ostream &stream, Transform<U> const &v);

    Quat<T> m_rotation;
	Vec3<T> m_translation;
	T		m_scale;
};


#undef BB_CONSTEXPR


} /* namespace bigfx */

#endif // BB_MATHTRANSFORM_H

