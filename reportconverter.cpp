#include <QDebug>
#include <cstring>
#include "reportconverter.h"

static int reportSuccess = 0;

void ReportConverter::GamepadReport( uint8_t * gamepad_buf)
{
    if (gamepad_buf != nullptr)   // nullptr
    {
        memcpy((uint8_t *)&(gEnv.pDeviceConfig->gamepad_report)+1, gamepad_buf, sizeof(joy_report_t)-1);          // +1 ALIGHNING !!!!!!!!!!!!!!!!!!
    }
}

// temp dummy
void USB_CUSTOM_HID_SendReport(uint8_t * dummy, uint8_t dummy2){
    Q_UNUSED(dummy)
    Q_UNUSED(dummy2)
}

//std::vector<uint8_t>
std::vector<uint8_t> ReportConverter::SendConfigToDevice(uint8_t request_config_number)      // СНЕСТИ И ЗАБЫТЬ// ***1111111*
{
    dev_config_t tmp_dev_config{};                                         // !!!!!!!!!!
    memcpy((uint8_t *)&tmp_dev_config ,&gEnv.pDeviceConfig->config, sizeof(dev_config_t));

    uint8_t i;
    uint8_t pos = 2;
    //uint8_t joy_millis;
    uint8_t config_in_cnt;

    uint8_t tmp_buf[64];

    // requested config packet number
    config_in_cnt = request_config_number;
    tmp_buf[0] = REPORT_ID_CONFIG_OUT;
    tmp_buf[1] = config_in_cnt;


    switch(config_in_cnt)
    {
    case 1:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.firmware_version), sizeof(tmp_dev_config.firmware_version));
        pos += sizeof(tmp_dev_config.firmware_version);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.device_name), sizeof(tmp_dev_config.device_name));
        pos += sizeof(tmp_dev_config.device_name);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.button_debounce_ms), 4);
        pos += 4;
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.pins), sizeof(tmp_dev_config.pins));
        pos += sizeof(tmp_dev_config.pins);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 2:
        i = 0;
        while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
        {
            memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axis_config[i++]), sizeof(axis_config_t));
            pos += sizeof(axis_config_t);
        }
        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 3:
        i = 2;
        while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
        {
            memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axis_config[i++]), sizeof(axis_config_t));
            pos += sizeof(axis_config_t);
        }
        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 4:
        i = 4;
        while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
        {
            memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axis_config[i++]), sizeof(axis_config_t));
            pos += sizeof(axis_config_t);
        }
        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 5:

        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axis_config[6]), sizeof(axis_config_t));
        pos += sizeof(axis_config_t);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axis_config[7]), sizeof(axis_config_t));
        pos += sizeof(axis_config_t);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 6:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.buttons[0]), 60);
        memcpy(&tmp_buf[64-sizeof(tmp_dev_config.button_timer1_ms)], (uint8_t *) &(tmp_dev_config.button_timer1_ms), sizeof(tmp_dev_config.button_timer1_ms));

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 7:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.buttons[20]), 60);
        memcpy(&tmp_buf[64-sizeof(tmp_dev_config.button_timer2_ms)], (uint8_t *) &(tmp_dev_config.button_timer2_ms), sizeof(tmp_dev_config.button_timer2_ms));

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 8:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.buttons[40]), 60);
        memcpy(&tmp_buf[64-sizeof(tmp_dev_config.button_timer3_ms)], (uint8_t *) &(tmp_dev_config.button_timer3_ms), sizeof(tmp_dev_config.button_timer3_ms));

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 9:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.buttons[60]), 60);
        memcpy(&tmp_buf[64-sizeof(tmp_dev_config.a2b_debounce_ms)], (uint8_t *) &(tmp_dev_config.a2b_debounce_ms), sizeof(tmp_dev_config.a2b_debounce_ms));

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 10:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.buttons[80]), 60);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 11:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.buttons[100]), 60);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 12:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.buttons[120]), 8*sizeof(button_t));
        pos += 8*sizeof(button_t);

        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[0]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[1]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 13:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[2]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[3]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[4]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[5]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 14:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[6]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.axes_to_buttons[7]), sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);

        for (i=0; i<4; i++)
        {
            memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.shift_registers[i]), sizeof(shift_reg_config_t));
            pos += sizeof(shift_reg_config_t);
        }

        for (i=0; i<5; i++)
        {
            memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.shift_config[i]), sizeof(shift_modificator_t));
            pos += sizeof(shift_modificator_t);
        }

        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.vid), sizeof(tmp_dev_config.vid));
        pos += sizeof(tmp_dev_config.vid);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.pid), sizeof(tmp_dev_config.pid));
        pos += sizeof(tmp_dev_config.pid);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.is_dynamic_config), sizeof(tmp_dev_config.is_dynamic_config));
        pos += sizeof(tmp_dev_config.is_dynamic_config);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 15:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.led_pwm_config), sizeof(tmp_dev_config.led_pwm_config));
        pos += sizeof(tmp_dev_config.led_pwm_config);
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.leds[0]), MAX_LEDS_NUM*sizeof(led_config_t));
        pos += MAX_LEDS_NUM*sizeof(led_config_t);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    case 16:
        memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_dev_config.encoders[0]), MAX_ENCODERS_NUM*sizeof(encoder_t));
        pos += MAX_ENCODERS_NUM*sizeof(encoder_t);

        USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
        break;

    default:
        break;

    }



    if (config_in_cnt == 16) {
        qDebug() << "All config SENT";
        reportSuccess = 0;
        //    } else if (config_in_cnt == 16) {
        //        qDebug() << "ERRRROORRR!!!!!!!!!!!!! \nERRRROORRR!!!!!!!!!!!!!\n" << "reportSuccess SEND= " << reportSuccess;
        //        reportSuccess = 0;
    }

    std::vector<uint8_t> vec_buf(64);
    for(int i = 0; i < 64; ++i){
        vec_buf[i] = tmp_buf[i];
    }

    return vec_buf;
}


