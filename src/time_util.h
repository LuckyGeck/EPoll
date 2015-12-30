#pragma once

#include <ctime>
#include <string>

namespace NTime {

std::string TimeToString(std::time_t time);
std::time_t GetCurrectTime();

} // namespace NTime
