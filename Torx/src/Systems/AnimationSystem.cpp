#include "AnimationSystem.h";
#include "../Components/CAnimator.h"
#include "../Core/Coordinator.hpp"

extern Coordinator ecs;

void AnimationSystem::Init() {};

// This system simply updates the animated entities' animators.
void AnimationSystem::Update(float deltaTime)
{
	for (const auto& entity : mEntities)
	{
		ecs.GetComponent<CAnimator>(entity).animator.UpdateAnimation(deltaTime);
	}
}