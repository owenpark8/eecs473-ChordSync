#pragma once
/**
 * Interface used to send messages from the RPI to the MCU
 * Communication done through UART w a custom message structure
 */
namespace messaging {

    /**
     * Communication structure aka "language" between RPI and MCU
     */
    enum class message_e {
        pause,
        resume,
        rewind,
    };

    /**
     * @brief Send a synchronization command
     * @details used by RPI only
     */
    void write_message(message_e message);

    /**
     * @brief Read a synchronization command
     * @details used by MCU only
     */
    void read_message(message_e message);
};