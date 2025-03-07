#pragma once 

#include "Animation.h"
#include <vector>

class Animator
{
public:

	Animator() {}
	Animator(Animation* Animation);

	void UpdateAnimation(float dt);

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> GetFinalBoneMatrices();

	float GetCurrentTime();
	
	Animation* GetCurrentAnimation();
	
private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};