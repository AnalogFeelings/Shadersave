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
#include <shobjidl.h>
#include <Settings.h>
#include <Defines.h>
#include <filesystem>
#include <Registry.h>
#include <unordered_set>

auto ValidateSettings(PSETTINGS settings) -> VOID;

std::unordered_set<std::string> validBindings =
{
	BUFFER_A,
	BUFFER_B,
	BUFFER_C,
	BUFFER_D
};

auto Settings::LoadFromRegistry() -> SETTINGS
{
	SETTINGS settings =
	{
		.MainPath = ReadRegistryString(REGISTRY_SUBKEY, SHADER_PATH),
		.BufferAPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERA_PATH),
		.BufferBPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERB_PATH),
		.BufferCPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERC_PATH),
		.BufferDPath = ReadRegistryString(REGISTRY_SUBKEY, BUFFERD_PATH),

		.Channel0 = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL0_BINDING),
		.Channel1 = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL1_BINDING),
		.Channel2 = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL2_BINDING),
		.Channel3 = ReadRegistryString(REGISTRY_SUBKEY, CHANNEL3_BINDING),

		.FramerateCap = ReadRegistryDword(REGISTRY_SUBKEY, FRAMERATE_CAP)
	};

	ValidateSettings(&settings);

	return settings;
}

auto Settings::SaveToRegistry(PSETTINGS settings) -> BOOL
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