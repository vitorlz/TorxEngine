#include "bulletDebugDrawer.h"
#include <glad/glad.h>
#include "../Rendering/RenderingUtil.h"
#include "../Core/Common.h"
#include <vector>
#include <iostream>

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{

	//std::cout << "debug lines count: " << Common::debugLinesCount << "\n";

	if (Common::debugLinesCount == 0)
	{
		m_points.clear();
		glBindBuffer(GL_ARRAY_BUFFER, RenderingUtil::mBulletDebugLinesVBO);
		glBufferData(GL_ARRAY_BUFFER, 10000 * 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	}
	

	m_points.push_back(from.x());
	m_points.push_back(from.y());
	m_points.push_back(from.z());
	m_points.push_back(color.x());
	m_points.push_back(color.y());
	m_points.push_back(color.z());

	m_points.push_back(to.x());
	m_points.push_back(to.y());
	m_points.push_back(to.z());
	m_points.push_back(color.x());
	m_points.push_back(color.y());
	m_points.push_back(color.z());
	
	glBindBuffer(GL_ARRAY_BUFFER, RenderingUtil::mBulletDebugLinesVBO);

	//GLint bufferSize = 0;
	//glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	//std::cout << "Buffer size in bytes: " << bufferSize << "\n";
	//std::cout << "Points size in bytes: " << m_points.size() * sizeof(float) << "\n";
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_points.size() * sizeof(float), m_points.data());

	
	Common::debugLinesCount++;
}

void BulletDebugDrawer::drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) {}
void BulletDebugDrawer::reportErrorWarning(const char*) {}
void BulletDebugDrawer::draw3dText(const btVector3&, const char*) {}
void BulletDebugDrawer::setDebugMode(int p) 
{ 
	m_debugMode = p; 
}

int BulletDebugDrawer::getDebugMode(void) const 
{ 
	return m_debugMode;
}