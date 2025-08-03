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
		.FramerateCap = Registry::ReadDword(REGISTRY_SUBKEY, FRAMERATE_CAP),

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

	RET_IF_COND_FALSE(Registry::SetDword(REGISTRY_SUBKEY, FRAMERATE_CAP, settings.FramerateCap));

	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, COMMON_PATH, settings.CommonPath));

	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, SHADER_PATH, settings.MainPath));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL0_BINDING, settings.MainChannels[0]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL1_BINDING, settings.MainChannels[1]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL2_BINDING, settings.MainChannels[2]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL3_BINDING, settings.MainChannels[3]));

	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERA_PATH, settings.BufferAPath));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL0_BINDING, settings.BufferAChannels[0]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL1_BINDING, settings.BufferAChannels[1]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL2_BINDING, settings.BufferAChannels[2]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL3_BINDING, settings.BufferAChannels[3]));

	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERB_PATH, settings.BufferBPath));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL0_BINDING, settings.BufferBChannels[0]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL1_BINDING, settings.BufferBChannels[1]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL2_BINDING, settings.BufferBChannels[2]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL3_BINDING, settings.BufferBChannels[3]));

	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERC_PATH, settings.BufferCPath));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL0_BINDING, settings.BufferCChannels[0]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL1_BINDING, settings.BufferCChannels[1]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL2_BINDING, settings.BufferCChannels[2]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL3_BINDING, settings.BufferCChannels[3]));

	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERD_PATH, settings.BufferDPath));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL0_BINDING, settings.BufferDChannels[0]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL1_BINDING, settings.BufferDChannels[1]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL2_BINDING, settings.BufferDChannels[2]));
	RET_IF_COND_FALSE(Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL3_BINDING, settings.BufferDChannels[3]));

	return true;
}

auto ValidateSettings(RenderSettings& settings) -> void
{
	if (settings.FramerateCap < 0 || settings.FramerateCap > 500)
		settings.FramerateCap = 60;

	if (!std::filesystem::is_regular_file(settings.CommonPath))
		settings.CommonPath = std::string();

	if (!std::filesystem::is_regular_file(settings.MainPath))
		settings.MainPath = std::string();
	for (std::string& mainChannel : settings.MainChannels)
	{
		if (!std::filesystem::is_regular_file(mainChannel) && !Globals::ValidBindings.contains(mainChannel))
			mainChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferAPath))
		settings.BufferAPath = std::string();
	for (std::string& bufferAChannel : settings.BufferAChannels)
	{
		if (!std::filesystem::is_regular_file(bufferAChannel) && !Globals::ValidBindings.contains(bufferAChannel))
			bufferAChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferBPath))
		settings.BufferBPath = std::string();
	for (std::string& bufferBChannel : settings.BufferBChannels)
	{
		if (!std::filesystem::is_regular_file(bufferBChannel) && !Globals::ValidBindings.contains(bufferBChannel))
			bufferBChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferCPath))
		settings.BufferCPath = std::string();
	for (std::string& bufferCChannel : settings.BufferCChannels)
	{
		if (!std::filesystem::is_regular_file(bufferCChannel) && !Globals::ValidBindings.contains(bufferCChannel))
			bufferCChannel = std::string();
	}

	if (!std::filesystem::is_regular_file(settings.BufferDPath))
		settings.BufferDPath = std::string();
	for (std::string& bufferDChannel : settings.BufferDChannels)
	{
		if (!std::filesystem::is_regular_file(bufferDChannel) && !Globals::ValidBindings.contains(bufferDChannel))
			bufferDChannel = std::string();
	}
}