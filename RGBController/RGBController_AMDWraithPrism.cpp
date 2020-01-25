/*-----------------------------------------*\
|  RGBController_AMDWraithPrism.cpp         |
|                                           |
|  Generic RGB Interface for AMD Wraith     |
|  Prism                                    |
|                                           |
|  Adam Honse (CalcProgrammer1) 12/25/2019  |
\*-----------------------------------------*/

#include "RGBController_AMDWraithPrism.h"

RGBController_AMDWraithPrism::RGBController_AMDWraithPrism(AMDWraithPrismController* wraith_ptr)
{
    wraith = wraith_ptr;

    name = "AMD Wraith Prism";
    type = DEVICE_TYPE_COOLER;
    version = wraith->GetFirmwareVersionString();

    mode Static;
    Static.name   = "Static";
    Static.value  = AMD_WRAITH_PRISM_EFFECT_CHANNEL_STATIC;
    Static.flags  = MODE_FLAG_HAS_COLOR | MODE_FLAG_PER_LED_COLOR;
    Static.random = false;
    modes.push_back(Static);

    mode Breathing;
    Breathing.name      = "Breathing";
    Breathing.value     = AMD_WRAITH_PRISM_EFFECT_CHANNEL_BREATHING;
    Breathing.flags     = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_COLOR | MODE_FLAG_PER_LED_COLOR | MODE_FLAG_RANDOM_COLOR;
    Breathing.speed_min = AMD_WRAITH_PRISM_SPEED_SLOWEST;
    Breathing.speed_max = AMD_WRAITH_PRISM_SPEED_FASTEST;
    Breathing.random    = false;
    Breathing.speed     = AMD_WRAITH_PRISM_SPEED_NORMAL;
    modes.push_back(Breathing);

    mode ColorCycle;
    ColorCycle.name      = "Color Cycle";
    ColorCycle.value     = AMD_WRAITH_PRISM_EFFECT_CHANNEL_COLOR_CYCLE;
    ColorCycle.flags     = MODE_FLAG_HAS_SPEED;
    ColorCycle.speed_min = AMD_WRAITH_PRISM_SPEED_SLOWEST;
    ColorCycle.speed_max = AMD_WRAITH_PRISM_SPEED_FASTEST;
    ColorCycle.random    = false;
    ColorCycle.speed     = AMD_WRAITH_PRISM_SPEED_NORMAL;
    modes.push_back(ColorCycle);

    mode Rainbow;
    Rainbow.name      = "Rainbow";
    Rainbow.value     = AMD_WRAITH_PRISM_EFFECT_CHANNEL_RAINBOW;
    Rainbow.flags     = MODE_FLAG_HAS_SPEED;
    Rainbow.speed_min = AMD_WRAITH_PRISM_SPEED_SLOWEST;
    Rainbow.speed_max = AMD_WRAITH_PRISM_SPEED_FASTEST;
    Rainbow.random    = false;
    Rainbow.speed     = AMD_WRAITH_PRISM_SPEED_NORMAL;
    modes.push_back(Rainbow);

    mode Bounce;
    Bounce.name      = "Bounce";
    Bounce.value     = AMD_WRAITH_PRISM_EFFECT_CHANNEL_BOUNCE;
    Bounce.flags     = MODE_FLAG_HAS_SPEED;
    Bounce.speed_min = AMD_WRAITH_PRISM_SPEED_SLOWEST;
    Bounce.speed_max = AMD_WRAITH_PRISM_SPEED_FASTEST;
    Bounce.random    = true;
    Bounce.speed     = AMD_WRAITH_PRISM_SPEED_NORMAL;
    modes.push_back(Bounce);

    mode Chase;
    Chase.name      = "Chase";
    Chase.value     = AMD_WRAITH_PRISM_EFFECT_CHANNEL_CHASE;
    Chase.flags     = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_COLOR | MODE_FLAG_PER_LED_COLOR | MODE_FLAG_RANDOM_COLOR;
    Chase.speed_min = AMD_WRAITH_PRISM_SPEED_SLOWEST;
    Chase.speed_max = AMD_WRAITH_PRISM_SPEED_FASTEST;
    Chase.random    = false;
    Chase.speed     = AMD_WRAITH_PRISM_SPEED_NORMAL;
    modes.push_back(Chase);

    mode Swirl;
    Swirl.name      = "Swirl";
    Swirl.value     = AMD_WRAITH_PRISM_EFFECT_CHANNEL_SWIRL;
    Swirl.flags     = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_COLOR | MODE_FLAG_PER_LED_COLOR | MODE_FLAG_RANDOM_COLOR;
    Swirl.speed_min = AMD_WRAITH_PRISM_SPEED_SLOWEST;
    Swirl.speed_max = AMD_WRAITH_PRISM_SPEED_FASTEST;
    Swirl.random    = false;
    Swirl.speed     = AMD_WRAITH_PRISM_SPEED_NORMAL;
    modes.push_back(Swirl);

    led logo_led;
    logo_led.name = "Logo";
    leds.push_back(logo_led);
    colors.push_back(0x00000000);

    zone logo_zone;
    logo_zone.name = "Logo";
    logo_zone.type = ZONE_TYPE_SINGLE;
    std::vector<int> logo_zone_map;
    logo_zone_map.push_back(0);
    logo_zone.map.push_back(logo_zone_map);
    zones.push_back(logo_zone);

    led fan_led;
    fan_led.name = "Fan";
    leds.push_back(fan_led);
    colors.push_back(0x00000000);

    zone fan_zone;
    fan_zone.name = "Fan";
    fan_zone.type = ZONE_TYPE_SINGLE;
    std::vector<int> fan_zone_map;
    fan_zone_map.push_back(1);
    fan_zone.map.push_back(fan_zone_map);
    zones.push_back(fan_zone);

    led ring_led;
    ring_led.name = "Ring";
    leds.push_back(ring_led);
    colors.push_back(0x00000000);

    zone ring_zone;
    ring_zone.name = "Ring";
    ring_zone.type = ZONE_TYPE_SINGLE;
    std::vector<int> ring_zone_map;
    ring_zone_map.push_back(2);
    ring_zone.map.push_back(ring_zone_map);
    zones.push_back(ring_zone);   
}

