#include "PhysicsSystem.h"
#include "../Core/Coordinator.hpp"
#include "../Rendering/RenderingUtil.h"
#include "../Util/ShaderManager.h"
#include "../Util/Util.h"
#include "glm/glm.hpp"
#include "../Components/CTransform.h"
#include "../Components/CModel.h"
#include "../Components/CPlayer.h"
#include "../Components/CRigidBody.h"
#include "../Components/CSingleton_Input.h"
#include "../Physics/BulletDebugDrawer.h"
#include "../Physics/Raycast.h"
#include "../UI/UI.h"

#include "btBulletDynamicsCommon.h"
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

#include <LinearMath/btVector3.h>
#include "LinearMath/btAlignedObjectArray.h"

extern Coordinator ecs;

btAlignedObjectArray<btCollisionShape*> collisionShapes;

btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);


BulletDebugDrawer debugDrawer;
void PhysicsSystem::Init() 
{

	Raycast::setDynamicsWorld(dynamicsWorld);

	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

	dynamicsWorld->setDebugDrawer(&debugDrawer);
	

	RenderingUtil::CreateBulletDebugBuffers();

	///create a few basic rigid bodies
	btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -50, 0));
	{
		btScalar mass(0.);
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

	for (const auto& entity : mEntities)
	{

		auto& rigidBody = ecs.GetComponent<CRigidBody>(entity);
		auto& transform = ecs.GetComponent<CTransform>(entity);

		{
			//create a few dynamic rigidbodies
			// Re-using the same collision is better for memory usage and performance
			btBoxShape* colShape = new btBoxShape(btVector3(btScalar(transform.scale.x) * 0.5, btScalar(transform.scale.y) * 0.5, btScalar(transform.scale.z) * 0.5));

			collisionShapes.push_back(colShape);

			btTransform startTransform;
			/// Create Dynamic Objects
			startTransform.setIdentity();

			btScalar mass(1.f);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);

			startTransform.setOrigin(btVector3(
				btScalar(transform.position.x),
				btScalar(transform.position.y),
				btScalar(transform.position.z)));

			btQuaternion quatRot(transform.rotation.x, transform.rotation.y, transform.rotation.z);

			startTransform.setRotation(quatRot);

			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
			rigidBody.body = new btRigidBody(rbInfo);

			rigidBody.body->setUserIndex(entity);

			//add the body to the dynamics world
			dynamicsWorld->addRigidBody(rigidBody.body);
		}
		
	}
}

void PhysicsSystem::Update(float deltaTime)
{
	dynamicsWorld->stepSimulation(deltaTime, 20, 1.0f/165.0f);

	for (const auto& entity : mEntities)
	{
		auto& rigidBody = ecs.GetComponent<CRigidBody>(entity);

		auto& transform = ecs.GetComponent<CTransform>(entity);

		btTransform trans;
		if (rigidBody.body && rigidBody.body->getMotionState()) //&& rigidBody.body->getActivationState() == ACTIVE_TAG)
		{
			rigidBody.body->getMotionState()->getWorldTransform(trans);

			btScalar x, y, z;

			btQuaternion rotation  = trans.getRotation();

			transform.position = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));

			transform.rotationMatrix = glm::mat4_cast(glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z()));	
		}
	}

	if (UI::isOpen)
	{
		int entityHit = Raycast::mouseRaycast();

		

		glm::vec3 mouseRayDir = Raycast::getMouseRayDir();

		std::cout << "entity hit: " << entityHit << "\n";

		if (entityHit != -1)
		{
			std::cout << Util::vec3ToString(mouseRayDir);
			btRigidBody* entityHitRb = ecs.GetComponent<CRigidBody>(entityHit).body;
			entityHitRb->activate();
			entityHitRb->applyCentralImpulse(btVector3(mouseRayDir.x, mouseRayDir.y, mouseRayDir.z));
		}
	}
	
	if (Common::bulletLinesDebug)
	{
		dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		dynamicsWorld->debugDrawWorld();
		dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_NoDebug);
	} 

	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	if (inputSing.pressedKeys[V])
	{

		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		for (const auto& entity : mEntities)
		{
			auto& transform = ecs.GetComponent<CTransform>(entity);

			for (int k = 0; k < 5; k++)
			{
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						
						transform.position = glm::vec3(0.2f * i, 10.0f + 0.2f * k, 0.2f * j);
						transform.scale = glm::vec3(0.1f, 0.1f, 0.1f);
						transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
					}
				}
			}
		}

		PhysicsSystem::Init();
	}
}