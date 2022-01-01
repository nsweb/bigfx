
#ifndef BB_ENTITY_H
#define BB_ENTITY_H

namespace bigfx
{

class BIGFX_API Component;
class BIGFX_API EntityPattern;
class Archive;

namespace json
{
	class Object;
};

class BIGFX_API Entity 
{
	CLASS_EQUIP_BASE_H(Entity)

public:

	enum State
	{
		Empty,
		Created,
		InWorld,
	};

	Array<Component*>	m_components;

					Entity();
	virtual			~Entity();

	static Entity*	NewEntity()		{ return new Entity();	}

	virtual void	Create( EntityPattern* pattern, class json::Object* proto = nullptr, Name in_name = Name() );
	virtual void	Destroy();	
	virtual void	AddToWorld();
	virtual void	RemoveFromWorld();
	virtual void	Tick( float DeltaSeconds );
    virtual void    Serialize(Archive& file);
	virtual void    PostLoad();

	bool			IsInWorld()		{ return m_state == InWorld; }
	Component*		GetComponent( Name const& component_name );
	Component*		GetCompatibleComponent( Name const& component_name );
	Name const&		GetName()		{ return m_name;			}

protected:
	EntityPattern*		m_pattern;
	Name				m_name;
	State				m_state;

};

} /* namespace bigfx */

#endif // BB_ENTITY_H