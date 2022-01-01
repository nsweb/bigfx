

#include "../bigfx.h"
#include "component.h"
#include "entity.h"

namespace bigfx
{
CLASS_EQUIP_CPP(Component);

Component::Component() : m_owner(nullptr)
{

}

Component::~Component()
{

}

void Component::Create( Entity* owner, class json::Object* proto )
{
	m_owner = owner;
}
void Component::Destroy()
{

}
void Component::AddToWorld()
{

}
void Component::RemoveFromWorld()
{

}
void Component::Serialize(Archive& file)
{
        
}
void Component::PostLoad()
{

}
Component* Component::GetEntityComponent( Name const& component_name )
{
    return m_owner->GetComponent( component_name );
}

} /* namespace bigfx */