///////////////////////////////////////////////           !!!!!!!!!!!!!!!!           ///////////////////////////////

uint16_t pos_test = 0;
dev_config_t ReportConverter::GetConfigFromDevice(uint8_t * hid_buf)
{
    uint8_t i;
    uint8_t pos = 2;
    //uint8_t tmp_buf_size = 64;
    static dev_config_t tmp_dev_config;

    switch (hid_buf[1])
    {
    case 1:
    {
        memcpy((uint8_t *) &(tmp_dev_config.firmware_version), &hid_buf[pos], sizeof(tmp_dev_config.firmware_version));
        pos += sizeof(tmp_dev_config.firmware_version);
        memcpy((uint8_t *) &(tmp_dev_config.device_name), &hid_buf[pos], sizeof(tmp_dev_config.device_name));
        pos += sizeof(tmp_dev_config.device_name);
        memcpy((uint8_t *) &(tmp_dev_config.button_debounce_ms), &hid_buf[pos], 4);
        pos += 4;
        memcpy((uint8_t *) &(tmp_dev_config.pins), &hid_buf[pos], sizeof(tmp_dev_config.pins));
        pos += sizeof(tmp_dev_config.pins);
    }
        break;

    case 2:
    {
        i = 0;
        while(uint8_t(64 - pos) > sizeof(axis_config_t))
        {
            memcpy((uint8_t *) &(tmp_dev_config.axis_config[i++]), &hid_buf[pos], sizeof(axis_config_t));
            pos += sizeof(axis_config_t);
        }
    }
        break;

    case 3:
    {
        i = 2;
        while(uint8_t(64 - pos) > sizeof(axis_config_t))
        {
            memcpy((uint8_t *) &(tmp_dev_config.axis_config[i++]), &hid_buf[pos], sizeof(axis_config_t));
            pos += sizeof(axis_config_t);
        }
    }
        break;

    case 4:
    {
        i = 4;
        while(uint8_t(64 - pos) > sizeof(axis_config_t))
        {
            memcpy((uint8_t *) &(tmp_dev_config.axis_config[i++]), &hid_buf[pos], sizeof(axis_config_t));
            pos += sizeof(axis_config_t);
        }
    }
        break;

    case 5:
    {
        memcpy((uint8_t *) &(tmp_dev_config.axis_config[6]), &hid_buf[pos], sizeof(axis_config_t));
        pos += sizeof(axis_config_t);
        memcpy((uint8_t *) &(tmp_dev_config.axis_config[7]), &hid_buf[pos], sizeof(axis_config_t));
        pos += sizeof(axis_config_t);

    }
        break;

    case 6:
    {
        memcpy((uint8_t *) &(tmp_dev_config.buttons[0]), &hid_buf[pos], 60);
        memcpy((uint8_t *) &(tmp_dev_config.button_timer1_ms), &hid_buf[64-sizeof(tmp_dev_config.button_timer1_ms)], sizeof(tmp_dev_config.button_timer1_ms));
    }
        break;

    case 7:
    {
        memcpy((uint8_t *) &(tmp_dev_config.buttons[20]), &hid_buf[pos], 60);
        memcpy((uint8_t *) &(tmp_dev_config.button_timer2_ms), &hid_buf[64-sizeof(tmp_dev_config.button_timer2_ms)], sizeof(tmp_dev_config.button_timer2_ms));
    }
        break;

    case 8:
    {
        memcpy((uint8_t *) &(tmp_dev_config.buttons[40]), &hid_buf[pos], 60);
        memcpy((uint8_t *) &(tmp_dev_config.button_timer3_ms), &hid_buf[64-sizeof(tmp_dev_config.button_timer3_ms)], sizeof(tmp_dev_config.button_timer3_ms));
    }
        break;

    case 9:
    {
        memcpy((uint8_t *) &(tmp_dev_config.buttons[60]), &hid_buf[pos], 60);
        memcpy((uint8_t *) &(tmp_dev_config.a2b_debounce_ms), &hid_buf[64-sizeof(tmp_dev_config.a2b_debounce_ms)], sizeof(tmp_dev_config.a2b_debounce_ms));
    }
        break;

    case 10:
    {
        memcpy((uint8_t *) &(tmp_dev_config.buttons[80]), &hid_buf[pos], 60);
    }
        break;

    case 11:
    {
        memcpy((uint8_t *) &(tmp_dev_config.buttons[100]), &hid_buf[pos], 60);
    }
        break;

    case 12:
    {
        memcpy((uint8_t *) &(tmp_dev_config.buttons[120]), &hid_buf[pos], 8*sizeof(button_t));
        pos += 8*sizeof(button_t);

        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[0]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[1]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[2]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
    }
        break;

    case 13:
    {
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[2]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[3]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[4]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[5]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        break;
    }
    case 14:
    {
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[6]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);
        memcpy((uint8_t *) &(tmp_dev_config.axes_to_buttons[7]), &hid_buf[pos], sizeof(axis_to_buttons_t));
        pos += sizeof(axis_to_buttons_t);

        for (i=0; i<4; i++)
        {
            memcpy((uint8_t *) &(tmp_dev_config.shift_registers[i]), &hid_buf[pos], sizeof(shift_reg_config_t));
            pos += sizeof(shift_reg_config_t);
        }


        memcpy((uint8_t *) &(tmp_dev_config.shift_config[0]), &hid_buf[pos], sizeof(shift_modificator_t));
        pos += sizeof(shift_modificator_t);
        memcpy((uint8_t *) &(tmp_dev_config.shift_config[1]), &hid_buf[pos], sizeof(shift_modificator_t));
        pos += sizeof(shift_modificator_t);
        memcpy((uint8_t *) &(tmp_dev_config.shift_config[2]), &hid_buf[pos], sizeof(shift_modificator_t));
        pos += sizeof(shift_modificator_t);
        memcpy((uint8_t *) &(tmp_dev_config.shift_config[3]), &hid_buf[pos], sizeof(shift_modificator_t));
        pos += sizeof(shift_modificator_t);
        memcpy((uint8_t *) &(tmp_dev_config.shift_config[4]), &hid_buf[pos], sizeof(shift_modificator_t));
        pos += sizeof(shift_modificator_t);



        memcpy((uint8_t *) &(tmp_dev_config.vid), &hid_buf[pos], sizeof(tmp_dev_config.vid));
        pos += sizeof(tmp_dev_config.vid);
        memcpy((uint8_t *) &(tmp_dev_config.pid), &hid_buf[pos], sizeof(tmp_dev_config.pid));
        pos += sizeof(tmp_dev_config.pid);
        memcpy((uint8_t *) &(tmp_dev_config.is_dynamic_config), &hid_buf[pos], sizeof(tmp_dev_config.is_dynamic_config));
        pos += sizeof(tmp_dev_config.is_dynamic_config);

    }
        break;

    case 15:
    {
        memcpy((uint8_t *) &(tmp_dev_config.led_pwm_config), &hid_buf[pos], sizeof(tmp_dev_config.led_pwm_config));
        pos += sizeof(tmp_dev_config.led_pwm_config);
        memcpy((uint8_t *) &(tmp_dev_config.leds[0]), &hid_buf[pos], MAX_LEDS_NUM*sizeof(led_config_t));
        pos += MAX_LEDS_NUM*sizeof(led_config_t);
    }
        break;

    case 16:
    {
        memcpy((uint8_t *) &(tmp_dev_config.encoders[0]), &hid_buf[pos], MAX_ENCODERS_NUM*sizeof(encoder_t));
        pos += MAX_ENCODERS_NUM*sizeof(encoder_t);
    }
        break;

    default:
        break;
    }

    if (hid_buf[1] == 16)
    {
        //if (reportSuccess != 16) qDebug() << "ERRRROORRR!!!!!!!!!!!!! \nERRRROORRR!!!!!!!!!!!!!\n" << "reportSuccess = " << reportSuccess;
        //else
        qDebug() << "All config recived. copy to app conf";
        reportSuccess = 0;
    }
    return tmp_dev_config;
}








