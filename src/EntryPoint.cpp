// Shadersave - A screensaver that can run Shadertoy shaders locally.
// Copyright (C) 2024 Analog Feelings
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <windows.h>
#include <scrnsave.h>
#include <Renderer.h>
#include <Defines.h>
#include <Settings.h>
#include <filesystem>
#include <shobjidl.h>
#include <atlbase.h>
#include <Globals.h>

auto WINAPI ScreenSaverProc(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	switch (message)
	{
		case WM_CREATE:
		{
			::GetClientRect(hWnd, &Globals::ClientRect);
			int windowWidth = Globals::ClientRect.right;
			int windowHeight = Globals::ClientRect.bottom;
			RenderSettings settings = Settings::LoadFromRegistry();

			Globals::FrameDurationUs = static_cast<int>(1000000 / std::stoi(settings.FramerateCap));

			Globals::MainWindow = hWnd;

			Globals::FrameTimer = CreateWaitableTimerW(NULL, TRUE, L"RenderLoopTimer");
			if (!Globals::FrameTimer)
			{
				std::string error = "Error initializing WaitableTimer.";

				::MessageBox(hWnd, error.c_str(), "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

				return -1;
			}

			bool contextResult = Renderer::InitContext(hWnd, Globals::DeviceContext, Globals::GlRenderContext);
			if (!contextResult)
			{
				std::string error = "Error initializing OpenGL context.\n" + Globals::LastError;

				::MessageBox(hWnd, error.c_str(), "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

				return -1;
			}

			bool rendererResult = Renderer::InitRenderer(windowWidth, windowHeight, settings);
			if (!rendererResult)
			{
				std::string error = "Error initializing OpenGL renderer.\n" + Globals::LastError;

				::MessageBox(hWnd, error.c_str(), "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

				return -1;
			}

			return 0;
		}
		case WM_PAINT:
			PAINTSTRUCT paintStruct;

			Globals::FrameDueTime.QuadPart = -1 * Globals::FrameDurationUs * 10;
			SetWaitableTimer(Globals::FrameTimer, &Globals::FrameDueTime, 0, NULL, NULL, FALSE);
			WaitForSingleObject(Globals::FrameTimer, INFINITE);

			::BeginPaint(hWnd, &paintStruct);
			Renderer::DoRender(Globals::DeviceContext);
			::EndPaint(hWnd, &paintStruct);

			::InvalidateRect(hWnd, &Globals::ClientRect, FALSE);

			return 0;
		case WM_DESTROY:
			Renderer::UninitializeRenderer();

			return 0;
	}

	return ::DefScreenSaverProc(hWnd, message, wParam, lParam);
}

auto WINAPI RegisterDialogClasses(HANDLE hInst) -> BOOL
{
	return true;
}