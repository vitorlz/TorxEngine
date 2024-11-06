#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"
#include "Types.hpp"
#include <memory>
#include <iostream>

// This class just bundles the different managers together because they need to communicate and for ease of use

class Coordinator
{
public:
	void Init()
	{
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();
	}


	// Entity methods

	Entity CreateEntity() {
		return mEntityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity)
	{
		mEntityManager->DestroyEntity(entity);

		mComponentManager->EntityDestroyed(entity);

		mSystemManager->EntityDestroyed(entity);
	}

	// Component methods
	template<typename T> void RegisterComponent()
	{
		mComponentManager->RegisterComponent<T>();
	}

	template<typename T> void AddComponent(Entity entity, T component)
	{
		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		// set the bit that corresponds to the type of the component being added to 1 (true). 
		signature.set(mComponentManager->GetComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		// iterate through all the systems and compare the entity's new signature with the system's signature to see if the entity's new signature
		// corresponds with the system's. If so, insert the entity from the system's entity set.
		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T> void RemoveComponent(Entity entity)
	{

		std::cout << "remove called \n";
		mComponentManager->RemoveComponent<T>(entity);

		auto signature = mEntityManager->GetSignature(entity);
		// set the bit that corresponds to the type of the component being added to 0 (false)
		signature.set(mComponentManager->GetComponentType<T>(), false);
		mEntityManager->SetSignature(entity, signature);

		// iterate through all the systems and compare the entity's new signature with the system's signature to see if the entity's new signature
		// corresponds with the system's. If not, remove the entity from the system's entity set.
		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T> T& GetComponent(Entity entity)
	{
		return mComponentManager->GetComponent<T>(entity);
	}

	template<typename T> ComponentType GetComponentType()
	{
		return mComponentManager->GetComponentType<T>();
	}

	// System methods

	template<typename T> std::shared_ptr<T> RegisterSystem()
	{
		return mSystemManager->RegisterSystem<T>();
	}

	template<typename T> void SetSystemSignature(Signature signature)
	{
		mSystemManager->SetSignature<T>(signature);
	}

	template<typename T> bool HasComponent(Entity entity)
	{

		Signature entitySignature = mEntityManager->GetSignature(entity);

		return entitySignature.test(GetComponentType<T>());
	}

	std::vector<Entity> GetLivingEntities() 
	{
		return mEntityManager->GetLivingEntities();
	}

private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;
};