
#ifndef BB_COPOSITION_H
#define BB_COPOSITION_H

#include "component.h"

namespace bigfx
{

class BIGFX_API CoPosition : public Component 
{
	CLASS_EQUIP_H(CoPosition, Component);

public:
						CoPosition();
	virtual				~CoPosition();

	static Component*	NewComponent()              { return new CoPosition();	}
    virtual void        Serialize(Archive& file) override;

	void				SetPosition( vec3 p );
	vec3				GetPosition() const         { return m_transform.GetTranslation(); }
	void				SetRotation( quat r );
	quat				GetRotation() const         { return m_transform.GetRotation(); }
    void                SetScale( float s );
    float               GetScale() const            { return m_transform.GetScale(); }
    void                SetTransform( transform const& t );
	const transform&	GetTransform() const		{ return m_transform;	}

protected:
	transform	m_transform;
};

} /* namespace bigfx */

#endif // BB_COPOSITION_H
