#include "buttonconfig.h"
#include "ui_buttonconfig.h"
#include <QTimer>
#include <QSettings>
#include <QDebug>

#ifdef DYNAMIC_CREATION
    #include <QScrollArea>
    #include <QScrollBar>
#endif

static const int TICKS_NS = 500;

ButtonConfig::ButtonConfig(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ButtonConfig)
{
    ui->setupUi(this);
    m_logicButtonInFocus = -1;
    m_isShifts_act = false;
    m_shift1_act = false;
    m_shift2_act = false;
    m_shift3_act = false;
    m_shift4_act = false;
    m_shift5_act = false;

    // dynamic creation with scroll
#ifdef DYNAMIC_CREATION
    connect(ui->scrollArea_LogButtons->verticalScrollBar(), &QScrollBar::valueChanged, this, &ButtonConfig::logScrollValueChanged);
#else
    for (int i = 0; i < MAX_BUTTONS_NUM; i++) {
        ButtonLogical *logicalButtonsWidget = new ButtonLogical(i, this);
        ui->layoutV_LogicalButton->addWidget(logicalButtonsWidget);
        m_logicButtonPtrList.append(logicalButtonsWidget);
        connect(m_logicButtonPtrList[i], &ButtonLogical::functionTypeChanged,
                this, &ButtonConfig::functionTypeChanged);
    }
    gEnv.pAppSettings->beginGroup("OtherSettings");
    ui->checkBox_AutoPhysBut->setChecked(gEnv.pAppSettings->value("AutoSetPhysButton", true).toBool());
    gEnv.pAppSettings->endGroup();
    on_checkBox_AutoPhysBut_toggled(ui->checkBox_AutoPhysBut->isChecked());
#endif
    logicaButtonsCreator();

    connect(ui->spinBox_ButtonsPolling, qOverload<int>(&QSpinBox::valueChanged), this, &ButtonConfig::spinBoxStep);
    connect(ui->spinBox_EncoderPolling, qOverload<int>(&QSpinBox::valueChanged), this, &ButtonConfig::spinBoxStep);

    Q_ASSERT(ui->groupBox_LogicalButtons->objectName() == QStringLiteral("groupBox_LogicalButtons"));
    Q_ASSERT(ui->groupBox_PhysicalButtons->objectName() == QStringLiteral("groupBox_PhysicalButtons"));
}

ButtonConfig::~ButtonConfig()
{
    gEnv.pAppSettings->beginGroup("OtherSettings");
    gEnv.pAppSettings->setValue("AutoSetPhysButton", ui->checkBox_AutoPhysBut->isChecked());
    gEnv.pAppSettings->endGroup();
    delete ui;
}

void ButtonConfig::retranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < m_logicButtonPtrList.size(); ++i) {
        m_logicButtonPtrList[i]->retranslateUi();
    }
}


#ifdef DYNAMIC_CREATION
void ButtonConfig::createLogButtons(int count)
{
    int size = m_logicButtonPtrList.size();
    for (int i = 0; i < count; i++) {
        if (size == MAX_BUTTONS_NUM) {
            break;
        }
        ButtonLogical *logical_buttons_widget = new ButtonLogical(size, this);
        ui->layoutV_LogicalButton->addWidget(logical_buttons_widget);
        m_logicButtonPtrList.append(logical_buttons_widget);
        //m_logicButtonPtrList[tmp]->setMinimumHeight(30);
        m_logicButtonPtrList[size]->initialization();

        connect(m_logicButtonPtrList[size], SIGNAL(functionIndexChanged(int, int, int)),
                this, SLOT(functionTypeChanged(int, int, int)));
        size++;
    }
}
void ButtonConfig::logScrollValueChanged(int value)
{
    // dynamic creation with scroll
    QScrollBar *scroll = ui->scrollArea_LogButtons->verticalScrollBar();
    int size = m_logicButtonPtrList.size();
    if (size != MAX_BUTTONS_NUM && (value / float(scroll->maximum())) >= 1.0f)
    {
        createLogButtons(4);
    }
}
void ButtonConfig::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // dynamic creation with scroll
    QScrollBar *scroll = ui->scrollArea_LogButtons->verticalScrollBar();
    int size = m_logicButtonPtrList.size();
    if (isVisible() && scroll->isVisible() == false && size != MAX_BUTTONS_NUM)
    {
        createLogButtons(4);
    }
}
#endif
// dynamic creation of widgets. its decrease app startup time
void ButtonConfig::logicaButtonsCreator()
{
    static int tmp = 0;     //
    static QElapsedTimer timer;
    if (tmp >= MAX_BUTTONS_NUM) {
        if (MAX_BUTTONS_NUM != 128) {
            qCritical() << "buttonconfig.cpp MAX_BUTTONS_NUM != 128";
        }
        qDebug() << "LogicaButtonsCreator() finished in"<<timer.elapsed()<<"ms";
        emit logicalButtonsCreated();
        return;
    }
    // как я понял таймер срабатывает после полной загрузки приложения(оно отобразится)
    // т.к. в LogicaButtonsCreator заходит при инициализации, но срабатывает после запуска приложения
    QTimer::singleShot(10, this, [&] {
        if (tmp == 0) {
            timer.start();
        }
        // dynamic creation with scroll
#ifdef DYNAMIC_CREATION
        createLogButtons(8);
        tmp += 8;
        ui->layoutV_LogicalButton->activate();
        // stop creating after QScrollBar is visible
        // 30 its ButtonLogical height
        #ifndef DYNAMIC_CREATION_ALL
        if (tmp * 30 > window()->height()) {
            qDebug() << "LogicaButtonsCreator() finished in"<<timer.elapsed()<<"ms";
            emit logicalButtonsCreated();
            return;
        }
        gEnv.pAppSettings->beginGroup("OtherSettings");
        ui->checkBox_AutoPhysBut->setChecked(gEnv.pAppSettings->value("AutoSetPhysButton", true).toBool());
        gEnv.pAppSettings->endGroup();
        on_checkBox_AutoPhysBut_toggled(ui->checkBox_AutoPhysBut->isChecked());
        #endif
#else
        // MAX_BUTTONS_NUM(128)/8 = 16 ДОЛЖНО ДЕЛИТЬСЯ БЕЗ ОСТАТКА
        for (int i = 0; i < MAX_BUTTONS_NUM; i++) // old value =8 // useless atm
        {
            m_logicButtonPtrList[tmp]->initialization();
            tmp++;
        }
#endif
        logicaButtonsCreator();
    });
}

