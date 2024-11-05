#pragma once 

#include <vector>

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
	SHIFT_W,
	SHIFT_A,
	SHIFT_S,
	SHIFT_D,
	SPACE,
	TAB,
	LEFT_CONTROL,
	ESC,
};

class Common
{
public:

	static const unsigned int SCR_WIDTH;
	static const unsigned int SCR_HEIGHT;
	static bool normalsDebug;
	static bool wireframeDebug;
	static bool worldPosDebug;
	static bool aces;
	static bool reinhard;
	static bool uncharted;
	static float exposure;
	static bool bloomOn;
	static int bloomKernelSize;
	static float bloomStdDeviation;
	static float bloomIntervalMultiplier;
	static std::vector<float> bloomWeights;
};