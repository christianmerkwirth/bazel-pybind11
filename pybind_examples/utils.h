#include <cstdint>
#include <string>
#include <vector>


namespace utils {

using pair_of_strings = std::pair<std::string, std::string>;
// Format: leg_id, departure_datetime_local, duration_min
using InputLeg = std::tuple<std::string, std::string, int32_t>; 

std::vector<int32_t> compute_total_air_duration_min(
    const std::vector<std::string>& itin_list,
    const std::vector<pair_of_strings>& airport_timezone_list,
    const std::vector<InputLeg>& legs);

std::vector<int32_t> compute_total_duration_min(
    const std::vector<std::string>& itin_list,
    const std::vector<pair_of_strings>& airport_timezone_list,
    const std::vector<InputLeg>& legs);

} // namespace utils
