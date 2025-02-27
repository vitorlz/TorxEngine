#pragma once

#include "Util/Window.h"

#include <iostream>
#include "Core/ECSCore.h"

namespace Torx
{
	enum EngineMode
	{
		EDITOR,
		PLAY
	};   

	class Engine
	{
	public:
		static Engine& getInstance();

	private:
		Engine();

	public:
		Engine(Engine const&) = delete;
		void operator = (Engine const&) = delete;

		static void ConfigWindow(int width, int height, const char*  title);

		static Window& GetWindow();

		void Init(const std::string scenePath);
		void Init();

		void Run();

		static EngineMode MODE;
	private:
		static Window m_window;
	};
}

