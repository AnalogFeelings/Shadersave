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

#include <Registry.h>
#include <Settings.h>
#include <Defines.h>
#include <Globals.h>

#include <filesystem>
#include <string>

auto ValidateSettings(RenderSettings& settings) -> void;

auto Settings::LoadFromRegistry() -> RenderSettings
{
	RenderSettings settings =
	{
		.CommonPath = Registry::ReadString(REGISTRY_SUBKEY, COMMON_PATH),

		.MainPath = Registry::ReadString(REGISTRY_SUBKEY, SHADER_PATH),
		.MainChannels =
		{
			Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL0_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL1_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL2_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL3_BINDING)
		},

		.BufferAPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_PATH),
		.BufferAChannels =
		{
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL0_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL1_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL2_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL3_BINDING)
		},

		.BufferBPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_PATH),
		.BufferBChannels = 
		{
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL0_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL1_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL2_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL3_BINDING)
		},

		.BufferCPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_PATH),
		.BufferCChannels = 
		{
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL0_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL1_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL2_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL3_BINDING)
		},

		.BufferDPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_PATH),
		.BufferDChannels = 
		{
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL0_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL1_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL2_BINDING),
			Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL3_BINDING)
		}
	};

	ValidateSettings(settings);

	return settings;
}

auto Settings::SaveToRegistry(RenderSettings& settings) -> bool
{
	ValidateSettings(settings);

	bool result = Registry::SetString(REGISTRY_SUBKEY, COMMON_PATH, settings.CommonPath);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_PATH, settings.MainPath);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL0_BINDING, settings.MainChannels[0]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL1_BINDING, settings.MainChannels[1]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL2_BINDING, settings.MainChannels[2]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL3_BINDING, settings.MainChannels[3]);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_PATH, settings.BufferAPath);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL0_BINDING, settings.BufferAChannels[0]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL1_BINDING, settings.BufferAChannels[1]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL2_BINDING, settings.BufferAChannels[2]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL3_BINDING, settings.BufferAChannels[3]);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_PATH, settings.BufferBPath);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL0_BINDING, settings.BufferBChannels[0]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL1_BINDING, settings.BufferBChannels[1]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL2_BINDING, settings.BufferBChannels[2]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL3_BINDING, settings.BufferBChannels[3]);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_PATH, settings.BufferCPath);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL0_BINDING, settings.BufferCChannels[0]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL1_BINDING, settings.BufferCChannels[1]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL2_BINDING, settings.BufferCChannels[2]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL3_BINDING, settings.BufferCChannels[3]);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_PATH, settings.BufferDPath);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL0_BINDING, settings.BufferDChannels[0]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL1_BINDING, settings.BufferDChannels[1]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL2_BINDING, settings.BufferDChannels[2]);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL3_BINDING, settings.BufferDChannels[3]);
	if (!result)
		return false;

	return true;
}

auto ValidateSettings(RenderSettings& settings) -> void
{
	if (!std::filesystem::is_regular_file(settings.CommonPath))
		settings.CommonPath = std::string();

	if (!std::filesystem::is_regular_file(settings.MainPath))
		settings.MainPath = std::string();
	for (std::string& MainChannel : settings.MainChannels)
	{
		if (!std::filesystem::is_regular_file(MainChannel) && !Globals::ValidBindings.contains(MainChannel))
			MainChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferAPath))
		settings.BufferAPath = std::string();
	for (std::string& BufferAChannel : settings.BufferAChannels)
	{
		if (!std::filesystem::is_regular_file(BufferAChannel) && !Globals::ValidBindings.contains(BufferAChannel))
			BufferAChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferBPath))
		settings.BufferBPath = std::string();
	for (std::string& BufferBChannel : settings.BufferBChannels)
	{
		if (!std::filesystem::is_regular_file(BufferBChannel) && !Globals::ValidBindings.contains(BufferBChannel))
			BufferBChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferCPath))
		settings.BufferCPath = std::string();
	for (std::string& BufferCChannel : settings.BufferCChannels)
	{
		if (!std::filesystem::is_regular_file(BufferCChannel) && !Globals::ValidBindings.contains(BufferCChannel))
			BufferCChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferDPath))
		settings.BufferDPath = std::string();
	for (std::string& BufferDChannel : settings.BufferDChannels)
	{
		if (!std::filesystem::is_regular_file(BufferDChannel) && !Globals::ValidBindings.contains(BufferDChannel))
			BufferDChannel = std::string();
	}
}