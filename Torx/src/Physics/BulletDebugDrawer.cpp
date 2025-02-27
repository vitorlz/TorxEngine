#include "bulletDebugDrawer.h"
#include <glad/glad.h>
#include "../Util/RenderingUtil.h"
#include "../Core/Common.h"
#include <vector>
#include <iostream>

std::vector<float> BulletDebugDrawer::m_points;
int BulletDebugDrawer::m_LinesCount;

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
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

	m_LinesCount++;
}

void BulletDebugDrawer::drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) {}
void BulletDebugDrawer::reportErrorWarning(const char*) {}
void BulletDebugDrawer::draw3dText(const btVector3&, const char*) {}
void BulletDebugDrawer::setDebugMode(int p) 
{ 
	m_DebugMode = p; 
}

int BulletDebugDrawer::getDebugMode(void) const 
{ 
	return m_DebugMode;
}

void BulletDebugDrawer::drawLines()
{
	glNamedBufferSubData(RenderingUtil::mBulletDebugLinesVBO, 0, BulletDebugDrawer::m_points.size() * sizeof(btScalar), BulletDebugDrawer::m_points.data());

	glBindVertexArray(RenderingUtil::mBulletDebugLinesVAO);
	glDrawArrays(GL_LINES, 0, m_LinesCount * 2);
	glBindVertexArray(0);

	m_points.clear();
	m_LinesCount = 0;
}


