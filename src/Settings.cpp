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

#include <filesystem>
#include <unordered_set>
#include <string>

std::unordered_set<std::string> ValidBindings =
{
	BUFFER_A,
	BUFFER_B,
	BUFFER_C,
	BUFFER_D
};

auto ValidateSettings(RenderSettings& settings) -> void;

auto Settings::LoadFromRegistry() -> RenderSettings
{
	RenderSettings settings =
	{
		.MainPath = Registry::ReadString(REGISTRY_SUBKEY, SHADER_PATH),
		.BufferAPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_PATH),
		.BufferBPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_PATH),
		.BufferCPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_PATH),
		.BufferDPath = Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_PATH),
		.CommonPath = Registry::ReadString(REGISTRY_SUBKEY, COMMON_PATH),

		.MainChannel0 = Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL0_BINDING),
		.MainChannel1 = Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL1_BINDING),
		.MainChannel2 = Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL2_BINDING),
		.MainChannel3 = Registry::ReadString(REGISTRY_SUBKEY, SHADER_CHANNEL3_BINDING),

		.BufferAChannel0 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL0_BINDING),
		.BufferAChannel1 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL1_BINDING),
		.BufferAChannel2 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL2_BINDING),
		.BufferAChannel3 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERA_CHANNEL3_BINDING),

		.BufferBChannel0 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL0_BINDING),
		.BufferBChannel1 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL1_BINDING),
		.BufferBChannel2 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL2_BINDING),
		.BufferBChannel3 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERB_CHANNEL3_BINDING),

		.BufferCChannel0 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL0_BINDING),
		.BufferCChannel1 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL1_BINDING),
		.BufferCChannel2 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL2_BINDING),
		.BufferCChannel3 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERC_CHANNEL3_BINDING),

		.BufferDChannel0 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL0_BINDING),
		.BufferDChannel1 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL1_BINDING),
		.BufferDChannel2 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL2_BINDING),
		.BufferDChannel3 = Registry::ReadString(REGISTRY_SUBKEY, BUFFERD_CHANNEL3_BINDING)
	};

	ValidateSettings(settings);

	return settings;
}

auto Settings::SaveToRegistry(RenderSettings& settings) -> bool
{
	ValidateSettings(settings);

	bool result = Registry::SetString(REGISTRY_SUBKEY, SHADER_PATH, settings.MainPath);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_PATH, settings.BufferAPath);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_PATH, settings.BufferBPath);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_PATH, settings.BufferCPath);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_PATH, settings.BufferDPath);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, COMMON_PATH, settings.CommonPath);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL0_BINDING, settings.MainChannel0);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL1_BINDING, settings.MainChannel1);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL2_BINDING, settings.MainChannel2);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, SHADER_CHANNEL3_BINDING, settings.MainChannel3);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL0_BINDING, settings.BufferAChannel0);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL1_BINDING, settings.BufferAChannel1);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL2_BINDING, settings.BufferAChannel2);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERA_CHANNEL3_BINDING, settings.BufferAChannel3);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL0_BINDING, settings.BufferBChannel0);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL1_BINDING, settings.BufferBChannel1);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL2_BINDING, settings.BufferBChannel2);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERB_CHANNEL3_BINDING, settings.BufferBChannel3);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL0_BINDING, settings.BufferCChannel0);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL1_BINDING, settings.BufferCChannel1);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL2_BINDING, settings.BufferCChannel2);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERC_CHANNEL3_BINDING, settings.BufferCChannel3);
	if (!result)
		return false;

	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL0_BINDING, settings.BufferDChannel0);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL1_BINDING, settings.BufferDChannel1);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL2_BINDING, settings.BufferDChannel2);
	if (!result)
		return false;
	result = Registry::SetString(REGISTRY_SUBKEY, BUFFERD_CHANNEL3_BINDING, settings.BufferDChannel3);
	if (!result)
		return false;

	return true;
}

auto ValidateSettings(RenderSettings& settings) -> void
{
	if(!std::filesystem::is_regular_file(settings.MainPath))
		settings.MainPath = std::string();
	if(!std::filesystem::is_regular_file(settings.BufferAPath))
		settings.BufferAPath = std::string();
	if(!std::filesystem::is_regular_file(settings.BufferBPath))
		settings.BufferBPath = std::string();
	if(!std::filesystem::is_regular_file(settings.BufferCPath))
		settings.BufferCPath = std::string();
	if(!std::filesystem::is_regular_file(settings.BufferDPath))
		settings.BufferDPath = std::string();
	if (!std::filesystem::is_regular_file(settings.CommonPath))
		settings.CommonPath = std::string();

	if(!std::filesystem::is_regular_file(settings.MainChannel0) && !ValidBindings.contains(settings.MainChannel0))
		settings.MainChannel0 = std::string();
	if(!std::filesystem::is_regular_file(settings.MainChannel1) && !ValidBindings.contains(settings.MainChannel1))
		settings.MainChannel1 = std::string();
	if(!std::filesystem::is_regular_file(settings.MainChannel2) && !ValidBindings.contains(settings.MainChannel2))
		settings.MainChannel2 = std::string();
	if(!std::filesystem::is_regular_file(settings.MainChannel3) && !ValidBindings.contains(settings.MainChannel3))
		settings.MainChannel3 = std::string();

	if (!std::filesystem::is_regular_file(settings.BufferAChannel0) && !ValidBindings.contains(settings.BufferAChannel0))
		settings.BufferAChannel0 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferAChannel1) && !ValidBindings.contains(settings.BufferAChannel1))
		settings.BufferAChannel1 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferAChannel2) && !ValidBindings.contains(settings.BufferAChannel2))
		settings.BufferAChannel2 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferAChannel3) && !ValidBindings.contains(settings.BufferAChannel3))
		settings.BufferAChannel3 = std::string();

	if (!std::filesystem::is_regular_file(settings.BufferBChannel0) && !ValidBindings.contains(settings.BufferBChannel0))
		settings.BufferBChannel0 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferBChannel1) && !ValidBindings.contains(settings.BufferBChannel1))
		settings.BufferBChannel1 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferBChannel2) && !ValidBindings.contains(settings.BufferBChannel2))
		settings.BufferBChannel2 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferBChannel3) && !ValidBindings.contains(settings.BufferBChannel3))
		settings.BufferBChannel3 = std::string();

	if (!std::filesystem::is_regular_file(settings.BufferCChannel0) && !ValidBindings.contains(settings.BufferCChannel0))
		settings.BufferCChannel0 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferCChannel1) && !ValidBindings.contains(settings.BufferCChannel1))
		settings.BufferCChannel1 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferCChannel2) && !ValidBindings.contains(settings.BufferCChannel2))
		settings.BufferCChannel2 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferCChannel3) && !ValidBindings.contains(settings.BufferCChannel3))
		settings.BufferCChannel3 = std::string();

	if (!std::filesystem::is_regular_file(settings.BufferDChannel0) && !ValidBindings.contains(settings.BufferDChannel0))
		settings.BufferDChannel0 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferDChannel1) && !ValidBindings.contains(settings.BufferDChannel1))
		settings.BufferDChannel1 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferDChannel2) && !ValidBindings.contains(settings.BufferDChannel2))
		settings.BufferDChannel2 = std::string();
	if (!std::filesystem::is_regular_file(settings.BufferDChannel3) && !ValidBindings.contains(settings.BufferDChannel3))
		settings.BufferDChannel3 = std::string();
}