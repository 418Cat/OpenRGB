/*-----------------------------------------*\
|  CorsairKbdV1Controller.cpp               |
|                                           |
|  Driver for first-generation Corsair Kxx  |
|  RGB keyboard lighting controller         |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/9/2020    |
\*-----------------------------------------*/

#include "CorsairKbdV1Controller.h"

static void send_usb_msg(hid_device* dev, char * data_pkt)
{
    char usb_pkt[65];
    int actual;
    usb_pkt[0] = 0x21;
    for(int i = 1; i < 65; i++)
    {
        usb_pkt[i] = data_pkt[i-1];
    }
    hid_send_feature_report(dev, (unsigned char *)usb_pkt, 65);
}

CorsairKbdV1Controller::CorsairKbdV1Controller(hid_device* dev_handle)
{
    dev = dev_handle;

    char data_pkt[64] = { 0 };

    data_pkt[0] = 0x07;
    data_pkt[1] = 0x05;
    data_pkt[2] = 0x02;
    data_pkt[3] = 0x00;
    data_pkt[4] = 0x03;

    send_usb_msg(dev, data_pkt);
}

CorsairKbdV1Controller::~CorsairKbdV1Controller()
{

}

char* CorsairKbdV1Controller::GetDeviceName()
{
    return device_name;
}

void CorsairKbdV1Controller::SetLEDs(std::vector<RGBColor> colors)
{
    char data_pkt[5][64] = { 0 };
    char red_val[144];
    char grn_val[144];
    char blu_val[144];

    for(int color_idx = 0; color_idx < colors.size(); color_idx++)
    {
        RGBColor      color = colors[color_idx];
        unsigned char red   = RGBGetRValue(color);
        unsigned char grn   = RGBGetGValue(color);
        unsigned char blu   = RGBGetBValue(color);

        if( red > 7 ) red = 7;
        if( grn > 7 ) grn = 7;
        if( blu > 7 ) blu = 7;

        red = 7 - red;
        grn = 7 - grn;
        blu = 7 - blu;

        red_val[color_idx] = red;
        grn_val[color_idx] = grn;
        blu_val[color_idx] = blu;
    }

    // Perform USB control message to keyboard
    //
    // Request Type:  0x21
    // Request:       0x09
    // Value          0x0300
    // Index:         0x03
    // Size:          64

    data_pkt[0][0] = 0x7F;
    data_pkt[0][1] = 0x01;
    data_pkt[0][2] = 0x3C;

    data_pkt[1][0] = 0x7F;
    data_pkt[1][1] = 0x02;
    data_pkt[1][2] = 0x3C;

    data_pkt[2][0] = 0x7F;
    data_pkt[2][1] = 0x03;
    data_pkt[2][2] = 0x3C;

    data_pkt[3][0] = 0x7F;
    data_pkt[3][1] = 0x04;
    data_pkt[3][2] = 0x24;

    data_pkt[4][0] = 0x07;
    data_pkt[4][1] = 0x27;
    data_pkt[4][2] = 0x00;
    data_pkt[4][2] = 0x00;
    data_pkt[4][4] = 0xD8; //Number of payload bytes

    for(int i = 0; i < 60; i++)
    {
        data_pkt[0][i+4] = red_val[i*2+1] << 4 | red_val[i*2];
    }

    for(int i = 0; i < 12; i++)
    {
        data_pkt[1][i+4] = red_val[i*2+121] << 4 | red_val[i*2+120];
    }

    for(int i = 0; i < 48; i++)
    {
        data_pkt[1][i+16] = grn_val[i*2+1] << 4 | grn_val[i*2];
    }

    for(int i = 0; i < 24; i++)
    {
        data_pkt[2][i+4] = grn_val[i*2+97] << 4 | grn_val[i*2+96];
    }

    for(int i = 0; i < 36; i++)
    {
        data_pkt[2][i+28] = blu_val[i*2+1] << 4 | blu_val[i*2];
    }

    for(int i = 0; i < 36; i++)
    {
        data_pkt[3][i+4] = blu_val[i*2+73] << 4 | blu_val[i*2+72];
    }

    send_usb_msg(dev, data_pkt[0]);
    send_usb_msg(dev, data_pkt[1]);
    send_usb_msg(dev, data_pkt[2]);
    send_usb_msg(dev, data_pkt[3]);
    send_usb_msg(dev, data_pkt[4]);
}
