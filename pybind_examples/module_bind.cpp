#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "pybind_examples/utils.h"


namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<std::string>);

PYBIND11_MODULE(myutils, m) {
    m.doc() = "myfoo module"; // optional module docstring

    py::bind_vector<std::vector<std::string>>(m, "StringVector");
    m.def("compute_total_air_duration_min", &utils::compute_total_air_duration_min,
      "Compute total air duration for an itinerary",
      py::arg("itin_list"),
      py::arg("airport_timezone_list"),
      py::arg("legs"), py::return_value_policy::take_ownership);

    m.def("compute_total_duration_min", &utils::compute_total_duration_min,
      "Compute total duration for an itinerary",
      py::arg("itin_list"),
      py::arg("airport_timezone_list"),
      py::arg("legs"), py::return_value_policy::take_ownership);
}
