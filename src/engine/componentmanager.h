


#ifndef BB_COMPONENTMANAGER_H
#define BB_COMPONENTMANAGER_H

#include "basemanager.h"

namespace bigfx
{

class BIGFX_API Component;

class BIGFX_API ComponentManager : public BaseManager 
{

public:
					ComponentManager();
	virtual			~ComponentManager();

	virtual void	AddComponentToWorld( Component* component );
	virtual void	RemoveComponentFromWorld( Component* component );


protected:

};

} /* namespace bigfx */

#endif // BB_COMPONENTMANAGER_H