// single step for spinbox
void ButtonConfig::spinBoxStep(int value)
{
    int remainder = value % TICKS_NS;
    QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
    if (sb == nullptr || remainder == 0) return;

    value = (value / TICKS_NS) * TICKS_NS;
    if (remainder > TICKS_NS / 2) {
        sb->setValue(value + TICKS_NS);
    } else {
        sb->setValue(value);
    }
}

// set physical button for focused logical button
void ButtonConfig::setPhysicButton(int buttonIndex)
{
    if (m_autoPhysButEnabled) {
        int buttonInFocus = m_logicButtonPtrList[0]->currentFocus();
        if (buttonInFocus >= 0) {
            m_logicButtonPtrList[buttonInFocus]->setPhysicButton(buttonIndex);
        }
    }
}

void ButtonConfig::on_checkBox_AutoPhysBut_toggled(bool checked)
{
    m_autoPhysButEnabled = checked;
    m_logicButtonPtrList[0]->setAutoPhysBut(checked);
}

void ButtonConfig::physButtonsCreator(int count)
{
    // delete all
    while (!m_PhysButtonPtrList.empty()) {
        QWidget *widget = m_PhysButtonPtrList.takeLast();
        ui->layoutG_PhysicalButton->removeWidget(widget);
        widget->deleteLater();
    }
    // add
    int row = 0;
    int column = 0;
    ui->layoutG_PhysicalButton->setAlignment(Qt::AlignTop);
    for (int i = 0; i < count; i++) {
        if (column >= 8) // phys buttons column
        {
            row++;
            column = 0;
        }
        ButtonPhysical *physicalButtonWidget = new ButtonPhysical(i, this);
        ui->layoutG_PhysicalButton->addWidget(physicalButtonWidget, row, column);
        m_PhysButtonPtrList.append(physicalButtonWidget);
        column++;
        connect(physicalButtonWidget, &ButtonPhysical::physButtonPressed, this, &ButtonConfig::setPhysicButton);
    }
}

void ButtonConfig::functionTypeChanged(button_type_t current, button_type_t previous, int buttonIndex)
{
    if (current == ENCODER_INPUT_A) {
        emit encoderInputChanged(buttonIndex + 1, 0);
    } else if (current == ENCODER_INPUT_B) {
        emit encoderInputChanged(0, buttonIndex + 1);
    }

    if (previous == ENCODER_INPUT_A) {
        emit encoderInputChanged((buttonIndex + 1) * -1, 0); // send negative number
    } else if (previous == ENCODER_INPUT_B) {
        emit encoderInputChanged(0, (buttonIndex + 1) * -1);
    }
    typeLimit(current, previous);
}

