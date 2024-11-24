#include "Util.h"
#include "../Core/Common.h"
#include <numbers>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <sstream>
#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "../Components/CModel.h"
#include "../Components/CSingleton_Input.h"
#include "../Components/CPlayer.h"
#include "../Components/CRigidBody.h"
#include "../Components/CMesh.h"
#include "../Core/Coordinator.hpp"

extern Coordinator ecs;

std::vector<float> Util::gaussian_weights(int kernel_size, float stdDeviation, float intervalMultiplier)
{
	float variance = stdDeviation * stdDeviation;
	float weight;
	float x = 1;
	std::vector<float> weights;

	for (float i = 1; i <= kernel_size; i++)
	{

		weight = (1 / pow(2 * std::numbers::pi * variance, 0.5)) * exp(-((x * x) / (2 * variance)));
		weights.push_back(weight);

		x += intervalMultiplier * i;
	}

	return weights;
};

unsigned int sphereVAO = 0;
unsigned int indexCount;
void Util::renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

std::string Util::vec3ToString(glm::vec3 vec)
{
    std::stringstream ss;
    ss << vec.x << ", " <<  vec.y << ", " << vec.z << "\n"; 
    return ss.str();
}

Entity Util::duplicateEntity(Entity entity)
{
    Entity duplicateEntity = ecs.CreateEntity();

    if (ecs.HasComponent<CLight>(entity))
    {

        auto componentCopy = ecs.GetComponent<CLight>(entity);

        ecs.AddComponent<CLight>(duplicateEntity,
            componentCopy
        );
    }
    if (ecs.HasComponent<CMesh>(entity))
    {

        auto componentCopy = ecs.GetComponent<CMesh>(entity);

        ecs.AddComponent<CMesh>(duplicateEntity,
            componentCopy
        );
    }
    if (ecs.HasComponent<CModel>(entity))
    {

        auto componentCopy = ecs.GetComponent<CModel>(entity);

        ecs.AddComponent<CModel>(duplicateEntity,
            componentCopy
        );
    }
    if (ecs.HasComponent<CRigidBody>(entity))
    {

        auto componentCopy = ecs.GetComponent<CRigidBody>(entity);

        ecs.AddComponent<CRigidBody>(duplicateEntity,
            componentCopy
        );
    }
    if (ecs.HasComponent<CTransform>(entity))
    {

        auto componentCopy = ecs.GetComponent<CTransform>(entity);

        ecs.AddComponent<CTransform>(duplicateEntity,
            componentCopy
        );
    }

    return duplicateEntity;
}

void Util::decomposeMtx(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
{
    pos = m[3];
    for (int i = 0; i < 3; i++)
        scale[i] = glm::length(glm::vec3(m[i]));
    const glm::mat3 rotMtx(
        glm::vec3(m[0]) / scale[0],
        glm::vec3(m[1]) / scale[1],
        glm::vec3(m[2]) / scale[2]);
    rot = glm::quat_cast(rotMtx);
}