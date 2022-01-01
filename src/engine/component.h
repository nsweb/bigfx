


#ifndef BB_COMPONENT_H
#define BB_COMPONENT_H

namespace bigfx
{
namespace json
{
	class Object;
};

class BIGFX_API Entity;
    class Archive;

class BIGFX_API Component 
{
	CLASS_EQUIP_BASE_H(Component)

public:
						Component();
	virtual				~Component();

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
    virtual void        Serialize(Archive& file);
	virtual void		PostLoad();
	inline Entity*		GetEntity()				{ return m_owner;		}
    Component*          GetEntityComponent( Name const& component_name );
    

private:
	Entity*		m_owner;
};

} /* namespace bigfx */

#endif // BB_COMPONENT_H