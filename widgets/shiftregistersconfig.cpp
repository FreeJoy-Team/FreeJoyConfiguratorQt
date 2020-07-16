#include "shiftregistersconfig.h"
#include "ui_shiftregistersconfig.h"

ShiftRegistersConfig::ShiftRegistersConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShiftRegistersConfig)
{
    ui->setupUi(this);
    shift_buttons_count_ = 0;

    ui->layoutV_ShiftRegisters->setAlignment(Qt::AlignTop);
    // shift registers spawn
    for (int i = 0; i < MAX_SHIFT_REG_NUM; i++)
    {
        ShiftRegisters * shift_register = new ShiftRegisters(i, this);
        ui->layoutV_ShiftRegisters->addWidget(shift_register);
        ShiftRegistersAdrList.append(shift_register);
        //shift_register->hide();
        connect(shift_register, SIGNAL(buttonCountChanged(int, int)),
                this, SLOT(shiftRegButtonsCalc(int, int)));
    }
}

ShiftRegistersConfig::~ShiftRegistersConfig()
{
    delete ui;
}

void ShiftRegistersConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < ShiftRegistersAdrList.size(); ++i) {
        ShiftRegistersAdrList[i]->RetranslateUi();
    }
}


void ShiftRegistersConfig::shiftRegButtonsCalc(int count, int previous_count)
{
    shift_buttons_count_ += count - previous_count;
    emit shiftRegButtonsCountChanged(shift_buttons_count_);
}


bool ShiftRegistersConfig::SortByPinNumber(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs)
{
    return lhs.pin_number < rhs.pin_number;
}

bool ShiftRegistersConfig::SortNullLast(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs)
{
    Q_UNUSED(lhs);
    return rhs.pin_number == 0;
}