RGBController_AMDWraithPrism::~RGBController_AMDWraithPrism()
{

}

void RGBController_AMDWraithPrism::UpdateLEDs()
{
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char grn = RGBGetGValue(colors[0]);
    unsigned char blu = RGBGetBValue(colors[0]);
    wraith->SetLogoColor(red, grn, blu);

    red = RGBGetRValue(colors[1]);
    grn = RGBGetGValue(colors[1]);
    blu = RGBGetBValue(colors[1]);
    wraith->SetFanColor(red, grn, blu);

    red = RGBGetRValue(colors[2]);
    grn = RGBGetGValue(colors[2]);
    blu = RGBGetBValue(colors[2]);
    wraith->SetRingColor(red, grn, blu);
}

void RGBController_AMDWraithPrism::UpdateZoneLEDs(int zone)
{
    RGBColor      color = colors[zone];
    unsigned char red   = RGBGetRValue(color);
    unsigned char grn   = RGBGetGValue(color);
    unsigned char blu   = RGBGetBValue(color);

    if(zone == 0)
    {
        wraith->SetLogoColor(red, grn, blu);
    }
    else if(zone == 1)
    {
        wraith->SetFanColor(red, grn, blu);
    }
    else if(zone == 2)
    {
        wraith->SetRingColor(red, grn, blu);
    }
}

void RGBController_AMDWraithPrism::UpdateSingleLED(int led)
{
    UpdateZoneLEDs(led);
}

void RGBController_AMDWraithPrism::SetCustomMode()
{
    SetMode(0);
}

void RGBController_AMDWraithPrism::UpdateMode()
{
    wraith->SetRingMode(modes[active_mode].value, modes[active_mode].speed, modes[active_mode].direction, modes[active_mode].random);

    switch(modes[active_mode].value)
    {
        case AMD_WRAITH_PRISM_EFFECT_CHANNEL_COLOR_CYCLE:
        case AMD_WRAITH_PRISM_EFFECT_CHANNEL_RAINBOW:
        case AMD_WRAITH_PRISM_EFFECT_CHANNEL_BOUNCE:
            wraith->SetFanMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_COLOR_CYCLE, modes[active_mode].speed, modes[active_mode].random);
            wraith->SetLogoMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_COLOR_CYCLE, modes[active_mode].speed, modes[active_mode].random);
            break;
        
        case AMD_WRAITH_PRISM_EFFECT_CHANNEL_BREATHING:
            wraith->SetFanMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_BREATHING, modes[active_mode].speed, modes[active_mode].random);
            wraith->SetLogoMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_BREATHING, modes[active_mode].speed, modes[active_mode].random);
            break;

        default:
            if(modes[active_mode].random)
            {
                wraith->SetFanMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_COLOR_CYCLE, modes[active_mode].speed, modes[active_mode].random);
                wraith->SetLogoMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_COLOR_CYCLE, modes[active_mode].speed, modes[active_mode].random);
            }
            else
            {
                wraith->SetFanMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_STATIC, modes[active_mode].speed, modes[active_mode].random);
                wraith->SetLogoMode(AMD_WRAITH_PRISM_FAN_LOGO_MODE_STATIC, modes[active_mode].speed, modes[active_mode].random);
            }
            break;
    }

    UpdateLEDs();
}