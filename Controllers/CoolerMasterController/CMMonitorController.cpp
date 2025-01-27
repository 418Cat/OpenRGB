/*-------------------------------------------------------------------*\
|  CMMonitorController.cpp                                            |
|                                                                     |
|  Driver for Coolermaster Gaming Monitor                             |
|                                                                     |
|  morg (Morgan Guimard) 9/18/2023                                    |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "CMMonitorController.h"
#include <cstring>

using namespace std::chrono_literals;

CMMonitorController::CMMonitorController(hid_device* dev_handle, const hid_device_info& info)
{
    dev                 = dev_handle;
    location            = info.path;

    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        serial_number = "";
    }
    else
    {
        std::wstring return_wstring = serial_string;
        serial_number = std::string(return_wstring.begin(), return_wstring.end());
    }
}

CMMonitorController::~CMMonitorController()
{
    hid_close(dev);
}

std::string CMMonitorController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string CMMonitorController::GetSerialString()
{
    return(serial_number);
}

void CMMonitorController::SetMode(uint8_t mode_value, const RGBColor& color, uint8_t speed, uint8_t brightness)
{
    if(software_mode_enabled)
    {
        DisableSoftwareMode();
    }

    uint8_t usb_buf[CM_MONITOR_PACKET_LENGTH];
    memset(usb_buf, 0x00, CM_MONITOR_PACKET_LENGTH);

    usb_buf[1]  = 0x80;
    usb_buf[2]  = (mode_value == CM_MONITOR_OFF_MODE) ? 0x0F : 0x0B;
    usb_buf[3]  = 0x02;
    usb_buf[4]  = 0x02;
    usb_buf[5]  = mode_value;
    usb_buf[6]  = (mode_value == CM_MONITOR_OFF_MODE) ? 0x00 : 0x08;;
    usb_buf[7]  = speed;
    usb_buf[8]  = brightness;
    usb_buf[9]  = RGBGetRValue(color);
    usb_buf[10] = RGBGetGValue(color);
    usb_buf[11] = RGBGetBValue(color);

    hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);
}

void CMMonitorController::SetCustomMode(const std::vector<RGBColor>& colors, uint8_t brightnesss)
{
    if(software_mode_enabled)
    {
        DisableSoftwareMode();
    }

    /*---------------------------------------------------------*\
    | Creates the color buffer                                  |
    \*---------------------------------------------------------*/
    uint8_t color_data[CM_MONITOR_COLOR_DATA_LENGTH];
    memset(color_data, 0x00, CM_MONITOR_COLOR_DATA_LENGTH);

    uint8_t offset = 0;

    for(const RGBColor& color: colors)
    {
        color_data[offset++] = RGBGetRValue(color);
        color_data[offset++] = RGBGetGValue(color);
        color_data[offset++] = RGBGetBValue(color);
    }

    /*---------------------------------------------------------*\
    | Sends the 8 sequence packets                              |
    \*---------------------------------------------------------*/
    uint8_t usb_buf[CM_MONITOR_PACKET_LENGTH];

    offset = 0;

    for(unsigned int i = 0; i < 7; i++)
    {
        memset(usb_buf, 0x00, CM_MONITOR_PACKET_LENGTH);

        usb_buf[1] = i < 6 ? i : 0x86;

        /*---------------------------------------------------------*\
        | First packet contains static data                         |
        \*---------------------------------------------------------*/
        if(i == 0)
        {
            usb_buf[2] = 0x10;
            usb_buf[3] = 0x02;
            usb_buf[4] = 0x02;
            usb_buf[5] = 0x80;
            usb_buf[6] = brightnesss;

            memcpy(&usb_buf[7], &color_data[offset], CM_MONITOR_PACKET_LENGTH - 7);
            offset += 58;
        }
        else
        {
            memcpy(&usb_buf[2], &color_data[offset], CM_MONITOR_PACKET_LENGTH - 2);
            offset += (CM_MONITOR_PACKET_LENGTH -2);
        }

        hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);
    }
}

