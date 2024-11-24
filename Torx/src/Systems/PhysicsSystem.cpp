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
#include "../Components/CMesh.h"
#include "../Physics/BulletDebugDrawer.h"
#include "../Physics/Raycast.h"
#include "../UI/UI.h"
#include "../Editor/Editor.h"

#include "btBulletDynamicsCommon.h"
#include <BulletCollision/CollisionShapes/btShapeHull.h>
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

	RenderingUtil::CreateBulletDebugBuffers();

	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

	dynamicsWorld->setDebugDrawer(&debugDrawer);
	dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	for (const auto& entity : mEntities)
	{

		auto& rigidBody = ecs.GetComponent<CRigidBody>(entity);
		auto& transform = ecs.GetComponent<CTransform>(entity);

		{
			btTransform startTransform;
			/// Create Dynamic Objects
			startTransform.setIdentity();

			btScalar mass = rigidBody.mass;

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btCollisionShape* colShapeDynamic = nullptr;
			btBvhTriangleMeshShape* colShapeStatic = nullptr;

			if (isDynamic)
			{

				if (ecs.HasComponent<CModel>(entity))
				{
					auto& model = ecs.GetComponent<CModel>(entity);

					btConvexHullShape* convexHullShape = new btConvexHullShape();


					for (const Mesh& mesh : model.model.meshes)
					{
						for (const Vertex& vertex : mesh.vertices) {
							convexHullShape->addPoint(btVector3(vertex.Position.x, vertex.Position.y, vertex.Position.z), false);
						}
					}

					convexHullShape->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));

					convexHullShape->recalcLocalAabb();
					convexHullShape->optimizeConvexHull();
					convexHullShape->setMargin(0.005f);

					colShapeDynamic = convexHullShape;

					collisionShapes.push_back(colShapeDynamic);
				}
				else if (ecs.HasComponent<CMesh>(entity))
				{
					auto& meshComponent = ecs.GetComponent<CMesh>(entity);

					btConvexHullShape* convexHullShape = new btConvexHullShape();


					
					for (const Vertex& vertex : meshComponent.mesh.vertices) {
						convexHullShape->addPoint(btVector3(vertex.Position.x, vertex.Position.y, vertex.Position.z), false);
					}
					convexHullShape->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));

					convexHullShape->recalcLocalAabb();
					convexHullShape->optimizeConvexHull();
					convexHullShape->setMargin(0.005f);

					colShapeDynamic = convexHullShape;

					collisionShapes.push_back(colShapeDynamic);
				}
				
			}
			else
			{

				if (ecs.HasComponent<CModel>(entity))
				{
					auto& model = ecs.GetComponent<CModel>(entity);

					btTriangleIndexVertexArray* triangleIndexVertex = new btTriangleIndexVertexArray();

					for (const Mesh& mesh : model.model.meshes)
					{
						btIndexedMesh indexedMesh;

						indexedMesh.m_vertexBase = (const unsigned char*)mesh.vertices.data();
						indexedMesh.m_vertexStride = sizeof(mesh.vertices[0]);
						indexedMesh.m_numVertices = mesh.vertices.size();
						indexedMesh.m_triangleIndexBase = (const unsigned char*)mesh.indices.data();
						indexedMesh.m_triangleIndexStride = sizeof(unsigned int) * 3;
						indexedMesh.m_numTriangles = mesh.indices.size() / 3;
						indexedMesh.m_indexType = PHY_INTEGER;

						//std::cout << mesh.indices.size() << "\n";

						triangleIndexVertex->addIndexedMesh(indexedMesh, PHY_INTEGER);

					}

					colShapeStatic = new btBvhTriangleMeshShape(triangleIndexVertex, false);

					colShapeStatic->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
					colShapeStatic->recalcLocalAabb();

					collisionShapes.push_back(colShapeStatic);
				}
				else if (ecs.HasComponent<CMesh>(entity))
				{
					auto& meshComponent = ecs.GetComponent<CMesh>(entity);

					btTriangleIndexVertexArray* triangleIndexVertex = new btTriangleIndexVertexArray();

					btIndexedMesh indexedMesh;

					indexedMesh.m_vertexBase = (const unsigned char*)meshComponent.mesh.vertices.data();
					indexedMesh.m_vertexStride = sizeof(meshComponent.mesh.vertices[0]);
					indexedMesh.m_numVertices = meshComponent.mesh.vertices.size();
					indexedMesh.m_triangleIndexBase = (const unsigned char*)meshComponent.mesh.indices.data();
					indexedMesh.m_triangleIndexStride = sizeof(unsigned int) * 3;
					indexedMesh.m_numTriangles = meshComponent.mesh.indices.size() / 3;
					indexedMesh.m_indexType = PHY_INTEGER;

					//std::cout << mesh.indices.size() << "\n";

					triangleIndexVertex->addIndexedMesh(indexedMesh, PHY_INTEGER);

					colShapeStatic = new btBvhTriangleMeshShape(triangleIndexVertex, false);

					colShapeStatic->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
					colShapeStatic->recalcLocalAabb();

					collisionShapes.push_back(colShapeStatic);
				}
				
			}
		
			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShapeDynamic->calculateLocalInertia(mass, localInertia);

			startTransform.setOrigin(btVector3(
				btScalar(transform.position.x),
				btScalar(transform.position.y),
				btScalar(transform.position.z)));

			btQuaternion quatRot(glm::radians(transform.rotation.y), glm::radians(transform.rotation.x), glm::radians(transform.rotation.z));

			startTransform.setRotation(quatRot);

			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			if (isDynamic)
			{
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShapeDynamic, localInertia);
				rigidBody.body = new btRigidBody(rbInfo);
			}
			else 
			{
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShapeStatic, localInertia);
				rigidBody.body = new btRigidBody(rbInfo);
			}
		
			rigidBody.body->setUserIndex(entity);

			dynamicsWorld->addRigidBody(rigidBody.body);
		}
	}
}

