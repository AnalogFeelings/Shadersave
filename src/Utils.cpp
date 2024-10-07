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

#include <Utils.h>

#include <Windows.h>
#include <memory>

auto Utils::GetLastErrorAsString() -> std::string
{
    long errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string();

    char* messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);

    std::string message(messageBuffer, size);

    // FormatMessage uses LocalAlloc so use LocalFree.
    LocalFree(messageBuffer);

    return message;
}

auto Utils::ConvertWideStringToNarrow(const wchar_t* string) -> std::string
{
    unsigned long length = std::wcslen(string);
    std::unique_ptr<CHAR[]> stringConverted = std::make_unique<CHAR[]>(length + 1);

    std::wcstombs(stringConverted.get(), string, length);

    return std::string(stringConverted.get());
}