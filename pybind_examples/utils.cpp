#include <iostream>
#include <string>
#include <utility>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_split.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"

#include "utils.h"

namespace utils {

struct Airport {
  std::string id;
  absl::TimeZone tz;
};

struct Leg {
  std::string id;
  std::string origin;
  std::string destination;
  absl::CivilSecond departure_datetime_local;
  absl::Time departure_datetime;
  int32_t duration_min;

  std::string debug_string() const {
    std::vector<std::string> v = {
        origin,
        destination,
        absl::FormatCivilTime(departure_datetime_local),
        std::to_string(duration_min)};
    return absl::StrJoin(v, ";");
  }
};

using LegId = std::string;
using AirportId = std::string;

using AirportTable = absl::flat_hash_map<AirportId, Airport>;
using LegTable = absl::flat_hash_map<LegId, Leg>;


void IngestAirports(
    AirportTable* const airport_table,
    const std::vector<pair_of_strings>& airport_timezone_list) {

  for (const auto& airport : airport_timezone_list) {
    const std::string id = airport.first;
    absl::TimeZone tz;
    if (absl::LoadTimeZone(airport.second, &tz)) {
      Airport port{id, tz};
      airport_table->try_emplace(id, port);
    }
  }
  std::cout << "Ingested " << airport_table->size() << " airport_table." << std::endl;
}

void IngestLegs(LegTable* const leg_table, const std::vector<InputLeg>& legs, const AirportTable& airport_table) {
  for (const auto& leg : legs) {
    const std::string id = std::get<0>(leg);
    const std::string origin = id.substr(0, 3);
    const std::string destination = id.substr(3, 6);
    absl::CivilSecond departure_datetime_local;

    std::string departure_datetime_local_str = std::get<1>(leg);
    // Unfortunately the abseil library demands a 'T' character between date and time part of a string
    // to be parser properly, e.g. "2022-06-19T12:25:45".
    departure_datetime_local_str[10] = 'T';
    if (!absl::ParseLenientCivilTime(departure_datetime_local_str, &departure_datetime_local))
      continue;    

    const int32_t duration_min = std::get<2>(leg);
    // Determine origin timezone so we can covert the local departure time to an absolute timestamp.
    const auto airport_iter = airport_table.find(origin);
    if (airport_iter == airport_table.end())
      continue;

    absl::Time departure_datetime = absl::FromCivil(departure_datetime_local, airport_iter->second.tz);

    Leg row{id, origin, destination, departure_datetime_local, departure_datetime, duration_min};
    leg_table->try_emplace(id, row);
    //std::cout << row.debug_string() << std::endl;
  }
  std::cout << "Ingested " << leg_table->size() << " legs." << std::endl;
}

std::vector<int32_t> compute_total_air_duration_min(
    const std::vector<std::string>& itin_list,
    const std::vector<pair_of_strings>& airport_timezone_list,
    const std::vector<InputLeg>& legs) {
  std::vector<int32_t> total_duration_min(itin_list.size());
  AirportTable airport_table;
  LegTable leg_table;

  // First populate tables.
  IngestAirports(&airport_table, airport_timezone_list);
  IngestLegs(&leg_table, legs, airport_table);

  uint32_t counter = 0;
  for (const std::string& itin : itin_list) {
    int32_t total = 0;
    // Splits on semicolon.
    for (const absl::string_view& leg_id : absl::StrSplit(itin, ";")) {
      // And do the leg map lookup.
      const auto leg_iter = leg_table.find(leg_id);
      if (leg_iter != leg_table.end()) {
        total += leg_iter->second.duration_min;
      } else {
        total = -99999;
        break;
      }
    }
    total_duration_min[counter++] = total;
  }
  
  return total_duration_min;
}

std::vector<int32_t> compute_total_duration_min(
    const std::vector<std::string>& itin_list,
    const std::vector<pair_of_strings>& airport_timezone_list,
    const std::vector<InputLeg>& legs) {
  std::vector<int32_t> total_duration_min(itin_list.size());
  AirportTable airport_table;
  LegTable leg_table;

  // First populate tables.
  IngestAirports(&airport_table, airport_timezone_list);
  IngestLegs(&leg_table, legs, airport_table);

  uint32_t counter = 0;
  for (const std::string& itin : itin_list) {
    // Splits on semicolon.
    const std::vector<absl::string_view> legs = absl::StrSplit(itin, ";");
    if (legs.size() == 0) {
      total_duration_min[counter++] = -99999;
      continue;
    }

    // Determine origin.
    const auto origin_iter = leg_table.find(legs.front());
    if (origin_iter == leg_table.end()) {
      total_duration_min[counter++] = -99999;
      continue;
    }

    // Determine destination.
    const auto destination_iter = leg_table.find(legs.back());
    if (destination_iter == leg_table.end()) {
      total_duration_min[counter++] = -99999;
      continue;
    }

    const absl::Duration total_duration = (
      destination_iter->second.departure_datetime -
      origin_iter->second.departure_datetime);
      //+ absl::Minutes(destination_iter->second.duration_min));

    total_duration_min[counter++] = total_duration / absl::Minutes(1);
  }
  
  return total_duration_min;
}

} // namespace utils
