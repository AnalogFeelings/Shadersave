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
#include <settings.h>
#include <registry.h>
#include <filesystem>
#include <shobjidl.h>
#include <atlbase.h>

HDC deviceContextHandle;
HGLRC glRenderContextHandle;
RECT clientRect;
std::shared_ptr<GLRenderer> glRenderer;

auto LoadSettings() -> SETTINGS;
auto SaveSettings(PSETTINGS settings) -> BOOL;
auto ValidateSettings(PSETTINGS settings) -> VOID;
auto OpenFilePicker(HWND owner) -> std::string;

auto WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	switch (message)
	{
		case WM_CREATE:
		{
			::GetClientRect(hWnd, &clientRect);
			INT windowWidth = clientRect.right;
			INT windowHeight = clientRect.bottom;
			SETTINGS settings = LoadSettings();

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

			BOOL rendererResult = glRenderer->InitRenderer(windowWidth, windowHeight, settings);
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
	switch (message)
	{
		case WM_INITDIALOG:
		{
			HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if(!SUCCEEDED(comResult))
			{
				::MessageBox(hDlg, "Error initializing COM.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

				return FALSE;
			}

			SETTINGS settings = LoadSettings();

			SetDlgItemText(hDlg, IDC_SHADERPATH, settings.ShaderPath.c_str());
			SetDlgItemText(hDlg, IDC_FRAMECAP, std::to_string(settings.FramerateCap).c_str());

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_ABOUTBUTT:
				{
					std::string versionString = std::string(SHADERSAVE_VERSION);
					std::string builtString = "Shadersave v" + versionString + " by Analog Feelings\nhttps://github.com/AnalogFeelings";

					::MessageBox(hDlg, builtString.c_str(), "About Shadersave", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);

					break;
				}
				case IDC_CANCELBUTT:
				{
					::EndDialog(hDlg, 0);
					break;
				}
				case IDC_OKBUTT:
				{
					CHAR shaderPath[MAX_PATH];
					GetDlgItemText(hDlg, IDC_SHADERPATH, shaderPath, MAX_PATH);
					UINT frameCap = GetDlgItemInt(hDlg, IDC_FRAMECAP, nullptr, FALSE);

					SETTINGS settings =
					{
						.ShaderPath = std::string(shaderPath),
						.FramerateCap = frameCap
					};

					BOOL saveResult = SaveSettings(&settings);
					if (!saveResult)
					{
						::MessageBox(hDlg, "Could not save settings!", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

						break;
					}

					::EndDialog(hDlg, 0);
					break;
				}
				case IDC_BROWSEBUTT:
				{
					std::string filePath = OpenFilePicker(hDlg);

					SetDlgItemText(hDlg, IDC_SHADERPATH, filePath.c_str());

					break;
				}
			}
			return FALSE;
		}
		case WM_CLOSE:
		{
			CoUninitialize();
			::EndDialog(hDlg, 0);

			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}

auto WINAPI RegisterDialogClasses(HANDLE hInst) -> BOOL
{
	return TRUE;
}

auto LoadSettings() -> SETTINGS
{
	std::string shaderPath = ReadRegistryString(REGISTRY_SUBKEY, SHADER_PATH);
	UINT framerateCap = ReadRegistryDword(REGISTRY_SUBKEY, FRAMERATE_CAP);

	SETTINGS settings =
	{
		.ShaderPath = shaderPath,
		.FramerateCap = framerateCap
	};

	ValidateSettings(&settings);

	return settings;
}

auto SaveSettings(PSETTINGS settings) -> BOOL
{
	ValidateSettings(settings);

	BOOL pathResult = SetRegistryString(REGISTRY_SUBKEY, SHADER_PATH, settings->ShaderPath);
	if (!pathResult)
		return FALSE;

	BOOL frameResult = SetRegistryDword(REGISTRY_SUBKEY, FRAMERATE_CAP, settings->FramerateCap);
	if (!frameResult)
		return FALSE;

	return TRUE;
}

auto ValidateSettings(PSETTINGS settings) -> VOID
{
	if(!std::filesystem::is_regular_file(settings->ShaderPath))
		settings->ShaderPath = std::string();

	DEVMODE deviceMode = {};
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &deviceMode);

	if(settings->FramerateCap > deviceMode.dmDisplayFrequency)
		settings->FramerateCap = deviceMode.dmDisplayFrequency;
}

// I hate working with COM.
auto OpenFilePicker(HWND owner) -> std::string
{
	CComPtr<IFileOpenDialog> filePicker;

	HRESULT currentResult = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, 
		IID_IFileOpenDialog, reinterpret_cast<void**>(&filePicker));

	if(!SUCCEEDED(currentResult))
		return std::string();

	currentResult = filePicker->Show(owner);

	if(!SUCCEEDED(currentResult))
		return std::string();

	CComPtr<IShellItem> shellItem;
	currentResult = filePicker->GetResult(&shellItem);

	if(!SUCCEEDED(currentResult))
		return std::string();

	PWSTR filePath;
	currentResult = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

	if(!SUCCEEDED(currentResult))
		return std::string();

	ULONG64 pathLength = std::wcslen(filePath);
	std::unique_ptr<CHAR[]> filePathConverted = std::make_unique<CHAR[]>(pathLength + 1);

	std::wcstombs(filePathConverted.get(), filePath, pathLength);

	std::string result = std::string(filePathConverted.get());

	CoTaskMemFree(filePath);

	return result;
}