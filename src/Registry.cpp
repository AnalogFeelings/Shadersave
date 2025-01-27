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

#include <Utils.h>
#include <Globals.h>
#include <Registry.h>

auto Registry::ReadString(const std::string& subKey, const std::string& item) -> std::string
{
    char data[MAX_PATH];
    unsigned long bufferSize = MAX_PATH;

    LSTATUS result = ::RegGetValue(HKEY_CURRENT_USER, subKey.c_str(), item.c_str(), RRF_RT_REG_SZ, nullptr, data, &bufferSize);
    if (result != ERROR_SUCCESS)
        return std::string();

    return std::string(data);
}

auto Registry::ReadDword(const std::string& subKey, const std::string& item) -> UINT
{
    DWORD data;
    DWORD bufferSize = sizeof(DWORD);

    LSTATUS result = ::RegGetValue(HKEY_CURRENT_USER, subKey.c_str(), item.c_str(), RRF_RT_DWORD, nullptr, &data, &bufferSize);
    if (result != ERROR_SUCCESS)
        return INFINITE;

    return data;
}

auto Registry::SetString(const std::string& subKey, const std::string& item, const std::string& value) -> bool
{
    LSTATUS result = ::RegSetKeyValue(HKEY_CURRENT_USER, subKey.c_str(), item.c_str(), REG_SZ, value.c_str(), value.length());
    if (result != ERROR_SUCCESS)
    {
        Globals::LastError = Utils::GetLastErrorAsString();

        return false;
    }

    return true;
}

auto Registry::SetDword(const std::string& subKey, const std::string& item, UINT value) -> bool
{
    LSTATUS result = ::RegSetKeyValue(HKEY_CURRENT_USER, subKey.c_str(), item.c_str(), REG_DWORD, &value, sizeof(UINT));
    if (result != ERROR_SUCCESS)
    {
        Globals::LastError = Utils::GetLastErrorAsString();

        return false;
    }

    return true;
}