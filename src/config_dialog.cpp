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

auto OpenFilePicker(HWND owner) -> std::string;
auto GetControlText(HWND hDlg, int control) -> std::string;

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
					std::string shaderPath = GetControlText(hDlg, IDC_SHADERPATH);
					std::string bufferAPath = GetControlText(hDlg, IDC_SHADERPATHA);
					std::string bufferBPath = GetControlText(hDlg, IDC_SHADERPATHB);
					std::string bufferCPath = GetControlText(hDlg, IDC_SHADERPATHC);
					std::string bufferDPath = GetControlText(hDlg, IDC_SHADERPATHD);

					std::string channel0Binding = GetControlText(hDlg, IDC_CHANNEL0);
					std::string channel1Binding = GetControlText(hDlg, IDC_CHANNEL1);
					std::string channel2Binding = GetControlText(hDlg, IDC_CHANNEL2);
					std::string channel3Binding = GetControlText(hDlg, IDC_CHANNEL3);

					UINT frameCap = ::GetDlgItemInt(hDlg, IDC_FRAMECAP, nullptr, FALSE);

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

auto GetControlText(HWND hDlg, int control) -> std::string
{
	CHAR buffer[MAX_PATH];
	::GetDlgItemText(hDlg, control, buffer, MAX_PATH);

	return std::string(buffer);
}