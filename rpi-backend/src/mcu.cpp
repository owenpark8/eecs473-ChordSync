#include "messaging.hpp"
#include "serial.hpp"

#include "mcu.hpp"

namespace mcu {
    auto send_message(Message const& msg) -> bool {
        std::size_t buf_size = msg.size();
        std::uint8_t buf[buf_size];
        msg.encode(buf);

        return serial::send(buf, buf_size);
    }

    auto update_loaded_song_id() -> bool {
        constexpr Message msg = create_request_song_id_message();
        return send_message(msg);
    }

    auto play_loaded_song() -> bool {
        constexpr Message msg = create_start_loaded_song_message();
        return send_message(msg);
    }

    std::mutex mut{};
    std::uint8_t current_song_id = 0x00;
    song_status_e current_song_status = song_status_e::UNKNOWN;
} // namespace mcu
