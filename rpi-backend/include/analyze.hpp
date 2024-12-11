#pragma once

#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <data.hpp>

namespace py = pybind11;

class ChordAnalyzer {
public:
    ChordAnalyzer() : m_running(true), m_worker_thread(&ChordAnalyzer::worker_thread_function, this) {}

    ~ChordAnalyzer() {
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_running = false;
            m_condition.notify_all();
        }
        if (m_worker_thread.joinable()) {
            m_worker_thread.join();
        }
    }

    [[nodiscard]] auto record_chord(data::songs::SongInfo reference_chord) -> std::future<bool> {
        std::promise<bool> promise;
        auto future = promise.get_future();

        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_request_queue.emplace(std::move(reference_chord), std::move(promise));
        }
        m_condition.notify_all();

        return future;
    }

private:
    void worker_thread_function() {
        py::scoped_interpreter guard{};

        auto sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, PY_VENV_PATH);
        sys.attr("path").attr("insert")(0, PY_MODULE_PATH);

        auto get_record_convert_module = py::module::import("record_convert");


        while (m_running || !m_request_queue.empty()) {
            std::optional<std::pair<data::songs::SongInfo, std::promise<bool>>> request;

            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_condition.wait(lock, [this] { return !m_request_queue.empty() || !m_running; });

                if (!m_request_queue.empty()) {
                    request = std::move(m_request_queue.front());
                    m_request_queue.pop();
                }
            }

            if (request) {
                auto [reference_chord, promise] = std::move(*request);

                bool result = false;

                try {
                    std::string command = "arecord --duration=" + std::to_string(reference_chord.length.count()) + " --rate=88200 --format=S16_LE " +
                                          std::to_string(reference_chord.id) + "_rec.wav";
                    if (system(command.c_str()) != 0) {
                        std::cerr << "Error executing command: " << command << std::endl;
                        throw std::runtime_error("Could not record song!");
                    }

                    py::object get_prediction = get_record_convert_module.attr("prediction");
                    get_prediction(reference_chord.id, 120);

                    py::object get_record_convert = get_record_convert_module.attr("record_convert_offset");
                    auto numbers = get_record_convert(reference_chord.id).cast<std::vector<std::vector<int>>>();

                    py::object get_remove_files = get_record_convert_module.attr("remove_files");
                    get_remove_files(reference_chord.id);

                    std::vector<data::songs::Note> recorded_chord;
                    for (auto& number: numbers) {
                        data::songs::Note entry{};
                        entry.midi_note = static_cast<uint8_t>(number[0]);
                        entry.start_timestamp_ms = static_cast<uint32_t>(number[1]);
                        entry.length_ms = static_cast<uint16_t>(number[2] - entry.start_timestamp_ms);
                        recorded_chord.push_back(entry);
                    }

                    int match_count = 0;
                    for (auto const& ref_note: reference_chord.notes) {
                        for (auto const& note: recorded_chord) {
                            if (note.midi_note == ref_note.midi_note) {
                                match_count++;
                                break;
                            }
                        }
                    }

                    result = (match_count == reference_chord.notes.size() && match_count > 0);
                } catch (py::error_already_set const& e) {
                    std::cerr << "Python error: " << e.what() << std::endl;
                    throw;
                }

                try {
                    promise.set_value(result);
                } catch (std::exception const& e) {
                    promise.set_exception(std::make_exception_ptr(e));
                }
            }
        }
    }

    std::atomic<bool> m_running;
    std::thread m_worker_thread;
    std::queue<std::pair<data::songs::SongInfo, std::promise<bool>>> m_request_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
};