void PhysicsSystem::Update(float deltaTime)
{
	dynamicsWorld->stepSimulation(deltaTime, 20, 1.0f / 165.0f);

	if (mEntities.size() > dynamicsWorld->getNumCollisionObjects())
	{
		
		Entity newEntity;
		std::vector<Entity> oldEntities;
		for (Entity entity : mEntities)
		{
			for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
			{
				newEntity = entity;
				if (entity == dynamicsWorld->getCollisionObjectArray()[i]->getUserIndex())
				{
					oldEntities.push_back(entity);
					break;
				}
			}
		}

		for (Entity entity : mEntities)
		{
			auto it = std::find(oldEntities.begin(), oldEntities.end(), entity);

			if (it == oldEntities.end())
			{
				newEntity = entity;
			}
		}

		auto& rigidBody = ecs.GetComponent<CRigidBody>(newEntity);
		auto& transform = ecs.GetComponent<CTransform>(newEntity);

		{
			btTransform startTransform;
			/// Create Dynamic Objects
			startTransform.setIdentity();

			btScalar mass = rigidBody.mass;

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btCollisionShape* colShapeDynamic = nullptr;
			btBvhTriangleMeshShape* colShapeStatic = nullptr;

			if (isDynamic)
			{

				if (ecs.HasComponent<CModel>(newEntity))
				{
					auto& model = ecs.GetComponent<CModel>(newEntity);

					btConvexHullShape* convexHullShape = new btConvexHullShape();


					for (const Mesh& mesh : model.model.meshes)
					{
						for (const Vertex& vertex : mesh.vertices) {
							convexHullShape->addPoint(btVector3(vertex.Position.x, vertex.Position.y, vertex.Position.z), false);
						}
					}

					convexHullShape->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));

					convexHullShape->recalcLocalAabb();
					convexHullShape->optimizeConvexHull();
					convexHullShape->setMargin(0.005f);

					colShapeDynamic = convexHullShape;

					collisionShapes.push_back(colShapeDynamic);
				}
				else if (ecs.HasComponent<CMesh>(newEntity))
				{
					auto& meshComponent = ecs.GetComponent<CMesh>(newEntity);

					btConvexHullShape* convexHullShape = new btConvexHullShape();



					for (const Vertex& vertex : meshComponent.mesh.vertices) {
						convexHullShape->addPoint(btVector3(vertex.Position.x, vertex.Position.y, vertex.Position.z), false);
					}
					convexHullShape->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));

					convexHullShape->recalcLocalAabb();
					convexHullShape->optimizeConvexHull();
					convexHullShape->setMargin(0.005f);

					colShapeDynamic = convexHullShape;

					collisionShapes.push_back(colShapeDynamic);
				}

			}
			else
			{

				if (ecs.HasComponent<CModel>(newEntity))
				{
					auto& model = ecs.GetComponent<CModel>(newEntity);

					btTriangleIndexVertexArray* triangleIndexVertex = new btTriangleIndexVertexArray();

					for (const Mesh& mesh : model.model.meshes)
					{
						btIndexedMesh indexedMesh;

						indexedMesh.m_vertexBase = (const unsigned char*)mesh.vertices.data();
						indexedMesh.m_vertexStride = sizeof(mesh.vertices[0]);
						indexedMesh.m_numVertices = mesh.vertices.size();
						indexedMesh.m_triangleIndexBase = (const unsigned char*)mesh.indices.data();
						indexedMesh.m_triangleIndexStride = sizeof(unsigned int) * 3;
						indexedMesh.m_numTriangles = mesh.indices.size() / 3;
						indexedMesh.m_indexType = PHY_INTEGER;

						//std::cout << mesh.indices.size() << "\n";

						triangleIndexVertex->addIndexedMesh(indexedMesh, PHY_INTEGER);

					}

					colShapeStatic = new btBvhTriangleMeshShape(triangleIndexVertex, false);

					colShapeStatic->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
					colShapeStatic->recalcLocalAabb();

					collisionShapes.push_back(colShapeStatic);
				}
				else if (ecs.HasComponent<CMesh>(newEntity))
				{
					auto& meshComponent = ecs.GetComponent<CMesh>(newEntity);

					btTriangleIndexVertexArray* triangleIndexVertex = new btTriangleIndexVertexArray();

					btIndexedMesh indexedMesh;

					indexedMesh.m_vertexBase = (const unsigned char*)meshComponent.mesh.vertices.data();
					indexedMesh.m_vertexStride = sizeof(meshComponent.mesh.vertices[0]);
					indexedMesh.m_numVertices = meshComponent.mesh.vertices.size();
					indexedMesh.m_triangleIndexBase = (const unsigned char*)meshComponent.mesh.indices.data();
					indexedMesh.m_triangleIndexStride = sizeof(unsigned int) * 3;
					indexedMesh.m_numTriangles = meshComponent.mesh.indices.size() / 3;
					indexedMesh.m_indexType = PHY_INTEGER;

					//std::cout << mesh.indices.size() << "\n";

					triangleIndexVertex->addIndexedMesh(indexedMesh, PHY_INTEGER);

					colShapeStatic = new btBvhTriangleMeshShape(triangleIndexVertex, false);

					colShapeStatic->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
					colShapeStatic->recalcLocalAabb();

					collisionShapes.push_back(colShapeStatic);
				}

			}

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShapeDynamic->calculateLocalInertia(mass, localInertia);

			startTransform.setOrigin(btVector3(
				btScalar(transform.position.x),
				btScalar(transform.position.y),
				btScalar(transform.position.z)));

			btQuaternion quatRot(glm::radians(transform.rotation.y), glm::radians(transform.rotation.x), glm::radians(transform.rotation.z));

			startTransform.setRotation(quatRot);

			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			if (isDynamic)
			{
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShapeDynamic, localInertia);
				rigidBody.body = new btRigidBody(rbInfo);
			}
			else
			{
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShapeStatic, localInertia);
				rigidBody.body = new btRigidBody(rbInfo);
			}

			rigidBody.body->setUserIndex(newEntity);

			dynamicsWorld->addRigidBody(rigidBody.body);
		}
	}

	for (const auto& entity : mEntities)
	{
		auto& rigidBody = ecs.GetComponent<CRigidBody>(entity);

		auto& transform = ecs.GetComponent<CTransform>(entity);

		if (Common::usingGuizmo && rigidBody.body)
		{
			btTransform physicsTransform;

			physicsTransform.setIdentity();

			physicsTransform.setOrigin(btVector3(
				btScalar(transform.position.x),
				btScalar(transform.position.y),
				btScalar(transform.position.z)));

			btQuaternion quatRot;

			quatRot.setEulerZYX(glm::radians(transform.rotation.z), glm::radians(transform.rotation.y), glm::radians(transform.rotation.x));

			physicsTransform.setRotation(quatRot);

			rigidBody.body->getCollisionShape()->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));

			rigidBody.body->getMotionState()->setWorldTransform(physicsTransform);
			rigidBody.body->setWorldTransform(physicsTransform);

			dynamicsWorld->updateSingleAabb(rigidBody.body);

			continue;
		}

		btTransform trans;
		if (rigidBody.body && rigidBody.body->getMotionState() && rigidBody.mass > 0 && !Common::usingGuizmo)
		{
			rigidBody.body->getMotionState()->getWorldTransform(trans);

			btScalar x, y, z;

			btQuaternion rotation  = trans.getRotation();

			transform.position = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));

			glm::vec3 eulerRot = glm::eulerAngles(glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z()));

			transform.rotation = glm::vec3(glm::degrees(eulerRot.x), glm::degrees(eulerRot.y), glm::degrees(eulerRot.z));
		}
	}

	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	static bool shotFired{ false };
	if (UI::isOpen)
	{
		if (inputSing.pressedKeys[MOUSE_LEFT] && !shotFired && !Editor::isOn())
		{
			int entityHit = Raycast::mouseRaycast();

			glm::vec3 mouseRayDir = glm::normalize(Raycast::getMouseRayDir());
			shotFired = true;
			//std::cout << Util::vec3ToString(mouseRayDir);
			btRigidBody* entityHitRb = ecs.GetComponent<CRigidBody>(entityHit).body;
			btVector3 offset = Raycast::getMouseHitPointWorld() - entityHitRb->getCenterOfMassPosition() ;

			std::cout << "entity hit: " << entityHit << "\n";
			std::cout << "Hitpoint World: " << Raycast::getMouseHitPointWorld().x() << ", " << Raycast::getMouseHitPointWorld().y() << ", " << Raycast::getMouseHitPointWorld().z() << "\n";
			std::cout << "Center of mass position: " << entityHitRb->getCenterOfMassPosition().x() << ", " << entityHitRb->getCenterOfMassPosition().y() << ", " << entityHitRb->getCenterOfMassPosition().z() << "\n";
			std::cout << "Offset: " << offset.x() << ", " << offset.y() << ", " << offset.z() << "\n";
			entityHitRb->activate();
			entityHitRb->applyImpulse(btVector3(mouseRayDir.x, mouseRayDir.y, mouseRayDir.z) * 2, offset);
		}
		else if (!inputSing.pressedKeys[MOUSE_RIGHT])
		{
			shotFired = false;
		}
	}

	if (mEntities.size() < dynamicsWorld->getNumCollisionObjects())
	{
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			Entity entity = dynamicsWorld->getCollisionObjectArray()[i]->getUserIndex();
			if (!ecs.isAlive(entity) || !ecs.HasComponent<CRigidBody>(entity))
			{
				btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState())
				{
					delete body->getMotionState();
				}
				dynamicsWorld->removeCollisionObject(obj);
				delete obj->getCollisionShape();
				delete obj;
			};
		}
	}

	if (Common::bulletLinesDebug)
	{
		dynamicsWorld->debugDrawWorld();
	}
}