#pragma once

#include <format>
#include <string_view>


namespace Logging {
void Attach();
void Detach();

void Print(std::string_view szText);

template <typename... tArgList>
inline void Print(std::string_view szFormat, tArgList... argList) {
  Print(std::vformat(szFormat, std::make_format_args(argList...)));
}
} // namespace Logging

#define LOG(...) Logging::Print(__VA_ARGS__)