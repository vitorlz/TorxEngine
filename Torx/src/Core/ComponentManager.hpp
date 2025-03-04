#pragma once

#include "ComponentArray.hpp"
#include "Types.hpp"
#include <any>
#include <memory>
#include <unordered_map>
#include "../Components/CPlayer.h"

// The component manager is in charge of communicating with the different component arrays when a component needs to be added or removed

class ComponentManager
{
public:

	// We will need to call this each time we create a new component and want to use it in the game. This will increment the 
	// 
	template<typename T> void RegisterComponent()
	{
		// create unique key for the given type T
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once");

		// Add this component type to the component type map. The mNextComponentType will be which bit in the signature this type of component
		// will correspond to

		mComponentTypes.insert({ typeName, mNextComponentType });

		// Create a ComponentArray pointer and add it to the component arrays map
		mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		// Increment the value so that the next component registered will correspond to the next bit in the signature.
		++mNextComponentType;
	}

	template<typename T> ComponentType GetComponentType() 
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		// Return this component's type (which bit it corresponds to in the signature) - used for creating signatures.
		return mComponentTypes[typeName];
	}

	template<typename T> void AddComponent(Entity entity, T component)
	{
		// Add a component to an entity by inserting the component in a component array of its type and also mapping the entity to that component's
		// index in the array and mapping the index to the entity (in the component array class).
		GetComponentArray<T>()->InsertData(entity, component);

		if (std::is_same<T, CPlayer>::value)
		{
			mPlayerList.push_back(entity);
		}
	}

	template<typename T> void RemoveComponent(Entity entity) 
	{
		// Remove an entity's component
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T> T& GetComponent(Entity entity)
	{
		// Get a reference to an entity's component of type T
		return GetComponentArray<T>()->GetData(entity);
	}

	void EntityDestroyed(Entity entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it

		for (auto const& pair : mComponentArrays)
		{
			auto const& componentArray = pair.second;

			componentArray->EntityDestroyed(entity);			
		}
	}

	std::vector<Entity> GetPlayerList()
	{
		return mPlayerList;
	}

private:
	// map unique key (a type string pointer that is different for each type) to a component type. For example CTransform will have a key,
	// CGravity will have another key and so on. This ComponentType is simply a u_int8_t that we defined in Types.hpp. The value stored with
	// the key will be the unique ID of the component that will correspond to the component's bit in the bitset (the signature). 

	std::unordered_map<std::string, ComponentType> mComponentTypes{};

	// We store the component arrays as pointers to the base class. 
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	// The component type (unique ID / bit) to be assigned to the next component. Starts at 0.
	ComponentType mNextComponentType{0};

	std::vector<Entity> mPlayerList{};

	// Get the statically casted pointer to the ComponentArray of type T.
	template<typename T> std::shared_ptr<ComponentArray<T>> GetComponentArray() 
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use");

		// We return the a pointer to the derived class by downcasting the base class pointer.
		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}
};