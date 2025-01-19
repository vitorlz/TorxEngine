#include "../Components/CNativeScript.h"
#include "../Core/Coordinator.hpp"
#include "NativeScriptSystem.h"
#include "../include/Engine.h"

extern Coordinator ecs;

void NativeScriptSystem::Init() {}

void NativeScriptSystem::Update(float dt)
{
	if (Torx::Engine::MODE != Torx::PLAY)
	{
		return;
	}

	for (const auto& entity : mEntities)
	{
		auto& scriptComponent = ecs.GetComponent<CNativeScript>(entity);
		// in the UI: choose a script name from a drop down that contains all the registered scripts, assign that script name to the 
		// component, and use that name here in order to actually create instantiate the script. To use a script we have to simply create a 
		// script and register it. We still don't have much of a runtime because we dont have a "game start" thing going on, so onCreate()
		// is kind of useless right now;

		if (!scriptComponent.script)
		{
			scriptComponent.script = ScriptFactory::Create(scriptComponent.name);

			if (scriptComponent.script)
			{
				scriptComponent.script->m_entity = entity;
				scriptComponent.script->onCreate();
			}
		}
		else
		{
			scriptComponent.script->onUpdate(dt);
		}
	}
};	

