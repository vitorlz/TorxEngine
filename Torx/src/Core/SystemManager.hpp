#pragma once

#include "System.hpp"
#include "Types.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>

class SystemManager
{
public:
	template<typename T> std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

		// Create a pointer to the system and return it so it can be used externally
		// std::make_shared<T>() creates an instance of type T using the default constructor and returns a pointer to it.
		auto system = std::make_shared<T>();
		mSystems.insert({ typeName, system });
		return system;
	}

	// remember that signature is just a type we defined in types.hpp (std::bitset<MAX_COMPONENTS>)
	template<typename T> void SetSignature(Signature signature) {
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "Trying to use system before it is registered.");

		mSignatures.insert({ typeName, signature });
	}

	void EntityDestroyed(Entity entity)
	{
		// changed some stuff here if it doesn't work the problem is prob here
		for (const auto& pair : mSystems)
		{
			// we are getting the system here as a constant reference to a pointer in order to prevent some small inefficiencies
			const auto& system = pair.second;

			// removes the entity from the system's entities set if it exists
			system->mEntities.erase(entity);
		}
	}

	// If we add or remove a component form an entity, its signature will change, and we need to update the entities set 
	// of each of the systems that used that entity
	void EntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (const auto& pair : mSystems)
		{
			const auto& type = pair.first;
			const auto& system = pair.second;
			const auto& systemSignature = mSignatures[type];

			// Entity signature matches system signature - insert into set (remember how comparison of binary numbers work).
			if ((entitySignature & systemSignature) == systemSignature)
			{
				system->mEntities.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else
			{
				system->mEntities.erase(entity);

			}
		}
	}

private:
	// Map from system type (unique ID) to a signature
	std::unordered_map<const char*, Signature> mSignatures{};

	// Map from system type (unique ID) to a system pointer
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};