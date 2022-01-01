

#include "../bigfx.h"
#include "entity.h"
#include "entitymanager.h"
#include "component.h"
#include "componentmanager.h"

namespace bigfx
{

CLASS_EQUIP_CPP(Entity);

Entity::Entity() : m_state(Empty), m_pattern(nullptr)
{
	
}

Entity::~Entity()
{
	
}

void Entity::Create( EntityPattern* pattern, class json::Object* proto, Name in_name )
{
	BB_ASSERT( m_state == Empty );

	m_pattern = pattern;
	m_name = in_name;

	// Create associated components
	EntityManager* entity_manager = EntityManager::GetStaticInstance();
	for( int i = 0; i < pattern->m_Components.size(); ++i )
	{
		ComponentFactory* factory = entity_manager->FindComponentFactory( pattern->m_Components[i] );
		if( !factory )
		{
			BB_LOG( Entity, Error, "Could not find component <%s>", pattern->m_Components[i].c_str() );
		}
		else
		{
			Component* new_component = factory->m_CreateFunc();
			new_component->Create( this, proto );

			m_components.push_back( new_component );
		}
	}

	m_state = Created;
}
void Entity::Destroy()
{
	BB_ASSERT( m_state == Created || m_state == InWorld );

	for( int i = 0; i < m_components.size(); ++i )
	{
		Component* component = m_components[i];
		component->Destroy();
	}
	m_components.clear();

	m_state = Empty;
}

void Entity::AddToWorld()
{
	BB_ASSERT( m_state == Created );

	EntityManager* entity_manager = EntityManager::GetStaticInstance();
	for( int i = 0; i < m_components.size(); ++i )
	{
		ComponentFactory* factory = entity_manager->FindComponentFactory( m_components[i]->GetClassName() );
		if( factory && factory->m_Manager )
		{
            m_components[i]->AddToWorld();
			factory->m_Manager->AddComponentToWorld( m_components[i] );
		}
	}

	m_state = InWorld;
}
void Entity::RemoveFromWorld()
{
	BB_ASSERT( m_state == InWorld );

	EntityManager* entity_manager = EntityManager::GetStaticInstance();
	for( int i = 0; i < m_components.size(); ++i )
	{
		ComponentFactory* factory = entity_manager->FindComponentFactory( m_components[i]->GetClassName() );
		if( factory && factory->m_Manager )
		{
			factory->m_Manager->RemoveComponentFromWorld( m_components[i] );
            m_components[i]->RemoveFromWorld();
		}
	}

	m_state = Created;
}

void Entity::Tick( float delta_seconds )
{

}

Component* Entity::GetComponent( Name const& component_name )
{
	for( int i = 0; i < m_pattern->m_Components.size(); ++i )
	{
		if( m_pattern->m_Components[i] == component_name )
		{
			BB_ASSERT( m_components[i]->IsA( component_name) );
			return m_components[i];
		}
	}
	return nullptr;
}

Component* Entity::GetCompatibleComponent( Name const& component_name )
{
	for( int i = 0; i < m_components.size(); ++i )
	{
		if( m_components[i]->IsA( component_name ) )
		{
			return m_components[i];
		}
	}
	return nullptr;
}
    
void Entity::Serialize(Archive& file)
{
    for( int i = 0; i < m_components.size(); ++i )
    {
        m_components[i]->Serialize(file);
    }
}

void Entity::PostLoad()
{
	for (int i = 0; i < m_components.size(); ++i)
	{
		m_components[i]->PostLoad();
	}
}

} /* namespace bigfx */