void ButtonConfig::typeLimit(button_type_t current, button_type_t previous)
{
    static int limitCountArray[m_typeLimCount]{};
    static bool limitIsEnable[m_typeLimCount]{};

    for (int i = 0; i < m_typeLimCount; ++i)
    {
        if (current == m_ButtonsTypeLimit[i].type)
        {
            limitCountArray[i]++;
        }
        if (previous == m_ButtonsTypeLimit[i].type)
        {
            limitCountArray[i]--;
        }

        if (limitCountArray[i] >= m_ButtonsTypeLimit[i].maxCount && limitIsEnable[i] == false)
        {
            limitIsEnable[i] = true;
            for (int j = 0; j < m_logicButtonPtrList.size(); ++j)
            {
                if (m_logicButtonPtrList[j]->currentButtonType() != current)
                {
                    m_logicButtonPtrList[j]->disableButtonType(current, true);
                }
            }
        }

        if (limitIsEnable[i] == true && limitCountArray[i] < m_ButtonsTypeLimit[i].maxCount)
        {
            limitIsEnable[i] = false;
            for (int j = 0; j < m_logicButtonPtrList.size(); ++j)
            {
                m_logicButtonPtrList[j]->disableButtonType(previous, false);
            }
        }
    }
}

void ButtonConfig::setUiOnOff(int value)
{
    if (value > 0) {
        ui->spinBox_Shift1->setEnabled(true);
        ui->spinBox_Shift2->setEnabled(true);
        ui->spinBox_Shift3->setEnabled(true);
        ui->spinBox_Shift4->setEnabled(true);
        ui->spinBox_Shift5->setEnabled(true);
    } else {
        ui->spinBox_Shift1->setEnabled(false);
        ui->spinBox_Shift2->setEnabled(false);
        ui->spinBox_Shift3->setEnabled(false);
        ui->spinBox_Shift4->setEnabled(false);
        ui->spinBox_Shift5->setEnabled(false);
    }
    for (int i = 0; i < m_logicButtonPtrList.size(); ++i) {
        m_logicButtonPtrList[i]->setSpinBoxOnOff(value);
        m_logicButtonPtrList[i]->setMaxPhysButtons(value);
    }

    physButtonsCreator(value);
}

void ButtonConfig::buttonStateChanged()
{
    int number = 0;
    params_report_t *paramsRep = &gEnv.pDeviceConfig->paramsReport;

    // logical buttons state
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            number = j + (i *8);
            if (number < m_logicButtonPtrList.size()) {
                if ((paramsRep->log_button_data[i] & (1 << (j & 0x07)))) {
                    m_logicButtonPtrList[number]->setButtonState(true);
                } else if ((paramsRep->log_button_data[i] & (1 << (j & 0x07))) == false) {
                    m_logicButtonPtrList[number]->setButtonState(false);
                }
            }
        }
    }

    // physical button state
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            number = j + (i *8);
//            if (number >= m_PhysButtonPtrList.size()) {
//                i = 99;
//                break;// goto
//            }

            if (number < m_PhysButtonPtrList.size()) {
                if ((paramsRep->phy_button_data[i] & (1 << (j & 0x07)))) {
                    m_PhysButtonPtrList[number]->setButtonState(true);
                } else if ((paramsRep->phy_button_data[i] & (1 << (j & 0x07))) == false) {
                    m_PhysButtonPtrList[number]->setButtonState(false);
                }
            }
        }
    }


    // shift state
    for (int i = 0; i < SHIFT_COUNT; ++i) // выглядит как избыточный код, но так необходимо для оптимизации
    {
        if (paramsRep->shift_button_data & (1 << (i & 0x07))) {
            m_isShifts_act = true;

            if (i == 0 && m_shift1_act == false) {
                m_defaultShiftStyle = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift1_logicalButton->setStyleSheet(m_defaultShiftStyle + "background-color: rgb(0, 128, 0);");
                //ui->groupBox_Shift1->setStyleSheet("background-color: rgb(0, 128, 0);");
                //ui->spinBox_Shift1->setStyleSheet("background-color: rgb(0, 128, 0);");
                m_shift1_act = true;
            } else if (i == 1 && m_shift2_act == false) {
                m_defaultShiftStyle = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift2_logicalButton->setStyleSheet(m_defaultShiftStyle + "background-color: rgb(0, 128, 0);");
                m_shift2_act = true;
            } else if (i == 2 && m_shift3_act == false) {
                m_defaultShiftStyle = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift3_logicalButton->setStyleSheet(m_defaultShiftStyle + "background-color: rgb(0, 128, 0);");
                m_shift3_act = true;
            } else if (i == 3 && m_shift4_act == false) {
                m_defaultShiftStyle = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift4_logicalButton->setStyleSheet(m_defaultShiftStyle + "background-color: rgb(0, 128, 0);");
                m_shift4_act = true;
            } else if (i == 4 && m_shift5_act == false) {
                m_defaultShiftStyle = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift5_logicalButton->setStyleSheet(m_defaultShiftStyle + "background-color: rgb(0, 128, 0);");
                m_shift5_act = true;
            }

        } else if (m_isShifts_act == true) {
            if (i == 0 && m_shift1_act == true) {
                ui->text_shift1_logicalButton->setStyleSheet(m_defaultShiftStyle);
                m_shift1_act = false;
            } else if (i == 0 && m_shift2_act == true) {
                ui->text_shift2_logicalButton->setStyleSheet(m_defaultShiftStyle);
                m_shift2_act = false;
            } else if (i == 0 && m_shift3_act == true) {
                ui->text_shift3_logicalButton->setStyleSheet(m_defaultShiftStyle);
                m_shift3_act = false;
            } else if (i == 0 && m_shift4_act == true) {
                ui->text_shift4_logicalButton->setStyleSheet(m_defaultShiftStyle);
                m_shift4_act = false;
            } else if (i == 0 && m_shift5_act == true) {
                ui->text_shift5_logicalButton->setStyleSheet(m_defaultShiftStyle);
                m_shift5_act = false;
            }

            if (m_shift1_act == false && m_shift2_act == false && m_shift3_act == false && m_shift4_act == false
                && m_shift5_act == false) {
                m_isShifts_act = false;
            }
        }
    }
}

