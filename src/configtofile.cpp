#include "configtofile.h"
#include <QSettings>
#include <QMessageBox>
#include "common_defines.h"

#include <QDebug>

static const int SHIFT_COUNT = 6;
static const int MAX_A2B_BUTTONS = 12;

void ConfigToFile::loadDeviceConfigFromFile(QWidget *parent, const QString &fileName, dev_config_t &devC)
{
    qDebug()<<"Load device config from file";
    QSettings deviceSettings(fileName, QSettings::IniFormat);
    bool tmp;

    // load Device USB config from file
    deviceSettings.beginGroup("DeviceUsbConfig");

    devC.firmware_version = QString(deviceSettings.value("FirmwareVersion", devC.firmware_version).toString()).toUShort(&tmp ,16);
    std::string tmp_string = deviceSettings.value("DeviceName", QString(devC.device_name)).toString().toStdString();
    for (uint i = 0; i < sizeof(devC.device_name); i++) {
        if (i < tmp_string.size()){
            devC.device_name[i] = tmp_string[i];
        } else {
            devC.device_name[i] = '\0';
        }
    }

    devC.vid = QString(deviceSettings.value("Vid", QString::number(devC.vid, 16)).toString()).toUShort(&tmp ,16);
    devC.pid = QString(deviceSettings.value("Pid", QString::number(devC.pid, 16)).toString()).toUShort(&tmp ,16);
    devC.exchange_period_ms = uint8_t(deviceSettings.value("USBExchange", devC.exchange_period_ms).toInt());
    deviceSettings.endGroup();

    // load Pins config from file
    deviceSettings.beginGroup("PinsConfig");

    devC.pins[0] = int8_t(deviceSettings.value("A0", devC.pins[0]).toInt());
    devC.pins[1] = int8_t(deviceSettings.value("A1", devC.pins[1]).toInt());
    devC.pins[2] = int8_t(deviceSettings.value("A2", devC.pins[2]).toInt());
    devC.pins[3] = int8_t(deviceSettings.value("A3", devC.pins[3]).toInt());
    devC.pins[4] = int8_t(deviceSettings.value("A4", devC.pins[4]).toInt());
    devC.pins[5] = int8_t(deviceSettings.value("A5", devC.pins[5]).toInt());
    devC.pins[6] = int8_t(deviceSettings.value("A6", devC.pins[6]).toInt());
    devC.pins[7] = int8_t(deviceSettings.value("A7", devC.pins[7]).toInt());
    devC.pins[8] = int8_t(deviceSettings.value("A8", devC.pins[8]).toInt());
    devC.pins[9] = int8_t(deviceSettings.value("A9", devC.pins[9]).toInt());
    devC.pins[10] = int8_t(deviceSettings.value("A10", devC.pins[10]).toInt());
    devC.pins[11] = int8_t(deviceSettings.value("A15", devC.pins[11]).toInt());
    devC.pins[12] = int8_t(deviceSettings.value("B0", devC.pins[12]).toInt());
    devC.pins[13] = int8_t(deviceSettings.value("B1", devC.pins[13]).toInt());
    devC.pins[14] = int8_t(deviceSettings.value("B3", devC.pins[14]).toInt());
    devC.pins[15] = int8_t(deviceSettings.value("B4", devC.pins[15]).toInt());
    devC.pins[16] = int8_t(deviceSettings.value("B5", devC.pins[16]).toInt());
    devC.pins[17] = int8_t(deviceSettings.value("B6", devC.pins[17]).toInt());
    devC.pins[18] = int8_t(deviceSettings.value("B7", devC.pins[18]).toInt());
    devC.pins[19] = int8_t(deviceSettings.value("B8", devC.pins[19]).toInt());
    devC.pins[20] = int8_t(deviceSettings.value("B9", devC.pins[20]).toInt());
    devC.pins[21] = int8_t(deviceSettings.value("B10", devC.pins[21]).toInt());
    devC.pins[22] = int8_t(deviceSettings.value("B11", devC.pins[22]).toInt());
    devC.pins[23] = int8_t(deviceSettings.value("B12", devC.pins[23]).toInt());
    devC.pins[24] = int8_t(deviceSettings.value("B13", devC.pins[24]).toInt());
    devC.pins[25] = int8_t(deviceSettings.value("B14", devC.pins[25]).toInt());
    devC.pins[26] = int8_t(deviceSettings.value("B15", devC.pins[26]).toInt());
    devC.pins[27] = int8_t(deviceSettings.value("C13", devC.pins[27]).toInt());
    devC.pins[28] = int8_t(deviceSettings.value("C14", devC.pins[28]).toInt());
    devC.pins[29] = int8_t(deviceSettings.value("C15", devC.pins[29]).toInt());
    deviceSettings.endGroup();

    // load Shift config from file
    deviceSettings.beginGroup("ShiftConfig");
    for (int i = 0; i < SHIFT_COUNT - 1; ++i) { // -1 "No shift"    (SHIFT_COUNT = shift_count + "No shift")
        devC.shift_config[i].button = int8_t(deviceSettings.value("Shift" + QString::number(i), devC.shift_config[i].button).toInt());
    }
    deviceSettings.endGroup();

    // load Timer config from file
    deviceSettings.beginGroup("TimersConfig");

    devC.button_timer1_ms = uint16_t(deviceSettings.value("Timer1", devC.button_timer1_ms).toInt());   // вроде и без (int16_t) работает
    devC.button_timer2_ms = uint16_t(deviceSettings.value("Timer2", devC.button_timer2_ms).toInt());
    devC.button_timer3_ms = uint16_t(deviceSettings.value("Timer3", devC.button_timer3_ms).toInt());
    devC.button_debounce_ms = uint16_t(deviceSettings.value("Debounce", devC.button_debounce_ms).toInt());
    devC.encoder_press_time_ms = uint8_t(deviceSettings.value("EncoderPress", devC.encoder_press_time_ms).toInt());
    deviceSettings.endGroup();

    // load Buttons config from file
    for (int i = 0; i < MAX_BUTTONS_NUM; ++i) {
        deviceSettings.beginGroup("ButtonsConfig_" + QString::number(i));

        devC.buttons[i].physical_num = int8_t(deviceSettings.value("ButtonPhysicNumber", devC.buttons[i].physical_num).toInt());
        devC.buttons[i].type = uint8_t(deviceSettings.value("ButtonType", devC.buttons[i].type).toInt());
        devC.buttons[i].shift_modificator = uint8_t(deviceSettings.value("ShiftMod", devC.buttons[i].shift_modificator).toInt());
        devC.buttons[i].is_inverted = uint8_t(deviceSettings.value("Inverted", devC.buttons[i].is_inverted).toInt());
        devC.buttons[i].is_disabled = uint8_t(deviceSettings.value("Disabled", devC.buttons[i].is_disabled).toInt());
        devC.buttons[i].delay_timer = uint8_t(deviceSettings.value("DelayTimer", devC.buttons[i].delay_timer).toInt());
        devC.buttons[i].press_timer = uint8_t(deviceSettings.value("PressTimer", devC.buttons[i].press_timer).toInt());
        deviceSettings.endGroup();
    }

    // load Axes config from file
    for (int i = 0; i < MAX_AXIS_NUM; ++i)
    {
        deviceSettings.beginGroup("AxesConfig_" + QString::number(i));

        devC.axis_config[i].calib_min = int16_t(deviceSettings.value("CalibMin", devC.axis_config[i].calib_min).toInt());
        devC.axis_config[i].calib_center = int16_t(deviceSettings.value("CalibCenter", devC.axis_config[i].calib_center).toInt());
        devC.axis_config[i].calib_max = int16_t(deviceSettings.value("CalibMax", devC.axis_config[i].calib_max).toInt());
        devC.axis_config[i].is_centered = uint8_t(deviceSettings.value("IsCentered", devC.axis_config[i].is_centered).toInt());

        devC.axis_config[i].out_enabled = uint8_t(deviceSettings.value("OutEnabled", devC.axis_config[i].out_enabled).toInt());
        devC.axis_config[i].inverted = uint8_t(deviceSettings.value("Inverted", devC.axis_config[i].inverted).toInt());
        devC.axis_config[i].function = uint8_t(deviceSettings.value("Function", devC.axis_config[i].function).toInt());
        devC.axis_config[i].filter = uint8_t(deviceSettings.value("Filter", devC.axis_config[i].filter).toInt());

        devC.axis_config[i].resolution = uint8_t(deviceSettings.value("Resolution", devC.axis_config[i].resolution).toInt());
        devC.axis_config[i].channel = uint8_t(deviceSettings.value("Channel", devC.axis_config[i].channel).toInt());
        devC.axis_config[i].deadband_size = uint8_t(deviceSettings.value("DeadbandSize", devC.axis_config[i].deadband_size).toInt());
        devC.axis_config[i].is_dynamic_deadband = uint8_t(deviceSettings.value("DynDeadbandEnabled", devC.axis_config[i].is_dynamic_deadband).toInt());

        devC.axis_config[i].source_main = int8_t(deviceSettings.value("SourceMain", devC.axis_config[i].source_main).toInt());
        devC.axis_config[i].source_secondary = uint8_t(deviceSettings.value("SourceSecond", devC.axis_config[i].source_secondary).toInt());
        devC.axis_config[i].offset_angle = uint8_t(deviceSettings.value("OffsetAngle", devC.axis_config[i].offset_angle).toInt());

        devC.axis_config[i].button1 = int8_t(deviceSettings.value("Button1", devC.axis_config[i].button1).toInt());
        devC.axis_config[i].button2 = int8_t(deviceSettings.value("Button2", devC.axis_config[i].button2).toInt());
        devC.axis_config[i].button3 = int8_t(deviceSettings.value("Button3", devC.axis_config[i].button3).toInt());
        devC.axis_config[i].divider = uint8_t(deviceSettings.value("Divider", devC.axis_config[i].divider).toInt());
        devC.axis_config[i].i2c_address = uint8_t(deviceSettings.value("I2cAddress", devC.axis_config[i].i2c_address).toInt());
        devC.axis_config[i].button1_type = uint8_t(deviceSettings.value("Button1Type", devC.axis_config[i].button1_type).toInt());
        devC.axis_config[i].button2_type = uint8_t(deviceSettings.value("Button2Type", devC.axis_config[i].button2_type).toInt());
        devC.axis_config[i].button3_type = uint8_t(deviceSettings.value("Button3Type", devC.axis_config[i].button3_type).toInt());
        devC.axis_config[i].prescaler = uint8_t(deviceSettings.value("Prescaler", devC.axis_config[i].prescaler).toInt());
        deviceSettings.endGroup();
        // axes curves
        deviceSettings.beginGroup("AxesCurvesConfig_" + QString::number(i));
        for (int j = 0; j < 11; ++j) {      // 11 - axis curve points count
            devC.axis_config[i].curve_shape[j] = int8_t(deviceSettings.value("Point_" + QString::number(j), devC.axis_config[i].curve_shape[j]).toInt());
        }
        deviceSettings.endGroup();
    }

    // load Axes To Buttons config from file
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        deviceSettings.beginGroup("Axes2bConfig_" + QString::number(i));

        devC.axes_to_buttons[i].buttons_cnt = uint8_t(deviceSettings.value("ButtonsCount", devC.axes_to_buttons[i].buttons_cnt).toInt());
        for (int j = 0; j < MAX_A2B_BUTTONS + 1; ++j) {
            devC.axes_to_buttons[i].points[j] = uint8_t(deviceSettings.value("Point_" + QString::number(j), devC.axes_to_buttons[i].points[j]).toInt());
        }
        deviceSettings.endGroup();
    }

    // load Shift Registers config from file
    for (int i = 0; i < MAX_SHIFT_REG_NUM; ++i) {
        deviceSettings.beginGroup("ShiftRegsConfig_" + QString::number(i));

        devC.shift_registers[i].type = uint8_t(deviceSettings.value("ShiftType", devC.shift_registers[i].type).toInt());
        devC.shift_registers[i].button_cnt = uint8_t(deviceSettings.value("ButtonCnt", devC.shift_registers[i].button_cnt).toInt());
        deviceSettings.endGroup();
    }

    // load Encoders config from file
    for (int i = 0; i < MAX_ENCODERS_NUM; ++i) {
        deviceSettings.beginGroup("EncodersConfig_" + QString::number(i));

        devC.encoders[i] = uint8_t(deviceSettings.value("EncType", devC.encoders[i]).toInt());
        deviceSettings.endGroup();
    }

    // load LEDs config from file
    deviceSettings.beginGroup("LedsPWMConfig");
    devC.led_pwm_config[0].duty_cycle = uint8_t(deviceSettings.value("PinPA8", devC.led_pwm_config[0].duty_cycle).toInt());
    devC.led_pwm_config[1].duty_cycle = uint8_t(deviceSettings.value("PinPB0", devC.led_pwm_config[1].duty_cycle).toInt());
    devC.led_pwm_config[2].duty_cycle = uint8_t(deviceSettings.value("PinPB1", devC.led_pwm_config[2].duty_cycle).toInt());
    devC.led_pwm_config[3].duty_cycle = uint8_t(deviceSettings.value("PinPB4", devC.led_pwm_config[3].duty_cycle).toInt());

    devC.led_pwm_config[0].is_axis = deviceSettings.value("PinPA8_AxisEnabled", devC.led_pwm_config[0].is_axis).toBool();
    devC.led_pwm_config[1].is_axis = deviceSettings.value("PinPB0_AxisEnabled", devC.led_pwm_config[1].is_axis).toBool();
    devC.led_pwm_config[2].is_axis = deviceSettings.value("PinPB1_AxisEnabled", devC.led_pwm_config[2].is_axis).toBool();
    devC.led_pwm_config[3].is_axis = deviceSettings.value("PinPB4_AxisEnabled", devC.led_pwm_config[3].is_axis).toBool();

    devC.led_pwm_config[0].axis_num = uint8_t(deviceSettings.value("PinPA8_AxisNum", devC.led_pwm_config[0].axis_num).toInt());
    devC.led_pwm_config[1].axis_num = uint8_t(deviceSettings.value("PinPB0_AxisNum", devC.led_pwm_config[1].axis_num).toInt());
    devC.led_pwm_config[2].axis_num = uint8_t(deviceSettings.value("PinPB1_AxisNum", devC.led_pwm_config[2].axis_num).toInt());
    devC.led_pwm_config[3].axis_num = uint8_t(deviceSettings.value("PinPB4_AxisNum", devC.led_pwm_config[3].axis_num).toInt());
    deviceSettings.endGroup();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        deviceSettings.beginGroup("LedsConfig_" + QString::number(i));

        devC.leds[i].input_num = int8_t(deviceSettings.value("InputNum", devC.leds[i].input_num).toInt());
        devC.leds[i].type = uint8_t(deviceSettings.value("LedType", devC.leds[i].type).toInt());
        deviceSettings.endGroup();
    }

    oldConfigHandler(parent, devC);
}


