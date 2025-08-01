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

#pragma once

#include <string>
#include <stdint.h>

namespace Registry
{
	auto ReadString(const std::string& subKey, const std::string& item) -> std::string;
	auto SetString(const std::string& subKey, const std::string& item, const std::string& value) -> bool;

	auto ReadDword(const std::string& subKey, const std::string& item) -> uint32_t;
	auto SetDword(const std::string& subKey, const std::string& item, uint32_t value) -> bool;
}