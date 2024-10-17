#pragma once

#include <cstdint>

#include "main.h"

class Pin {
public:
    Pin() = default;

    Pin(GPIO_TypeDef* port, std::uint16_t pin) : m_port{port}, m_pin{pin} {}

    [[nodiscard]] auto read() const -> GPIO_PinState { return HAL_GPIO_ReadPin(m_port, m_pin); }

    auto write(GPIO_PinState val) const -> void { HAL_GPIO_WritePin(m_port, m_pin, val); }

    auto set() const -> void { HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_SET); }
    auto reset() const -> void { HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_RESET); }

private:
    GPIO_TypeDef* m_port{};
    std::uint16_t m_pin{};
};

class SPI {
    static constexpr uint32_t m_timeout = HAL_MAX_DELAY;

public:
    SPI() = default;

    SPI(SPI_HandleTypeDef* hspi, Pin csx) : m_hspi(hspi), m_csx(csx) {
        m_csx.reset();
    }

    auto spi_write(uint8_t const* data, uint16_t const size) const -> void {
        m_csx.set();
        HAL_SPI_Transmit(m_hspi, data, size, m_timeout);
        m_csx.reset();
    }

    auto spi_write_long(uint8_t const* data, std::size_t size) const -> void {
        m_csx.set();
        while (size > 0) {
            const uint16_t chunk_size = size > 32768 ? 32768 : size;
            HAL_SPI_Transmit(m_hspi, data, chunk_size, m_timeout);
            data += chunk_size;
            size -= chunk_size;
        }
        m_csx.reset();
    }

    auto spi_read(uint8_t* data, uint16_t const size) const -> void {
        HAL_SPI_Receive(m_hspi, data, size, m_timeout);
    }

private:
    SPI_HandleTypeDef* m_hspi{};
    Pin m_csx{};
};