//void ReportConverter::GetConfigFromDevice(DeviceConfig * device_config, uint8_t * hid_buf)
//dev_config_t ReportConverter::GetConfigFromDevice(uint8_t * hid_buf)
//{
//    //uint8_t i;
//    uint8_t pos = 2;
//    static dev_config_t tmp_dev_config;

//    switch (hid_buf[1])
//    {
//        case 1:
//        {
//            qDebug() << "case 1";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config), &hid_buf[pos], 62);
//            pos_test = 62;
//        }
//            break;

//        case 2:
//        {
//            qDebug() << "case 2";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 124;
//        }
//            break;

//        case 3:
//        {
//            qDebug() << "case 3";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 186;
//        }
//            break;

//        case 4:
//        {
//            qDebug() << "case 4";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 248;
//        }
//            break;

//        case 5:
//        {
//            qDebug() << "case 5";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 310;
//        }
//            break;

//        case 6:
//        {
//            qDebug() << "case 6";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 372;
//        }
//            break;

//        case 7:
//        {
//            qDebug() << "case 7";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 434;
//        }
//            break;

//        case 8:
//        {
//            qDebug() << "case 8";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 496;
//        }
//            break;

//        case 9:
//        {
//            qDebug() << "case 9";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 558;
//        }
//            break;

//        case 10:
//        {
//            qDebug() << "case 10";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 620;
//        }
//            break;

//        case 11:
//        {
//            qDebug() << "case 11";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 682;
//        }
//            break;

//        case 12:
//        {
//            qDebug() << "case 12";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 744;
//        }
//            break;

//        case 13:
//        {
//            qDebug() << "case 13";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 806;
//        }
//            break;
//        case 14:
//        {
//            qDebug() << "case 14";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 868;
//        }
//            break;

//        case 15:
//        {
//            qDebug() << "case 15";
//            reportSuccess++;
//            memcpy((uint8_t *) &(tmp_dev_config)+pos_test, &hid_buf[pos], 62);
//            pos_test = 0;
//        }
//            break;

//        default:
//            break;
//    }

//    if (hid_buf[1] == 15)
//    {
//        if (reportSuccess != 15) qDebug() << "ERRRROORRR!!!!!!!!!!!!! \nERRRROORRR!!!!!!!!!!!!!\n" << "reportSuccess = " << reportSuccess;
//        else qDebug() << "All config recived. copy to app conf";
//        reportSuccess = 0;
//    }
//    return tmp_dev_config;
//}
