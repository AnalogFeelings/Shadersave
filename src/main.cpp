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
#include <glrenderer.h>
#include <resources.h>
#include <defines.h>

HDC deviceContextHandle;
HGLRC glRenderContextHandle;
RECT clientRect;
std::shared_ptr<GLRenderer> glRenderer;

auto WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	switch (message)
	{
	case WM_CREATE:
	{
		::GetClientRect(hWnd, &clientRect);
		INT windowWidth = clientRect.right;
		INT windowHeight = clientRect.bottom;

		glRenderer = std::make_shared<GLRenderer>();

		BOOL contextResult = glRenderer->InitContext(hWnd, deviceContextHandle, glRenderContextHandle);
		if (!contextResult)
		{
			GLenum error = glGetError();
			CHAR buffer[GLEW_ERROR_SIZE];

			std::snprintf(buffer, GLEW_ERROR_SIZE, "Error initializing GLEW.\n%s", glewGetErrorString(error));

			::MessageBox(hWnd, buffer, "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

			return -1;
		}

		BOOL rendererResult = glRenderer->InitRenderer(windowWidth, windowHeight);
		if (!rendererResult)
		{
			CHAR buffer[OPENGL_ERROR_SIZE];
			std::snprintf(buffer, OPENGL_ERROR_SIZE, "Error initializing OpenGL renderer.\n%s", glRenderer->QuadShader->ShaderLog);

			::MessageBox(hWnd, buffer, "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

			return -1;
		}

		return 0;
	}
	case WM_PAINT:
		PAINTSTRUCT paintStruct;

		::BeginPaint(hWnd, &paintStruct);
		glRenderer->DoRender(deviceContextHandle);
		::EndPaint(hWnd, &paintStruct);

		::InvalidateRect(hWnd, &clientRect, FALSE);

		return 0;
	case WM_DESTROY:
		if (glRenderer)
			glRenderer->CloseRenderer(hWnd, deviceContextHandle, glRenderContextHandle);

		return 0;
	}

	return ::DefScreenSaverProc(hWnd, message, wParam, lParam);
}

auto WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) -> BOOL
{
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ABOUTBUTT:
			std::string versionString = std::string(SHADERSAVE_VERSION);
			std::string builtString = "Shadersave v" + versionString + " by Analog Feelings\nhttps://github.com/AnalogFeelings";

			::MessageBox(hDlg, builtString.c_str(), "About Shadersave", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);

			break;
		}
	case WM_CLOSE:
		::EndDialog(hDlg, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

auto WINAPI RegisterDialogClasses(HANDLE hInst) -> BOOL
{
	return TRUE;
}