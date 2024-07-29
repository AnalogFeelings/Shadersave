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
#include <unordered_set>

HDC deviceContextHandle;
HGLRC glRenderContextHandle;
RECT clientRect;
std::shared_ptr<GLRenderer> glRenderer;

std::unordered_set<std::string> validBindings =
{
	"BUFFER_A",
	"BUFFER_B",
	"BUFFER_C",
	"BUFFER_D"
};

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
			HRESULT comResult = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if(FAILED(comResult))
			{
				::MessageBox(hDlg, "Error initializing COM.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

				return FALSE;
			}

			SETTINGS settings = LoadSettings();

			::SetDlgItemText(hDlg, IDC_SHADERPATH, settings.MainPath.c_str());
			::SetDlgItemText(hDlg, IDC_SHADERPATHA, settings.BufferAPath.c_str());
			::SetDlgItemText(hDlg, IDC_SHADERPATHB, settings.BufferBPath.c_str());
			::SetDlgItemText(hDlg, IDC_SHADERPATHC, settings.BufferCPath.c_str());
			::SetDlgItemText(hDlg, IDC_SHADERPATHD, settings.BufferDPath.c_str());

			::SetDlgItemText(hDlg, IDC_CHANNEL0, settings.Channel0.c_str());
			::SetDlgItemText(hDlg, IDC_CHANNEL1, settings.Channel1.c_str());
			::SetDlgItemText(hDlg, IDC_CHANNEL2, settings.Channel2.c_str());
			::SetDlgItemText(hDlg, IDC_CHANNEL3, settings.Channel3.c_str());

			::SetDlgItemText(hDlg, IDC_FRAMECAP, std::to_string(settings.FramerateCap).c_str());

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
					::GetDlgItemText(hDlg, IDC_SHADERPATH, shaderPath, MAX_PATH);
					CHAR bufferAPath[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_SHADERPATHA, bufferAPath, MAX_PATH);
					CHAR bufferBPath[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_SHADERPATHB, bufferBPath, MAX_PATH);
					CHAR bufferCPath[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_SHADERPATHC, bufferCPath, MAX_PATH);
					CHAR bufferDPath[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_SHADERPATHA, bufferDPath, MAX_PATH);

					CHAR channel0Binding[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_CHANNEL0, channel0Binding, MAX_PATH);
					CHAR channel1Binding[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_CHANNEL1, channel1Binding, MAX_PATH);
					CHAR channel2Binding[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_CHANNEL2, channel2Binding, MAX_PATH);
					CHAR channel3Binding[MAX_PATH];
					::GetDlgItemText(hDlg, IDC_CHANNEL3, channel3Binding, MAX_PATH);

					UINT frameCap = ::GetDlgItemInt(hDlg, IDC_FRAMECAP, nullptr, FALSE);

					SETTINGS settings =
					{
						.MainPath = std::string(shaderPath),
						.BufferAPath = std::string(bufferAPath),
						.BufferBPath = std::string(bufferBPath),
						.BufferCPath = std::string(bufferCPath),
						.BufferDPath = std::string(bufferDPath),

						.Channel0 = std::string(channel0Binding),
						.Channel1 = std::string(channel1Binding),
						.Channel2 = std::string(channel2Binding),
						.Channel3 = std::string(channel3Binding),

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

					::SetDlgItemText(hDlg, IDC_SHADERPATH, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTA:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_SHADERPATHA, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTB:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_SHADERPATHB, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTC:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_SHADERPATHC, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTD:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_SHADERPATHD, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTCHN0:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_CHANNEL0, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTCHN1:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_CHANNEL1, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTCHN2:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_CHANNEL2, filePath.c_str());

					break;
				}
				case IDC_BROWSEBUTTCHN3:
				{
					std::string filePath = OpenFilePicker(hDlg);

					::SetDlgItemText(hDlg, IDC_CHANNEL3, filePath.c_str());

					break;
				}
			}
			return FALSE;
		}
		case WM_CLOSE:
		{
			::CoUninitialize();
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
	std::string bufferAPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERA_PATH);
	std::string bufferBPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERB_PATH);
	std::string bufferCPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERC_PATH);
	std::string bufferDPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERD_PATH);

	std::string channel0Binding = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL0_BINDING);
	std::string channel1Binding = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL1_BINDING);
	std::string channel2Binding = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL2_BINDING);
	std::string channel3Binding = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL3_BINDING);

	UINT framerateCap = ReadRegistryDword(REGISTRY_SUBKEY, FRAMERATE_CAP);

	SETTINGS settings =
	{
		.MainPath = shaderPath,
		.BufferAPath = bufferAPath,
		.BufferBPath = bufferBPath,
		.BufferCPath = bufferCPath,
		.BufferDPath = bufferDPath,

		.Channel0 = channel0Binding,
		.Channel1 = channel1Binding,
		.Channel2 = channel2Binding,
		.Channel3 = channel3Binding,

		.FramerateCap = framerateCap
	};

	ValidateSettings(&settings);

	return settings;
}

