#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <playerMode.hpp>
//#include <SQLiteCpp/SQLiteCpp.h>
//#include <ctre.hpp>
//#include <fmt/format.h>
//#include <httplib.h>

#include <iostream>
#include <vector>

namespace py = pybind11;


auto main() -> int {
    playerMode* player = new playerMode(1, 1, "C4", "1", "1", 15, 95);
    bool outcome = player->analysis("C4");

    if (outcome == false)
        std::cout << "Wrong!" << std::endl;
    else
        std::cout << "Correct!" << std::endl;
    /*py::scoped_interpreter guard{};

    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(0, PY_VENV_PATH);
    sys.attr("path").attr("insert")(0, PY_MODULE_PATH);

    try {
        py::module pybind_module = py::module::import("basic_pitch");
        std::cout << "Module imported successfully!" << std::endl;
    } catch (py::error_already_set const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    auto get_record_convert_module = py::module_::import("record_convert");
    py::object get_record_convert = get_record_convert_module.attr("record_convert");

    std::vector<std::vector<int>> numbers = get_record_convert("-r", 1, 30, 95).cast<std::vector<std::vector<int>>>();

    for (std::size_t i = 0; i < numbers.size(); i++) {
        std::cout << "Note: " << numbers[i][0] << std::endl;       // First element of the i-th vector
        std::cout << "Start Time: " << numbers[i][1] << std::endl; // Second element
        std::cout << "End Time: " << numbers[i][2] << std::endl;   // Third element
    }

    std::cout << std::endl;*/
    return 0;
}
