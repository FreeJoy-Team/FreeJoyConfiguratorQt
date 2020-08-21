#include "buttonlogical.h"
#include "ui_buttonlogical.h"


ButtonLogical::ButtonLogical(int button_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ButtonLogical)
{
    ui->setupUi(this);
    button_number_ = button_number;
    function_previous_index_ = 0;
    is_activated_ = false;
    ui->label_LogicalButtonNumber->setNum(button_number_ + 1);

    // add gui text
    for (int i = 0; i < LOGICAL_FUNCTION_COUNT; i++) {      // add gui text
        ui->comboBox_ButtonFunction->addItem(logical_function_list_[i].gui_name);
    }
    for (int i = 0; i < SHIFT_COUNT; i++) {
        ui->comboBox_ShiftIndex->addItem(shift_list_[i].gui_name);
    }
    for (int i = 0; i < TIMER_COUNT; i++) {
        ui->comboBox_DelayTimerIndex->addItem(timer_list_[i].gui_name);
        ui->comboBox_PressTimerIndex->addItem(timer_list_[i].gui_name);
    }

    connect(ui->comboBox_ButtonFunction, SIGNAL(currentIndexChanged(int)),
            this, SLOT(functionTypeChanged(int)));
    connect(ui->spinBox_PhysicalButtonNumber, SIGNAL(valueChanged(int)),
            this, SLOT(editingOnOff(int)));
}

ButtonLogical::~ButtonLogical()
{
    delete ui;
}

void ButtonLogical::RetranslateUi()
{
    ui->retranslateUi(this);
}

void ButtonLogical::ReadFromConfig()                // rename to ReadFromConfig
{
    // physical
    ui->spinBox_PhysicalButtonNumber->setValue(gEnv.pDeviceConfig->config.buttons[button_number_].physical_num + 1);        // !!!!
    // isDisable
    ui->checkBox_IsDisable->setChecked(gEnv.pDeviceConfig->config.buttons[button_number_].is_disabled);
    // isInvert
    ui->checkBox_IsInvert->setChecked(gEnv.pDeviceConfig->config.buttons[button_number_].is_inverted);

    // logical button function
    for (int i = 0; i < LOGICAL_FUNCTION_COUNT; i++) {      // get config
        if (gEnv.pDeviceConfig->config.buttons[button_number_].type == logical_function_list_[i].device_enum_index )
        {
            ui->comboBox_ButtonFunction->setCurrentIndex(i);
            break;
        }
    }
    // shift
    for (int i = 0; i < SHIFT_COUNT; i++) {
        if (gEnv.pDeviceConfig->config.buttons[button_number_].shift_modificator == shift_list_[i].device_enum_index )
        {
            ui->comboBox_ShiftIndex->setCurrentIndex(i);
            break;
        }
    }
    // delay timer
    for (int i = 0; i < TIMER_COUNT; i++) {
        if (gEnv.pDeviceConfig->config.buttons[button_number_].delay_timer == timer_list_[i].device_enum_index )
        {
            ui->comboBox_DelayTimerIndex->setCurrentIndex(i);
            break;
        }
    }
    // toggle timer
    for (int i = 0; i < TIMER_COUNT; i++) {
        if (gEnv.pDeviceConfig->config.buttons[button_number_].press_timer == timer_list_[i].device_enum_index )
        {
            ui->comboBox_PressTimerIndex->setCurrentIndex(i);
            break;
        }
    }
}

void ButtonLogical::WriteToConfig()
{
    gEnv.pDeviceConfig->config.buttons[button_number_].physical_num = ui->spinBox_PhysicalButtonNumber->value() - 1;        // !!!!
    gEnv.pDeviceConfig->config.buttons[button_number_].is_disabled = ui->checkBox_IsDisable->isChecked();
    gEnv.pDeviceConfig->config.buttons[button_number_].is_inverted = ui->checkBox_IsInvert->isChecked();

    gEnv.pDeviceConfig->config.buttons[button_number_].type = logical_function_list_[ui->comboBox_ButtonFunction->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.buttons[button_number_].shift_modificator = shift_list_[ui->comboBox_ShiftIndex->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.buttons[button_number_].delay_timer = timer_list_[ui->comboBox_DelayTimerIndex->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.buttons[button_number_].press_timer = timer_list_[ui->comboBox_PressTimerIndex->currentIndex()].device_enum_index;
}

void ButtonLogical::SetMaxPhysButtons(int max_phys_buttons)
{
    ui->spinBox_PhysicalButtonNumber->setMaximum(max_phys_buttons);
}

void ButtonLogical::SetSpinBoxOnOff(int max_phys_buttons)
{
    if (max_phys_buttons > 0){
        ui->spinBox_PhysicalButtonNumber->setEnabled(true);
    } else {
        ui->spinBox_PhysicalButtonNumber->setEnabled(false);
    }
}

void ButtonLogical::functionTypeChanged(int index)
{
    emit functionIndexChanged(index, function_previous_index_, button_number_);
    function_previous_index_ = index;
}

void ButtonLogical::editingOnOff(int value)
{
    if(value > 0){
        ui->checkBox_IsInvert->setEnabled(true);
        ui->checkBox_IsDisable->setEnabled(true);
        ui->comboBox_ButtonFunction->setEnabled(true);
        ui->comboBox_ShiftIndex->setEnabled(true);
        ui->comboBox_DelayTimerIndex->setEnabled(true);
        ui->comboBox_PressTimerIndex->setEnabled(true);
    } else {
        ui->checkBox_IsInvert->setEnabled(false);
        ui->checkBox_IsDisable->setEnabled(false);
        ui->comboBox_ButtonFunction->setEnabled(false);
        ui->comboBox_ShiftIndex->setEnabled(false);
        ui->comboBox_DelayTimerIndex->setEnabled(false);
        ui->comboBox_PressTimerIndex->setEnabled(false);
    }
}

void ButtonLogical::ButtonState(bool is_activated)
{
    is_activated_ = is_activated;
    if (is_activated_){
        default_style_ = this->styleSheet();
        this->setStyleSheet(default_style_ + "background-color: rgb(0, 128, 0);");
    } else {
        this->setStyleSheet(default_style_);//("background: palette(window)");        // баг тряска если ("") // текущий вариант не пашет с чёрным стилем
    }
}
