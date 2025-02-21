#pragma once 

#include <vector>
#include <glm/glm.hpp>

enum InputKey {
	NONE,
	W,
	A,
	S,
	D,
	F,
	Q,
	V,
	K,
	NUMBER_1,
	NUMBER_2,
	NUMBER_3,
	NUMBER_4,
	NUMBER_5,
	SHIFT_W,
	SHIFT_A,
	SHIFT_S,
	SHIFT_D,
	SPACE,
	TAB,
	LEFT_CONTROL,
	ESC,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_MIDDLE_SHIFT
};

class Common
{
public:
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;
	static bool normalsDebug;
	static bool wireframeDebug;
	static bool worldPosDebug;
	static bool lightPosDebug;
	static bool bulletLinesDebug;
	static bool bulletLinesReady;
	static int debugLinesCount;
	static bool aces;
	static bool reinhard;
	static bool uncharted;
	static float exposure;
	static bool bloomOn;
	static int bloomKernelSize;
	static float bloomStdDeviation;
	static float bloomIntervalMultiplier;
	static bool pbrDemonstration;
	static bool albedoDebug;
	static bool roughnessDebug;
	static bool metallicDebug;
	static bool aoDebug;
	static bool emissionDebug;
	static std::vector<float> bloomWeights;
	static glm::mat4 playerViewMatrix;
	static bool usingGuizmo;
	static bool voxelize;
	static bool showVoxelDebug;
	static int	voxelGridDimensions;
	static bool vxgi;
	static bool showDiffuseAccumulation;
	static bool showTotalIndirectDiffuseLight;
	static float diffuseConeSpread;
	static float voxelizationAreaSize;
	static float vxSpecularBias;
	static float specularStepSizeMultiplier;
	static float specularConeOriginOffset;
	static bool showTotalIndirectSpecularLight;
	static float specularConeMaxDistance;
	static float ssrMaxDistance;
	static float ssrResolution;
	static int ssrSteps;
	static float ssrThickness;
	static float ssrSpecularBias;
	static float ssrMaxBlurDistance;
	static int ssaoKernelSize;
	static float ssaoRadius;
	static float ssaoPower;
	static bool ssaoOn;
	static bool showAO;
	static glm::vec3 textColor;
	static glm::mat4 currentViewMatrix;
	static glm::mat4 currentProjMatrix;
	static glm::vec3 currentCamPos;
};