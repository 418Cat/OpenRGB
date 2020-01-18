#include "OpenRGBDevicePage.h"
#include "hsv.h"

using namespace Ui;

OpenRGBDevicePage::OpenRGBDevicePage(RGBController *dev, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OpenRGBDevicePageUi)
{
    ui->setupUi(this);

    /*-----------------------------------------------------*\
    | Store device pointer                                  |
    \*-----------------------------------------------------*/
    device = dev;

    /*-----------------------------------------------------*\
    | Set up the color palette buttons                      |
    \*-----------------------------------------------------*/
    QPalette pal;

    pal = ui->ButtonRed->palette();
    pal.setColor(QPalette::Button, QColor(255, 0, 0));
    ui->ButtonRed->setAutoFillBackground(true);
    ui->ButtonRed->setPalette(pal);
    ui->ButtonRed->setFlat(true);
    ui->ButtonRed->update();

    pal = ui->ButtonYellow->palette();
    pal.setColor(QPalette::Button, QColor(255, 255, 0));
    ui->ButtonYellow->setAutoFillBackground(true);
    ui->ButtonYellow->setPalette(pal);
    ui->ButtonYellow->setFlat(true);
    ui->ButtonYellow->update();

    pal = ui->ButtonGreen->palette();
    pal.setColor(QPalette::Button, QColor(0, 255, 0));
    ui->ButtonGreen->setAutoFillBackground(true);
    ui->ButtonGreen->setPalette(pal);
    ui->ButtonGreen->setFlat(true);
    ui->ButtonGreen->update();

    pal = ui->ButtonCyan->palette();
    pal.setColor(QPalette::Button, QColor(0, 255, 255));
    ui->ButtonCyan->setAutoFillBackground(true);
    ui->ButtonCyan->setPalette(pal);
    ui->ButtonCyan->setFlat(true);
    ui->ButtonCyan->update();

    pal = ui->ButtonBlue->palette();
    pal.setColor(QPalette::Button, QColor(0, 0, 255));
    ui->ButtonBlue->setAutoFillBackground(true);
    ui->ButtonBlue->setPalette(pal);
    ui->ButtonBlue->setFlat(true);
    ui->ButtonBlue->update();

    pal = ui->ButtonMagenta->palette();
    pal.setColor(QPalette::Button, QColor(255, 0, 255));
    ui->ButtonMagenta->setAutoFillBackground(true);
    ui->ButtonMagenta->setPalette(pal);
    ui->ButtonMagenta->setFlat(true);
    ui->ButtonMagenta->update();

    /*-----------------------------------------------------*\
    | Fill in the combo boxes with device information       |
    \*-----------------------------------------------------*/
    ui->ModeBox->blockSignals(true);
    ui->ModeBox->clear();

    for (std::size_t i = 0; i < device->modes.size(); i++)
    {
        ui->ModeBox->addItem(device->modes[i].name.c_str());
    }

    ui->ModeBox->setCurrentIndex(device->GetMode());
    ui->ModeBox->blockSignals(false);

    /*-----------------------------------------------------*\
    | Update mode user interface elements                   |
    \*-----------------------------------------------------*/
    UpdateModeUi();

    ui->ZoneBox->blockSignals(true);
    ui->ZoneBox->clear();

    for (std::size_t i = 0; i < device->zones.size(); i++)
    {
        ui->ZoneBox->addItem(device->zones[i].name.c_str());
    }

    ui->ZoneBox->setCurrentIndex(0);
    ui->ZoneBox->blockSignals(false);

    ui->LEDBox->blockSignals(true);
    ui->LEDBox->clear();

    for (std::size_t i = 0; i < device->leds.size(); i++)
    {
        ui->LEDBox->addItem(device->leds[i].name.c_str());
    }

    ui->LEDBox->setCurrentIndex(0);
    ui->LEDBox->blockSignals(false);

    /*-----------------------------------------------------*\
    | Update color picker with color of first LED           |
    \*-----------------------------------------------------*/
    on_LEDBox_currentIndexChanged(0);
}

OpenRGBDevicePage::~OpenRGBDevicePage()
{
    delete ui;
}

void Ui::OpenRGBDevicePage::on_ZoneBox_currentIndexChanged(int /*index*/)
{

}