void ConfigToFile::oldConfigHandler(QWidget *parent, dev_config_t &devC)
{
    if (devC.firmware_version != FIRMWARE_VERSION) {
        qDebug()<<"Firmware warning";
        if ((devC.firmware_version & 0xFFF0) == 0x1620)
        {
            if (devC.pins[19] == I2C_SCL || devC.pins[20] == I2C_SDA) {
                QString warning(tr("Firmware version in config file doesnt match configurator version. Check settings before write config."));
                QString differences(tr("Pins B8, B9 reset! In this version I2C moved from pins B8, B9 to B10, B11. Check it!"));
                QMessageBox::warning(parent, tr("Firmware version!"), warning + " " + differences);
                devC.pins[19] = devC.pins[20] = NOT_USED;
            } else {
                //QMessageBox::information(this, tr("Firmware version!"), warning);
                qDebug()<<"Loaded old config file";
            }
            for (int i = 0; i < MAX_AXIS_NUM; ++i) {
                if (devC.axes_to_buttons[i].buttons_cnt == 1) {
                    devC.axes_to_buttons[i].buttons_cnt = 0;
                }
            }
        }
        devC.firmware_version = FIRMWARE_VERSION;
    }
}



void ConfigToFile::saveDeviceConfigToFile(const QString &fileName, dev_config_t &devC)
{
    qDebug()<<"SaveDeviceConfigToFile() started";
    QSettings deviceSettings(fileName, QSettings::IniFormat);
    if (deviceSettings.isWritable() == false) {
        qDebug()<<"SaveDeviceConfigToFile() failed."<<fileName<<"Read-Only";
        return;
    }

    // save Device USB config to file
    deviceSettings.beginGroup("DeviceUsbConfig");
    deviceSettings.setValue("FirmwareVersion", QString::number(devC.firmware_version, 16));
    deviceSettings.setValue("DeviceName", QString(devC.device_name));
    deviceSettings.setValue("Vid", QString::number(devC.vid, 16));
    deviceSettings.setValue("Pid", QString::number(devC.pid, 16));
    deviceSettings.setValue("USBExchange", devC.exchange_period_ms);
    deviceSettings.endGroup();
    // save Pins config to file
    deviceSettings.beginGroup("PinsConfig");        // for(;;) x3
    deviceSettings.setValue("A0", devC.pins[0]);
    deviceSettings.setValue("A1", devC.pins[1]);
    deviceSettings.setValue("A2", devC.pins[2]);
    deviceSettings.setValue("A3", devC.pins[3]);
    deviceSettings.setValue("A4", devC.pins[4]);
    deviceSettings.setValue("A5", devC.pins[5]);
    deviceSettings.setValue("A6", devC.pins[6]);
    deviceSettings.setValue("A7", devC.pins[7]);
    deviceSettings.setValue("A8", devC.pins[8]);
    deviceSettings.setValue("A9", devC.pins[9]);
    deviceSettings.setValue("A10", devC.pins[10]);
    deviceSettings.setValue("A15", devC.pins[11]);
    deviceSettings.setValue("B0", devC.pins[12]);
    deviceSettings.setValue("B1", devC.pins[13]);
    deviceSettings.setValue("B3", devC.pins[14]);
    deviceSettings.setValue("B4", devC.pins[15]);
    deviceSettings.setValue("B5", devC.pins[16]);
    deviceSettings.setValue("B6", devC.pins[17]);
    deviceSettings.setValue("B7", devC.pins[18]);
    deviceSettings.setValue("B8", devC.pins[19]);
    deviceSettings.setValue("B9", devC.pins[20]);
    deviceSettings.setValue("B10", devC.pins[21]);
    deviceSettings.setValue("B11", devC.pins[22]);
    deviceSettings.setValue("B12", devC.pins[23]);
    deviceSettings.setValue("B13", devC.pins[24]);
    deviceSettings.setValue("B14", devC.pins[25]);
    deviceSettings.setValue("B15", devC.pins[26]);
    deviceSettings.setValue("C13", devC.pins[27]);
    deviceSettings.setValue("C14", devC.pins[28]);
    deviceSettings.setValue("C15", devC.pins[29]);
    deviceSettings.endGroup();

    // save Shift config to file
    deviceSettings.beginGroup("ShiftConfig");
    for (int i = 0; i < SHIFT_COUNT - 1; ++i) { // -1 "No shift"    (SHIFT_COUNT = shift_count + "No shift")
        deviceSettings.setValue("Shift" + QString::number(i), devC.shift_config[i].button);
    }
    deviceSettings.endGroup();

    // save Timer config to file
    deviceSettings.beginGroup("TimersConfig");

    deviceSettings.setValue("Timer1", devC.button_timer1_ms);
    deviceSettings.setValue("Timer2", devC.button_timer2_ms);
    deviceSettings.setValue("Timer3", devC.button_timer3_ms);
    deviceSettings.setValue("Debounce", devC.button_debounce_ms);
    deviceSettings.setValue("EncoderPress", devC.encoder_press_time_ms);
    deviceSettings.endGroup();

    // save Buttons config to file
    for (int i = 0; i < MAX_BUTTONS_NUM; ++i) {
        deviceSettings.beginGroup("ButtonsConfig_" + QString::number(i));

        deviceSettings.setValue("ButtonPhysicNumber", devC.buttons[i].physical_num);
        deviceSettings.setValue("ButtonType", devC.buttons[i].type);
        deviceSettings.setValue("ShiftMod", devC.buttons[i].shift_modificator);
        deviceSettings.setValue("Inverted", devC.buttons[i].is_inverted);
        deviceSettings.setValue("Disabled", devC.buttons[i].is_disabled);
        deviceSettings.setValue("DelayTimer", devC.buttons[i].delay_timer);
        deviceSettings.setValue("PressTimer", devC.buttons[i].press_timer);
        deviceSettings.endGroup();
    }

    // save Axes config to file
    for (int i = 0; i < MAX_AXIS_NUM; ++i)
    {
        deviceSettings.beginGroup("AxesConfig_" + QString::number(i));

        deviceSettings.setValue("CalibMin", devC.axis_config[i].calib_min);
        deviceSettings.setValue("CalibCenter", devC.axis_config[i].calib_center);
        deviceSettings.setValue("CalibMax", devC.axis_config[i].calib_max);
        deviceSettings.setValue("IsCentered", devC.axis_config[i].is_centered);

        deviceSettings.setValue("OutEnabled", devC.axis_config[i].out_enabled);
        deviceSettings.setValue("Inverted", devC.axis_config[i].inverted);
        deviceSettings.setValue("Function", devC.axis_config[i].function);
        deviceSettings.setValue("Filter", devC.axis_config[i].filter);

        deviceSettings.setValue("Resolution", devC.axis_config[i].resolution);
        deviceSettings.setValue("Channel", devC.axis_config[i].channel);
        deviceSettings.setValue("DeadbandSize", devC.axis_config[i].deadband_size);
        deviceSettings.setValue("DynDeadbandEnabled", devC.axis_config[i].is_dynamic_deadband);

        deviceSettings.setValue("SourceMain", devC.axis_config[i].source_main);
        deviceSettings.setValue("SourceSecond", devC.axis_config[i].source_secondary);
        deviceSettings.setValue("OffsetAngle", devC.axis_config[i].offset_angle);

        deviceSettings.setValue("Button1", devC.axis_config[i].button1);
        deviceSettings.setValue("Button2", devC.axis_config[i].button2);
        deviceSettings.setValue("Button3", devC.axis_config[i].button3);
        deviceSettings.setValue("Divider", devC.axis_config[i].divider);
        deviceSettings.setValue("I2cAddress", devC.axis_config[i].i2c_address);
        deviceSettings.setValue("Button1Type", devC.axis_config[i].button1_type);
        deviceSettings.setValue("Button2Type", devC.axis_config[i].button2_type);
        deviceSettings.setValue("Button3Type", devC.axis_config[i].button3_type);
        deviceSettings.setValue("Prescaler", devC.axis_config[i].prescaler);
        deviceSettings.endGroup();
        // axes curves
        deviceSettings.beginGroup("AxesCurvesConfig_" + QString::number(i));
        for (int j = 0; j < 11; ++j) {      // 11 - axis curve points count
            deviceSettings.setValue("Point_" + QString::number(j), devC.axis_config[i].curve_shape[j]);
        }
        deviceSettings.endGroup();
    }

    // save Axes To Buttons config to file
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        deviceSettings.beginGroup("Axes2bConfig_" + QString::number(i));

        deviceSettings.setValue("ButtonsCount", devC.axes_to_buttons[i].buttons_cnt);
        for (int j = 0; j < MAX_A2B_BUTTONS + 1; ++j) {
            deviceSettings.setValue("Point_" + QString::number(j), devC.axes_to_buttons[i].points[j]);
        }
        deviceSettings.endGroup();
    }

    // save Shift Registers config to file
    for (int i = 0; i < MAX_SHIFT_REG_NUM; ++i) {
        deviceSettings.beginGroup("ShiftRegsConfig_" + QString::number(i));

        deviceSettings.setValue("ShiftType", devC.shift_registers[i].type);
        deviceSettings.setValue("ButtonCnt", devC.shift_registers[i].button_cnt);
        deviceSettings.endGroup();
    }

    // save Encoders config to file
    for (int i = 0; i < MAX_ENCODERS_NUM; ++i) {
        deviceSettings.beginGroup("EncodersConfig_" + QString::number(i));

        deviceSettings.setValue("EncType", devC.encoders[i]);
        deviceSettings.endGroup();
    }

    // save LEDs config to file
    deviceSettings.beginGroup("LedsPWMConfig");
    deviceSettings.setValue("PinPA8", devC.led_pwm_config[0].duty_cycle);
    deviceSettings.setValue("PinPB0", devC.led_pwm_config[1].duty_cycle);
    deviceSettings.setValue("PinPB1", devC.led_pwm_config[2].duty_cycle);
    deviceSettings.setValue("PinPB4", devC.led_pwm_config[3].duty_cycle);

    deviceSettings.setValue("PinPA8_AxisEnabled", devC.led_pwm_config[0].is_axis);
    deviceSettings.setValue("PinPB0_AxisEnabled", devC.led_pwm_config[1].is_axis);
    deviceSettings.setValue("PinPB1_AxisEnabled", devC.led_pwm_config[2].is_axis);
    deviceSettings.setValue("PinPB4_AxisEnabled", devC.led_pwm_config[3].is_axis);

    deviceSettings.setValue("PinPA8_AxisNum", devC.led_pwm_config[0].axis_num);
    deviceSettings.setValue("PinPB0_AxisNum", devC.led_pwm_config[1].axis_num);
    deviceSettings.setValue("PinPB1_AxisNum", devC.led_pwm_config[2].axis_num);
    deviceSettings.setValue("PinPB4_AxisNum", devC.led_pwm_config[3].axis_num);
    deviceSettings.endGroup();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        deviceSettings.beginGroup("LedsConfig_" + QString::number(i));

        deviceSettings.setValue("InputNum", devC.leds[i].input_num);
        deviceSettings.setValue("LedType", devC.leds[i].type);
        deviceSettings.endGroup();
    }
    qDebug()<<"SaveDeviceConfigToFile() finished";
}
