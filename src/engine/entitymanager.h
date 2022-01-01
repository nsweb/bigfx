


#ifndef BB_ENTITYMANAGER_H
#define BB_ENTITYMANAGER_H

#include "basemanager.h"
#include "entitypattern.h"

namespace bigfx
{

class BIGFX_API Entity;
class BIGFX_API Component;

#define DECLARE_COMPONENT( Klass ) \
{ \
	ComponentFactory Factory; \
	Factory.m_Name = Klass::StaticClass(); \
	Factory.m_CreateFunc = Klass::NewComponent; \
	Factory.m_Manager = nullptr; \
	EntityManager::GetStaticInstance()->RegisterFactory( Factory ); \
}

#define DECLARE_COMPONENT_MGR( Klass, Manager ) \
{ \
	ComponentFactory Factory; \
	Factory.m_Name = Klass::StaticClass(); \
	Factory.m_CreateFunc = Klass::NewComponent; \
	Factory.m_Manager = Manager::GetStaticInstance(); \
	EntityManager::GetStaticInstance()->RegisterFactory( Factory ); \
}

#define DECLARE_ENTITYPATTERN( Name, Klass, CompList, OptionalCompList ) \
{ \
	EntityPattern Pattern; \
	Pattern.m_Name = #Name; \
	Pattern.m_CreateFunc = Klass::NewEntity; \
	Pattern.SetComponentList CompList; \
	Pattern.SetOptionalComponentList OptionalCompList; \
	EntityManager::GetStaticInstance()->RegisterPattern( Pattern ); \
}

//BIGBALL_TEMPLATE template class BIGFX_API Array<Entity*>;
//BIGBALL_TEMPLATE template class BIGFX_API Array<ComponentFactory>;
//BIGBALL_TEMPLATE template class BIGFX_API Array<EntityPattern>;

namespace json
{
	class Object;
};

class BIGFX_API EntityManager : public BaseManager 
{
	STATIC_MANAGER_H(EntityManager)

public:
						EntityManager();
	virtual				~EntityManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& tick_ctxt );

	Entity*				CreateEntity( char const* pattern_name, class json::Object* proto = nullptr, Name name = Name() );
	Entity*				CreateEntityFromJson( char const* json_path, Name name = Name()  );

	void				AddEntityToWorld( Entity* entity );
	void				RemoveEntityFromWorld( Entity* entity );
	void				DestroyEntity( Entity* entity );
	Entity*				FindEntityByName( Name name );

	void				RegisterFactory( ComponentFactory& factory );
	void				RegisterPattern( EntityPattern const& pattern );
	ComponentFactory*	FindComponentFactory( Name const& component_name );

protected:
	Array<Entity*>		m_entities;

	Array<ComponentFactory>		m_component_factories;
	Array<EntityPattern>		m_entity_patterns;
};

} /* namespace bigfx */

#endif // BB_ENTITYMANAGER_H