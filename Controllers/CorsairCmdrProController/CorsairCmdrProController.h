/*---------------------------------------------------------*\
|  Definitions for Corsair Commander Pro                    |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 1/16/2020        |
\*---------------------------------------------------------*/

#include "RGBController.h"
#include <vector>
#include <libusb-1.0/libusb.h>

#pragma once

enum
{
    CORSAIR_CMDR_PRO_PACKET_ID_DIRECT           = 0x32,     /* Direct mode LED update packet        */
    CORSAIR_CMDR_PRO_PACKET_ID_COMMIT           = 0x33,     /* Commit changes packet                */
    CORSAIR_CMDR_PRO_PACKET_ID_BEGIN            = 0x34,     /* Begin effect packet                  */
    CORSAIR_CMDR_PRO_PACKET_ID_EFFECT_CONFIG    = 0x35,     /* Effect mode configuration packet     */
    CORSAIR_CMDR_PRO_PACKET_ID_TEMPERATURE      = 0x36,     /* Update temperature value packet      */
    CORSAIR_CMDR_PRO_PACKET_ID_RESET            = 0x37,     /* Reset channel packet                 */
    CORSAIR_CMDR_PRO_PACKET_ID_PORT_STATE       = 0x38,     /* Set port state packet                */
    CORSAIR_CMDR_PRO_PACKET_ID_BRIGHTNESS       = 0x39,     /* Set brightness packet                */
    CORSAIR_CMDR_PRO_PACKET_ID_LED_COUNT        = 0x3A,     /* Set LED count packet                 */
    CORSAIR_CMDR_PRO_PACKET_ID_PROTOCOL         = 0x3B,     /* Set protocol packet                  */
};

enum
{
    CORSAIR_CMDR_PRO_DIRECT_CHANNEL_RED         = 0x00,     /* Red channel for direct update        */
    CORSAIR_CMDR_PRO_DIRECT_CHANNEL_GREEN       = 0x01,     /* Green channel for direct update      */
    CORSAIR_CMDR_PRO_DIRECT_CHANNEL_BLUE        = 0x02,     /* Blue channel for direct update       */
};

enum
{
    CORSAIR_CMDR_PRO_PORT_STATE_HARDWARE        = 0x01,     /* Effect hardware control of channel   */
    CORSAIR_CMDR_PRO_PORT_STATE_SOFTWARE        = 0x02,     /* Direct software control of channel   */
};

enum
{
    CORSAIR_CMDR_PRO_LED_TYPE_LED_STRIP         = 0x0A,     /* Corsair LED Strip Type               */
    CORSAIR_CMDR_PRO_LED_TYPE_HD_FAN            = 0x0C,     /* Corsair HD-series Fan Type           */
    CORSAIR_CMDR_PRO_LED_TYPE_SP_FAN            = 0x01,     /* Corsair SP-series Fan Type           */
    CORSAIR_CMDR_PRO_LED_TYPE_ML_FAN            = 0x02,     /* Corsair ML-series Fan Type           */
};

enum
{
    CORSAIR_CMDR_PRO_CHANNEL_1                  = 0x00,     /* Channel 1                            */
    CORSAIR_CMDR_PRO_CHANNEL_2                  = 0x01,     /* Channel 2                            */
    CORSAIR_CMDR_PRO_NUM_CHANNELS               = 0x02,     /* Number of channels                   */
};

enum
{
    CORSAIR_CMDR_PRO_SPEED_SLOW                 = 0x00,     /* Slow speed                           */
    CORSAIR_CMDR_PRO_SPEED_MEDIUM               = 0x01,     /* Medium speed                         */
    CORSAIR_CMDR_PRO_SPEED_FAST                 = 0x02,     /* Fast speed                           */
};

enum
{
    CORSAIR_CMDR_PRO_MODE_RAINBOW_WAVE          = 0x00,     /* Rainbow Wave mode                    */
    CORSAIR_CMDR_PRO_MODE_COLOR_SHIFT           = 0x01,     /* Color Shift mode                     */
    CORSAIR_CMDR_PRO_MODE_COLOR_PULSE           = 0x02,     /* Color Pulse mode                     */
    CORSAIR_CMDR_PRO_MODE_COLOR_WAVE            = 0x03,     /* Color Wave mode                      */
    CORSAIR_CMDR_PRO_MODE_STATIC                = 0x04,     /* Static mode                          */
    CORSAIR_CMDR_PRO_MODE_TEMPERATURE           = 0x05,     /* Temperature mode                     */
    CORSAIR_CMDR_PRO_MODE_VISOR                 = 0x06,     /* Visor mode                           */
    CORSAIR_CMDR_PRO_MODE_MARQUEE               = 0x07,     /* Marquee mode                         */
    CORSAIR_CMDR_PRO_MODE_BLINK                 = 0x08,     /* Blink mode                           */
    CORSAIR_CMDR_PRO_MODE_SEQUENTIAL            = 0x09,     /* Sequential mode                      */
    CORSAIR_CMDR_PRO_MODE_RAINBOW               = 0x0A,     /* Rainbow mode                         */
};

class CorsairCmdrProController
{
public:
    CorsairCmdrProController(libusb_device_handle* dev_handle);
    ~CorsairCmdrProController();
    
    char*           GetLEDString();
    unsigned int    GetStripsOnChannel(unsigned int channel);

    void            SetChannelEffect(unsigned char channel,
                                     unsigned char mode,
                                     unsigned char speed,
                                     unsigned char direction,
                                     bool          random,
                                     unsigned char red1,
                                     unsigned char grn1,
                                     unsigned char blu1,
                                     unsigned char red2,
                                     unsigned char grn2,
                                     unsigned char blu2
                                    );

    void            SetChannelLEDs(unsigned char channel, std::vector<RGBColor> colors);
    void            SendKeepalive();
    unsigned int    channel_leds[CORSAIR_CMDR_PRO_NUM_CHANNELS];

    void            KeepaliveThread();

private:
    libusb_device_handle*   dev;

    void            SendDirect
                        (
                        unsigned char   channel,
                        unsigned char   start,
                        unsigned char   count,
                        unsigned char   color_channel,
                        unsigned char*  color_data
                        );

    void            SendCommit();

    void            SendBegin
                        (
                        unsigned char   channel
                        );

    void            SendEffectConfig
                        (
                        unsigned char   channel,
                        unsigned char   count,
                        unsigned char   led_type,
                        unsigned char   mode,
                        unsigned char   speed,
                        unsigned char   direction,
                        unsigned char   change_style,
                        unsigned char   color_0_red,
                        unsigned char   color_0_green,
                        unsigned char   color_0_blue,
                        unsigned char   color_1_red,
                        unsigned char   color_1_green,
                        unsigned char   color_1_blue,
                        unsigned char   color_2_red,
                        unsigned char   color_2_green,
                        unsigned char   color_2_blue,
                        unsigned short  temperature_0,
                        unsigned short  temperature_1,
                        unsigned short  temperature_2
                        );

    void            SendTemperature();

    void            SendReset
                        (
                        unsigned char   channel
                        );

    void            SendPortState
                        (
                        unsigned char   channel,
                        unsigned char   state
                        );

    void            SendBrightness();

    void            SendLEDCount();

    void            SendProtocol();
};
