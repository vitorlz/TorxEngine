#pragma once

#include "btBulletDynamicsCommon.h"
#include <LinearMath/btVector3.h>
#include "LinearMath/btAlignedObjectArray.h"
#include <vector>

class BulletDebugDrawer : public btIDebugDraw
{
public:
	static std::vector<float> m_points;
	static void drawLines();
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&);
	void reportErrorWarning(const char*);
	void draw3dText(const btVector3&, const char*);
	void setDebugMode(int p);
	int getDebugMode(void) const;
private:
	int m_DebugMode;
	static int m_LinesCount;
};