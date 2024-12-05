#pragma once

#include "hardware.hpp"

namespace amp {
    Pin AMP = Pin(AMP_AMP_GPIO_Port, AMP_AMP_Pin);
    Pin MOD = Pin(AMP_MOD_GPIO_Port, AMP_MOD_Pin);
    Pin DLY = Pin(AMP_DLY_GPIO_Port, AMP_DLY_Pin);
    Pin RVB = Pin(AMP_RVB_GPIO_Port, AMP_RVB_Pin);
    Pin PWR = Pin(AMP_PWR_GPIO_Port, AMP_PWR_Pin);

    auto power_on() -> void {
        MOD.reset();
        DLY.reset();
        RVB.reset();

        PWR.set();
        HAL_Delay(2000);
        PWR.reset();

        AMP.set();
        HAL_Delay(2000);
        AMP.reset();
    }
} // namespace amp
