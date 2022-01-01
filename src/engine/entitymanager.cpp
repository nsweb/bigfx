

#include "../bigfx.h"
#include "entity.h"
#include "component.h"
#include "entitymanager.h"
#include "tickcontext.h"
#include "../core/json.h"

//DECLARE_ENTITY( PlanetId, PlanetClass, (COPLANET | CORENDER) )


namespace bigfx
{

STATIC_MANAGER_CPP( EntityManager )


EntityManager::EntityManager()
{
	m_pStaticInstance = this;
}

EntityManager::~EntityManager()
{
	m_pStaticInstance = nullptr;
}

void EntityManager::Create()
{

}
void EntityManager::Destroy()
{
	// Destroy remaining entities (no remove from world)
	for( int i = 0; i < m_entities.size(); ++i )
	{
		Entity* entity = m_entities[i];
		entity->Destroy();
		delete entity;
	}
	m_entities.clear();
}

void EntityManager::Tick( TickContext& tick_ctxt )
{
	for( int i = 0; i < m_entities.size(); ++i )
	{
		m_entities[i]->Tick( tick_ctxt.m_delta_seconds );
	}
}

Entity* EntityManager::CreateEntity( char const* pattern_name, json::Object* proto, Name name )
{
	int pattern_idx = m_entity_patterns.FindByKey( Name(pattern_name) );
	if( pattern_idx == INDEX_NONE )
	{
		BB_LOG( Entity, Error, "Unknown entity pattern name <%s>", pattern_name );
		return nullptr;
	}

	EntityPattern& pattern = m_entity_patterns[pattern_idx];
	Entity* new_entity = pattern.m_CreateFunc();
	new_entity->Create( &pattern, proto, name );

	m_entities.push_back( new_entity );
	
	return new_entity;
}

Entity* EntityManager::CreateEntityFromJson( char const* json_path, Name InName )
{
	json::Object jsnObj;
	if( jsnObj.ParseFile( json_path ) )
	{
		json::TokenIdx EntTok = jsnObj.GetToken( "entity", json::OBJECT );
		json::TokenIdx PatTok = jsnObj.GetToken( "pattern", json::STRING, EntTok );
		String strPattern;
		jsnObj.GetStringValue( PatTok, strPattern );
		return CreateEntity( strPattern.c_str(), &jsnObj, InName );
	}

	//tinyxml2::XMLDocument XMLProto;
	//tinyxml2::XMLError err = XMLProto.LoadFile( XMLPath );
	//tinyxml2::XMLElement* RootElt = XMLProto.FirstChildElement();
	//String RootName( RootElt->Name() );
	//if( RootName == "entity" )
	//{
	//	const char* PatternName = RootElt->Attribute( "pattern" );
	//	return CreateEntity( PatternName, &XMLProto );
	//}

	BB_LOG( Entity, Error, "JSON file <%s> is not a valid entity prototype", json_path );

	return nullptr;
}

void EntityManager::AddEntityToWorld( Entity* entity )
{
	if( !entity->IsInWorld() )
	{
		entity->AddToWorld();
	}
}

void EntityManager::RemoveEntityFromWorld( Entity* entity )
{
	if( entity->IsInWorld() )
	{
		entity->RemoveFromWorld();
	}
}

void EntityManager::DestroyEntity( Entity* entity )
{
	entity->Destroy();
	m_entities.remove( entity );

	delete entity;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void EntityManager::RegisterPattern( EntityPattern const& Pattern )
{
	BB_ASSERT( -1 == m_entity_patterns.FindByKey( Pattern.m_Name ) );

	m_entity_patterns.push_back( Pattern );
}

void EntityManager::RegisterFactory( ComponentFactory& Factory )
{
	BB_ASSERT( -1 == m_component_factories.FindByKey( Factory.m_Name ) );

	m_component_factories.push_back( Factory );
}

ComponentFactory* EntityManager::FindComponentFactory( Name const& ComponentName )
{
	int FactoryIdx = m_component_factories.FindByKey( ComponentName );
	if( FactoryIdx != INDEX_NONE )
		return &m_component_factories[FactoryIdx];

	return nullptr;
}

Entity* EntityManager::FindEntityByName( Name InName )
{
	for( int i = 0; i < m_entities.size(); ++i )
	{
		Entity* entity = m_entities[i];
		if( entity->GetName() == InName )
			return entity;
	}
	return nullptr;
}


} /*namespace bigfx*/