auto SaveSettings(PSETTINGS settings) -> BOOL
{
	ValidateSettings(settings);

	BOOL pathResult = SetRegistryString(REGISTRY_SUBKEY, SHADER_PATH, settings->MainPath);
	if (!pathResult)
		return FALSE;
	BOOL bufferAResult = SetRegistryString(REGISTRY_SUBKEY, BUFFERA_PATH, settings->BufferAPath);
	if (!bufferAResult)
		return FALSE;
	BOOL bufferBResult = SetRegistryString(REGISTRY_SUBKEY, BUFFERB_PATH, settings->BufferBPath);
	if (!bufferBResult)
		return FALSE;
	BOOL bufferCResult = SetRegistryString(REGISTRY_SUBKEY, BUFFERC_PATH, settings->BufferCPath);
	if (!bufferCResult)
		return FALSE;
	BOOL bufferDResult = SetRegistryString(REGISTRY_SUBKEY, BUFFERD_PATH, settings->BufferDPath);
	if (!bufferDResult)
		return FALSE;

	BOOL channel0Result = SetRegistryString(REGISTRY_SUBKEY, CHANNEL0_BINDING, settings->Channel0);
	if (!channel0Result)
		return FALSE;
	BOOL channel1Result = SetRegistryString(REGISTRY_SUBKEY, CHANNEL1_BINDING, settings->Channel1);
	if (!channel1Result)
		return FALSE;
	BOOL channel2Result = SetRegistryString(REGISTRY_SUBKEY, CHANNEL2_BINDING, settings->Channel2);
	if (!channel2Result)
		return FALSE;
	BOOL channel3Result = SetRegistryString(REGISTRY_SUBKEY, CHANNEL3_BINDING, settings->Channel3);
	if (!channel3Result)
		return FALSE;

	BOOL frameResult = SetRegistryDword(REGISTRY_SUBKEY, FRAMERATE_CAP, settings->FramerateCap);
	if (!frameResult)
		return FALSE;

	return TRUE;
}

auto ValidateSettings(PSETTINGS settings) -> VOID
{
	if(!std::filesystem::is_regular_file(settings->MainPath))
		settings->MainPath = std::string();
	if(!std::filesystem::is_regular_file(settings->BufferAPath))
		settings->BufferAPath = std::string();
	if(!std::filesystem::is_regular_file(settings->BufferBPath))
		settings->BufferBPath = std::string();
	if(!std::filesystem::is_regular_file(settings->BufferCPath))
		settings->BufferCPath = std::string();
	if(!std::filesystem::is_regular_file(settings->BufferDPath))
		settings->BufferDPath = std::string();

	if(!std::filesystem::is_regular_file(settings->Channel0) && !validBindings.contains(settings->Channel0))
		settings->Channel0 = std::string();
	if(!std::filesystem::is_regular_file(settings->Channel1) && !validBindings.contains(settings->Channel1))
		settings->Channel1 = std::string();
	if(!std::filesystem::is_regular_file(settings->Channel2) && !validBindings.contains(settings->Channel2))
		settings->Channel2 = std::string();
	if(!std::filesystem::is_regular_file(settings->Channel3) && !validBindings.contains(settings->Channel3))
		settings->Channel3 = std::string();

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

	if(FAILED(currentResult))
		return std::string();

	currentResult = filePicker->Show(owner);

	if(FAILED(currentResult))
		return std::string();

	CComPtr<IShellItem> shellItem;
	currentResult = filePicker->GetResult(&shellItem);

	if(FAILED(currentResult))
		return std::string();

	PWSTR filePath;
	currentResult = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

	if(FAILED(currentResult))
		return std::string();

	ULONG64 pathLength = std::wcslen(filePath);
	std::unique_ptr<CHAR[]> filePathConverted = std::make_unique<CHAR[]>(pathLength + 1);

	std::wcstombs(filePathConverted.get(), filePath, pathLength);

	std::string result = std::string(filePathConverted.get());

	CoTaskMemFree(filePath);

	return result;
}