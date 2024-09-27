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

#include <Resources.h>
#include <Defines.h>
#include <Settings.h>
#include <Globals.h>

#include <windows.h>
#include <scrnsave.h>
#include <filesystem>
#include <shobjidl.h>
#include <atlbase.h>
#include <unordered_map>

/// <summary>
/// A map of which browse button maps to which text box.
/// </summary>
std::unordered_map<INT, INT> BrowseButtonMap =
{
	{ IDC_MAINSHADERBROWSE, IDC_MAINSHADERPATH },
	{ IDC_MAINCHANN0BROWSE, IDC_MAINCHANNEL0 },
	{ IDC_MAINCHANN1BROWSE, IDC_MAINCHANNEL1 },
	{ IDC_MAINCHANN2BROWSE, IDC_MAINCHANNEL2 },
	{ IDC_MAINCHANN3BROWSE, IDC_MAINCHANNEL3 },

	{ IDC_BUFFERASHADERBROWSE, IDC_BUFFERASHADERPATH },
	{ IDC_BUFFERACHANN0BROWSE, IDC_BUFFERACHANNEL0 },
	{ IDC_BUFFERACHANN1BROWSE, IDC_BUFFERACHANNEL1 },
	{ IDC_BUFFERACHANN2BROWSE, IDC_BUFFERACHANNEL2 },
	{ IDC_BUFFERACHANN3BROWSE, IDC_BUFFERACHANNEL3 },

	{ IDC_BUFFERBSHADERBROWSE, IDC_BUFFERBSHADERPATH },
	{ IDC_BUFFERBCHANN0BROWSE, IDC_BUFFERBCHANNEL0 },
	{ IDC_BUFFERBCHANN1BROWSE, IDC_BUFFERBCHANNEL1 },
	{ IDC_BUFFERBCHANN2BROWSE, IDC_BUFFERBCHANNEL2 },
	{ IDC_BUFFERBCHANN3BROWSE, IDC_BUFFERBCHANNEL3 },

	{ IDC_BUFFERCSHADERBROWSE, IDC_BUFFERCSHADERPATH },
	{ IDC_BUFFERCCHANN0BROWSE, IDC_BUFFERCCHANNEL0 },
	{ IDC_BUFFERCCHANN1BROWSE, IDC_BUFFERCCHANNEL1 },
	{ IDC_BUFFERCCHANN2BROWSE, IDC_BUFFERCCHANNEL2 },
	{ IDC_BUFFERCCHANN3BROWSE, IDC_BUFFERCCHANNEL3 },

	{ IDC_BUFFERDSHADERBROWSE, IDC_BUFFERDSHADERPATH },
	{ IDC_BUFFERDCHANN0BROWSE, IDC_BUFFERDCHANNEL0 },
	{ IDC_BUFFERDCHANN1BROWSE, IDC_BUFFERDCHANNEL1 },
	{ IDC_BUFFERDCHANN2BROWSE, IDC_BUFFERDCHANNEL2 },
	{ IDC_BUFFERDCHANN3BROWSE, IDC_BUFFERDCHANNEL3 },

	{ IDC_COMMONSHADERBROWSE, IDC_COMMONSHADERPATH },
};

