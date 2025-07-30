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

namespace Utils
{
	/// <summary>
	/// Returns the last Win32 error in string representation.
	/// </summary>
	/// <returns>Self-explanatory.</returns>
	auto GetLastErrorAsString() -> std::string;

	/// <summary>
	/// Converts a wide string into a narrow C++ string.
	/// </summary>
	/// <param name="string">A pointer to the string to convert.</param>
	/// <returns>A conversion to UTF-8 of the input string.</returns>
	auto ConvertWideStringToNarrow(const wchar_t* string) -> std::string;
}
