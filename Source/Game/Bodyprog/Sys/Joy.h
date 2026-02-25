#pragma once

namespace Silent::Game
{
    struct s_ControllerData;

    constexpr q0_7 STICK_THRESHOLD = FP_STICK(0.5f);

    void Joy_Init(void);

    void Joy_ReadP1(void);

    void Joy_Update(void);

    void Joy_ControllerDataUpdate(void);

    void ControllerData_AnalogToDigital(s_ControllerData* cont, bool arg1);
}
