/*-----------------------------------------*\
|  RGBController_HyperX.h                   |
|                                           |
|  Generic RGB Interface for OpenAuraSDK    |
|  HyperX Predator RGB interface            |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/29/2019   |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "HyperXController.h"

class RGBController_HyperX : public RGBController
{
public:
    RGBController_HyperX(HyperXController* hyperx_ptr);
    void        UpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        UpdateMode();

private:
    HyperXController* hyperx;
};
