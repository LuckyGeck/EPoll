#include "time_util.h"


namespace NTime {

std::string TimeToString(std::time_t time) {
    std::tm* ptm = std::localtime(&time);
    std::string result(32, ' ');
    // YYYY-MM-DD HH:MM:SS
    std::strftime(&result[0], result.size(), "%Y-%m-%d %H:%M:%S", ptm);
    return result;
}

std::time_t GetCurrectTime() {
    return std::time(NULL);
}

} // namespace NTime
