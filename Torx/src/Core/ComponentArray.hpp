#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <unordered_map>

// we use the virtual keyword so that we can have base class pointers that point to a derived class instance and then we can call the 
// methods specific to each derived class using the base class pointers, and depending on which derived class the base class pointer points to, 
// the function will have a different behavior.

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};

// With this template<typename T> we can do ComponentArray<someComponent> and the instance of the class and its functions will all work 
// with someComponent.

template<typename T> class ComponentArray : public IComponentArray
{
public:
	void InsertData(Entity entity, T component)
	{

		// check if the key (entity) already exists in the map. Remember that the entity ID will only be a key in the EntityToIndexMap if
		// it already points to an index of a component array of this type, that is, it already has a component of this type.
		assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once");

		// put a new entry at end and update the maps
		size_t newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
		++mSize;
	}

	void RemoveData(Entity entity)
	{
		// If the first statement is true it means that the component exists
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

		// copy element at end into deleted element's place to maintain density (we wanted a tightly packed array)

		size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
		size_t indexOfLastElement = mSize - 1;
		mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
		mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(indexOfLastElement);

		--mSize;
	}


	// returns reference of a given entity's component of type T. We get the index at which the given entity's component resides by just doing
	// mEntityToIndexMap(entity). Then we retrieve the component by simply doing mComponentArray(mEntityToIndexMap(entity));
	T& GetData(Entity entity) 
	{
		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existing component");
		
		// Return a reference to the entity's component.
		return mComponentArray[mEntityToIndexMap[entity]];
	}

	void EntityDestroyed(Entity entity) override
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			// Remove the entity's component if it existed
			RemoveData(entity);
		}
	}

private:
	// The packed array of components (of generic type T), set to a specified maximum amount, mathcing the maximum number
	// of entities allowed to exist simultaneosly, so that each entity has a unique spot
	
	// array of components of type T. We will have multiple arrays, each for a different component type.
	std::array<T, MAX_ENTITIES> mComponentArray{};

	// Map from an entity ID to an array index.
	std::unordered_map<Entity, size_t> mEntityToIndexMap{};

	// Map from an array index to an entity ID
	std::unordered_map<size_t, Entity> mIndexToEntityMap{};
	
	// Total size of valid entries in the array (how many instances of the component we have in the component array)
	size_t mSize{};
};