void Ui::OpenRGBDevicePage::on_LEDBox_currentIndexChanged(int index)
{
    /*-----------------------------------------------------*\
    | Update color picker with color of selected LED        |
    \*-----------------------------------------------------*/
    RGBColor color = device->GetLED(index);
    UpdatingColor = true;
    ui->RedSpinBox->setValue(RGBGetRValue(color));
    ui->GreenSpinBox->setValue(RGBGetGValue(color));
    ui->BlueSpinBox->setValue(RGBGetBValue(color));
    UpdatingColor = false;
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_ModeBox_currentIndexChanged(int /*index*/)
{
    /*-----------------------------------------------------*\
    | Update mode user interface elements                   |
    \*-----------------------------------------------------*/
    UpdateModeUi();

    /*-----------------------------------------------------*\
    | Change device mode                                    |
    \*-----------------------------------------------------*/
    UpdateMode();
}

void Ui::OpenRGBDevicePage::on_RandomCheck_clicked()
{
    /*-----------------------------------------------------*\
    | Change device mode                                    |
    \*-----------------------------------------------------*/
    UpdateMode();
}

void Ui::OpenRGBDevicePage::on_SpeedSlider_valueChanged(int /*value*/)
{
    /*-----------------------------------------------------*\
    | Change device mode                                    |
    \*-----------------------------------------------------*/
    UpdateMode();
}

void Ui::OpenRGBDevicePage::on_DirectionBox_currentIndexChanged(int /*index*/)
{
    /*-----------------------------------------------------*\
    | Change device mode                                    |
    \*-----------------------------------------------------*/
    UpdateMode();
}

void Ui::OpenRGBDevicePage::UpdateModeUi()
{
    /*-----------------------------------------------------*\
    | Read new mode flags and settings                      |
    \*-----------------------------------------------------*/
    int  selected_mode   = ui->ModeBox->currentIndex();

    /*-----------------------------------------------------*\
    | Don't update the UI if the current mode is invalid    |
    \*-----------------------------------------------------*/
    if(selected_mode < device->modes.size())
    {
        bool supports_random = ( device->modes[selected_mode].flags & MODE_FLAG_HAS_COLOR )
                            && ( device->modes[selected_mode].flags & MODE_FLAG_RANDOM_COLOR );
        bool supports_speed  = ( device->modes[selected_mode].flags & MODE_FLAG_HAS_SPEED );
        bool supports_dir_lr = ( device->modes[selected_mode].flags & MODE_FLAG_HAS_DIRECTION_LR );
        bool supports_dir_ud = ( device->modes[selected_mode].flags & MODE_FLAG_HAS_DIRECTION_UD );
        bool supports_dir_hv = ( device->modes[selected_mode].flags & MODE_FLAG_HAS_DIRECTION_HV );
        bool random          = device->modes[selected_mode].random;
        unsigned int dir     = device->modes[selected_mode].direction;

        if(supports_speed)
        {
            if(device->modes[selected_mode].speed_min > device->modes[selected_mode].speed_max)
            {
                InvertedSpeed = true;
                ui->SpeedSlider->setMinimum(device->modes[selected_mode].speed_max);
                ui->SpeedSlider->setMaximum(device->modes[selected_mode].speed_min);
            }
            else
            {
                InvertedSpeed = false;
                ui->SpeedSlider->setMinimum(device->modes[selected_mode].speed_min);
                ui->SpeedSlider->setMaximum(device->modes[selected_mode].speed_max);
            }
            
            ui->SpeedSlider->setValue(device->modes[selected_mode].speed);
            ui->SpeedSlider->setEnabled(true);
        }
        else
        {
            ui->SpeedSlider->setEnabled(false);
        }

        ui->DirectionBox->clear();
        
        if(supports_dir_lr)
        {
            ui->DirectionBox->addItem("Left");
            ui->DirectionBox->addItem("Right");
        }

        if(supports_dir_ud)
        {
            ui->DirectionBox->addItem("Up");
            ui->DirectionBox->addItem("Down");
        }

        if(supports_dir_hv)
        {
            ui->DirectionBox->addItem("Horizontal");
            ui->DirectionBox->addItem("Vertical");
        }

        if(supports_dir_lr || supports_dir_ud || supports_dir_hv)
        {
            if((supports_dir_lr)
             &&((dir == MODE_DIRECTION_LEFT)
              ||(dir == MODE_DIRECTION_RIGHT)))
            {
                ui->DirectionBox->setCurrentIndex(dir);
            }

            if((supports_dir_ud)
             &&((dir == MODE_DIRECTION_UP)
              ||(dir == MODE_DIRECTION_DOWN)))
            {
                if(supports_dir_lr)
                {
                    ui->DirectionBox->setCurrentIndex(dir);
                }
                else
                {
                    ui->DirectionBox->setCurrentIndex(dir - 2);
                }
            }

            if((supports_dir_hv)
             &&((dir == MODE_DIRECTION_HORIZONTAL)
              ||(dir == MODE_DIRECTION_VERTICAL)))
            {
                if(supports_dir_lr && supports_dir_ud)
                {
                    ui->DirectionBox->setCurrentIndex(dir);
                }
                else if(supports_dir_lr || supports_dir_ud)
                {
                    ui->DirectionBox->setCurrentIndex(dir - 2);
                }
                else
                {
                    ui->DirectionBox->setCurrentIndex(dir - 4);
                }
            }

            ui->DirectionBox->setEnabled(true);
        }
        else
        {
            ui->DirectionBox->setEnabled(false);
        }
        
        if(supports_random)
        {
            ui->RandomCheck->setEnabled(true);
            ui->RandomCheck->setCheckState(random ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        }
        else
        {
            ui->RandomCheck->setEnabled(false);
        }
    }
}

void Ui::OpenRGBDevicePage::UpdateMode()
{
    /*-----------------------------------------------------*\
    | Read user interface                                   |
    \*-----------------------------------------------------*/
    int  current_mode      = ui->ModeBox->currentIndex();
    int  current_speed     = ui->SpeedSlider->value();
    bool current_random    = ui->RandomCheck->checkState();
    int  current_dir_idx   = ui->DirectionBox->currentIndex();
    int  current_direction = 0;
    bool supports_dir_lr = ( device->modes[current_mode].flags & MODE_FLAG_HAS_DIRECTION_LR );
    bool supports_dir_ud = ( device->modes[current_mode].flags & MODE_FLAG_HAS_DIRECTION_UD );
    bool supports_dir_hv = ( device->modes[current_mode].flags & MODE_FLAG_HAS_DIRECTION_HV );

    /*-----------------------------------------------------*\
    | Set the direction value                               |
    \*-----------------------------------------------------*/
    if(supports_dir_hv)
    {
        if(supports_dir_lr && supports_dir_ud)
        {
            current_direction = current_dir_idx;
        }
        else if(supports_dir_lr || supports_dir_ud)
        {
            current_direction = current_dir_idx + 2;
        }
        else
        {
            current_direction = current_dir_idx + 4;
        }
    }

    if(supports_dir_ud)
    {
        if(supports_dir_lr)
        {
            current_direction = current_dir_idx;
        }
        else
        {
            current_direction = current_dir_idx + 2;
        }
    }

    if((supports_dir_lr)
     &&(current_dir_idx < 2))
    {
        current_direction = current_dir_idx;
    }
    
    device->modes[current_mode].direction = current_direction;

    /*-----------------------------------------------------*\
    | If Speed Slider is inverted, invert value             |
    \*-----------------------------------------------------*/
    if(InvertedSpeed)
    {
        current_speed = device->modes[current_mode].speed_min - current_speed;
    }

    /*-----------------------------------------------------*\
    | Don't set the mode if the current mode is invalid     |
    \*-----------------------------------------------------*/
    if(current_mode < device->modes.size())
    {
        /*-----------------------------------------------------*\
        | Update mode parameters                                |
        \*-----------------------------------------------------*/
        device->modes[current_mode].speed  = current_speed;
        device->modes[current_mode].random = current_random;

        /*-----------------------------------------------------*\
        | Change device mode                                    |
        \*-----------------------------------------------------*/
        device->SetMode(current_mode);
    }
}

void Ui::OpenRGBDevicePage::SetDevice(unsigned char red, unsigned char green, unsigned char blue)
{
    UpdatingColor = true;
    ui->RedSpinBox->setValue(red);
    ui->GreenSpinBox->setValue(green);
    ui->BlueSpinBox->setValue(blue);
    UpdatingColor = false;
    updateHSV();

    on_SetDeviceButton_clicked();
}

void Ui::OpenRGBDevicePage::on_SetDeviceButton_clicked()
{
    /*-----------------------------------------------------*\
    | Set all device LEDs to the current color              |
    \*-----------------------------------------------------*/
    RGBColor color = ToRGBColor(
        ui->RedSpinBox->text().toInt(),
        ui->GreenSpinBox->text().toInt(),
        ui->BlueSpinBox->text().toInt()
    );

    device->SetAllLEDs(color);
}

void Ui::OpenRGBDevicePage::on_SetZoneButton_clicked()
{
    /*-----------------------------------------------------*\
    | Set all LEDs in the selected zone to the current color|
    \*-----------------------------------------------------*/
    RGBColor color = ToRGBColor(
        ui->RedSpinBox->text().toInt(),
        ui->GreenSpinBox->text().toInt(),
        ui->BlueSpinBox->text().toInt()
    );

    device->SetAllZoneLEDs(ui->ZoneBox->currentIndex(), color);
}

void Ui::OpenRGBDevicePage::on_SetLEDButton_clicked()
{
    /*-----------------------------------------------------*\
    | Set the selected LED to the current color             |
    \*-----------------------------------------------------*/
    RGBColor color = ToRGBColor(
        ui->RedSpinBox->text().toInt(),
        ui->GreenSpinBox->text().toInt(),
        ui->BlueSpinBox->text().toInt()
    );

    device->SetLED(ui->LEDBox->currentIndex(), color);
}

void Ui::OpenRGBDevicePage::on_ButtonRed_clicked()
{
    UpdatingColor = true;
    ui->RedSpinBox->setValue(255);
    ui->GreenSpinBox->setValue(0);
    ui->BlueSpinBox->setValue(0);
    UpdatingColor = false;
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_ButtonYellow_clicked()
{
    UpdatingColor = true;
    ui->RedSpinBox->setValue(255);
    ui->GreenSpinBox->setValue(255);
    ui->BlueSpinBox->setValue(0);
    UpdatingColor = false;
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_ButtonGreen_clicked()
{
    UpdatingColor = true;
    ui->RedSpinBox->setValue(0);
    ui->GreenSpinBox->setValue(255);
    ui->BlueSpinBox->setValue(0);
    UpdatingColor = false;
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_ButtonCyan_clicked()
{
    UpdatingColor = true;
    ui->RedSpinBox->setValue(0);
    ui->GreenSpinBox->setValue(255);
    ui->BlueSpinBox->setValue(255);
    UpdatingColor = false;
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_ButtonBlue_clicked()
{
    UpdatingColor = true;
    ui->RedSpinBox->setValue(0);
    ui->GreenSpinBox->setValue(0);
    ui->BlueSpinBox->setValue(255);
    UpdatingColor = false;
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_ButtonMagenta_clicked()
{
    UpdatingColor = true;
    ui->RedSpinBox->setValue(255);
    ui->GreenSpinBox->setValue(0);
    ui->BlueSpinBox->setValue(255);
    UpdatingColor = false;
    updateHSV();
}

void Ui::OpenRGBDevicePage::updateRGB()
{
    if(UpdatingColor)
    {
        return;
    }

    UpdatingColor = true;

    hsv_t hsv;
    hsv.hue = ui->HueSpinBox->value();
    hsv.saturation = ui->SatSpinBox->value();
    hsv.value = ui->ValSpinBox->value();

    RGBColor rgb = hsv2rgb(&hsv);

    ui->RedSpinBox->setValue(RGBGetRValue(rgb));
    ui->GreenSpinBox->setValue(RGBGetGValue(rgb));
    ui->BlueSpinBox->setValue(RGBGetBValue(rgb));

    UpdatingColor = false;
}

void Ui::OpenRGBDevicePage::updateHSV()
{
    if(UpdatingColor)
    {
        return;
    }

    UpdatingColor = true;

    RGBColor rgb = ToRGBColor(ui->RedSpinBox->value(), ui->GreenSpinBox->value(), ui->BlueSpinBox->value());

    hsv_t hsv;

    rgb2hsv(rgb, &hsv);

    ui->HueSpinBox->setValue(hsv.hue);
    ui->SatSpinBox->setValue(hsv.saturation);
    ui->ValSpinBox->setValue(hsv.value);

    UpdatingColor = false;
}

void Ui::OpenRGBDevicePage::on_RedSpinBox_valueChanged(int /*arg1*/)
{
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_HueSpinBox_valueChanged(int /*arg1*/)
{
    updateRGB();
}

void Ui::OpenRGBDevicePage::on_GreenSpinBox_valueChanged(int /*arg1*/)
{
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_SatSpinBox_valueChanged(int /*arg1*/)
{
    updateRGB();
}

void Ui::OpenRGBDevicePage::on_BlueSpinBox_valueChanged(int /*arg1*/)
{
    updateHSV();
}

void Ui::OpenRGBDevicePage::on_ValSpinBox_valueChanged(int /*arg1*/)
{
    updateRGB();
}

void Ui::OpenRGBDevicePage::on_SetAllButton_clicked()
{
    unsigned char red   = ui->RedSpinBox->value();
    unsigned char green = ui->GreenSpinBox->value();
    unsigned char blue  = ui->BlueSpinBox->value();

    emit SetAllDevices(red, green, blue);
}
