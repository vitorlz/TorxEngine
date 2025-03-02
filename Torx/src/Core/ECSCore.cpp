#include "ECSCore.h"

#include "Coordinator.hpp"

#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "../Components/CModel.h"
#include "../Components/CSingleton_Input.h"
#include "../Components/CPlayer.h"
#include "../Components/CRigidBody.h"
#include "../Components/CMesh.h"
#include "../Components/CAnimator.h"
#include "../Components/CCamera.h"
#include "../Components/CNativeScript.h"
          
#include "../Systems/RenderSystem.h"
#include "../Util/RenderingUtil.h"
#include "../Systems/LightSystem.h"
#include "../Systems/GeneralInputSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/NativeScriptSystem.h"

extern Coordinator ecs;

std::vector<std::shared_ptr<System>> ECSCore::m_systems{};

void ECSCore::RegisterCoreComponentsAndSystems()
{
    ecs.Init();

    ecs.RegisterComponent<CTransform>();
    ecs.RegisterComponent<CModel>();
    ecs.RegisterComponent<CLight>();
    ecs.RegisterComponent<CPlayer>();
    ecs.RegisterComponent<CRigidBody>();
    ecs.RegisterComponent<CMesh>();
    ecs.RegisterComponent<CAnimator>();
    ecs.RegisterComponent<CCamera>();
    ecs.RegisterComponent<CNativeScript>();

    auto renderSystem = ecs.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        ecs.SetSystemSignature<RenderSystem>(signature);
    }

    auto physicsSystem = ecs.RegisterSystem<PhysicsSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CRigidBody>());
        ecs.SetSystemSignature<PhysicsSystem>(signature);
    }

    auto lightSystem = ecs.RegisterSystem<LightSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CLight>());
        ecs.SetSystemSignature<LightSystem>(signature);
    }

    auto generalInputSystem = ecs.RegisterSystem<GeneralInputSystem>();
    {
        Signature signature;
        ecs.SetSystemSignature<GeneralInputSystem>(signature);
    }

    auto  animationSystem = ecs.RegisterSystem<AnimationSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CAnimator>());
        ecs.SetSystemSignature<AnimationSystem>(signature);
    }

    auto nativeScriptSystem = ecs.RegisterSystem<NativeScriptSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CNativeScript>());
        ecs.SetSystemSignature<NativeScriptSystem>(signature);
    }

    m_systems.push_back(generalInputSystem);
    m_systems.push_back(animationSystem);
    m_systems.push_back(lightSystem);
    m_systems.push_back(renderSystem);
    m_systems.push_back(physicsSystem);
    m_systems.push_back(nativeScriptSystem);
}

void ECSCore::InitSystems()
{
    for (auto system : m_systems)
    {
        system->Init();
    }
}

void ECSCore::UpdateSystems(float dt)
{
    for (auto system : m_systems)
    {
        system->Update(dt);
    }
}

