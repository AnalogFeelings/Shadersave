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
#include <Renderer.h>
#include <Defines.h>
#include <Settings.h>
#include <filesystem>
#include <shobjidl.h>
#include <atlbase.h>
#include <Globals.h>

auto WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	switch (message)
	{
		case WM_CREATE:
		{
			::GetClientRect(hWnd, &Globals::ClientRect);
			INT windowWidth = Globals::ClientRect.right;
			INT windowHeight = Globals::ClientRect.bottom;
			SETTINGS settings = Settings::LoadFromRegistry();

			Globals::MainWindow = hWnd;

			BOOL contextResult = Renderer::InitContext(hWnd, Globals::DeviceContext, Globals::GlRenderContext);
			if (!contextResult)
			{
				GLenum error = glGetError();
				CHAR buffer[GLEW_ERROR_SIZE];

				std::snprintf(buffer, GLEW_ERROR_SIZE, "Error initializing GLEW.\n%s", glewGetErrorString(error));

				::MessageBox(hWnd, buffer, "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

				return -1;
			}

			BOOL rendererResult = Renderer::InitRenderer(windowWidth, windowHeight, settings);
			if (!rendererResult)
			{
				CHAR buffer[OPENGL_ERROR_SIZE];
				std::snprintf(buffer, OPENGL_ERROR_SIZE, "Error initializing OpenGL renderer.\n%s", "placeholder");

				::MessageBox(hWnd, buffer, "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

				return -1;
			}

			return 0;
		}
		case WM_PAINT:
			PAINTSTRUCT paintStruct;

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
	return TRUE;
}