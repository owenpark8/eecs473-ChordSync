#pragma once

#include <cstdint>

#include "main.h"

class Pin {
public:
    Pin() = default;

    Pin(GPIO_TypeDef* port, std::uint16_t pin) : m_port{port}, m_pin{pin} {}

    [[nodiscard]] auto read() const -> GPIO_PinState { return HAL_GPIO_ReadPin(m_port, m_pin); }

    auto write(GPIO_PinState val) const -> void { HAL_GPIO_WritePin(m_port, m_pin, val); }

private:
    GPIO_TypeDef* m_port{};
    std::uint16_t m_pin{};
};

class SPI {
public:
    void spi_write(uint8_t const* data, uint16_t size);

    void spi_read(uint8_t* data, uint16_t size);

private:
    SPI_TypeDef* m_hpi{};
    Pin m_csx{};
};
