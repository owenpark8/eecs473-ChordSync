#pragma once

#include "hardware.hpp"

class Amplifier {
public:
	Amplifier() = default;

    Amplifier(Pin amp, Pin mod, Pin dly, Pin rvb, Pin pwr)
        : m_amp(amp), m_mod(mod), m_dly(dly), m_rvb(rvb), m_pwr(pwr) {}

    void power_on() {
        m_mod.reset();
        m_dly.reset();
        m_rvb.reset();

        m_pwr.set();
        HAL_Delay(2000);
        m_pwr.reset();

        m_amp.set();
        HAL_Delay(2000);
        m_amp.reset();
    }

private:
    Pin m_amp;
    Pin m_mod;
    Pin m_dly;
    Pin m_rvb;
    Pin m_pwr;
};
