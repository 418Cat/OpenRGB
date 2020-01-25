/*-----------------------------------------*\
|  RGBController_LEDStrip.h                 |
|                                           |
|  Generic RGB Interface for OpenAuraSDK    |
|  KeyboardVisualizer LED strip interface   |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/20/2019   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "serial_port.h"
#include "LEDStripController.h"

class RGBController_LEDStrip : public RGBController
{
public:
    RGBController_LEDStrip(LEDStripController* ledstrip_ptr);
    void        UpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        UpdateMode();

private:
    LEDStripController* strip;
};
