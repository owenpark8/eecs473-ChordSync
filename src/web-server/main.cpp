#include <cstddef>
#include <thread>

#include "web.hpp"

auto main(int argc, char* args[]) -> int {
    std::thread t(web::server);

    t.join();
}
