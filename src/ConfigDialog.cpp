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

#include <Resources.h>
#include <Defines.h>
#include <Settings.h>
#include <Globals.h>
#include <Utils.h>

#include <windows.h>
#include <scrnsave.h>
#include <filesystem>
#include <shobjidl.h>
#include <atlbase.h>
#include <unordered_map>
#include <stdint.h>

/// <summary>
/// A map of which browse button maps to which text box.
/// </summary>
std::unordered_map<int32_t, int32_t> BrowseButtonMap =
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
auto GetControlText(HWND hDlg, int32_t control) -> std::string;

auto WINAPI ScreenSaverConfigureDialog(HWND hDlg, uint32_t message, WPARAM wParam, LPARAM lParam) -> BOOL
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

			RenderSettings settings = Settings::LoadFromRegistry();

			::SetDlgItemText(hDlg, IDC_FRAMERATECAP, std::to_string(settings.FramerateCap).c_str());

			::SetDlgItemText(hDlg, IDC_MAINSHADERPATH, settings.MainPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERASHADERPATH, settings.BufferAPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBSHADERPATH, settings.BufferBPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCSHADERPATH, settings.BufferCPath.c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDSHADERPATH, settings.BufferDPath.c_str());
			::SetDlgItemText(hDlg, IDC_COMMONSHADERPATH, settings.CommonPath.c_str());

			::SetDlgItemText(hDlg, IDC_MAINCHANNEL0, settings.MainChannels[0].c_str());
			::SetDlgItemText(hDlg, IDC_MAINCHANNEL1, settings.MainChannels[1].c_str());
			::SetDlgItemText(hDlg, IDC_MAINCHANNEL2, settings.MainChannels[2].c_str());
			::SetDlgItemText(hDlg, IDC_MAINCHANNEL3, settings.MainChannels[3].c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL0, settings.BufferAChannels[0].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL1, settings.BufferAChannels[1].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL2, settings.BufferAChannels[2].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERACHANNEL3, settings.BufferAChannels[3].c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL0, settings.BufferBChannels[0].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL1, settings.BufferBChannels[1].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL2, settings.BufferBChannels[2].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERBCHANNEL3, settings.BufferBChannels[3].c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL0, settings.BufferCChannels[0].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL1, settings.BufferCChannels[1].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL2, settings.BufferCChannels[2].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERCCHANNEL3, settings.BufferCChannels[3].c_str());

			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL0, settings.BufferDChannels[0].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL1, settings.BufferDChannels[1].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL2, settings.BufferDChannels[2].c_str());
			::SetDlgItemText(hDlg, IDC_BUFFERDCHANNEL3, settings.BufferDChannels[3].c_str());

			return true;
		}
		case WM_COMMAND:
		{
			uint16_t senderControl = LOWORD(wParam);

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
					RenderSettings settings =
					{
						.FramerateCap = std::stoul(::GetControlText(hDlg, IDC_FRAMERATECAP)),

						.CommonPath = ::GetControlText(hDlg, IDC_COMMONSHADERPATH),

						.MainPath = ::GetControlText(hDlg, IDC_MAINSHADERPATH),
						.MainChannels = 
						{
							::GetControlText(hDlg, IDC_MAINCHANNEL0),
							::GetControlText(hDlg, IDC_MAINCHANNEL1),
							::GetControlText(hDlg, IDC_MAINCHANNEL2),
							::GetControlText(hDlg, IDC_MAINCHANNEL3)
						},

						.BufferAPath = ::GetControlText(hDlg, IDC_BUFFERASHADERPATH),
						.BufferAChannels = 
						{
							::GetControlText(hDlg, IDC_BUFFERACHANNEL0),
							::GetControlText(hDlg, IDC_BUFFERACHANNEL1),
							::GetControlText(hDlg, IDC_BUFFERACHANNEL2),
							::GetControlText(hDlg, IDC_BUFFERACHANNEL3)
						},

						.BufferBPath = ::GetControlText(hDlg, IDC_BUFFERBSHADERPATH),
						.BufferBChannels =
						{
							::GetControlText(hDlg, IDC_BUFFERBCHANNEL0),
							::GetControlText(hDlg, IDC_BUFFERBCHANNEL1),
							::GetControlText(hDlg, IDC_BUFFERBCHANNEL2),
							::GetControlText(hDlg, IDC_BUFFERBCHANNEL3)
						},

						.BufferCPath = ::GetControlText(hDlg, IDC_BUFFERCSHADERPATH),
						.BufferCChannels = 
						{
							::GetControlText(hDlg, IDC_BUFFERCCHANNEL0),
							::GetControlText(hDlg, IDC_BUFFERCCHANNEL1),
							::GetControlText(hDlg, IDC_BUFFERCCHANNEL2),
							::GetControlText(hDlg, IDC_BUFFERCCHANNEL3)
						},

						.BufferDPath = ::GetControlText(hDlg, IDC_BUFFERDSHADERPATH),
						.BufferDChannels = 
						{
							::GetControlText(hDlg, IDC_BUFFERDCHANNEL0),
							::GetControlText(hDlg, IDC_BUFFERDCHANNEL1),
							::GetControlText(hDlg, IDC_BUFFERDCHANNEL2),
							::GetControlText(hDlg, IDC_BUFFERDCHANNEL3)
						}
					};

					bool saveResult = Settings::SaveToRegistry(settings);
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
					int32_t correspondingTextBox = BrowseButtonMap.at(senderControl);

					::SetDlgItemText(hDlg, correspondingTextBox, filePath.c_str());

					break;
				}
			}
			return false;
		}
		case WM_CLOSE:
		{
			::CoUninitialize();
			::EndDialog(hDlg, 0);

			break;
		}
		default:
			return false;
	}
	return true;
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

	std::string result = Utils::ConvertWideStringToNarrow(filePath);

	CoTaskMemFree(filePath);

	return result;
}

auto GetControlText(HWND hDlg, int32_t control) -> std::string
{
	char buffer[MAX_PATH];
	::GetDlgItemText(hDlg, control, buffer, MAX_PATH);

	return std::string(buffer);
}