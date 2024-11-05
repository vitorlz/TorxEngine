#include "Common.h"


// -------- SCREEN SETTINGS -----------------------------------
const unsigned int Common::SCR_WIDTH = 1280;
const unsigned int Common::SCR_HEIGHT = 800;

// ------------- DEBUG ----------------------------------------
bool Common::normalsDebug{ false };
bool Common::wireframeDebug{ false };
bool Common::worldPosDebug{ false };

// ------------- TONE MAPPING ----------------------------------
bool  Common::aces{ true };
bool  Common::reinhard{ false };
bool  Common::uncharted{ false };
float Common::exposure{ 2.0f };

// -------------- BLOOM ---------------------------------------
bool Common::bloomOn{ true };
int Common::bloomKernelSize{ 5 };
float Common::bloomStdDeviation{ 1.0f };
float Common::bloomIntervalMultiplier{ 0.2f };
std::vector<float> Common::bloomWeights;
