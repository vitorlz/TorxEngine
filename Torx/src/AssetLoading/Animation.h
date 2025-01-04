#pragma once

#include "model.h"
#include "Bone.h"

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    Animation() = default;
    Animation(const std::string& animationPath, Model* model);

    ~Animation()
    {
    }

    Bone* FindBone(const std::string& name);
    float GetTicksPerSecond();
    float GetDuration();
    std::string GetAnimationPath();
    const AssimpNodeData& GetRootNode();
    const std::map<std::string, BoneInfo>& GetBoneIDMap();
private:
    void ReadMissingBones(const aiAnimation* animation, Model& model);
    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

    float m_Duration;
    int m_TicksPerSecond;
    std::string m_Path;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};
