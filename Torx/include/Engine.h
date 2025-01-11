#pragma once

#include "../src/Util/Window.h"

#include <iostream>
#include "../src/Core/ECSCore.h"

namespace Torx
{
	class Engine
	{
	public:
		Engine() = default;

		static void ConfigWindow(int width, int height, const char*  title);

		static Window& GetWindow();

		void Init(const std::string scenePath);
		void Init();

		void Run(float deltaTime);
	private:

		static Window m_window;
	};
}

