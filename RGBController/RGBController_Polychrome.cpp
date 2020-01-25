/*-----------------------------------------*\
|  RGBController_Polychrome.cpp             |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  ASRock ASR LED and Polychrome RGB Driver |
|                                           |
|  Adam Honse (CalcProgrammer1) 12/15/2019  |
\*-----------------------------------------*/

#include "RGBController_Polychrome.h"


void RGBController_Polychrome::UpdateLEDs()
{
    for (std::size_t led = 0; led < colors.size(); led++)
    {
        unsigned char red = RGBGetRValue(colors[led]);
        unsigned char grn = RGBGetGValue(colors[led]);
        unsigned char blu = RGBGetBValue(colors[led]);

        polychrome->SetLEDColor(led, red, grn, blu);
    }
}

void RGBController_Polychrome::UpdateZoneLEDs(int zone)
{
    UpdateLEDs();
}

void RGBController_Polychrome::UpdateSingleLED(int led)
{
    UpdateLEDs();
}

static const char* polychrome_zone_names[] =
{
    "Motherboard"
};

RGBController_Polychrome::RGBController_Polychrome(PolychromeController* polychrome_ptr)
{
    polychrome = polychrome_ptr;

    name = polychrome->GetDeviceName();

    mode polychrome_modes[POLYCHROME_NUM_MODES];

    polychrome_modes[0].name = "Static";
    polychrome_modes[1].name = "Breathing";
    polychrome_modes[2].name = "Flashing";

    for (int i = 0; i < POLYCHROME_NUM_MODES; i++)
    {
        modes.push_back(polychrome_modes[i]);
    }

    colors.resize(polychrome->GetLEDCount());

    // Search through all LEDs and create zones for each channel type
    for (unsigned int i = 0; i < polychrome->GetLEDCount(); i++)
    {
        zone* new_zone = new zone();
        led* new_led = new led();

        std::vector<int>* zone_row = new std::vector<int>();

        // Set zone name to channel name
        new_zone->name = polychrome_zone_names[i];
        new_led->name = polychrome_zone_names[i];

        zone_row->push_back(i);

        // Aura devices can be either single or linear, never matrix
        // That means only one row is needed
        new_zone->map.push_back(*zone_row);

        // Push new LED to LEDs vector
        leds.push_back(*new_led);

        // Push new zone to zones vector
        zones.push_back(*new_zone);
    }
}

void RGBController_Polychrome::SetCustomMode()
{
    SetMode(0);
}

void RGBController_Polychrome::UpdateMode()
{
    polychrome->SetMode(active_mode);
}