void ButtonConfig::readFromConfig()
{
    dev_config_t *devc = &gEnv.pDeviceConfig->config;

    // dynamic creation with scroll
#ifdef DYNAMIC_CREATION
    for (int i = MAX_BUTTONS_NUM; i > 0; --i) {
        if (gEnv.pDeviceConfig->config.buttons[i -1].physical_num != -1) {
            int size = m_logicButtonPtrList.size();
            int count = i - size;
            createLogButtons(count);
            ui->layoutV_LogicalButton->activate();
            break;
        }
    }
#endif

    // logical buttons
    for (int i = 0; i < m_logicButtonPtrList.size(); i++) {
        m_logicButtonPtrList[i]->readFromConfig();
    }
    // other
    ui->spinBox_Shift1->setValue(devc->shift_config[0].button + 1);
    ui->spinBox_Shift2->setValue(devc->shift_config[1].button + 1);
    ui->spinBox_Shift3->setValue(devc->shift_config[2].button + 1);
    ui->spinBox_Shift4->setValue(devc->shift_config[3].button + 1);
    ui->spinBox_Shift5->setValue(devc->shift_config[4].button + 1);

    ui->spinBox_Timer1->setValue(devc->button_timer1_ms);
    ui->spinBox_Timer2->setValue(devc->button_timer2_ms);
    ui->spinBox_Timer3->setValue(devc->button_timer3_ms);

    ui->spinBox_DebounceTimer->setValue(devc->button_debounce_ms);
    ui->spinBox_A2bDebounce->setValue(devc->a2b_debounce_ms);

    ui->spinBox_EncoderPressTimer->setValue(devc->encoder_press_time_ms);

    ui->spinBox_ButtonsPolling->setValue(devc->button_polling_interval_ticks * TICKS_NS);
    ui->spinBox_EncoderPolling->setValue(devc->encoder_polling_interval_ticks * TICKS_NS);
}

void ButtonConfig::writeToConfig()
{
    dev_config_t *devc = &gEnv.pDeviceConfig->config;
    devc->shift_config[0].button = ui->spinBox_Shift1->value() - 1;
    devc->shift_config[1].button = ui->spinBox_Shift2->value() - 1;
    devc->shift_config[2].button = ui->spinBox_Shift3->value() - 1;
    devc->shift_config[3].button = ui->spinBox_Shift4->value() - 1;
    devc->shift_config[4].button = ui->spinBox_Shift5->value() - 1;

    devc->button_timer1_ms = ui->spinBox_Timer1->value();
    devc->button_timer2_ms = ui->spinBox_Timer2->value();
    devc->button_timer3_ms = ui->spinBox_Timer3->value();

    devc->button_debounce_ms = ui->spinBox_DebounceTimer->value();
    devc->a2b_debounce_ms = ui->spinBox_A2bDebounce->value();

    devc->encoder_press_time_ms = ui->spinBox_EncoderPressTimer->value();

    devc->button_polling_interval_ticks = ui->spinBox_ButtonsPolling->value() / TICKS_NS;
    devc->encoder_polling_interval_ticks = ui->spinBox_EncoderPolling->value() / TICKS_NS;

    // logical buttons
    for (int i = 0; i < m_logicButtonPtrList.size(); ++i) {
        m_logicButtonPtrList[i]->writeToConfig();
    }
}