auto OpenFilePicker(HWND owner) -> std::string;
auto GetControlText(HWND hDlg, INT control) -> std::string;

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

			SETTINGS settings = Settings::LoadFromRegistry();

			::SetDlgItemText(hDlg, IDC_MAINSHADERPATH, settings.MainPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERASHADERPATH, settings.BufferAPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBSHADERPATH, settings.BufferBPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCSHADERPATH, settings.BufferCPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDSHADERPATH, settings.BufferDPath.c_str());
			::SetDlgItemText(hDlg, IDC_COMMONSHADERPATH, settings.CommonPath.c_str());

			::SetDlgItemText(hDlg, IDC_MAINCHANNEL0, settings.MainChannel0.c_str());
			::SetDlgItemText(hDlg, IDC_MAINCHANNEL1, settings.MainChannel1.c_str());
			::SetDlgItemText(hDlg, IDC_MAINCHANNEL2, settings.MainChannel2.c_str());
			::SetDlgItemText(hDlg, IDC_MAINCHANNEL3, settings.MainChannel3.c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL0, settings.BufferAChannel0.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL1, settings.BufferAChannel1.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL2, settings.BufferAChannel2.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL3, settings.BufferAChannel3.c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL0, settings.BufferBChannel0.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL1, settings.BufferBChannel1.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL2, settings.BufferBChannel2.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL3, settings.BufferBChannel3.c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL0, settings.BufferCChannel0.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL1, settings.BufferCChannel1.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL2, settings.BufferCChannel2.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL3, settings.BufferCChannel3.c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL0, settings.BufferDChannel0.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL1, settings.BufferDChannel1.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL2, settings.BufferDChannel2.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL3, settings.BufferDChannel3.c_str());

			return TRUE;
		}
		case WM_COMMAND:
		{
			USHORT messageType = HIWORD(wParam);
			USHORT senderControl = LOWORD(wParam);

			switch (senderControl)
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
					SETTINGS settings =
					{
						.MainPath = ::GetControlText(hDlg, IDC_MAINSHADERPATH),
						.BufferAPath = ::GetControlText(hDlg, IDC_BUFFERASHADERPATH),
						.BufferBPath = ::GetControlText(hDlg, IDC_BUFFERBSHADERPATH),
						.BufferCPath = ::GetControlText(hDlg, IDC_BUFFERCSHADERPATH),
						.BufferDPath = ::GetControlText(hDlg, IDC_BUFFERDSHADERPATH),
						.CommonPath = ::GetControlText(hDlg, IDC_COMMONSHADERPATH),

						.MainChannel0 = ::GetControlText(hDlg, IDC_MAINCHANNEL0),
						.MainChannel1 = ::GetControlText(hDlg, IDC_MAINCHANNEL1),
						.MainChannel2 = ::GetControlText(hDlg, IDC_MAINCHANNEL2),
						.MainChannel3 = ::GetControlText(hDlg, IDC_MAINCHANNEL3),

						.BufferAChannel0 = ::GetControlText(hDlg, IDC_BUFFERACHANNEL0),
						.BufferAChannel1 = ::GetControlText(hDlg, IDC_BUFFERACHANNEL1),
						.BufferAChannel2 = ::GetControlText(hDlg, IDC_BUFFERACHANNEL2),
						.BufferAChannel3 = ::GetControlText(hDlg, IDC_BUFFERACHANNEL3),

						.BufferBChannel0 = ::GetControlText(hDlg, IDC_BUFFERBCHANNEL0),
						.BufferBChannel1 = ::GetControlText(hDlg, IDC_BUFFERBCHANNEL1),
						.BufferBChannel2 = ::GetControlText(hDlg, IDC_BUFFERBCHANNEL2),
						.BufferBChannel3 = ::GetControlText(hDlg, IDC_BUFFERBCHANNEL3),

						.BufferCChannel0 = ::GetControlText(hDlg, IDC_BUFFERCCHANNEL0),
						.BufferCChannel1 = ::GetControlText(hDlg, IDC_BUFFERCCHANNEL1),
						.BufferCChannel2 = ::GetControlText(hDlg, IDC_BUFFERCCHANNEL2),
						.BufferCChannel3 = ::GetControlText(hDlg, IDC_BUFFERCCHANNEL3),

						.BufferDChannel0 = ::GetControlText(hDlg, IDC_BUFFERDCHANNEL0),
						.BufferDChannel1 = ::GetControlText(hDlg, IDC_BUFFERDCHANNEL1),
						.BufferDChannel2 = ::GetControlText(hDlg, IDC_BUFFERDCHANNEL2),
						.BufferDChannel3 = ::GetControlText(hDlg, IDC_BUFFERDCHANNEL3),
					};

					BOOL saveResult = Settings::SaveToRegistry(&settings);
					if (!saveResult)
					{
						std::string error = "Could not save settings!\n" + Globals::LastError;
						::MessageBox(hDlg, error.c_str(), "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);

						::EndDialog(hDlg, -1);
						break;
					}

					::EndDialog(hDlg, 0);
					break;
				}
				default:
				{
					if (!BrowseButtonMap.contains(senderControl))
						break;
					
					std::string filePath = OpenFilePicker(hDlg);
					INT correspondingTextBox = BrowseButtonMap.at(senderControl);

					::SetDlgItemText(hDlg, correspondingTextBox, filePath.c_str());

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

auto GetControlText(HWND hDlg, INT control) -> std::string
{
	CHAR buffer[MAX_PATH];
	::GetDlgItemText(hDlg, control, buffer, MAX_PATH);

	return std::string(buffer);
}