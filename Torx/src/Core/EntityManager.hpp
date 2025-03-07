#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <queue>

class EntityManager
{
public:
	EntityManager()
	{
		// Initialize the queue with all possible entity IDs
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			mAvailableEntities.push(entity);
		}
	}

	Entity CreateEntity()
	{
		// This will stop the program if we exceed the maximum number of entities (only workds in debug)
		assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existance");



		// Take an ID from the front of the queue
		Entity id = mAvailableEntities.front();
		mAvailableEntities.pop();
		mLivingEntityIDs.push_back(id);
		++mLivingEntityCount;
		


		return id;
	}

	void DestroyEntity(Entity entity)
	{
		// If we try to destroy an entity with an index greater than the max (out of bounds) the program stops (only works in debug)
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Invalidate the destroyed entity's signature. It clears the bits of the entity's signature to 0 (meaning that it does not hold
		// any components because it was destroyed, so systems will stop using this entity).
		mSignatures[entity].reset();

		// put the destroyed ID at the back of the queue
		mAvailableEntities.push(entity);

		for (int i = 0; i < mLivingEntityIDs.size(); i++)
		{
			if (mLivingEntityIDs[i] == entity)
			{
				std::cout << "Entity destroyed: " << entity << "\n";

				mLivingEntityIDs.erase(mLivingEntityIDs.begin() + i);
				break;
			}
		}

		--mLivingEntityCount;
	}

	void SetSignature(Entity entity, Signature signature) 
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		std::cout << "Entity: " << entity << " signature set\n";

		// Put this entity's signature into the array (indexed using the entity's ID)
		mSignatures[entity] = signature;
	}

	Signature GetSignature(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range");

		// Get this entity's signature from the array
 		return mSignatures[entity];
	}

	std::vector<Entity> GetLivingEntities()
	{
		return mLivingEntityIDs;
	}

	bool isAlive(Entity entity)
	{
		for (Entity e : mLivingEntityIDs)
		{
			if (e == entity)
			{
				return true;
			}
		}
		return false;
	}

	void ResetEntityIDs()
	{

		std::queue<Entity> empty;
		std::swap(mAvailableEntities, empty);

		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			mAvailableEntities.push(entity);
		}
	}

private:
	// Queue of unused entity IDs
	std::queue<Entity> mAvailableEntities{};

	std::vector<Entity> mLivingEntityIDs;

	// Array of signatures where the index corresponds to the entity ID. 
	std::array<Signature, MAX_ENTITIES> mSignatures{};

	// how many entities currently exist
	uint32_t  mLivingEntityCount{};
};