void ShiftRegistersConfig::shiftRegSelected(int latch_pin, int data_pin, QString pin_gui_name)      // сделать выключение у пинов при latch/data count == 4
{
    // add shift reg latch pin
    if (latch_pin != 0){
        if (latch_pin > 0){
            latch_pins_array[latch_pins_array.size() - 1].pin_number = latch_pin;       // latch_pins_array.size() - 1// мб сделать отдельную переменную?
            latch_pins_array[latch_pins_array.size() - 1].gui_name = pin_gui_name;
        }
        // delete shift reg latch pin
        else
        {
            latch_pin = -latch_pin;
            for (uint i = 0; i < latch_pins_array.size(); ++i) {
                if (latch_pin == latch_pins_array[i].pin_number){

                    latch_pins_array[i].pin_number = 0;
                    latch_pins_array[i].gui_name = ShiftRegistersAdrList[0]->not_defined_;      // hz
                }
            }
        }
        // sort ascending
        std::sort(latch_pins_array.begin(), latch_pins_array.end(), SortByPinNumber);           // хз можно ли через 1 сорт
        // sort null last
        std::stable_sort(latch_pins_array.begin(), latch_pins_array.end(), SortNullLast);
        //all unused pins = bigger pin
        for (size_t i = latch_pins_array.size() - 1; i >= 0; --i) {         // бесконечный цикл, доработать
            if (latch_pins_array[i].pin_number > 0){
                for (size_t j = latch_pins_array.size() - 1; j > i; --j) {
                    latch_pins_array[j].pin_number = latch_pins_array[i].pin_number;
                    latch_pins_array[j].gui_name = latch_pins_array[0].gui_name;
                }
                break;
            }
        }
        // update shiftreg ui
        for (uint i = 0; i < latch_pins_array.size() - 1; ++i) {
            ShiftRegistersAdrList[i]->SetLatchPin(latch_pins_array[i].pin_number, latch_pins_array[i].gui_name);
        }

    }
    // add shift reg data pin
    else if (data_pin != 0){        // data_pin != 0 необязательно

        if (data_pin > 0){
            data_pins_array[data_pins_array.size() - 1].pin_number = data_pin;
            data_pins_array[data_pins_array.size() - 1].gui_name = pin_gui_name;
        }
        else
        {
            data_pin = -data_pin;
            for (uint i = 0; i < data_pins_array.size(); ++i) {
                if (data_pin == data_pins_array[i].pin_number){
                    data_pins_array[i].pin_number = 0;
                    data_pins_array[i].gui_name = ShiftRegistersAdrList[i]->not_defined_;      // hz
                }
            }
        }

        std::sort(data_pins_array.begin(), data_pins_array.end(), SortByPinNumber);           // хз можно ли через 1 сорт
        std::stable_sort(data_pins_array.begin(), data_pins_array.end(), SortNullLast);

        for (uint i = 0; i < data_pins_array.size() - 1; ++i) {
            ShiftRegistersAdrList[i]->SetDataPin(data_pins_array[i].pin_number, data_pins_array[i].gui_name);
        }

    }

                                   // WARNING GOVNOKOD !!!!!!     // пиздец слишком сложно
//    int tmp_add = 0;
//    QString tmp_add_str;
//    int tmp_delete = 0;
//    QString tmp_delete_str;
//    // add shift reg latch pin
//    if (latch_pin > 0)
//    {
//        for (int i = 0; i < MAX_SHIFT_REG_NUM; ++i)
//        {
//            if (latch_pin != 0 && (latch_pin < ShiftRegistersAdrList[i]->GetLatchPin() || ShiftRegistersAdrList[i]->GetLatchPin() == 0))
//            {
//                if (ShiftRegistersAdrList[i]->GetLatchPin() != 0)
//                {
//                    tmp_add = ShiftRegistersAdrList[i]->GetLatchPin();
//                    tmp_add_str = ShiftRegistersAdrList[i]->GetLatchString();
//                    ShiftRegistersAdrList[i]->SetLatchPin(latch_pin, pin_gui_name);
//                    latch_pin = tmp_add;
//                    pin_gui_name = tmp_add_str;
//                }
//                else if (ShiftRegistersAdrList[i]->GetLatchPin() == 0)
//                {
//                    ShiftRegistersAdrList[i]->SetLatchPin(latch_pin, pin_gui_name);
//                }
//            }
//        }
//    }
//    // delete shift reg latch pin
//    else if (latch_pin < 0)
//    {
//        latch_pin = -latch_pin;
//        for (int i = 0; i < MAX_SHIFT_REG_NUM; ++i)
//        {
//            if (latch_pin != 0 && (ShiftRegistersAdrList[i]->GetLatchPin() == latch_pin))   // latch_pin != 0  nah
//            {
//                for (int j = i; j < MAX_SHIFT_REG_NUM; ++j)
//                {
//                    qDebug()<<"latch jjjjj = "<<j;
//                    if (j == MAX_SHIFT_REG_NUM - 1){
//                        break;
//                    }
//                    tmp_delete = ShiftRegistersAdrList[j+1]->GetLatchPin();
//                    tmp_delete_str = ShiftRegistersAdrList[j+1]->GetLatchString();
//                    ShiftRegistersAdrList[j]->SetLatchPin(ShiftRegistersAdrList[j+1]->GetLatchPin(), pin_gui_name);
//                    latch_pin = tmp_delete;
//                    pin_gui_name = tmp_delete_str;
//                }
//                break;
//            }
//        }
//    }

//    // add shift reg data pin
//    else if (data_pin > 0)
//    {
//        if (data_pin_count_ < MAX_SHIFT_REG_NUM){
//            data_pin_count_++;
//        }
//        qDebug()<<"data SELECTED = "<<data_pin_count_;
//        for (int i = 0; i < data_pin_count_; ++i)
//        {
//            if (data_pin != 0 && (data_pin < ShiftRegistersAdrList[i]->GetDataPin() || ShiftRegistersAdrList[i]->GetDataPin() == 0))
//            {
//                if (ShiftRegistersAdrList[i]->GetDataPin() != 0)
//                {
//                    tmp_add = ShiftRegistersAdrList[i]->GetDataPin();
//                    tmp_add_str = ShiftRegistersAdrList[i]->GetDataString();
//                    ShiftRegistersAdrList[i]->SetDataPin(data_pin, pin_gui_name);
//                    data_pin = tmp_add;
//                    pin_gui_name = tmp_add_str;
//                }
//                else if (ShiftRegistersAdrList[i]->GetDataPin() == 0)
//                {
//                    ShiftRegistersAdrList[i]->SetDataPin(data_pin, pin_gui_name);
//                    qDebug()<<"data string ="<<ShiftRegistersAdrList[i]->GetDataString();
//                }
//            }
//        }
//    }
//    // delete shift reg data pin
//    else if (data_pin < 0)
//    {
//        data_pin = -data_pin;
//        for (int i = 0; i < data_pin_count_; ++i)
//        {
//            if (data_pin != 0 && (ShiftRegistersAdrList[i]->GetDataPin() == data_pin))
//            {
//                for (int j = i; j < data_pin_count_; ++j)
//                {
//                    int tmp_2;
//                    if (tmp_2 > MAX_SHIFT_REG_NUM){
//                        tmp_2 = j;
//                    }
//                    tmp_delete = ShiftRegistersAdrList[tmp_2]->GetDataPin();
//                    tmp_delete_str = ShiftRegistersAdrList[tmp_2]->GetDataString();
//                    ShiftRegistersAdrList[j]->SetDataPin(ShiftRegistersAdrList[tmp_2]->GetDataPin(), pin_gui_name);
//                    data_pin = tmp_delete;
//                    pin_gui_name = tmp_delete_str;
//                }
//                break;
//            }
//        }
//        data_pin_count_--;
//    }
}

void ShiftRegistersConfig::ReadFromConfig()
{
    for (int i = 0; i < ShiftRegistersAdrList.size(); ++i) {
        ShiftRegistersAdrList[i]->ReadFromConfig();
    }
}

void ShiftRegistersConfig::WriteToConfig()
{
    for (int i = 0; i < ShiftRegistersAdrList.size(); ++i) {
        ShiftRegistersAdrList[i]->WriteToConfig();
    }
}
