#pragma once

#include <condition_variable>
#include <mutex>
#include <string>
#include <unordered_set>

#include <httplib.h>

#include "web_sources.hpp"

namespace web {
    constexpr char const png_type[] = "image/png";
    constexpr char const jpeg_type[] = "image/jpeg";
    constexpr char const html_type[] = "text/html";
    constexpr char const css_type[] = "text/css";
    constexpr char const js_type[] = "application/javascript";

    enum class source_files_e {
#define MAKE_SOURCE_FILES_ENUM(enum_name, ...) enum_name,
        SOURCE_FILES_ITER(MAKE_SOURCE_FILES_ENUM)
#undef MAKE_SOURCE_FILES_ENUM
    };

    extern std::unordered_set<std::string> const allowed_image_upload_types;

#ifdef DEBUG
    extern std::string const web_source_directory;
    auto read_file_into_string(std::string const& filename) -> std::string;
    auto get_source_file(std::string const& filename) -> std::string;
#endif

    constexpr auto get_source_file(source_files_e file) -> char const* {
#define MAKE_SOURCE_FILES_CASES(enum_name, content, ...)                                                                                             \
    case source_files_e::enum_name:                                                                                                                  \
        return content;

        switch (file) {
            SOURCE_FILES_ITER(MAKE_SOURCE_FILES_CASES)

            default:
                return nullptr;
        }
#undef MAKE_SOURCE_FILES_CASES
    }
} // namespace web


class WaitUntilThread {
public:
    WaitUntilThread() : m_stop_flag(false), m_worker_thread() {}

    void start_thread(std::chrono::seconds duration, std::function<void()> const& final_job) {
        m_stop_flag = false;
        m_final_job = final_job;
        m_worker_thread = std::thread(&WaitUntilThread::worker_function, this, duration);
    }

    void stop_thread() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop_flag = true;
        }
        m_cv.notify_one();
        if (m_worker_thread.joinable()) {
            m_worker_thread.join();
        }
    }

    ~WaitUntilThread() {
        if (m_worker_thread.joinable()) {
            stop_thread();
        }
    }

private:
    std::atomic<bool> m_stop_flag;
    std::thread m_worker_thread;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::optional<std::function<void()>> m_final_job;

    void worker_function(std::chrono::seconds duration) {
        std::unique_lock<std::mutex> lock(m_mutex);

        auto deadline = std::chrono::steady_clock::now() + duration;
        while (!m_stop_flag && std::chrono::steady_clock::now() < deadline) {
            m_cv.wait_until(lock, deadline, [this, deadline] { return m_stop_flag || std::chrono::steady_clock::now() >= deadline; });
        }

        if (!m_stop_flag && m_final_job.has_value()) {
            m_final_job.value()();
        }
        m_stop_flag = true;
    }
};

