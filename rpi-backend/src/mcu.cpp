#include "messaging.hpp"
#include "serial.hpp"

#include "mcu.hpp"

namespace mcu {
    auto update_loaded_song_id() -> bool {
        constexpr Message msg = create_request_song_id_message();
        std::size_t buf_size = msg.size();
        std::uint8_t buf[buf_size];
        msg.encode(buf);

        serial::send(buf, buf_size);

        return true;
    }

    auto get_current_song_id() -> song_id_t {
        return current_song_id;
    }

    std::mutex song_mut{};
    std::uint8_t current_song_id = 0x00;
    song_status_e current_song_status = song_status_e::UNKNOWN;
} // namespace mcu
