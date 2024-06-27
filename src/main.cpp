// Shadersave - A screensaver that can run Shadertoy shaders locally.
// Copyright (C) 2024-2024 Analog Feelings
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
#include "include/glrenderer.h"

#define ANIMATION_TIMER 1

auto WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	static HDC deviceContextHandle;
	static HGLRC glRenderContextHandle;
	static RECT clientRect;
	static GLRenderer* glRenderer;

	switch (message)
	{
	case WM_CREATE:
	{
		::GetClientRect(hWnd, &clientRect);
		INT windowWidth = clientRect.right;
		INT windowHeight = clientRect.bottom;

		DEVMODE deviceMode;
		ZeroMemory(&deviceMode, sizeof(deviceMode));

		deviceMode.dmSize = sizeof(deviceMode);
		deviceMode.dmDriverExtra = 0;

		UINT refreshRate;
		if (::EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &deviceMode) == NULL)
			refreshRate = std::floor(1 / 60 * 1000);
		else
			refreshRate = std::floor(1 / deviceMode.dmDisplayFrequency * 1000);

		glRenderer = new GLRenderer();

		BOOL contextResult = glRenderer->InitContext(hWnd, deviceContextHandle, glRenderContextHandle);
		if (!contextResult)
		{
			GLenum error = glGetError();
			CHAR buffer[96];

			std::snprintf(buffer, 96, "Error initializing GLEW.\n%s", glewGetErrorString(error));

			::MessageBox(hWnd, buffer, "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

			return -1;
		}

		BOOL rendererResult = glRenderer->InitRenderer(windowWidth, windowHeight);
		if(!rendererResult)
		{
			CHAR buffer[2048];
			std::snprintf(buffer, 2048, "Error initializing OpenGL renderer.\n%s", glRenderer->QuadShader->ShaderLog);

			::MessageBox(hWnd, buffer, "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

			return -1;
		}

		::SetTimer(hWnd, ANIMATION_TIMER, refreshRate, nullptr);
		return 0;
	}
	case WM_DESTROY:
		::KillTimer(hWnd, ANIMATION_TIMER);
		if(glRenderer) 
			glRenderer->CloseRenderer(hWnd, deviceContextHandle, glRenderContextHandle);

		return 0;
	case WM_TIMER:
		glRenderer->DoRender(deviceContextHandle);

		return 0;
	}

	return ::DefScreenSaverProc(hWnd, message, wParam, lParam);
}

auto WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) -> BOOL
{
	return FALSE;
}

auto WINAPI RegisterDialogClasses(HANDLE hInst) -> BOOL
{
	return TRUE;
}
