#include "Common.h"


// -------- SCREEN SETTINGS -----------------------------------
const unsigned int Common::SCR_WIDTH = 1920;
const unsigned int Common::SCR_HEIGHT = 1080;

// ------------- DEBUG ----------------------------------------
bool  Common::normalsDebug{ false };
bool  Common::wireframeDebug{ false };
bool  Common::worldPosDebug{ false };
bool  Common::lightPosDebug{ false };
bool  Common::albedoDebug{ false };
bool  Common::roughnessDebug{ false };
bool  Common::metallicDebug{ false };
bool  Common::aoDebug{ false };
bool  Common::emissionDebug{ false };
bool  Common::bulletLinesDebug{ false };
bool  Common::bulletLinesReady{ false };

// ------------- TONE MAPPING ----------------------------------
bool  Common::aces{ true };
bool  Common::reinhard{ false };
bool  Common::uncharted{ false };
bool  Common::pbrDemonstration{ false };
float Common::exposure{ 2.0f };

// -------------- BLOOM ---------------------------------------
bool  Common::bloomOn{ false };
int   Common::bloomKernelSize{ 5 };
float Common::bloomStdDeviation{ 1.0f };
float Common::bloomIntervalMultiplier{ 0.2f };
std::vector<float> Common::bloomWeights;

// ------------- TEMPORARY STUFF -------------------------------
glm::mat4 Common::playerViewMatrix;
bool Common::usingGuizmo{ false };

// --------------- VXGI ----------------------------------------
bool Common::voxelize{ true };
bool Common::showVoxelDebug{ false };
int	Common::voxelGridDimensions{ 128 };
bool Common::vxgi{ true };
bool Common::showDiffuseAccumulation{ false };
bool Common::showTotalIndirectDiffuseLight{ false };
float Common::diffuseConeSpread{ 2.1f };
float Common::voxelizationAreaSize{ 12.0f };
float Common::vxSpecularBias{ 1 };
float Common::specularStepSizeMultiplier{ 1 };
float Common::specularConeOriginOffset{ 0.8f };
bool Common::showTotalIndirectSpecularLight{ false };
float Common::specularConeMaxDistance{ 1 };

// --------------- SSR ----------------------------------------
float Common::ssrMaxDistance{ 20 };
float Common::ssrResolution{ 1.0f };
float Common::ssrThickness{ 0.5f };
float Common::ssrSpecularBias{ 1.0f };
float Common::ssrMaxBlurDistance{ 10 };
int   Common::ssrSteps{ 50 };

// --------------- SSAO ----------------------------------------
float Common::ssaoRadius{ 0.5f };
float Common::ssaoPower{ 1.0f };
int Common::ssaoKernelSize{ 64 };
bool Common::ssaoOn{ true };
bool Common::showAO{ false };