void CMMonitorController::SendDirect(const std::vector<RGBColor>& colors)
{
    if(!software_mode_enabled)
    {
        EnableSoftwareMode();
    }

    /*---------------------------------------------------------*\
    | Creates the color buffer                                  |
    \*---------------------------------------------------------*/
    uint8_t color_data[CM_MONITOR_COLOR_DATA_LENGTH];
    memset(color_data, 0x00, CM_MONITOR_COLOR_DATA_LENGTH);

    unsigned int offset = 0;

    for(const RGBColor& color: colors)
    {
        color_data[offset++] = RGBGetRValue(color);
        color_data[offset++] = RGBGetGValue(color);
        color_data[offset++] = RGBGetBValue(color);
    }

    /*---------------------------------------------------------*\
    | Sends the 14 sequence packets                             |
    \*---------------------------------------------------------*/
    uint8_t usb_buf[CM_MONITOR_PACKET_LENGTH];

    for(unsigned int p = 0; p < 2; p++)
    {
        offset = 0;

        for(unsigned int i = 0; i < 7; i++)
        {
            memset(usb_buf, 0x00, CM_MONITOR_PACKET_LENGTH);

            usb_buf[1] = i < 6 ? i : 0x86;

            if(i == 0)
            {
                usb_buf[2] = 0x07;
                usb_buf[3] = 0x02;
                usb_buf[4] = p + 1;
                usb_buf[5] = 0x01;
                usb_buf[6] = 0x80;

                memcpy(&usb_buf[7], &color_data[offset], CM_MONITOR_PACKET_LENGTH - 7);
                offset += CM_MONITOR_PACKET_LENGTH - 7;
            }
            else
            {
                memcpy(&usb_buf[2], &color_data[offset], CM_MONITOR_PACKET_LENGTH - 2);
                offset += (CM_MONITOR_PACKET_LENGTH -2);
            }

            hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);
        }
    }
}

void CMMonitorController::EnableSoftwareMode()
{
    uint8_t usb_buf[CM_MONITOR_PACKET_LENGTH];
    memset(usb_buf, 0x00, CM_MONITOR_PACKET_LENGTH);

    usb_buf[1]  = 0x80;
    usb_buf[2]  = 0x07;
    usb_buf[3]  = 0x02;
    usb_buf[4]  = 0x01;
    usb_buf[6]  = 0x01;
    hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);

    usb_buf[4]  = 0x02;
    hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);

    uint8_t read_buf[CM_MONITOR_PACKET_LENGTH];

    /*---------------------------------------------------------*\
    | We have to send a few black packets, with some read ones  |
    \*---------------------------------------------------------*/
    for(unsigned int p = 0; p < 4; p++)
    {
        for(unsigned int i = 0; i < 7; i++)
        {
            memset(usb_buf, 0x00, CM_MONITOR_PACKET_LENGTH);

            usb_buf[1] = i < 6 ? i : 0x86;

            if(i == 0)
            {
                usb_buf[2] = 0x07;
                usb_buf[3] = 0x02;
                usb_buf[4] = (p == 0 || p == 0 ) ? 0x01 : 0x02;
                usb_buf[5] = 0x01;
                usb_buf[6] = 0x80;
            }

            hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);

            if(p ==0 && (i == 2 || i == 4))
            {
                memset(read_buf, 0x00, CM_MONITOR_PACKET_LENGTH);
                hid_read(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);
            }
        }
    }

    software_mode_enabled = true;
}

void CMMonitorController::DisableSoftwareMode()
{
    uint8_t usb_buf[CM_MONITOR_PACKET_LENGTH];
    memset(usb_buf, 0x00, CM_MONITOR_PACKET_LENGTH);

    usb_buf[1]  = 0x80;
    usb_buf[2]  = 0x07;
    usb_buf[3]  = 0x02;
    usb_buf[4]  = 0x01;
    hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);

    usb_buf[4]  = 0x02;
    hid_write(dev, usb_buf, CM_MONITOR_PACKET_LENGTH);

    software_mode_enabled = false;
}
